#pragma once
#include <iostream>
#include <stdexcept>
#include <utility>
#include <type_traits>
#include <iomanip>
#include <cmath>
#include <complex>
#include <vector>
#include <algorithm>
#include <initializer_list>

// Structure to represent an eigenvalue
template<typename T>
struct Eigenvalue {
    std::complex<T> value;
    size_t algebraicMultiplicity;

    Eigenvalue(std::complex<T> val, size_t mult = 1)
        : value(val), algebraicMultiplicity(mult) {}

    // Check if eigenvalue is real (negligible imaginary part)
    bool isReal(T epsilon = 1e-10) const {
        return std::abs(value.imag()) < epsilon;
    }

    // Get real part
    T real() const { return value.real(); }

    // Get imaginary part
    T imag() const { return value.imag(); }

    // Display
    friend std::ostream& operator<<(std::ostream& os, const Eigenvalue& ev) {
        if (ev.isReal()) {
            os << ev.real();
        }
        else {
            os << ev.real();
            if (ev.imag() >= 0) os << " + ";
            else os << " - ";
            os << std::abs(ev.imag()) << "i";
        }
        if (ev.algebraicMultiplicity > 1) {
            os << " (x" << ev.algebraicMultiplicity << ")";
        }
        return os;
    }
};

template <typename T>
class Matrix {
    // Declare all Matrix instantiations as friends
    // This allows Matrix<float> to access private data of Matrix<int>
    template<typename U>
    friend class Matrix;

private:
    T** data;
    size_t rows;
    size_t cols;

    // Allocate memory for the matrix
    void allocate() {
        data = new T * [rows];
        for (size_t i = 0; i < rows; ++i) {
            data[i] = new T[cols]();  // Initialize to 0
        }
    }

    // Free memory
    void deallocate() {
        if (data != nullptr) {
            for (size_t i = 0; i < rows; ++i) {
                delete[] data[i];
            }
            delete[] data;
            data = nullptr;
        }
    }

    // Multiply diagonal elements
    T multiplyDiagonal() const {
        T result = static_cast<T>(1);
        for (size_t i = 0; i < rows; i++) {
            result *= data[i][i];
        }
        return result;
    }

public:
    // ==================== CONSTRUCTORS ====================

    // Main constructor
    explicit Matrix(size_t rows, size_t cols) : data(nullptr), rows(rows), cols(cols) {
        if (rows == 0 || cols == 0) {
            throw std::logic_error("A matrix should have a size greater than 0");
        }
        allocate();
    }

    Matrix(std::initializer_list<std::initializer_list<T>> list) {
        rows = list.size();
        if (rows == 0) throw std::logic_error("Matrix cannot be empty");

        cols = list.begin()->size();
        if (cols == 0) throw std::logic_error("Rows cannot be empty");

        allocate();

        size_t i = 0;
        for (const auto& row_list : list) {
            if (row_list.size() != cols) {
                throw std::logic_error("All rows must have the same size");
            }

            size_t j = 0;
            for (const auto& val : row_list) {
                data[i][j] = val;
                j++;
            }
            i++;
        }
    }

