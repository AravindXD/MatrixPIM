#include "../include/compiler.h"
#include "../include/frontend/parser.h"
#include "../include/optimizer/optimizer.h"
#include "../include/backend/codegen.h"
#include "../include/memorymap/memorymap.h"
#include <iostream>

// Constructor
PIMCompiler::PIMCompiler() {
    // Initialize components
    memoryMapper_ = std::make_unique<MemoryMap::MemoryMapper>();
    parser_ = std::make_unique<Frontend::Parser>();
    optimizer_ = std::make_unique<Optimizer::Optimizer>();
    codeGenerator_ = std::make_unique<Backend::CodeGenerator>(
        std::shared_ptr<MemoryMap::MemoryMapper>(memoryMapper_.get(), [](MemoryMap::MemoryMapper*){})
    );
}

// Destructor
PIMCompiler::~PIMCompiler() {
    // Reset pointers in reverse order of creation to avoid dangling references
    codeGenerator_.reset();
    optimizer_.reset();
    parser_.reset();
    memoryMapper_.reset();
}

// Compile a C++ file into pPIM instructions
bool PIMCompiler::compile(const std::string& inputFile, const std::string& outputFile) {
    // Reset the memory mapper
    memoryMapper_->reset();
    
    // Set verbosity on all components
    optimizer_->setVerbose(verbose_);
    codeGenerator_->setVerbose(verbose_);
    
    // Set optimization level
    optimizer_->setOptimizationLevel(optimizationLevel_);
    
    if (verbose_) {
        std::cout << "Compiling " << inputFile << " to " << outputFile << std::endl;
        std::cout << "Optimization level: " << optimizationLevel_ << std::endl;
    }
    
    // Parse the input file
    if (!parser_->parseFile(inputFile)) {
        std::cerr << "Error: Failed to parse input file " << inputFile << std::endl;
        return false;
    }
    
    // Get matrices and operations from the parser
    std::vector<Frontend::MatrixInfo> matrices = parser_->getMatrices();
    std::vector<Frontend::MatrixOperation> operations = parser_->getOperations();
    
    if (verbose_) {
        std::cout << "Parsed " << matrices.size() << " matrices and " 
                 << operations.size() << " operations" << std::endl;
    }
    
    // Apply optimizations to the operations
    operations = optimizer_->optimizeOperations(operations);
    
    // Generate instructions
    instructions_ = codeGenerator_->generateInstructions(matrices, operations);
    
    // Apply instruction-level optimizations
    instructions_ = optimizer_->optimizeInstructions(instructions_);
    
    // Write the output file
    if (!codeGenerator_->writeToFile(instructions_, outputFile)) {
        std::cerr << "Error: Failed to write output file " << outputFile << std::endl;
        return false;
    }
    
    if (verbose_) {
        std::cout << "Successfully compiled to " << outputFile << std::endl;
        std::cout << "Generated " << instructions_.size() << " instructions" << std::endl;
    }
    
    return true;
}

// Set optimization level
void PIMCompiler::setOptimizationLevel(int level) {
    optimizationLevel_ = level;
}

// Set verbose mode
void PIMCompiler::setVerbose(bool verbose) {
    verbose_ = verbose;
}

// Get generated instructions
std::vector<PIM_ISA::Instruction> PIMCompiler::getInstructions() const {
    return instructions_;
}

