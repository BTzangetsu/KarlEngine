#pragma once
#include <vector>
#include <functional>
#include <unordered_map>
#include <SFML/Graphics.hpp>

//KarlEngine namespace.
namespace KE {

	// Curve types
	enum CurveType {
		LINEAR,
		BEZIER,
		CATMULL_ROM
	};

	class Curve {

	private:
		// Control points that define the curve
		std::vector<sf::Vector2f> controlPoints;

		// Current curve type
		CurveType curveType;

		// Number of segments between control points
		int segments;

		// Vector of interpolated points that make up the curve
		std::vector<sf::Vector2f> computedPoints;

		// Vector of vertices that make up the curve (as quads for thickness)
		std::vector<sf::Vertex> curveVertices;

		// Curve thickness
		float thickness = 2.0f;

		// Curve color
		sf::Color color = sf::Color::Black;

		// Observer callbacks - multiple observers can watch this curve (multi-plan support)
		// Key: observer identifier (typically the CartesianPlan's 'this' pointer)
		// Value: callback function to notify observer of changes
		std::unordered_map<void*, std::function<void(Curve*)>> observers;

		// Static factorial table for Bezier computation optimization
		static std::vector<int> factorialTable;

		// Initialize factorial table up to maxN
		static void initFactorialTable(int maxN = 20) {
			if (factorialTable.empty()) {
				factorialTable.resize(maxN + 1);
				factorialTable[0] = 1;
				for (int i = 1; i <= maxN; ++i) {
					factorialTable[i] = factorialTable[i - 1] * i;
				}
			}
		}

		// Get factorial from pre-computed table
		static int factorial(int n) {
			if (factorialTable.empty()) initFactorialTable();
			if (n >= static_cast<int>(factorialTable.size())) {
				initFactorialTable(n + 10);
			}
			return factorialTable[n];
		}

		// Computes linear interpolation between control points
		void computeLinear() {
			computedPoints.clear();
			for (size_t i = 0; i < controlPoints.size() - 1; ++i) {
				for (int j = 0; j <= segments; ++j) {
					float t = static_cast<float>(j) / segments;
					sf::Vector2f point = (1.0f - t) * controlPoints[i] + t * controlPoints[i + 1];
					computedPoints.push_back(point);
				}
			}
		}

		// Computes Bezier curve points using Bernstein polynomials
		void computeBezier() {
			computedPoints.clear();
			int n = controlPoints.size() - 1;
			for (int j = 0; j <= segments * n; ++j) {
				float t = static_cast<float>(j) / (segments * n);
				sf::Vector2f point(0.f, 0.f);
				for (int i = 0; i <= n; ++i) {
					float binom = static_cast<float>(factorial(n)) /
						(factorial(i) * factorial(n - i));
					float bernstein = binom * pow(t, i) * pow(1.0f - t, n - i);
					point.x += bernstein * controlPoints[i].x;
					point.y += bernstein * controlPoints[i].y;
				}
				computedPoints.push_back(point);
			}
		}

		// Computes Catmull-Rom spline points
		void computeCatmullRom() {
			computedPoints.clear();
			for (size_t i = 0; i < controlPoints.size() - 1; ++i) {
				sf::Vector2f p0 = (i == 0) ? controlPoints[i] : controlPoints[i - 1];
				sf::Vector2f p1 = controlPoints[i];
				sf::Vector2f p2 = controlPoints[i + 1];
				sf::Vector2f p3 = (i + 2 < controlPoints.size()) ? controlPoints[i + 2] : controlPoints[i + 1];

				for (int j = 0; j <= segments; ++j) {
					float t = static_cast<float>(j) / segments;
					float t2 = t * t;
					float t3 = t2 * t;

					sf::Vector2f point;
					point.x = 0.5f * ((2.f * p1.x) +
						(-p0.x + p2.x) * t +
						(2.f * p0.x - 5.f * p1.x + 4.f * p2.x - p3.x) * t2 +
						(-p0.x + 3.f * p1.x - 3.f * p2.x + p3.x) * t3);

					point.y = 0.5f * ((2.f * p1.y) +
						(-p0.y + p2.y) * t +
						(2.f * p0.y - 5.f * p1.y + 4.f * p2.y - p3.y) * t2 +
						(-p0.y + 3.f * p1.y - 3.f * p2.y + p3.y) * t3);

					computedPoints.push_back(point);
				}
			}
		}

		// Computes the quad vertices to display between two points with thickness
		void computeVertice(const sf::Vector2f& start, const sf::Vector2f& end) {
			sf::Vector2f direction = end - start;
			float length = sqrt(direction.x * direction.x + direction.y * direction.y);
			if (length == 0) return;

			direction /= length; // Normalize
			sf::Vector2f perpendicular(-direction.y, direction.x);

			sf::Vector2f offset = (thickness / 2.f) * perpendicular;

			sf::Vertex quad[4];
			quad[0].position = start + offset;
			quad[1].position = end + offset;
			quad[2].position = end - offset;
			quad[3].position = start - offset;

			for (int i = 0; i < 4; ++i) {
				quad[i].color = color;
			}
			curveVertices.insert(curveVertices.end(), quad, quad + 4);
		}

