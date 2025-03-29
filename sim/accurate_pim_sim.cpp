#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>
#include <cmath>
#include <algorithm>

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

// Traditional architecture parameters
constexpr int CPU_CLOCK_MHZ = 3200;           // CPU clock in MHz
constexpr int CPU_CACHE_LINE_SIZE = 64;       // Cache line size in bytes
constexpr int CPU_L1_LATENCY_CYCLES = 4;      // L1 cache latency in cycles
constexpr int CPU_L2_LATENCY_CYCLES = 12;     // L2 cache latency in cycles
constexpr int CPU_L3_LATENCY_CYCLES = 40;     // L3 cache latency in cycles
constexpr int CPU_MEMORY_LATENCY_CYCLES = 200; // Main memory latency in cycles
constexpr int CPU_L1_SIZE_KB = 32;            // L1 cache size in KB
constexpr int CPU_L2_SIZE_KB = 256;           // L2 cache size in KB
constexpr int CPU_L3_SIZE_KB = 8192;          // L3 cache size in KB
constexpr int CPU_MEMORY_BW_GB_PER_SEC = 25;  // Memory bandwidth in GB/s

// pPIM architecture parameters
constexpr int PIM_CLOCK_MHZ = 1000;           // PIM clock in MHz (now 1 GHz)
constexpr int PIM_BANKS = 16;                 // Number of parallel banks
constexpr int PIM_CORES_PER_BANK = 32;        // Processing elements per bank
constexpr int PIM_READ_CYCLES = 2;            // Cycles for memory read
constexpr int PIM_WRITE_CYCLES = 2;           // Cycles for memory write
constexpr int PIM_COMPUTE_CYCLES = 1;         // Cycles for core computation
constexpr int PIM_PROG_CYCLES = 10;           // Cycles for PROG instruction
constexpr int PIM_MEMORY_BW_GB_PER_SEC = 100; // Internal memory bandwidth in GB/s

// CPU matrix multiplication
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

// Model CPU cache behavior for matrix multiplication
double model_cpu_time(int n, int m, int p) {
    // Matrix sizes in bytes
    const int a_size = n * m * sizeof(int);
    const int b_size = m * p * sizeof(int);
    const int c_size = n * p * sizeof(int);
    
    // Determine if matrices fit in different cache levels
    bool fit_in_l1 = (a_size + b_size + c_size) <= CPU_L1_SIZE_KB * 1024;
    bool fit_in_l2 = (a_size + b_size + c_size) <= CPU_L2_SIZE_KB * 1024;
    bool fit_in_l3 = (a_size + b_size + c_size) <= CPU_L3_SIZE_KB * 1024;
    
    // Total operations and memory accesses
    const long long operations = static_cast<long long>(n) * m * p * 2; // Multiply + Add per element
    
    // Memory accesses 
    // A: Each element of A is accessed p times
    // B: Each element of B is accessed n times
    // C: Each element of C is written once
    const long long a_accesses = static_cast<long long>(n) * m * p;
    const long long b_accesses = static_cast<long long>(m) * p * n;
    const long long c_accesses = static_cast<long long>(n) * p;
    
    // Estimate memory access latency based on cache level
    double avg_access_cycles;
    if (fit_in_l1) {
        avg_access_cycles = CPU_L1_LATENCY_CYCLES;
    } else if (fit_in_l2) {
        // Some hits in L1, some in L2
        avg_access_cycles = 0.7 * CPU_L1_LATENCY_CYCLES + 0.3 * CPU_L2_LATENCY_CYCLES;
    } else if (fit_in_l3) {
        // Mix of L1, L2, and L3 hits
        avg_access_cycles = 0.5 * CPU_L1_LATENCY_CYCLES + 0.3 * CPU_L2_LATENCY_CYCLES + 0.2 * CPU_L3_LATENCY_CYCLES;
    } else {
        // Mix of cache and memory accesses
        avg_access_cycles = 0.4 * CPU_L1_LATENCY_CYCLES + 0.3 * CPU_L2_LATENCY_CYCLES + 
                          0.2 * CPU_L3_LATENCY_CYCLES + 0.1 * CPU_MEMORY_LATENCY_CYCLES;
    }
    
    // For large matrices, account for bandwidth limitations
    if (!fit_in_l3) {
        // Total memory transfers in bytes
        long long bytes_transferred = (a_accesses + b_accesses + c_accesses) * sizeof(int);
        
        // Bandwidth constrained time in seconds
        double bandwidth_time = static_cast<double>(bytes_transferred) / (CPU_MEMORY_BW_GB_PER_SEC * 1e9);
        
        // Compute-bound time in seconds
        double compute_time = static_cast<double>(operations) / CPU_CLOCK_MHZ / 1e6;
        
        // Memory access time in seconds
        double memory_time = static_cast<double>(a_accesses + b_accesses + c_accesses) * 
                           avg_access_cycles / CPU_CLOCK_MHZ / 1e6;
        
        // Total time is max of bandwidth-constrained and compute-bound time
        return std::max({bandwidth_time, compute_time, memory_time}) * 1e6; // Convert to microseconds
    } else {
        // For cache-resident matrices, simply measure execution time
        std::vector<int> A(n * m, 1);
        std::vector<int> B(m * p, 1);
        std::vector<int> C(n * p, 0);
        
        auto start = std::chrono::high_resolution_clock::now();
        multiply_cpu(A.data(), B.data(), C.data(), n, m, p);
        auto end = std::chrono::high_resolution_clock::now();
        
        std::chrono::duration<double, std::micro> duration = end - start;
        return duration.count();
    }
}

