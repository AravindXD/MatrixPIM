#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <vector>
#include <memory>
#include "../frontend/parser.h"
#include "../pim_isa/instructions.h"

namespace Optimizer {

/**
 * @brief Optimizer class for optimizing matrix operations
 */
class Optimizer {
public:
    /**
     * @brief Constructor
     */
    Optimizer();
    
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
     * @brief Optimize matrix operations
     * 
     * @param operations Matrix operations to optimize
     * @return Optimized matrix operations
     */
    std::vector<Frontend::MatrixOperation> optimizeOperations(
        const std::vector<Frontend::MatrixOperation>& operations);
    
    /**
     * @brief Optimize pPIM instructions
     * 
     * @param instructions pPIM instructions to optimize
     * @return Optimized pPIM instructions
     */
    std::vector<PIM_ISA::Instruction> optimizeInstructions(
        const std::vector<PIM_ISA::Instruction>& instructions);
    
private:
    // Optimization level
    int optimizationLevel_{0};
    
    // Verbosity flag
    bool verbose_{false};
    
    /**
     * @brief Apply loop unrolling optimization
     * 
     * @param instructions pPIM instructions to optimize
     * @return Optimized pPIM instructions
     */
    std::vector<PIM_ISA::Instruction> applyLoopUnrolling(
        const std::vector<PIM_ISA::Instruction>& instructions);
    
    /**
     * @brief Apply instruction reordering optimization
     * 
     * @param instructions pPIM instructions to optimize
     * @return Optimized pPIM instructions
     */
    std::vector<PIM_ISA::Instruction> applyInstructionReordering(
        const std::vector<PIM_ISA::Instruction>& instructions);
    
    /**
     * @brief Apply memory access optimization
     * 
     * @param instructions pPIM instructions to optimize
     * @return Optimized pPIM instructions
     */
    std::vector<PIM_ISA::Instruction> applyMemoryAccessOptimization(
        const std::vector<PIM_ISA::Instruction>& instructions);
    
    /**
     * @brief Apply instruction scheduling optimization
     * 
     * @param instructions pPIM instructions to optimize
     * @return Optimized pPIM instructions
     */
    std::vector<PIM_ISA::Instruction> applyInstructionScheduling(
        const std::vector<PIM_ISA::Instruction>& instructions);
};

} // namespace Optimizer

#endif // OPTIMIZER_H

