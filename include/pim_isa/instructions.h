#ifndef PIM_ISA_INSTRUCTIONS_H
#define PIM_ISA_INSTRUCTIONS_H

#include <cstdint>
#include <string>
#include <vector>

namespace PIM_ISA {

/**
 * @brief Instruction types in the pPIM architecture
 */
enum class InstructionType : uint8_t {
    PROG, // Program the LUT cores with new functionality
    EXE,  // Execute operation within the cluster
    END   // Terminate operation
};

/**
 * @brief Core operation types that can be programmed into the LUTs
 */
enum class CoreOpType : uint8_t {
    MULTIPLIER,  // Multiplier function
    ADDER,       // Adder function
    MAC,         // Multiply-and-accumulate
    SHIFTER,     // Bit shifter
    LOGIC_AND,   // Logical AND
    LOGIC_OR,    // Logical OR
    LOGIC_XOR,   // Logical XOR
    COMPARATOR,  // Comparator
    CUSTOM       // Custom function (requires specific LUT configuration)
};

/**
 * @brief pPIM Instruction format
 * 
 * 18-17    16-11         10 9      8-0
 * Opcode   Read/Core Ptr. Rd Wr    Row Address
 */
struct Instruction {
    InstructionType type;          // 2-bit opcode (bits 17-18)
    union {
        uint8_t readPtr;           // 6-bit read pointer (bits 11-16) for memory access
        uint8_t corePtr;           // 6-bit core pointer (bits 11-16) for computation
    };
    bool read;                     // 1-bit read flag (bit 10)
    bool write;                    // 1-bit write flag (bit 9)
    uint16_t rowAddress;           // 9-bit row address (bits 0-8)
    
    // For PROG instructions only
    CoreOpType coreOpType;         // Type of operation to program
    std::vector<uint8_t> lutConfig; // LUT configuration data (for PROG instructions)
    
    /**
     * @brief Constructor for EXE and END instructions
     */
    Instruction(InstructionType type, uint8_t ptr, bool rd, bool wr, uint16_t address);
    
    /**
     * @brief Constructor for PROG instructions
     */
    Instruction(uint8_t ptr, CoreOpType opType, const std::vector<uint8_t>& config);
    
    /**
     * @brief Default constructor
     */
    Instruction() = default;
    
    /**
     * @brief Convert instruction to string representation
     */
    std::string toString() const;
    
    /**
     * @brief Convert instruction to binary representation
     */
    uint32_t toBinary() const;
};

/**
 * @brief Create a PROG instruction to program a specific LUT core
 */
Instruction createProgInstruction(uint8_t corePtr, CoreOpType opType, const std::vector<uint8_t>& lutConfig);

/**
 * @brief Create an EXE instruction for memory operation
 */
Instruction createMemoryInstruction(uint8_t readPtr, bool read, bool write, uint16_t rowAddress);

/**
 * @brief Create an EXE instruction for computation
 */
Instruction createComputeInstruction(uint8_t corePtr, uint16_t rowAddress);

/**
 * @brief Create an END instruction
 */
Instruction createEndInstruction();

} // namespace PIM_ISA

#endif // PIM_ISA_INSTRUCTIONS_H

