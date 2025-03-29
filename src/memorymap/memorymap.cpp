#include "../../include/memorymap/memorymap.h"
#include <stdexcept>
#include <algorithm>
#include <cmath>

namespace MemoryMap {

// Constructor
MemoryMapper::MemoryMapper() 
    : nextRowAddress_(0) {
}

// Map a matrix to memory
uint16_t MemoryMapper::mapMatrix(const std::string& matrixName, const MatrixDimensions& dimensions) {
    // Check if matrix is already mapped
    if (isMatrixMapped(matrixName)) {
        throw std::runtime_error("Matrix '" + matrixName + "' is already mapped");
    }
    
    // Calculate matrix size in memory rows
    uint16_t matrixSize = calculateMatrixSize(dimensions);
    
    // Check if we have enough space
    if (nextRowAddress_ + matrixSize > 512) { // 9-bit address space
        throw std::runtime_error("Not enough memory space to map matrix '" + matrixName + "'");
    }
    
    // Assign start address
    uint16_t startAddress = nextRowAddress_;
    
    // Update memory map
    matrixMap_[matrixName] = std::tuple<uint16_t, MatrixDimensions>(startAddress, dimensions);
    
    // Update next available row address
    nextRowAddress_ += matrixSize;
    
    return startAddress;
}

// Get mapped address for a matrix element
uint16_t MemoryMapper::getElementAddress(const std::string& matrixName, uint32_t row, uint32_t col) const {
    // Check if matrix is mapped
    if (!isMatrixMapped(matrixName)) {
        throw std::runtime_error("Matrix '" + matrixName + "' is not mapped");
    }
    
    // Get matrix information
    const auto& matrixInfo = matrixMap_.at(matrixName);
    uint16_t startAddress = std::get<0>(matrixInfo);
    const MatrixDimensions& dimensions = std::get<1>(matrixInfo);
    
    // Check if indices are valid
    if (row >= dimensions.rows || col >= dimensions.cols) {
        throw std::out_of_range("Matrix indices out of range");
    }
    
    // Calculate element address (row-major layout)
    // For pPIM architecture, we optimize for efficient MAC operations by ensuring
    // adjacent elements are in the same bank where possible
    
    // Calculate row offset (each row may span multiple memory rows)
    uint16_t rowOffset = (row * dimensions.cols) / 256; // 256 elements per memory row
    
    // Calculate element offset within memory row (not used in current implementation)
    // uint16_t elementOffset = (row * dimensions.cols + col) % 256;
    
    // Combine to get actual address
    uint16_t elementAddress = startAddress + rowOffset;
    
    return elementAddress;
}

// Get mapped address for a matrix row
uint16_t MemoryMapper::getRowAddress(const std::string& matrixName, uint32_t row) const {
    // Check if matrix is mapped
    if (!isMatrixMapped(matrixName)) {
        throw std::runtime_error("Matrix '" + matrixName + "' is not mapped");
    }
    
    // Get matrix information
    const auto& matrixInfo = matrixMap_.at(matrixName);
    uint16_t startAddress = std::get<0>(matrixInfo);
    const MatrixDimensions& dimensions = std::get<1>(matrixInfo);
    
    // Check if row index is valid
    if (row >= dimensions.rows) {
        throw std::out_of_range("Matrix row index out of range");
    }
    
    // Calculate row address
    uint16_t rowsPerMemoryRow = std::min(256 / dimensions.cols, 1U); // At least 1 element per row
    uint16_t rowOffset = row / rowsPerMemoryRow;
    
    return startAddress + rowOffset;
}

// Get bank index for a matrix element
uint8_t MemoryMapper::getBankIndex(const std::string& matrixName, uint32_t row, uint32_t col) const {
    // Check if matrix is mapped
    if (!isMatrixMapped(matrixName)) {
        throw std::runtime_error("Matrix '" + matrixName + "' is not mapped");
    }
    
    // Get matrix information
    const auto& matrixInfo = matrixMap_.at(matrixName);
    const MatrixDimensions& dimensions = std::get<1>(matrixInfo);
    
    // Check if indices are valid
    if (row >= dimensions.rows || col >= dimensions.cols) {
        throw std::out_of_range("Matrix indices out of range");
    }
    
    // Calculate bank index (assuming 4 banks per DRAM array)
    // For pPIM architecture, we want to distribute matrix elements across banks
    // to enable parallel processing
    
    // Simple distribution strategy - alternate banks based on row and column
    uint8_t bankIndex = (row + col) % 4;
    
    return bankIndex;
}

// Check if a matrix is mapped
bool MemoryMapper::isMatrixMapped(const std::string& matrixName) const {
    return matrixMap_.find(matrixName) != matrixMap_.end();
}

// Get matrix dimensions
MatrixDimensions MemoryMapper::getMatrixDimensions(const std::string& matrixName) const {
    // Check if matrix is mapped
    if (!isMatrixMapped(matrixName)) {
        throw std::runtime_error("Matrix '" + matrixName + "' is not mapped");
    }
    
    return std::get<1>(matrixMap_.at(matrixName));
}

// Get address range for a matrix
AddressRange MemoryMapper::getMatrixAddressRange(const std::string& matrixName) const {
    // Check if matrix is mapped
    if (!isMatrixMapped(matrixName)) {
        throw std::runtime_error("Matrix '" + matrixName + "' is not mapped");
    }
    
    // Get matrix information
    const auto& matrixInfo = matrixMap_.at(matrixName);
    uint16_t startAddress = std::get<0>(matrixInfo);
    const MatrixDimensions& dimensions = std::get<1>(matrixInfo);
    
    // Calculate matrix size in memory rows
    uint16_t matrixSize = calculateMatrixSize(dimensions);
    
    // Calculate end address
    uint16_t endAddress = startAddress + matrixSize - 1;
    
    return AddressRange(startAddress, endAddress);
}

// Reset memory map
void MemoryMapper::reset() {
    matrixMap_.clear();
    nextRowAddress_ = 0;
}

// Calculate required memory size for a matrix
uint16_t MemoryMapper::calculateMatrixSize(const MatrixDimensions& dimensions) const {
    // Calculate number of elements in the matrix
    uint32_t numElements = dimensions.rows * dimensions.cols;
    
    // Calculate number of memory rows needed (each row can hold up to 256 elements)
    uint16_t numRows = static_cast<uint16_t>(std::ceil(static_cast<double>(numElements) / 256.0));
    
    // Ensure at least one row
    return std::max(numRows, static_cast<uint16_t>(1));
}

} // namespace MemoryMap
