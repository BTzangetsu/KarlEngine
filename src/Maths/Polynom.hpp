#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <initializer_list>
#include <cmath>
#include <algorithm>
#include <complex>

namespace KE{

	class Polynom {

private:
	void computeRoots() {
		m_zeros.clear();
		size_t deg = getDeg();

		if (deg == 0) {
			// Polynomial constant, no roots (unless it's zero everywhere)
			return;
		}
		else if (deg == 1) {
			// Linear: ax + b = 0 => x = -b/a
			float a = getCoefAt(1);
			float b = getCoefAt(0);
			if (std::abs(a) > 1e-9f) {
				m_zeros.push_back(-b / a);
			}
		}
		else if (deg == 2) {
			// Quadratic: ax� + bx + c = 0
			float a = getCoefAt(2);
			float b = getCoefAt(1);
			float c = getCoefAt(0);

			float discriminant = b * b - 4 * a * c;

			if (discriminant > 1e-9f) {
				// Two real roots
				float sqrtDisc = std::sqrt(discriminant);
				m_zeros.push_back((-b + sqrtDisc) / (2 * a));
				m_zeros.push_back((-b - sqrtDisc) / (2 * a));
			}
			else if (std::abs(discriminant) <= 1e-9f) {
				// One double root
				m_zeros.push_back(-b / (2 * a));
			}
			// If discriminant < 0, no real roots (complex roots not stored)
		}
		else {
			// For degree > 2, use numerical methods (Newton-Raphson)
			// Try to find roots starting from different initial guesses
			std::vector<float> guesses = { -10.f, -5.f, -1.f, 0.f, 1.f, 5.f, 10.f };

			for (float guess : guesses) {
				float root = newtonRaphson(guess, 100, 1e-6f);

				// Check if it's actually a root
				if (std::abs(evaluate(root)) < 1e-4f) {
					// Check if we already found this root
					bool alreadyFound = false;
					for (float existingRoot : m_zeros) {
						if (std::abs(root - existingRoot) < 1e-4f) {
							alreadyFound = true;
							break;
						}
					}

					if (!alreadyFound) {
						m_zeros.push_back(root);
					}
				}
			}
		}

		// Sort roots
		std::sort(m_zeros.begin(), m_zeros.end());
	}

	// Newton-Raphson method to find a root
	float newtonRaphson(float x0, int maxIterations, float tolerance) const {
		float x = x0;
		Polynom derivative = getDerivation();

		for (int i = 0; i < maxIterations; i++) {
			float fx = evaluate(x);
			float fpx = derivative.evaluate(x);

			if (std::abs(fpx) < 1e-10f) {
				// Derivative too small, can't continue
				break;
			}

			float xNew = x - fx / fpx;

			if (std::abs(xNew - x) < tolerance) {
				return xNew;
			}

			x = xNew;
		}

		return x;
	}

protected:

	std::vector<float> m_coefs;
	std::vector<float> m_zeros;

	friend Polynom& operator*(float scalar, Polynom& polynom);

	void times(float scalar) {
		for (auto coef : m_coefs) {
			coef = coef * scalar;
		}
	}

public:

	Polynom(std::initializer_list<float> coefs) : m_coefs(coefs) {
		if (m_coefs.empty()) {
			m_coefs.push_back(0.f);
		}
	}

	Polynom(size_t deg = 0) :m_coefs(deg + 1, 0.f) {}

	Polynom(const Polynom& copied) :m_coefs(copied.m_coefs), m_zeros(copied.m_zeros) {

	}

	size_t getDeg() const {
		int deg = 0;
		for (int i = m_coefs.size() - 1; i >= 0; i--) {
			if (m_coefs.at(i) == 0.f) {
				continue;
			}
			else {
				deg = i;
				break;
			}
		}
		return deg;
	}

	float getCoefAt(size_t i)const {
		if (i >= m_coefs.size()) {
			throw std::out_of_range("the index: " + std::to_string(i) + " is out of range for size: " + std::to_string(m_coefs.size()));
		}
		return m_coefs.at(i);
	}

	void setCoefAt(size_t i, float coef) {
		if (i >= m_coefs.size()) {
			throw std::out_of_range("the index: " + std::to_string(i) + " is out of range for size: " + std::to_string(m_coefs.size()));
		}
		m_coefs[i] = coef;
	}

	Polynom getDerivation()const {
		if (m_coefs.size() <= 1) {
			return Polynom(0);  // Derivative of constant is 0
		}
		Polynom result(m_coefs.size() - 2);
		for (int i = 1; i < m_coefs.size(); i++) {
			float result_coef = i * getCoefAt(i);
			result.setCoefAt(i - 1, result_coef);
		}
		return result;
	}

