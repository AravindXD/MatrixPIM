#include "../../include/pim_isa/instructions.h"
#include <sstream>
#include <iomanip>
#include <stdexcept>

namespace PIM_ISA {

// Constructor for EXE and END instructions
Instruction::Instruction(InstructionType type, uint8_t ptr, bool rd, bool wr, uint16_t address)
    : type(type), read(rd), write(wr), rowAddress(address) {
    
    if (type == InstructionType::EXE) {
        readPtr = ptr; // Use readPtr or corePtr based on the instruction type
    } else {
        // For END instructions, we ignore the ptr value
        readPtr = 0;
    }
}

// Constructor for PROG instructions
Instruction::Instruction(uint8_t ptr, CoreOpType opType, const std::vector<uint8_t>& config)
    : type(InstructionType::PROG), corePtr(ptr), read(false), write(false), 
      rowAddress(0), coreOpType(opType), lutConfig(config) {
}

// Convert instruction to string representation
std::string Instruction::toString() const {
    std::stringstream ss;
    
    switch (type) {
        case InstructionType::PROG:
            ss << "PROG Core" << static_cast<int>(corePtr) << " ";
            
            switch (coreOpType) {
                case CoreOpType::MULTIPLIER:  ss << "MULTIPLIER"; break;
                case CoreOpType::ADDER:       ss << "ADDER"; break;
                case CoreOpType::MAC:         ss << "MAC"; break;
                case CoreOpType::SHIFTER:     ss << "SHIFTER"; break;
                case CoreOpType::LOGIC_AND:   ss << "LOGIC_AND"; break;
                case CoreOpType::LOGIC_OR:    ss << "LOGIC_OR"; break;
                case CoreOpType::LOGIC_XOR:   ss << "LOGIC_XOR"; break;
                case CoreOpType::COMPARATOR:  ss << "COMPARATOR"; break;
                case CoreOpType::CUSTOM:      ss << "CUSTOM"; break;
                default:                      ss << "UNKNOWN"; break;
            }
            
            // Add LUT configuration data
            if (!lutConfig.empty()) {
                ss << " [";
                for (size_t i = 0; i < lutConfig.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << "0x" << std::hex << std::setw(2) << std::setfill('0') 
                       << static_cast<int>(lutConfig[i]);
                }
                ss << "]";
            }
            break;
            
        case InstructionType::EXE:
            ss << "EXE ";
            if (read && !write) {
                ss << "Read";
            } else if (!read && write) {
                ss << "Write";
            } else if (read && write) {
                ss << "ReadWrite";
            } else {
                ss << "CorePtr" << static_cast<int>(corePtr);
            }
            
            ss << " RowAddress" << static_cast<int>(rowAddress);
            break;
            
        case InstructionType::END:
            ss << "END";
            break;
            
        default:
            ss << "UNKNOWN";
            break;
    }
    
    return ss.str();
}

// Convert instruction to binary representation
uint32_t Instruction::toBinary() const {
    uint32_t binary = 0;
    
    // Bits 17-18: Opcode
    binary |= (static_cast<uint32_t>(type) & 0x3) << 17;
    
    // Bits 11-16: Read/Core Ptr
    binary |= (static_cast<uint32_t>(corePtr) & 0x3F) << 11;
    
    // Bit 10: Read flag
    binary |= (read ? 1UL : 0UL) << 10;
    
    // Bit 9: Write flag
    binary |= (write ? 1UL : 0UL) << 9;
    
    // Bits 0-8: Row Address
    binary |= (rowAddress & 0x1FF);
    
    return binary;
}

// Create a PROG instruction
Instruction createProgInstruction(uint8_t corePtr, CoreOpType opType, const std::vector<uint8_t>& lutConfig) {
    if (corePtr > 63) { // 6-bit field
        throw std::out_of_range("Core pointer out of range (must be 0-63)");
    }
    
    return Instruction(corePtr, opType, lutConfig);
}

// Create an EXE instruction for memory operation
Instruction createMemoryInstruction(uint8_t readPtr, bool read, bool write, uint16_t rowAddress) {
    if (readPtr > 63) { // 6-bit field
        throw std::out_of_range("Read pointer out of range (must be 0-63)");
    }
    
    if (rowAddress > 511) { // 9-bit field
        throw std::out_of_range("Row address out of range (must be 0-511)");
    }
    
    return Instruction(InstructionType::EXE, readPtr, read, write, rowAddress);
}

// Create an EXE instruction for computation
Instruction createComputeInstruction(uint8_t corePtr, uint16_t rowAddress) {
    if (corePtr > 63) { // 6-bit field
        throw std::out_of_range("Core pointer out of range (must be 0-63)");
    }
    
    if (rowAddress > 511) { // 9-bit field
        throw std::out_of_range("Row address out of range (must be 0-511)");
    }
    
    // For computation instructions, both read and write are false
    return Instruction(InstructionType::EXE, corePtr, false, false, rowAddress);
}

// Create an END instruction
Instruction createEndInstruction() {
    return Instruction(InstructionType::END, 0, false, false, 0);
}

} // namespace PIM_ISA