		// Compute the curve vertex array as quads
		void computeCurveVertices() {
			curveVertices.clear();
			if (computedPoints.empty()) return;
			for (size_t i = 0; i < computedPoints.size() - 1; ++i) {
				computeVertice(computedPoints[i], computedPoints[i + 1]);
			}
		}

		// Notify all observers that the curve has been modified
		void notifyObservers() {
			for (auto& pair : observers) {
				if (pair.second) {
					pair.second(this);
				}
			}
		}

	public:

		// Default constructor
		Curve()
			: curveType(LINEAR), segments(20) {}

		// Constructor with parameters
		Curve(const std::vector<sf::Vector2f>& controlPoints,
			CurveType type,
			int segments = 20,
			float thickness = 2.0f,
			const sf::Color& color = sf::Color::Black)
			: controlPoints(controlPoints)
			, curveType(type)
			, segments(segments)
			, thickness(thickness)
			, color(color) {
			computeCurve();
		}

		// Copy constructor
		Curve(const Curve& other)
			: controlPoints(other.controlPoints)
			, curveType(other.curveType)
			, segments(other.segments)
			, computedPoints(other.computedPoints)
			, curveVertices(other.curveVertices)
			, thickness(other.thickness)
			, color(other.color) {
			// Note: observers are NOT copied - each curve maintains its own observer list
		}

		// Assignment operator
		Curve& operator=(const Curve& other) {
			if (this != &other) {
				controlPoints = other.controlPoints;
				curveType = other.curveType;
				segments = other.segments;
				computedPoints = other.computedPoints;
				curveVertices = other.curveVertices;
				thickness = other.thickness;
				color = other.color;
				// Note: observers are NOT copied - keep the existing observer list
			}
			return *this;
		}

		// Add an observer to this curve
		// observerId: unique identifier for the observer (typically 'this' pointer of the observer)
		// callback: function to call when curve is modified
		void addObserver(void* observerId, std::function<void(Curve*)> callback) {
			observers[observerId] = callback;
		}

		// Remove an observer from this curve
		void removeObserver(void* observerId) {
			observers.erase(observerId);
		}

		// Check if an observer is registered
		bool hasObserver(void* observerId) const {
			return observers.find(observerId) != observers.end();
		}

		// Get number of observers
		size_t getObserverCount() const {
			return observers.size();
		}

		// Set thickness of the curve line
		void setThickness(float newThickness) {
			thickness = newThickness;
			computeCurveVertices();
			notifyObservers();
		}

		// Get current thickness
		float getThickness() const {
			return thickness;
		}

		// Set curve color
		void setColor(const sf::Color& newColor) {
			color = newColor;
			computeCurveVertices();
			notifyObservers();
		}

		// Get current color
		const sf::Color& getColor() const {
			return color;
		}

		// Set the type of the curve
		void setCurveType(CurveType type) {
			curveType = type;
			computeCurve();
			notifyObservers();
		}

		// Get current curve type
		CurveType getCurveType() const {
			return curveType;
		}

		// Set control points
		void setControlPoints(const std::vector<sf::Vector2f>& points) {
			controlPoints = points;
			computeCurve();
			notifyObservers();
		}

		// Get control points
		const std::vector<sf::Vector2f>& getControlPoints() const {
			return controlPoints;
		}

		// Set number of segments
		void setSegments(int newSegments) {
			if (newSegments <= 0) {
				sf::err() << "Error: Segments must be positive." << std::endl;
				return;
			}
			segments = newSegments;
			computeCurve();
			notifyObservers();
		}

		// Get number of segments
		int getSegments() const {
			return segments;
		}

		// Get the curve vertices (as quads for rendering)
		const std::vector<sf::Vertex>* getCurveVertices() const {
			return &curveVertices;
		}

		// Get computed points (interpolated points along the curve)
		const std::vector<sf::Vector2f>& getComputedPoints() const {
			return computedPoints;
		}

		// Compute the curve based on current parameters
		void computeCurve() {
			// Validate input
			if (controlPoints.size() < 2) {
				sf::err() << "Error: Curve needs at least 2 control points." << std::endl;
				return;
			}

			if (curveType == CATMULL_ROM && controlPoints.size() < 4) {
				sf::err() << "Warning: Catmull-Rom works best with at least 4 control points." << std::endl;
			}

			// Compute curve points based on type
			switch (curveType) {
			case LINEAR:
				computeLinear();
				break;
			case BEZIER:
				computeBezier();
				break;
			case CATMULL_ROM:
				computeCatmullRom();
				break;
			default:
				break;
			}

			// Generate vertices for rendering
			computeCurveVertices();
		}
	};

	// Initialize static member
	std::vector<int> Curve::factorialTable;
}