#ifndef BACKEND_CODEGEN_H
#define BACKEND_CODEGEN_H

#include <string>
#include <vector>
#include <memory>
#include "../frontend/parser.h"
#include "../memorymap/memorymap.h"
#include "../pim_isa/instructions.h"

namespace Backend {

/**
 * @brief Class for generating pPIM instructions from parsed matrix operations
 */
class CodeGenerator {
public:
    /**
     * @brief Constructor
     * 
     * @param memoryMapper Memory mapper instance for matrix data mapping
     */
    CodeGenerator(std::shared_ptr<MemoryMap::MemoryMapper> memoryMapper);
    
    /**
     * @brief Generate instructions from parsed matrices and operations
     * 
     * @param matrices Parsed matrix information
     * @param operations Parsed matrix operations
     * @return Vector of generated instructions
     */
    std::vector<PIM_ISA::Instruction> generateInstructions(
        const std::vector<Frontend::MatrixInfo>& matrices,
        const std::vector<Frontend::MatrixOperation>& operations);
    
    /**
     * @brief Write generated instructions to an output file
     * 
     * @param instructions Generated instructions
     * @param outputFile Path to the output file
     * @return true if writing was successful
     */
    bool writeToFile(const std::vector<PIM_ISA::Instruction>& instructions, const std::string& outputFile);
    
    /**
     * @brief Set verbose mode
     * 
     * @param verbose Whether to enable verbose output
     */
    void setVerbose(bool verbose);
    
private:
    // Memory mapper
    std::shared_ptr<MemoryMap::MemoryMapper> memoryMapper_;
    
    // Verbosity flag
    bool verbose_{false};
    
    /**
     * @brief Generate instructions for matrix multiplication
     * 
     * @param op Matrix multiplication operation
     * @param instructions Vector to append generated instructions
     */
    void generateMatrixMultiplyInstructions(
        const Frontend::MatrixOperation& op,
        std::vector<PIM_ISA::Instruction>& instructions);
    
    /**
     * @brief Generate initialization instructions for LUT cores
     * 
     * @param instructions Vector to append generated instructions
     */
    void generateInitInstructions(std::vector<PIM_ISA::Instruction>& instructions);
    
    /**
     * @brief Generate LUT configuration for multiplier core
     * 
     * @return Configuration data
     */
    std::vector<uint8_t> generateMultiplierConfig() const;
    
    /**
     * @brief Generate LUT configuration for adder core
     * 
     * @return Configuration data
     */
    std::vector<uint8_t> generateAdderConfig() const;
    
    /**
     * @brief Generate LUT configuration for MAC core
     * 
     * @return Configuration data
     */
    std::vector<uint8_t> generateMACConfig() const;
};

} // namespace Backend

#endif // BACKEND_CODEGEN_H