    // Copy constructor (Rule of Five)
    Matrix(const Matrix& other) : data(nullptr), rows(other.rows), cols(other.cols) {
        allocate();
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                data[i][j] = other.data[i][j];
            }
        }
    }

    // Move constructor (Rule of Five)
    Matrix(Matrix&& other) noexcept
        : data(other.data), rows(other.rows), cols(other.cols) {
        other.data = nullptr;
        other.rows = 0;
        other.cols = 0;
    }

    // Destructor (Rule of Five)
    ~Matrix() {
        deallocate();
    }

    // ==================== ASSIGNMENT OPERATORS ====================

    // Copy assignment operator (Rule of Five)
    Matrix& operator=(const Matrix& other) {
        if (this == &other) return *this;  // Self-assignment protection

        // Reallocate if different size
        if (rows != other.rows || cols != other.cols) {
            deallocate();
            rows = other.rows;
            cols = other.cols;
            allocate();
        }

        // Copy data
        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                data[i][j] = other.data[i][j];
            }
        }
        return *this;
    }

    // Move assignment operator (Rule of Five)
    Matrix& operator=(Matrix&& other) noexcept {
        if (this != &other) {
            deallocate();
            data = other.data;
            rows = other.rows;
            cols = other.cols;
            other.data = nullptr;
            other.rows = 0;
            other.cols = 0;
        }
        return *this;
    }

    // ==================== ACCESSORS ====================

    T& at(size_t row, size_t col) {
        if (row >= rows || col >= cols) {
            throw std::out_of_range("Index out of bounds");
        }
        return data[row][col];
    }

    const T& at(size_t row, size_t col) const {
        if (row >= rows || col >= cols) {
            throw std::out_of_range("Index out of bounds");
        }
        return data[row][col];
    }

    size_t getRows() const { return rows; }
    size_t getCols() const { return cols; }
    bool isSquare() const { return cols == rows; }

    // ==================== OPERATORS ====================

    // Access with pair
    T& operator[](std::pair<size_t, size_t> index) {
        return at(index.first, index.second);
    }

    const T& operator[](std::pair<size_t, size_t> index) const {
        return at(index.first, index.second);
    }

    // Comparison operator
    bool operator==(const Matrix& other) const {
        if (cols != other.cols || rows != other.rows) {
            return false;
        }
        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                if (data[i][j] != other.data[i][j]) return false;
            }
        }
        return true;
    }

    bool operator!=(const Matrix& other) const {
        return !(*this == other);
    }

    // Output stream operator (friend function)
    friend std::ostream& operator<<(std::ostream& os, const Matrix& mat) {
        for (size_t i = 0; i < mat.rows; ++i) {
            for (size_t j = 0; j < mat.cols; ++j) {
                os << mat.data[i][j] << " ";
            }
            os << std::endl;
        }
        return os;
    }

    // ==================== MATHEMATICAL METHODS ====================

    // Matrix trace
    T trace() const {
        static_assert(std::is_arithmetic<T>::value, "Trace needs an arithmetic type.");
        if (!isSquare()) {
            throw std::logic_error("Non-square matrix doesn't have a trace.");
        }
        T result = static_cast<T>(0);
        for (size_t i = 0; i < rows; ++i) {
            result += data[i][i];
        }
        return result;
    }

    // Transpose (modifies the matrix)
    void transpose() {
        T** newData = new T * [cols];
        for (size_t i = 0; i < cols; ++i) {
            newData[i] = new T[rows];
            for (size_t j = 0; j < rows; ++j) {
                newData[i][j] = data[j][i];
            }
        }

        deallocate();
        data = newData;
        std::swap(rows, cols);
    }

    // Return a transposed copy
    Matrix getTranspose() const {
        Matrix result(cols, rows);
        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                result.data[j][i] = data[i][j];
            }
        }
        return result;
    }

    // Upper triangular matrix (Gauss with partial pivoting)
    Matrix getUpperTriangular() const {
        static_assert(std::is_arithmetic<T>::value, "Triangulation needs an arithmetic type.");
        if (!isSquare()) {
            throw std::logic_error("Non-square matrix cannot be triangularized.");
        }

        Matrix result(*this);  // Copy

        for (size_t k = 0; k < rows - 1; k++) {
            // Find pivot (largest element in absolute value)
            size_t pivotRow = k;
            T maxVal = std::abs(result.data[k][k]);

            for (size_t i = k + 1; i < rows; i++) {
                T val = std::abs(result.data[i][k]);
                if (val > maxVal) {
                    maxVal = val;
                    pivotRow = i;
                }
            }

            // Check if matrix is singular
            if (std::abs(result.data[pivotRow][k]) < 1e-10) {
                // Pivot too small, continue (singular matrix)
                continue;
            }

            // Swap rows if necessary
            if (pivotRow != k) {
                for (size_t j = 0; j < cols; j++) {
                    std::swap(result.data[k][j], result.data[pivotRow][j]);
                }
            }

            // Elimination: zero out all elements below pivot
            for (size_t i = k + 1; i < rows; i++) {
                T factor = result.data[i][k] / result.data[k][k];

                for (size_t j = k; j < cols; j++) {
                    result.data[i][j] -= factor * result.data[k][j];
                }
            }
        }

        return result;
    }

    // Lower triangular matrix
    Matrix getLowerTriangular() const {
        static_assert(std::is_arithmetic<T>::value, "Triangulation needs an arithmetic type.");
        if (!isSquare()) {
            throw std::logic_error("Non-square matrix cannot be triangularized.");
        }

        // L = (U^T)^T where U is upper triangular of A^T
        Matrix transposed = getTranspose();
        Matrix upperOfTransposed = transposed.getUpperTriangular();
        return upperOfTransposed.getTranspose();
    }

    // Determinant (via triangulation)
    T det() const {
        static_assert(std::is_arithmetic<T>::value, "Determinant needs an arithmetic type.");
        if (!isSquare()) {
            throw std::logic_error("Non-square matrix doesn't have a determinant.");
        }

        Matrix upper = getUpperTriangular();
        return upper.multiplyDiagonal();
    }

    // Matrix inverse (Gauss-Jordan method)
    Matrix getInverse() const {
        static_assert(std::is_arithmetic<T>::value, "Matrix inversion needs an arithmetic type.");
        if (!isSquare()) {
            throw std::logic_error("Only square matrices can be inverted.");
        }

        size_t n = rows;

        // Create augmented matrix [A | I]
        Matrix<T> augmented(n, 2 * n);

        // Copy A into left part
        for (size_t i = 0; i < n; i++) {
            for (size_t j = 0; j < n; j++) {
                augmented.data[i][j] = data[i][j];
            }
        }

        // Create I in right part
        for (size_t i = 0; i < n; i++) {
            augmented.data[i][n + i] = static_cast<T>(1);
        }

        // Apply Gauss-Jordan elimination
        for (size_t k = 0; k < n; k++) {
            // Find pivot (largest element in absolute value in column k)
            size_t pivotRow = k;
            T maxVal = std::abs(augmented.data[k][k]);

            for (size_t i = k + 1; i < n; i++) {
                T val = std::abs(augmented.data[i][k]);
                if (val > maxVal) {
                    maxVal = val;
                    pivotRow = i;
                }
            }

            // Check if matrix is singular (non-invertible)
            if (std::abs(augmented.data[pivotRow][k]) < 1e-10) {
                throw std::logic_error("Matrix is singular (non-invertible, determinant = 0).");
            }

            // Swap rows k and pivotRow
            if (pivotRow != k) {
                for (size_t j = 0; j < 2 * n; j++) {
                    std::swap(augmented.data[k][j], augmented.data[pivotRow][j]);
                }
            }

            // Normalize pivot row (divide by pivot)
            T pivot = augmented.data[k][k];
            for (size_t j = 0; j < 2 * n; j++) {
                augmented.data[k][j] /= pivot;
            }

            // Eliminate column k in all other rows
            for (size_t i = 0; i < n; i++) {
                if (i != k) {
                    T factor = augmented.data[i][k];
                    for (size_t j = 0; j < 2 * n; j++) {
                        augmented.data[i][j] -= factor * augmented.data[k][j];
                    }
                }
            }
        }

        // Extract inverse matrix (right part of augmented matrix)
        Matrix<T> inverse(n, n);
        for (size_t i = 0; i < n; i++) {
            for (size_t j = 0; j < n; j++) {
                inverse.data[i][j] = augmented.data[i][n + j];
            }
        }

        return inverse;
    }

    // Invert in place (modifies the matrix)
    void invert() {
        *this = getInverse();
    }

    // Check if matrix is invertible
    bool isInvertible() const {
        if (!isSquare()) return false;

        try {
            T determinant = det();
            return std::abs(determinant) > 1e-10;
        }
        catch (...) {
            return false;
        }
    }

    // ==================== EIGENVALUES ====================

    // Compute eigenvalues (iterative QR method for small matrices)
    std::vector<Eigenvalue<T>> getEigenvalues(size_t maxIterations = 100, T epsilon = 1e-10) const {
        static_assert(std::is_floating_point<T>::value,
            "Eigenvalue computation requires floating-point type (float, double, long double).");

        if (!isSquare()) {
            throw std::logic_error("Only square matrices have eigenvalues.");
        }

        size_t n = rows;

        // Special cases for small matrices
        if (n == 1) {
            return { Eigenvalue<T>(std::complex<T>(data[0][0], 0), 1) };
        }

        if (n == 2) {
            return getEigenvalues2x2();
        }

        // For n >= 3, use QR method
        return getEigenvaluesQR(maxIterations, epsilon);
    }

    // Special method for 2x2 matrices (analytical formula)
    std::vector<Eigenvalue<T>> getEigenvalues2x2() const {
        // For 2x2 matrix: [a b]
        //                 [c d]
        // Eigenvalues are solutions of: lambda^2 - (a+d)*lambda + (ad-bc) = 0

        T a = data[0][0];
        T b = data[0][1];
        T c = data[1][0];
        T d = data[1][1];

        T trace = a + d;
        T det = a * d - b * c;

        // Discriminant: Delta = trace^2 - 4*det
        T discriminant = trace * trace - 4 * det;

        std::vector<Eigenvalue<T>> eigenvalues;

        if (discriminant >= 0) {
            // Two real eigenvalues
            T sqrtDelta = std::sqrt(discriminant);
            T lambda1 = (trace + sqrtDelta) / 2;
            T lambda2 = (trace - sqrtDelta) / 2;

            if (std::abs(lambda1 - lambda2) < 1e-10) {
                // Double eigenvalue
                eigenvalues.push_back(Eigenvalue<T>(std::complex<T>(lambda1, 0), 2));
            }
            else {
                eigenvalues.push_back(Eigenvalue<T>(std::complex<T>(lambda1, 0), 1));
                eigenvalues.push_back(Eigenvalue<T>(std::complex<T>(lambda2, 0), 1));
            }
        }
        else {
            // Two complex conjugate eigenvalues
            T realPart = trace / 2;
            T imagPart = std::sqrt(-discriminant) / 2;

            eigenvalues.push_back(Eigenvalue<T>(std::complex<T>(realPart, imagPart), 1));
            eigenvalues.push_back(Eigenvalue<T>(std::complex<T>(realPart, -imagPart), 1));
        }

        return eigenvalues;
    }

    // QR method for nxn matrices
    std::vector<Eigenvalue<T>> getEigenvaluesQR(size_t maxIterations, T epsilon) const {
        size_t n = rows;
        Matrix<T> A(*this);  // Working copy

        // Iterative QR algorithm
        for (size_t iter = 0; iter < maxIterations; iter++) {
            // QR decomposition (simplified with Gram-Schmidt)
            auto [Q, R] = A.qrDecomposition();

            // A_next = R * Q
            A = R * Q;

            // Check convergence (if matrix is quasi-triangular)
            bool converged = true;
            for (size_t i = 1; i < n; i++) {
                for (size_t j = 0; j < i; j++) {
                    if (std::abs(A.data[i][j]) > epsilon) {
                        converged = false;
                        break;
                    }
                }
                if (!converged) break;
            }

            if (converged) break;
        }

        // Extract eigenvalues from diagonal (and 2x2 blocks)
        std::vector<Eigenvalue<T>> eigenvalues;

        for (size_t i = 0; i < n; ) {
            // Check if there is a 2x2 block (complex eigenvalues)
            if (i + 1 < n && std::abs(A.data[i + 1][i]) > epsilon) {
                // 2x2 block detected
                Matrix<T> block(2, 2);
                block.data[0][0] = A.data[i][i];
                block.data[0][1] = A.data[i][i + 1];
                block.data[1][0] = A.data[i + 1][i];
                block.data[1][1] = A.data[i + 1][i + 1];

                auto blockEigenvalues = block.getEigenvalues2x2();
                eigenvalues.insert(eigenvalues.end(), blockEigenvalues.begin(), blockEigenvalues.end());

                i += 2;
            }
            else {
                // Real eigenvalue on diagonal
                eigenvalues.push_back(Eigenvalue<T>(std::complex<T>(A.data[i][i], 0), 1));
                i++;
            }
        }

        // Group identical eigenvalues (multiplicity)
        return mergeEigenvalues(eigenvalues, epsilon);
    }

    // QR decomposition (modified Gram-Schmidt method)
    std::pair<Matrix<T>, Matrix<T>> qrDecomposition() const {
        size_t n = rows;
        Matrix<T> Q(n, n);
        Matrix<T> R(n, n);

        // Modified Gram-Schmidt
        for (size_t j = 0; j < n; j++) {
            // Copy column j of A into Q
            for (size_t i = 0; i < n; i++) {
                Q.data[i][j] = data[i][j];
            }

            // Orthogonalize with respect to previous columns
            for (size_t k = 0; k < j; k++) {
                R.data[k][j] = 0;
                for (size_t i = 0; i < n; i++) {
                    R.data[k][j] += Q.data[i][k] * data[i][j];
                }

                for (size_t i = 0; i < n; i++) {
                    Q.data[i][j] -= R.data[k][j] * Q.data[i][k];
                }
            }

            // Normalize
            R.data[j][j] = 0;
            for (size_t i = 0; i < n; i++) {
                R.data[j][j] += Q.data[i][j] * Q.data[i][j];
            }
            R.data[j][j] = std::sqrt(R.data[j][j]);

            if (R.data[j][j] > 1e-10) {
                for (size_t i = 0; i < n; i++) {
                    Q.data[i][j] /= R.data[j][j];
                }
            }
        }

        return { Q, R };
    }

    // Group identical eigenvalues
    std::vector<Eigenvalue<T>> mergeEigenvalues(const std::vector<Eigenvalue<T>>& eigenvalues, T epsilon) const {
        if (eigenvalues.empty()) return {};

        std::vector<Eigenvalue<T>> merged;

        for (const auto& ev : eigenvalues) {
            bool found = false;
            for (auto& m : merged) {
                // Compare taking tolerance into account
                if (std::abs(ev.value.real() - m.value.real()) < epsilon &&
                    std::abs(ev.value.imag() - m.value.imag()) < epsilon) {
                    m.algebraicMultiplicity += ev.algebraicMultiplicity;
                    found = true;
                    break;
                }
            }
            if (!found) {
                merged.push_back(ev);
            }
        }

        return merged;
    }

    // Compute largest eigenvalue (power method)
    Eigenvalue<T> getDominantEigenvalue(size_t maxIterations = 1000, T epsilon = 1e-10) const {
        static_assert(std::is_floating_point<T>::value,
            "Eigenvalue computation requires floating-point type.");

        if (!isSquare()) {
            throw std::logic_error("Only square matrices have eigenvalues.");
        }

        size_t n = rows;

        // Initial random vector (simplified: [1, 1, ..., 1])
        std::vector<T> v(n, 1.0);
        T eigenvalue = 0;

        for (size_t iter = 0; iter < maxIterations; iter++) {
            // Compute A * v
            std::vector<T> Av(n, 0);
            for (size_t i = 0; i < n; i++) {
                for (size_t j = 0; j < n; j++) {
                    Av[i] += data[i][j] * v[j];
                }
            }

            // Compute norm of Av
            T norm = 0;
            for (size_t i = 0; i < n; i++) {
                norm += Av[i] * Av[i];
            }
            norm = std::sqrt(norm);

            // Normalize
            for (size_t i = 0; i < n; i++) {
                Av[i] /= norm;
            }

            // Estimate eigenvalue (Rayleigh quotient)
            T newEigenvalue = 0;
            for (size_t i = 0; i < n; i++) {
                T sum = 0;
                for (size_t j = 0; j < n; j++) {
                    sum += data[i][j] * Av[j];
                }
                newEigenvalue += Av[i] * sum;
            }

            // Check convergence
            if (std::abs(newEigenvalue - eigenvalue) < epsilon) {
                return Eigenvalue<T>(std::complex<T>(newEigenvalue, 0), 1);
            }

            eigenvalue = newEigenvalue;
            v = Av;
        }

        return Eigenvalue<T>(std::complex<T>(eigenvalue, 0), 1);
    }

    // Fill matrix with a value
    void fill(const T& value) {
        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                data[i][j] = value;
            }
        }
    }

    // Create identity matrix
    static Matrix identity(size_t n) {
        static_assert(std::is_arithmetic<T>::value,
            "Identity matrix requires an arithmetic type.");
        if (n == 0) {
            throw std::logic_error("Identity matrix size must be greater than 0.");
        }
        Matrix result(n, n);
        for (size_t i = 0; i < n; i++) {
            result.data[i][i] = static_cast<T>(1);
        }
        return result;
    }

    // Swap two rows
    void swapRows(size_t row1, size_t row2) {
        if (row1 >= rows || row2 >= rows) {
            throw std::out_of_range("Row index out of bounds");
        }
        if (row1 != row2) {
            std::swap(data[row1], data[row2]);
        }
    }

    // Swap two columns
    void swapCols(size_t col1, size_t col2) {
        if (col1 >= cols || col2 >= cols) {
            throw std::out_of_range("Column index out of bounds");
        }
        if (col1 != col2) {
            for (size_t i = 0; i < rows; i++) {
                std::swap(data[i][col1], data[i][col2]);
            }
        }
    }

    // ==================== ARITHMETIC OPERATORS ====================

    // Matrix addition (mixed types supported)
    template<typename U>
    auto operator+(const Matrix<U>& other) const
        -> Matrix<decltype(T() + U())> {
        if (cols != other.cols || rows != other.rows) {
            throw std::logic_error("Both matrices should have the same size for addition");
        }

        using ResultType = decltype(T() + U());
        Matrix<ResultType> result(rows, cols);

        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                result.data[i][j] = data[i][j] + other.data[i][j];
            }
        }
        return result;
    }

    // Matrix subtraction (mixed types supported)
    template<typename U>
    auto operator-(const Matrix<U>& other) const
        -> Matrix<decltype(T() - U())> {
        if (cols != other.cols || rows != other.rows) {
            throw std::logic_error("Both matrices should have the same size for subtraction");
        }

        using ResultType = decltype(T() - U());
        Matrix<ResultType> result(rows, cols);

        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                result.data[i][j] = data[i][j] - other.data[i][j];
            }
        }
        return result;
    }

    // Matrix multiplication (mixed types supported)
    template<typename U>
    auto operator*(const Matrix<U>& other) const
        -> Matrix<decltype(T()* U())> {
        if (cols != other.rows) {
            throw std::logic_error("Matrix dimensions are incompatible for multiplication (cols != other.rows)");
        }

        using ResultType = decltype(T()* U());
        Matrix<ResultType> result(rows, other.cols);

        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < other.cols; j++) {
                ResultType sum = static_cast<ResultType>(0);
                for (size_t k = 0; k < cols; k++) {
                    sum += data[i][k] * other.data[k][j];
                }
                result.data[i][j] = sum;
            }
        }
        return result;
    }

    // Scalar multiplication (matrix * scalar) - mixed types
    template<typename U>
    auto operator*(const U& scalar) const
        -> Matrix<decltype(T()* U())> {
        using ResultType = decltype(T()* U());
        Matrix<ResultType> result(rows, cols);

        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                result.data[i][j] = data[i][j] * scalar;
            }
        }
        return result;
    }

    // Scalar division - mixed types
    template<typename U>
    auto operator/(const U& scalar) const
        -> Matrix<decltype(T() / U())> {
        if (scalar == static_cast<U>(0)) {
            throw std::logic_error("Division by zero");
        }

        using ResultType = decltype(T() / U());
        Matrix<ResultType> result(rows, cols);

        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                result.data[i][j] = data[i][j] / scalar;
            }
        }
        return result;
    }

    // Compound operators
    Matrix& operator+=(const Matrix& other) {
        if (cols != other.cols || rows != other.rows) {
            throw std::logic_error("Both matrices should have the same size for addition");
        }
        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                data[i][j] += other.data[i][j];
            }
        }
        return *this;
    }

    Matrix& operator-=(const Matrix& other) {
        if (cols != other.cols || rows != other.rows) {
            throw std::logic_error("Both matrices should have the same size for subtraction");
        }
        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                data[i][j] -= other.data[i][j];
            }
        }
        return *this;
    }

    Matrix& operator*=(const T& scalar) {
        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                data[i][j] *= scalar;
            }
        }
        return *this;
    }

    Matrix& operator/=(const T& scalar) {
        if (scalar == static_cast<T>(0)) {
            throw std::logic_error("Division by zero");
        }
        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                data[i][j] /= scalar;
            }
        }
        return *this;
    }

    // Unary negation
    Matrix operator-() const {
        Matrix result(rows, cols);
        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                result.data[i][j] = -data[i][j];
            }
        }
        return result;
    }
};

// ==================== EXTERNAL OPERATORS (FRIEND) ====================

// Scalar multiplication on the left (scalar * matrix)
template<typename T, typename U>
auto operator*(const U& scalar, const Matrix<T>& mat)
-> Matrix<decltype(U()* T())> {
    return mat * scalar;
}