// Model pPIM execution time with realistic parameters
double model_pim_time(int n, int m, int p) {
    // Calculate total operations
    const long long mul_ops = static_cast<long long>(n) * p * m;
    const long long add_ops = static_cast<long long>(n) * p * (m - 1);
    const long long total_ops = mul_ops + add_ops;
    
    // Calculate memory accesses
    const long long read_accesses = static_cast<long long>(n) * m + static_cast<long long>(m) * p;
    const long long write_accesses = static_cast<long long>(n) * p;
    
    // Instruction counts
    const int num_prog_instr = 3;  // MULTIPLIER, ADDER, MAC
    const int num_end_instr = 1;
    
    // Calculate computation cycles
    long long compute_cycles = total_ops / (PIM_BANKS * PIM_CORES_PER_BANK);
    compute_cycles = std::max(compute_cycles, 1LL); // At least 1 cycle
    
    // Calculate memory access cycles
    long long memory_cycles = (read_accesses * PIM_READ_CYCLES + write_accesses * PIM_WRITE_CYCLES) 
                           / PIM_BANKS;
    memory_cycles = std::max(memory_cycles, 1LL); // At least 1 cycle
    
    // Setup cycles (programming cores)
    int setup_cycles = num_prog_instr * PIM_PROG_CYCLES + num_end_instr;
    
    // Total cycles
    long long total_cycles = setup_cycles + std::max(compute_cycles, memory_cycles);
    
    // Calculate execution time in microseconds
    double execution_time = static_cast<double>(total_cycles) / PIM_CLOCK_MHZ;
    
    return execution_time;
}

int main() {
    // Print header
    std::cout << "====== Matrix Multiplication Real Performance Comparison ======" << std::endl;
    std::cout << "                          CPU                 |                pPIM              " << std::endl;
    std::cout << std::setw(15) << "Matrix Size" 
              << std::setw(15) << "Actual (μs)" 
              << std::setw(15) << "Model (μs)" 
              << std::setw(5) << "|" 
              << std::setw(15) << "Model (μs)" 
              << std::setw(15) << "Speedup" 
              << std::endl;
    std::cout << std::string(70, '-') << std::endl;
    
    // Run all tests
    for (int i = 0; i < NUM_TESTS; i++) {
        int n = dimensions[i][0];
        int m = dimensions[i][1];
        int p = dimensions[i][2];
        
        // Get matrix size as string
        std::string matrix_size = std::to_string(n) + "×" + std::to_string(m) + 
                                 " * " + std::to_string(m) + "×" + std::to_string(p);
        
        // Measure actual CPU time
        std::vector<int> A(n * m, 1);
        std::vector<int> B(m * p, 1);
        std::vector<int> C(n * p, 0);
        
        auto start = std::chrono::high_resolution_clock::now();
        multiply_cpu(A.data(), B.data(), C.data(), n, m, p);
        auto end = std::chrono::high_resolution_clock::now();
        
        std::chrono::duration<double, std::micro> actual_cpu_time = end - start;
        
        // Model CPU and PIM times
        double model_cpu = model_cpu_time(n, m, p);
        double model_pim = model_pim_time(n, m, p);
        
        // Calculate speedup
        double speedup = model_cpu / model_pim;
        
        // Print results
        std::cout << std::setw(15) << matrix_size 
                  << std::setw(15) << std::fixed << std::setprecision(2) << actual_cpu_time.count()
                  << std::setw(15) << std::fixed << std::setprecision(2) << model_cpu
                  << std::setw(5) << "|" 
                  << std::setw(15) << std::fixed << std::setprecision(2) << model_pim
                  << std::setw(15) << std::fixed << std::setprecision(2) << speedup << "×"
                  << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Notes:" << std::endl;
    std::cout << "- 'Actual': Measured execution time on CPU" << std::endl;
    std::cout << "- 'Model': Performance model prediction considering:" << std::endl;
    std::cout << "  * CPU: 3.2 GHz, 32KB L1, 256KB L2, 8MB L3, 25 GB/s memory bandwidth" << std::endl;
    std::cout << "  * pPIM: 1 GHz, 16 banks, 32 cores/bank, 2-cycle memory access, 100 GB/s internal bandwidth" << std::endl;
    
    return 0;
} 