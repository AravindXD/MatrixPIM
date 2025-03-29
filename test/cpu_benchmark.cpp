#include <iostream>
#include <chrono>
#include <vector>

// Matrix class for CPU implementation
class Matrix {
private:
    int rows_;
    int cols_;
    std::vector<int> data_;

public:
    Matrix(int rows, int cols) : rows_(rows), cols_(cols), data_(rows * cols, 0) {}
    
    int& operator()(int row, int col) {
        return data_[row * cols_ + col];
    }
    
    const int& operator()(int row, int col) const {
        return data_[row * cols_ + col];
    }
    
    int rows() const { return rows_; }
    int cols() const { return cols_; }
};

// Traditional CPU matrix multiplication
Matrix multiply_cpu(const Matrix& A, const Matrix& B) {
    if (A.cols() != B.rows()) {
        throw std::invalid_argument("Matrix dimensions don't match for multiplication");
    }
    
    Matrix C(A.rows(), B.cols());
    
    for (int i = 0; i < A.rows(); ++i) {
        for (int j = 0; j < B.cols(); ++j) {
            int sum = 0;
            for (int k = 0; k < A.cols(); ++k) {
                sum += A(i, k) * B(k, j);
            }
            C(i, j) = sum;
        }
    }
    
    return C;
}

// Function to run the benchmark
void run_benchmark(int a_rows, int a_cols, int b_cols) {
    // Create matrices
    Matrix A(a_rows, a_cols);
    Matrix B(a_cols, b_cols);
    
    // Initialize with some values
    for (int i = 0; i < a_rows; ++i) {
        for (int j = 0; j < a_cols; ++j) {
            A(i, j) = i + j + 1;
        }
    }
    
    for (int i = 0; i < a_cols; ++i) {
        for (int j = 0; j < b_cols; ++j) {
            B(i, j) = i * 2 + j + 1;
        }
    }
    
    // Measure execution time
    auto start = std::chrono::high_resolution_clock::now();
    
    // Perform matrix multiplication
    Matrix C = multiply_cpu(A, B);
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> duration = end - start;
    
    // Print results
    std::cout << "Matrix multiplication " << a_rows << "x" << a_cols << " * " 
              << a_cols << "x" << b_cols << std::endl;
    std::cout << "CPU Execution time: " << duration.count() << " microseconds" << std::endl;
    
    // Calculate estimated pPIM time based on 3-4x improvement
    double pim_time_min = duration.count() / 4.0;
    double pim_time_max = duration.count() / 3.0;
    std::cout << "Estimated pPIM time: " << pim_time_min << " - " << pim_time_max 
              << " microseconds (based on 3-4x speedup)" << std::endl;
    
    // Print first few values to verify correctness
    std::cout << "Sample output values (C):" << std::endl;
    for (int i = 0; i < std::min(3, A.rows()); ++i) {
        for (int j = 0; j < std::min(3, B.cols()); ++j) {
            std::cout << C(i, j) << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

int main() {
    std::cout << "=== Matrix Multiplication Benchmark ===" << std::endl;
    
    // Small matrix case (similar to real_output.asm)
    run_benchmark(3, 2, 4);
    
    // Medium matrix case (similar to complex_output.asm)
    run_benchmark(8, 6, 10);
    
    // Larger matrix case 
    run_benchmark(32, 32, 32);
    
    // Very large matrix case
    run_benchmark(100, 100, 100);
    
    return 0;
} 