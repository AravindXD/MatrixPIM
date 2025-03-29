#ifndef PIM_COMPILER_H
#define PIM_COMPILER_H

#include <string>
#include <vector>
#include <memory>

// Forward declarations
namespace Frontend {
    class Parser;
}

namespace Optimizer {
    class Optimizer;
}

namespace Backend {
    class CodeGenerator;
}

namespace MemoryMap {
    class MemoryMapper;
}

namespace PIM_ISA {
    struct Instruction;
}

/**
 * @brief Main compiler class that orchestrates the entire compilation process
 * 
 * The PIMCompiler class handles the complete process of translating C++ matrix multiplication
 * code into pPIM instructions. It coordinates between the frontend parser, optimizer,
 * memory mapper, and backend code generator.
 */
class PIMCompiler {
public:
    /**
     * @brief Constructor
     */
    PIMCompiler();
    
    /**
     * @brief Destructor
     */
    ~PIMCompiler();
    
    /**
     * @brief Compile a C++ file into pPIM instructions
     * 
     * @param inputFile Path to the input C++ file
     * @param outputFile Path to the output assembly file
     * @return true if compilation was successful, false otherwise
     */
    bool compile(const std::string& inputFile, const std::string& outputFile);
    
    /**
     * @brief Set optimization level
     * 
     * @param level Optimization level (0-3)
     */
    void setOptimizationLevel(int level);
    
    /**
     * @brief Set verbose mode
     * 
     * @param verbose Whether to enable verbose output
     */
    void setVerbose(bool verbose);
    
    /**
     * @brief Get generated instructions
     * 
     * @return Vector of pPIM instructions
     */
    std::vector<PIM_ISA::Instruction> getInstructions() const;
    
private:
    // Components
    std::unique_ptr<Frontend::Parser> parser_;
    std::unique_ptr<Optimizer::Optimizer> optimizer_;
    std::unique_ptr<Backend::CodeGenerator> codeGenerator_;
    std::unique_ptr<MemoryMap::MemoryMapper> memoryMapper_;
    
    // Compilation parameters
    int optimizationLevel_{0};
    bool verbose_{false};
    
    // Generated instructions
    std::vector<PIM_ISA::Instruction> instructions_;
};

#endif // PIM_COMPILER_H

