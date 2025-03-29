#include <iostream>

// Simple matrix class for demonstration
template<typename T>
class Matrix {
private:
    int rows_;
    int cols_;
    T* data_;

public:
    // Constructor
    Matrix(int rows, int cols) : rows_(rows), cols_(cols) {
        data_ = new T[rows * cols]();
    }
    
    // Destructor
    ~Matrix() {
        delete[] data_;
    }
    
    // Copy constructor
    Matrix(const Matrix& other) : rows_(other.rows_), cols_(other.cols_) {
        data_ = new T[rows_ * cols_];
        for (int i = 0; i < rows_ * cols_; ++i) {
            data_[i] = other.data_[i];
        }
    }
    
    // Get rows
    int rows() const {
        return rows_;
    }
    
    // Get columns
    int cols() const {
        return cols_;
    }
    
    // Get element
    T& operator()(int row, int col) {
        return data_[row * cols_ + col];
    }
    
    // Get element (const)
    const T& operator()(int row, int col) const {
        return data_[row * cols_ + col];
    }
    
    // Matrix multiplication
    Matrix operator*(const Matrix& other) const {
        if (cols_ != other.rows_) {
            throw std::invalid_argument("Matrix dimensions don't match for multiplication");
        }
        
        Matrix result(rows_, other.cols_);
        
        for (int i = 0; i < rows_; ++i) {
            for (int j = 0; j < other.cols_; ++j) {
                T sum = 0;
                for (int k = 0; k < cols_; ++k) {
                    sum += (*this)(i, k) * other(k, j);
                }
                result(i, j) = sum;
            }
        }
        
        return result;
    }
    
    // Print matrix
    void print() const {
        for (int i = 0; i < rows_; ++i) {
            for (int j = 0; j < cols_; ++j) {
                std::cout << (*this)(i, j) << " ";
            }
            std::cout << std::endl;
        }
    }
};

int main() {
    // Create matrices A and B
    Matrix<int> A(3, 2);
    Matrix<int> B(2, 4);
    
    // Initialize matrix A
    A(0, 0) = 1; A(0, 1) = 2;
    A(1, 0) = 3; A(1, 1) = 4;
    A(2, 0) = 5; A(2, 1) = 6;
    
    // Initialize matrix B
    B(0, 0) = 7; B(0, 1) = 8; B(0, 2) = 9; B(0, 3) = 10;
    B(1, 0) = 11; B(1, 1) = 12; B(1, 2) = 13; B(1, 3) = 14;
    
    // Print matrices
    std::cout << "Matrix A:" << std::endl;
    A.print();
    
    std::cout << "Matrix B:" << std::endl;
    B.print();
    
    // Multiply matrices
    Matrix<int> C = A * B;
    
    // Print result
    std::cout << "Matrix C = A * B:" << std::endl;
    C.print();
    
    return 0;
}