	Polynom getIntegration(float cte = 0.f) const {
		Polynom result(m_coefs.size());
		result.setCoefAt(0, cte);
		for (int i = 0; i < m_coefs.size(); i++) {
			float result_coef = m_coefs.at(i) / (i + 1);
			result.setCoefAt(i + 1, result_coef);
		}
		return result;
	}

	void integrate(float cte = 0.f) {
		*this = getIntegration(cte);
	}

	void derivate() {
		*this = getDerivation();
	}

	float evaluate(float x) const {
		float result = 0.f;
		for (int i = 0; i < m_coefs.size(); i++) {
			result += m_coefs.at(i) * std::pow(x, i);
		}
		return result;
	}

	//return the images of numbers between first and last included separated by offset
	std::vector<float> getImagesWith(float first, float last, float offset) {
		if (first > last) {
			throw std::logic_error("the first should not be greater than the last");
		}

		std::vector<float> result;
		if (first == last) {
			result.push_back(evaluate(last));
			return result;
		}

		float m_first = first;
		while (m_first < last) {
			result.push_back(evaluate(m_first));
			m_first += offset;
			if (m_first >= last) {
				result.push_back(evaluate(last));
				break;
			}
		}
		return result;
	}

	//this method return imagesCounts images from points between first and last included
	std::vector<float> getImagesFrom(float first, float last, size_t imagesCount) {
		float offset = (last - first) / imagesCount;
		return getImagesWith(first, last, offset);
	}

	const std::vector<float>& getRoots() {
		if (m_zeros.empty()) {
			computeRoots();
		}
		return m_zeros;
	}

	std::string toString(char symbol = 'X') {
		std::string result = "";
		result += std::to_string(m_coefs.at(0));

		if (m_coefs.size() <= 1) return;

		for (int i = 1; i < m_coefs.size(); i++) {
			if (m_coefs.at(i) == 0.f) {
				continue;
			}
			else if (m_coefs.at(i) == 1.f) {
				result += "+" + symbol + std::to_string(i);
			}
			else if (m_coefs.at(i) == -1.f) {
				result += "-" + symbol + std::to_string(i);
			}
			else if (m_coefs.at(i) < 0.f) {
				result += "-" + std::to_string(m_coefs.at(i)) + symbol + std::to_string(i);
			}
			else {
				result += "+" + std::to_string(m_coefs.at(i)) + symbol + std::to_string(i);
			}

		}
		return result;
	}

	//=============operators==================
	Polynom& operator=(const Polynom& copied) {
		m_coefs = copied.m_coefs;
		m_zeros = copied.m_zeros;
		return *this;
	}

	bool operator==(const Polynom& other) const {
		if (getDeg() != other.getDeg()) {
			return false;
		}
		for (size_t i = 0; i <= getDeg(); i++) {
			if (std::abs(m_coefs[i] - other.m_coefs[i]) > 1e-6f) {
				return false;
			}
		}
		return true;
	}

	Polynom operator+(const Polynom& other)const {
		size_t maxSize = std::max(m_coefs.size(), other.m_coefs.size());
		Polynom result(maxSize - 1);

		for (size_t i = 0; i < maxSize; i++) {
			float coef = 0.f;
			if (i < m_coefs.size()) coef += m_coefs[i];
			if (i < other.m_coefs.size()) coef += other.m_coefs[i];
			result.setCoefAt(i, coef);
		}
		return result;
	}

	Polynom operator-(const Polynom& other) const {
		size_t maxSize = std::max(m_coefs.size(), other.m_coefs.size());
		Polynom result(maxSize - 1);

		for (size_t i = 0; i < maxSize; i++) {
			float coef = 0.f;
			if (i < m_coefs.size()) coef += m_coefs[i];
			if (i < other.m_coefs.size()) coef -= other.m_coefs[i];
			result.setCoefAt(i, coef);
		}
		return result;
	}

	Polynom operator*(const Polynom& other) const {
		if (m_coefs.empty() || other.m_coefs.empty()) {
			return Polynom(0);
		}

		// deg(P*Q) = deg(P) + deg(Q)
		// We need (deg(P) + deg(Q) + 1) coefficients
		// size(P) = deg(P) + 1, so deg(P) = size(P) - 1
		// deg(result) = (size(P)-1) + (size(Q)-1) = size(P) + size(Q) - 2
		Polynom result(m_coefs.size() + other.m_coefs.size() - 2);

		for (size_t i = 0; i < m_coefs.size(); i++) {
			for (size_t j = 0; j < other.m_coefs.size(); j++) {
				float newCoef = result.getCoefAt(i + j) + m_coefs[i] * other.m_coefs[j];
				result.setCoefAt(i + j, newCoef);
			}
		}
		return result;
	}

	std::ostream& operator<<(std::ostream& out) {
		
		return out<< toString();
	}
};

Polynom& operator*(float scalar, Polynom& polynom) {
	polynom.times(scalar);
}
}