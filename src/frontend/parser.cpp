#include "../../include/frontend/parser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>
#include <stdexcept>

namespace Frontend {

// Constructor
Parser::Parser() {
}

// Parse source file
bool Parser::parseFile(const std::string& sourceFile) {
    // Clear previous data
    matrices_.clear();
    operations_.clear();
    
    // Open the file
    std::ifstream file(sourceFile);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << sourceFile << std::endl;
        return false;
    }
    
    // Read file contents
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string sourceCode = buffer.str();
    
    try {
        // Parse matrix declarations
        parseMatrixDeclarations(sourceCode);
        
        // Parse matrix operations
        parseMatrixOperations(sourceCode);
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing file: " << e.what() << std::endl;
        return false;
    }
}

// Get all matrices found in the source code
std::vector<MatrixInfo> Parser::getMatrices() const {
    std::vector<MatrixInfo> result;
    for (const auto& pair : matrices_) {
        result.push_back(pair.second);
    }
    return result;
}

// Get all operations found in the source code
std::vector<MatrixOperation> Parser::getOperations() const {
    return operations_;
}

// Get number of operations
size_t Parser::getOperationCount() const {
    return operations_.size();
}

// Get matrix info by name
MatrixInfo Parser::getMatrixInfo(const std::string& name) const {
    auto it = matrices_.find(name);
    if (it == matrices_.end()) {
        throw std::runtime_error("Matrix '" + name + "' not found");
    }
    return it->second;
}

// Check if matrix exists
bool Parser::hasMatrix(const std::string& name) const {
    return matrices_.find(name) != matrices_.end();
}

// Parse matrix declarations
void Parser::parseMatrixDeclarations(const std::string& sourceCode) {
    // Regex for matrix declarations like: Matrix A(3, 4); or Matrix<int> A(3, 4);
    std::regex matrixDeclRegex(R"(Matrix(?:<\w+>)?\s+(\w+)\s*\(\s*(\d+)\s*,\s*(\d+)\s*\))");
    
    // Start matching from the beginning of the source code
    auto begin = std::sregex_iterator(sourceCode.begin(), sourceCode.end(), matrixDeclRegex);
    auto end = std::sregex_iterator();
    
    for (std::sregex_iterator i = begin; i != end; ++i) {
        std::smatch match = *i;
        if (match.size() == 4) {
            std::string name = match[1].str();
            uint32_t rows = std::stoi(match[2].str());
            uint32_t cols = std::stoi(match[3].str());
            
            // For simplicity, we assume matrices are inputs by default unless they're used as outputs
            // We'll mark them as outputs when parsing operations
            matrices_.insert(std::make_pair(name, MatrixInfo(name, rows, cols, true, false)));
        }
    }
}

// Parse matrix operations
void Parser::parseMatrixOperations(const std::string& sourceCode) {
    // Regex for matrix multiplications like: C = A * B;
    std::regex multiplyRegex(R"((\w+)\s*=\s*(\w+)\s*\*\s*(\w+)\s*;)");
    
    // Start matching from the beginning of the source code
    auto begin = std::sregex_iterator(sourceCode.begin(), sourceCode.end(), multiplyRegex);
    auto end = std::sregex_iterator();
    
    for (std::sregex_iterator i = begin; i != end; ++i) {
        std::smatch match = *i;
        if (match.size() == 4) {
            std::string outputName = match[1].str();
            std::string inputName1 = match[2].str();
            std::string inputName2 = match[3].str();
            
            // Verify that input matrices exist
            if (!hasMatrix(inputName1)) {
                throw std::runtime_error("Matrix '" + inputName1 + "' used in operation but not declared");
            }
            
            if (!hasMatrix(inputName2)) {
                throw std::runtime_error("Matrix '" + inputName2 + "' used in operation but not declared");
            }
            
            // Create or update output matrix
            if (!hasMatrix(outputName)) {
                // If output matrix doesn't exist, we need to create it
                // For matrix multiplication, dimensions are: (A.rows, B.cols)
                const MatrixInfo& inputMatrix1 = getMatrixInfo(inputName1);
                const MatrixInfo& inputMatrix2 = getMatrixInfo(inputName2);
                
                // Validate dimensions for matrix multiplication
                if (inputMatrix1.cols != inputMatrix2.rows) {
                    throw std::runtime_error("Invalid matrix dimensions for multiplication: " + 
                                            inputName1 + "(" + std::to_string(inputMatrix1.rows) + "x" + std::to_string(inputMatrix1.cols) + ") * " +
                                            inputName2 + "(" + std::to_string(inputMatrix2.rows) + "x" + std::to_string(inputMatrix2.cols) + ")");
                }
                
                // Create output matrix with correct dimensions
                matrices_.insert(std::make_pair(outputName, MatrixInfo(outputName, inputMatrix1.rows, inputMatrix2.cols, false, true)));
            }
            
            // Mark output matrix as an output
            auto it = matrices_.find(outputName);
            if (it != matrices_.end()) {
                it->second.isOutput = true;
            }
            
            // Add operation
            std::vector<std::string> inputs = {inputName1, inputName2};
            operations_.push_back(MatrixOperation(OperationType::MULTIPLY, inputs, outputName));
        }
    }
    
    // For completeness, we could add more regex patterns for other operations (ADD, SUBTRACT, etc.)
}

// Extract matrix dimensions from declaration
std::tuple<uint32_t, uint32_t> Parser::extractMatrixDimensions(const std::string& declaration) {
    // Regex to extract dimensions from declaration like: Matrix A(3, 4);
    std::regex dimensionsRegex(R"(\(\s*(\d+)\s*,\s*(\d+)\s*\))");
    std::smatch match;
    
    if (std::regex_search(declaration, match, dimensionsRegex) && match.size() == 3) {
        uint32_t rows = std::stoi(match[1].str());
        uint32_t cols = std::stoi(match[2].str());
        return std::make_tuple(rows, cols);
    }
    
    throw std::runtime_error("Could not extract matrix dimensions from: " + declaration);
}

} // namespace Frontend

