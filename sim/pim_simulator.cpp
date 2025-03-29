#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <chrono>
#include <algorithm>

// pPIM architecture constants
constexpr int PROG_CYCLES = 10;      // Cycles for PROG instruction
constexpr int READ_CYCLES = 2;       // Cycles for memory read
constexpr int WRITE_CYCLES = 2;      // Cycles for memory write
constexpr int COMPUTE_CYCLES = 1;    // Cycles for core computation
constexpr int CLOCK_RATE_MHZ = 500;  // Assumed pPIM clock rate in MHz

// Instruction types
enum class InstructionType {
    PROG,
    EXE_READ,
    EXE_WRITE,
    EXE_COMPUTE,
    END,
    UNKNOWN
};

// Instruction representation
struct Instruction {
    InstructionType type;
    std::string params;
    int cycles;
};

// Parse an assembly file into instructions
std::vector<Instruction> parseAssembly(const std::string& filename) {
    std::vector<Instruction> instructions;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return instructions;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '/' || line[0] == ' ')
            continue;
        
        Instruction instr;
        instr.params = line;
        
        if (line.find("PROG") == 0) {
            instr.type = InstructionType::PROG;
            instr.cycles = PROG_CYCLES;
        } else if (line.find("EXE Read") == 0) {
            instr.type = InstructionType::EXE_READ;
            instr.cycles = READ_CYCLES;
        } else if (line.find("EXE Write") == 0) {
            instr.type = InstructionType::EXE_WRITE;
            instr.cycles = WRITE_CYCLES;
        } else if (line.find("EXE CorePtr") == 0) {
            instr.type = InstructionType::EXE_COMPUTE;
            instr.cycles = COMPUTE_CYCLES;
        } else if (line.find("END") == 0) {
            instr.type = InstructionType::END;
            instr.cycles = 1;
        } else {
            instr.type = InstructionType::UNKNOWN;
            instr.cycles = 0;
        }
        
        instructions.push_back(instr);
    }
    
    return instructions;
}

// Simulate execution of the pPIM assembly
void simulateExecution(const std::vector<Instruction>& instructions, const std::string& filename) {
    // Count instructions by type
    int progCount = 0;
    int readCount = 0;
    int writeCount = 0;
    int computeCount = 0;
    
    for (const auto& instr : instructions) {
        switch (instr.type) {
            case InstructionType::PROG: progCount++; break;
            case InstructionType::EXE_READ: readCount++; break;
            case InstructionType::EXE_WRITE: writeCount++; break;
            case InstructionType::EXE_COMPUTE: computeCount++; break;
            default: break;
        }
    }
    
    // Calculate total cycles for sequential execution
    int totalCycles = 0;
    for (const auto& instr : instructions) {
        totalCycles += instr.cycles;
    }
    
    // Calculate execution time in microseconds
    double executionTimeUs = static_cast<double>(totalCycles) / CLOCK_RATE_MHZ;
    
    // Estimate parallel execution time (assuming 20% cycle reduction due to bank parallelism)
    double parallelTimeUs = executionTimeUs * 0.8;
    
    // Output results
    std::cout << "=== pPIM Simulation for " << filename << " ===" << std::endl;
    std::cout << "Total instructions: " << instructions.size() << std::endl;
    std::cout << "  PROG instructions: " << progCount << std::endl;
    std::cout << "  READ instructions: " << readCount << std::endl;
    std::cout << "  WRITE instructions: " << writeCount << std::endl;
    std::cout << "  COMPUTE instructions: " << computeCount << std::endl;
    std::cout << std::endl;
    
    std::cout << "Total cycles: " << totalCycles << std::endl;
    std::cout << "Sequential execution time: " << executionTimeUs << " microseconds" << std::endl;
    std::cout << "Parallel execution time: " << parallelTimeUs << " microseconds" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    std::vector<std::string> filesToProcess;
    
    if (argc > 1) {
        // Process files specified on command line
        for (int i = 1; i < argc; ++i) {
            filesToProcess.push_back(argv[i]);
        }
    } else {
        // Default files if none specified
        filesToProcess.push_back("real_output.asm");
        filesToProcess.push_back("complex_output.asm");
    }
    
    // Process each assembly file
    for (const auto& file : filesToProcess) {
        auto instructions = parseAssembly(file);
        if (!instructions.empty()) {
            simulateExecution(instructions, file);
        }
    }
    
    return 0;
} 