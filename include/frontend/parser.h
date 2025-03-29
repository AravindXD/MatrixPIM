#ifndef FRONTEND_PARSER_H
#define FRONTEND_PARSER_H

#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <memory>

namespace Frontend {

/**
 * @brief Represents a matrix in the source code
 */
struct MatrixInfo {
    std::string name;        // Matrix name
    uint32_t rows;           // Number of rows
    uint32_t cols;           // Number of columns
    bool isInput;            // Whether this is an input matrix
    bool isOutput;           // Whether this is an output matrix
    
    // Default constructor for containers
    MatrixInfo() : rows(0), cols(0), isInput(false), isOutput(false) {}
    
    MatrixInfo(const std::string& n, uint32_t r, uint32_t c, bool in, bool out)
        : name(n), rows(r), cols(c), isInput(in), isOutput(out) {}
};

/**
 * @brief Represents a matrix operation in the source code
 */
enum class OperationType {
    MULTIPLY,
    ADD,
    SUBTRACT,
    TRANSPOSE
};

/**
 * @brief Represents a matrix operation
 */
struct MatrixOperation {
    OperationType type;                   // Operation type
    std::vector<std::string> inputs;      // Input matrix names
    std::string output;                   // Output matrix name
    
    MatrixOperation(OperationType t, const std::vector<std::string>& in, const std::string& out)
        : type(t), inputs(in), output(out) {}
};

/**
 * @brief Class for parsing C++ matrix multiplication code
 */
class Parser {
public:
    /**
     * @brief Constructor
     */
    Parser();
    
    /**
     * @brief Parse source file
     * 
     * @param sourceFile Path to the source file
     * @return true if parsing was successful
     */
    bool parseFile(const std::string& sourceFile);
    
    /**
     * @brief Get all matrices found in the source code
     * 
     * @return Vector of matrix information
     */
    std::vector<MatrixInfo> getMatrices() const;
    
    /**
     * @brief Get all operations found in the source code
     * 
     * @return Vector of matrix operations
     */
    std::vector<MatrixOperation> getOperations() const;
    
    /**
     * @brief Get number of operations
     * 
     * @return Number of operations
     */
    size_t getOperationCount() const;
    
    /**
     * @brief Get matrix info by name
     * 
     * @param name Matrix name
     * @return Matrix information
     */
    MatrixInfo getMatrixInfo(const std::string& name) const;
    
    /**
     * @brief Check if matrix exists
     * 
     * @param name Matrix name
     * @return true if matrix exists
     */
    bool hasMatrix(const std::string& name) const;
    
private:
    // Parsed matrices
    std::map<std::string, MatrixInfo> matrices_;
    
    // Parsed operations
    std::vector<MatrixOperation> operations_;
    
    /**
     * @brief Parse matrix declarations
     * 
     * @param sourceCode Source code content
     */
    void parseMatrixDeclarations(const std::string& sourceCode);
    
    /**
     * @brief Parse matrix operations
     * 
     * @param sourceCode Source code content
     */
    void parseMatrixOperations(const std::string& sourceCode);
    
    /**
     * @brief Extract matrix dimensions from declaration
     * 
     * @param declaration Matrix declaration string
     * @return Tuple of rows and columns
     */
    std::tuple<uint32_t, uint32_t> extractMatrixDimensions(const std::string& declaration);
};

} // namespace Frontend

#endif // FRONTEND_PARSER_H

