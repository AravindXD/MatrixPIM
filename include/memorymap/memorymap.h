#ifndef MEMORY_MAPPER_H
#define MEMORY_MAPPER_H

#include <cstdint>
#include <vector>
#include <tuple>
#include <map>
#include <string>

namespace MemoryMap {

/**
 * @brief Matrix dimensions
 */
struct MatrixDimensions {
    uint32_t rows;
    uint32_t cols;
    
    // Default constructor
    MatrixDimensions() : rows(0), cols(0) {}
    
    MatrixDimensions(uint32_t r, uint32_t c) : rows(r), cols(c) {}
};

/**
 * @brief Memory address range
 */
struct AddressRange {
    uint16_t startAddress;
    uint16_t endAddress;
    
    AddressRange(uint16_t start, uint16_t end) : startAddress(start), endAddress(end) {}
};

/**
 * @brief Class to handle mapping of matrix data to DRAM subarrays
 */
class MemoryMapper {
public:
    /**
     * @brief Constructor
     */
    MemoryMapper();
    
    /**
     * @brief Map a matrix to memory
     * 
     * @param matrixName Name of the matrix
     * @param dimensions Matrix dimensions
     * @return Row address of the matrix start
     */
    uint16_t mapMatrix(const std::string& matrixName, const MatrixDimensions& dimensions);
    
    /**
     * @brief Get mapped address for a matrix element
     * 
     * @param matrixName Name of the matrix
     * @param row Row index
     * @param col Column index
     * @return Row address of the matrix element
     */
    uint16_t getElementAddress(const std::string& matrixName, uint32_t row, uint32_t col) const;
    
    /**
     * @brief Get mapped address for a matrix row
     * 
     * @param matrixName Name of the matrix
     * @param row Row index
     * @return Row address of the matrix row
     */
    uint16_t getRowAddress(const std::string& matrixName, uint32_t row) const;
    
    /**
     * @brief Get bank index for a matrix element
     * 
     * @param matrixName Name of the matrix
     * @param row Row index
     * @param col Column index
     * @return Bank index
     */
    uint8_t getBankIndex(const std::string& matrixName, uint32_t row, uint32_t col) const;
    
    /**
     * @brief Check if a matrix is mapped
     * 
     * @param matrixName Name of the matrix
     * @return true if the matrix is mapped, false otherwise
     */
    bool isMatrixMapped(const std::string& matrixName) const;
    
    /**
     * @brief Get matrix dimensions
     * 
     * @param matrixName Name of the matrix
     * @return Matrix dimensions
     */
    MatrixDimensions getMatrixDimensions(const std::string& matrixName) const;
    
    /**
     * @brief Get address range for a matrix
     * 
     * @param matrixName Name of the matrix
     * @return Address range of the matrix
     */
    AddressRange getMatrixAddressRange(const std::string& matrixName) const;
    
    /**
     * @brief Reset memory map
     */
    void reset();
    
private:
    // Memory layout map (matrix name -> tuple of <start address, dimensions>)
    std::map<std::string, std::tuple<uint16_t, MatrixDimensions>> matrixMap_;
    
    // Memory allocation counter (next available row address)
    uint16_t nextRowAddress_{0};
    
    /**
     * @brief Calculate required memory size for a matrix
     * 
     * @param dimensions Matrix dimensions
     * @return Memory size in rows
     */
    uint16_t calculateMatrixSize(const MatrixDimensions& dimensions) const;
};

} // namespace MemoryMap

#endif // MEMORY_MAPPER_H

