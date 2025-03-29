#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>
#include <cmath>

// Number of matrix dimensions to test
constexpr int NUM_TESTS = 6;

// Matrix dimensions to simulate
const int dimensions[NUM_TESTS][3] = {
    {3, 2, 4},    // Small (3×2 * 2×4)
    {8, 6, 10},   // Medium (8×6 * 6×10)
    {32, 32, 32}, // Large (32×32 * 32×32)
    {64, 64, 64}, // Larger (64×64 * 64×64)
    {100, 100, 100}, // Very large (100×100 * 100×100)
    {256, 256, 256}  // Huge (256×256 * 256×256)
};

// pPIM architecture parameters
constexpr int PROG_CYCLES = 10;      // Cycles for PROG instruction
constexpr int READ_CYCLES = 2;       // Cycles for memory read
constexpr int WRITE_CYCLES = 2;      // Cycles for memory write
constexpr int COMPUTE_CYCLES = 1;    // Cycles for core computation
constexpr int CLOCK_RATE_MHZ = 500;  // Assumed pPIM clock rate in MHz

// CPU matrix multiplication for benchmarking
void multiply_cpu(int* A, int* B, int* C, int n, int m, int p) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < p; j++) {
            int sum = 0;
            for (int k = 0; k < m; k++) {
                sum += A[i * m + k] * B[k * p + j];
            }
            C[i * p + j] = sum;
        }
    }
}

// Estimate CPU execution time for matrix multiplication
double estimate_cpu_time(int n, int m, int p) {
    // Allocate matrices
    std::vector<int> A(n * m, 1);
    std::vector<int> B(m * p, 1);
    std::vector<int> C(n * p, 0);
    
    // Measure execution time
    auto start = std::chrono::high_resolution_clock::now();
    multiply_cpu(A.data(), B.data(), C.data(), n, m, p);
    auto end = std::chrono::high_resolution_clock::now();
    
    // Calculate time in microseconds
    std::chrono::duration<double, std::micro> duration = end - start;
    return duration.count();
}

// Estimate pPIM execution time based on instruction analysis
double estimate_pim_time(int n, int m, int p) {
    // Calculate number of instructions
    int num_prog_instr = 3;  // One each for MULTIPLIER, ADDER, MAC
    int num_read_instr = n * p * m * 2;  // Two reads per multiplication
    int num_compute_instr = n * p * m;  // One compute per multiplication
    int num_write_instr = n * p;  // One write per result element
    int num_end_instr = 1;
    
    // Calculate total cycles
    int total_cycles = 
        (num_prog_instr * PROG_CYCLES) +
        (num_read_instr * READ_CYCLES) +
        (num_compute_instr * COMPUTE_CYCLES) +
        (num_write_instr * WRITE_CYCLES) +
        num_end_instr;
    
    // Calculate time in microseconds (sequential)
    double seq_time = static_cast<double>(total_cycles) / CLOCK_RATE_MHZ;
    
    // Estimate parallel execution (assume 30% reduction from bank parallelism)
    double parallel_time = seq_time * 0.7;
    
    return parallel_time;
}

// Estimate pPIM execution time with optimization (merged operations)
double estimate_optimized_pim_time(int n, int m, int p) {
    // With optimization, we assume we can reduce some redundant reads
    // and better schedule operations across banks
    
    // Calculate number of instructions with optimization
    int num_prog_instr = 3;  // One each for MULTIPLIER, ADDER, MAC
    int num_read_instr = n * p * m * 1.6;  // Reduced reads with caching
    int num_compute_instr = n * p * m;  // Same compute ops
    int num_write_instr = n * p;  // Same writes
    int num_end_instr = 1;
    
    // Calculate total cycles
    int total_cycles = 
        (num_prog_instr * PROG_CYCLES) +
        (num_read_instr * READ_CYCLES) +
        (num_compute_instr * COMPUTE_CYCLES) +
        (num_write_instr * WRITE_CYCLES) +
        num_end_instr;
    
    // Calculate time in microseconds with more parallelism (50% reduction)
    double parallel_time = static_cast<double>(total_cycles) / CLOCK_RATE_MHZ * 0.5;
    
    return parallel_time;
}

int main() {
    // Print header
    std::cout << "====== Matrix Multiplication Performance Comparison ======" << std::endl;
    std::cout << std::setw(15) << "Matrix Size" << std::setw(15) << "CPU Time (μs)" 
              << std::setw(15) << "pPIM Time (μs)" << std::setw(15) << "Optimized PIM" 
              << std::setw(10) << "Speedup" << std::endl;
    std::cout << std::string(70, '-') << std::endl;
    
    // Run all tests
    for (int i = 0; i < NUM_TESTS; i++) {
        int n = dimensions[i][0];
        int m = dimensions[i][1];
        int p = dimensions[i][2];
        
        // Get matrix size as string
        std::string matrix_size = std::to_string(n) + "×" + std::to_string(m) + 
                                 " * " + std::to_string(m) + "×" + std::to_string(p);
        
        // Measure/estimate times
        double cpu_time = estimate_cpu_time(n, m, p);
        double pim_time = estimate_pim_time(n, m, p);
        double opt_pim_time = estimate_optimized_pim_time(n, m, p);
        
        // Calculate speedup
        double speedup = cpu_time / opt_pim_time;
        
        // Print results
        std::cout << std::setw(15) << matrix_size 
                  << std::setw(15) << std::fixed << std::setprecision(2) << cpu_time
                  << std::setw(15) << std::fixed << std::setprecision(2) << pim_time
                  << std::setw(15) << std::fixed << std::setprecision(2) << opt_pim_time
                  << std::setw(10) << std::fixed << std::setprecision(2) << speedup << "×"
                  << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Notes:" << std::endl;
    std::cout << "- CPU Time: Actual measured execution time on modern CPU (compiled with -O2)" << std::endl;
    std::cout << "- pPIM Time: Estimated execution time for pPIM architecture" << std::endl;
    std::cout << "- Optimized PIM: Estimated time with additional optimizations" << std::endl;
    std::cout << "- Speedup: Ratio of CPU time to Optimized pPIM time" << std::endl;
    
    return 0;
} 