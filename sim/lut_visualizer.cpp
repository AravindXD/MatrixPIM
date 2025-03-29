#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <map>
#include <filesystem>

// Forward declarations
void generateLUTVisualization(const struct PIMArchitecture& pim, int instructionNumber, const std::string& instruction);
void createIndexFile();
void updateIndexFile(int instructionNumber);

// LUT Core representation
struct LUTCore {
    uint8_t id;
    std::string type;
    std::vector<uint8_t> data;
    bool programmed = false;
};

// Bank representation
struct Bank {
    uint8_t id;
    std::vector<LUTCore> cores;
};

// PIM Architecture representation
struct PIMArchitecture {
    std::vector<Bank> banks;
    
    // Add a new bank
    void addBank(uint8_t id) {
        Bank bank;
        bank.id = id;
        banks.push_back(bank);
    }
    
    // Add a core to a specific bank
    void addCore(uint8_t bankId, uint8_t coreId) {
        for (auto& bank : banks) {
            if (bank.id == bankId) {
                LUTCore core;
                core.id = coreId;
                bank.cores.push_back(core);
                return;
            }
        }
        // If bank not found, create it
        Bank bank;
        bank.id = bankId;
        LUTCore core;
        core.id = coreId;
        bank.cores.push_back(core);
        banks.push_back(bank);
    }
    
    // Program a core
    bool programCore(uint8_t bankId, uint8_t coreId, const std::string& type, const std::vector<uint8_t>& data) {
        for (auto& bank : banks) {
            if (bank.id == bankId) {
                for (auto& core : bank.cores) {
                    if (core.id == coreId) {
                        core.type = type;
                        core.data = data;
                        core.programmed = true;
                        return true;
                    }
                }
                // Core not found, create it
                LUTCore core;
                core.id = coreId;
                core.type = type;
                core.data = data;
                core.programmed = true;
                bank.cores.push_back(core);
                return true;
            }
        }
        // Bank not found, create it
        Bank bank;
        bank.id = bankId;
        LUTCore core;
        core.id = coreId;
        core.type = type;
        core.data = data;
        core.programmed = true;
        bank.cores.push_back(core);
        banks.push_back(bank);
        return true;
    }
};

// Function to parse assembly and visualize LUT programming
void visualizeLUTProgramming(const std::string& asmFilePath) {
    std::ifstream asmFile(asmFilePath);
    if (!asmFile.is_open()) {
        std::cerr << "Error: Could not open assembly file: " << asmFilePath << std::endl;
        return;
    }
    
    // Create output directory
    std::filesystem::create_directory("lut_visualization");
    
    // Initialize PIM architecture (16 banks with 32 cores each)
    PIMArchitecture pim;
    for (int i = 0; i < 16; i++) {
        pim.addBank(i);
        for (int j = 0; j < 32; j++) {
            pim.addCore(i, j);
        }
    }
    
    // Read assembly line by line
    std::string line;
    int lineNumber = 0;
    int progInstructionCount = 0; // Separate counter for PROG instructions
    
    while (std::getline(asmFile, line)) {
        lineNumber++;
        
        // Skip comments and empty lines
        if (line.empty() || line[0] == '/' || line[0] == '\n') {
            continue;
        }
        
        // Parse PROG instructions
        if (line.find("PROG Core") != std::string::npos) {
            // Increment the PROG instruction counter
            progInstructionCount++;
            
            // Example: PROG Core0 MULTIPLIER [0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07]
            uint8_t coreId = 0;
            std::string coreType;
            std::vector<uint8_t> lutData;
            
            // Extract core ID
            size_t coreIdPos = line.find("Core") + 4;
            size_t coreIdEnd = line.find(" ", coreIdPos);
            if (coreIdPos != std::string::npos && coreIdEnd != std::string::npos) {
                coreId = std::stoi(line.substr(coreIdPos, coreIdEnd - coreIdPos));
            }
            
            // Extract core type (operation)
            size_t typePos = coreIdEnd + 1;
            size_t typeEnd = line.find(" ", typePos);
            if (typePos != std::string::npos && typeEnd != std::string::npos) {
                coreType = line.substr(typePos, typeEnd - typePos);
            }
            
            // Extract LUT data
            size_t dataStart = line.find("[");
            size_t dataEnd = line.find("]");
            if (dataStart != std::string::npos && dataEnd != std::string::npos) {
                std::string dataStr = line.substr(dataStart + 1, dataEnd - dataStart - 1);
                std::stringstream ss(dataStr);
                std::string hex;
                
                while (std::getline(ss, hex, ',')) {
                    // Remove whitespace and "0x" prefix
                    hex.erase(0, hex.find_first_not_of(" \t"));
                    hex.erase(hex.find_last_not_of(" \t") + 1);
                    if (hex.substr(0, 2) == "0x") {
                        hex = hex.substr(2);
                    }
                    
                    // Convert hex to uint8_t
                    lutData.push_back(static_cast<uint8_t>(std::stoul(hex, nullptr, 16)));
                }
            }
            
            // Program the core in the architecture (using bank 0 for simplicity)
            pim.programCore(0, coreId, coreType, lutData);
            
            // Generate visualization for this PROG instruction using the PROG counter
            generateLUTVisualization(pim, progInstructionCount, line);
            
            std::cout << "Generated visualization for PROG instruction " << progInstructionCount 
                      << " (line " << lineNumber << "): " << line << std::endl;
        }
    }
    
    std::cout << "LUT programming visualization complete!" << std::endl;
    std::cout << "Generated " << progInstructionCount << " visualization steps." << std::endl;
    std::cout << "Output saved to: lut_visualization/" << std::endl;
}

// Function to generate HTML visualization for current LUT state
void generateLUTVisualization(const PIMArchitecture& pim, int instructionNumber, const std::string& instruction) {
    std::stringstream filename;
    filename << "lut_visualization/lut_state_" << std::setw(3) << std::setfill('0') << instructionNumber << ".html";
    
    std::ofstream htmlFile(filename.str());
    if (!htmlFile.is_open()) {
        std::cerr << "Error: Could not create visualization file: " << filename.str() << std::endl;
        return;
    }
    
    // HTML header
    htmlFile << "<!DOCTYPE html>\n"
             << "<html>\n"
             << "<head>\n"
             << "    <title>LUT Programming Visualization</title>\n"
             << "    <style>\n"
             << "        body { font-family: Arial, sans-serif; margin: 20px; }\n"
             << "        .bank { margin-bottom: 20px; }\n"
             << "        .bank-title { font-weight: bold; margin-bottom: 10px; }\n"
             << "        .cores { display: flex; flex-wrap: wrap; }\n"
             << "        .core { border: 1px solid #ccc; margin: 5px; padding: 10px; width: 150px; }\n"
             << "        .core.programmed { background-color: #e6ffe6; }\n"
             << "        .instruction { font-family: monospace; background-color: #f5f5f5; padding: 10px; margin-bottom: 20px; border-left: 5px solid #2196F3; }\n"
             << "        .lut-data { font-family: monospace; font-size: 12px; margin-top: 10px; }\n"
             << "        .core-type { font-weight: bold; color: #2196F3; }\n"
             << "        h1 { color: #333; }\n"
             << "        .data-cell { display: inline-block; width: 25px; height: 20px; text-align: center; background-color: #f0f0f0; margin: 1px; }\n"
             << "    </style>\n"
             << "</head>\n"
             << "<body>\n"
             << "    <h1>LUT Programming Visualization - Step " << instructionNumber << "</h1>\n"
             << "    <div class='instruction'>" << instruction << "</div>\n";
    
    // Show only Bank 0 for simplicity
    for (const auto& bank : pim.banks) {
        if (bank.id == 0) {  // Only show Bank 0
            htmlFile << "    <div class='bank'>\n"
                     << "        <div class='bank-title'>Bank " << static_cast<int>(bank.id) << "</div>\n"
                     << "        <div class='cores'>\n";
            
            // Display first 16 cores (or fewer if not that many)
            int coreCount = 0;
            for (const auto& core : bank.cores) {
                if (coreCount >= 16) break;  // Only show first 16 cores
                
                htmlFile << "            <div class='core " << (core.programmed ? "programmed" : "") << "'>\n"
                         << "                <div>Core " << static_cast<int>(core.id) << "</div>\n";
                
                if (core.programmed) {
                    htmlFile << "                <div class='core-type'>" << core.type << "</div>\n"
                             << "                <div class='lut-data'>\n";
                    
                    // Display LUT data in a grid
                    for (const auto& value : core.data) {
                        htmlFile << "                    <div class='data-cell'>0x" 
                                 << std::hex << std::setw(2) << std::setfill('0') 
                                 << static_cast<int>(value) << "</div>\n";
                    }
                    
                    htmlFile << "                </div>\n";
                } else {
                    htmlFile << "                <div>Not Programmed</div>\n";
                }
                
                htmlFile << "            </div>\n";
                coreCount++;
            }
            
            htmlFile << "        </div>\n"
                     << "    </div>\n";
        }
    }
    
    // HTML footer
    htmlFile << "</body>\n"
             << "</html>\n";
    
    htmlFile.close();
    
    // Create an index file to navigate through all steps (only on the first call)
    if (instructionNumber == 1) {
        createIndexFile();
    } else {
        updateIndexFile(instructionNumber);
    }
}

// Create index HTML file
void createIndexFile() {
    std::ofstream indexFile("lut_visualization/index.html");
    if (!indexFile.is_open()) {
        std::cerr << "Error: Could not create index file" << std::endl;
        return;
    }
    
    // HTML header
    indexFile << "<!DOCTYPE html>\n"
              << "<html>\n"
              << "<head>\n"
              << "    <title>LUT Programming Visualization</title>\n"
              << "    <style>\n"
              << "        body { font-family: Arial, sans-serif; margin: 20px; }\n"
              << "        h1 { color: #333; }\n"
              << "        .steps { margin-top: 20px; }\n"
              << "        .step-button { margin: 5px; padding: 10px; background-color: #2196F3; color: white; border: none; cursor: pointer; }\n"
              << "        .step-button:hover { background-color: #0b7dda; }\n"
              << "        .viewer { margin-top: 20px; border: 1px solid #ccc; height: 800px; }\n"
              << "    </style>\n"
              << "</head>\n"
              << "<body>\n"
              << "    <h1>LUT Programming Visualization</h1>\n"
              << "    <div class='steps' id='step-buttons'>\n"
              << "        <button class='step-button' onclick=\"document.getElementById('viewer').src='lut_state_001.html'\">Step 1</button>\n"
              << "    </div>\n"
              << "    <iframe id='viewer' class='viewer' src='lut_state_001.html' width='100%'></iframe>\n"
              << "    <script>\n"
              << "        // Auto-load the first step\n"
              << "        window.onload = function() {\n"
              << "            document.getElementById('viewer').src = 'lut_state_001.html';\n"
              << "        };\n"
              << "    </script>\n"
              << "</body>\n"
              << "</html>\n";
    
    indexFile.close();
}

// Update index HTML file with a new step
void updateIndexFile(int instructionNumber) {
    // Check if index file exists, if not create it
    std::ifstream checkFile("lut_visualization/index.html");
    if (!checkFile.is_open()) {
        checkFile.close();
        createIndexFile();
        
        // If this is not the first instruction, add more buttons
        if (instructionNumber > 1) {
            // Add buttons for steps 2 through current instruction
            std::ifstream indexFileIn("lut_visualization/index.html");
            if (!indexFileIn.is_open()) {
                std::cerr << "Error: Could not open index file after creation" << std::endl;
                return;
            }
            
            std::stringstream buffer;
            buffer << indexFileIn.rdbuf();
            std::string content = buffer.str();
            indexFileIn.close();
            
            size_t insertPos = content.find("</div>", content.find("<div class='steps'"));
            if (insertPos != std::string::npos) {
                std::stringstream newButtons;
                for (int i = 2; i <= instructionNumber; i++) {
                    newButtons << "\n        <button class='step-button' "
                               << "onclick=\"document.getElementById('viewer').src='lut_state_"
                               << std::setw(3) << std::setfill('0') << i << ".html'\">"
                               << "Step " << i << "</button>";
                }
                
                content.insert(insertPos, newButtons.str());
                
                std::ofstream indexFileOut("lut_visualization/index.html");
                if (!indexFileOut.is_open()) {
                    std::cerr << "Error: Could not open index file for writing" << std::endl;
                    return;
                }
                
                indexFileOut << content;
                indexFileOut.close();
            }
        }
        
        return;
    }
    checkFile.close();
    
    // If index file exists, update it normally
    std::ifstream indexFileIn("lut_visualization/index.html");
    if (!indexFileIn.is_open()) {
        std::cerr << "Error: Could not open index file for reading" << std::endl;
        return;
    }
    
    std::stringstream buffer;
    buffer << indexFileIn.rdbuf();
    std::string content = buffer.str();
    indexFileIn.close();
    
    // Find the position to insert the new button
    size_t insertPos = content.find("</div>", content.find("<div class='steps'"));
    if (insertPos != std::string::npos) {
        std::stringstream newButton;
        newButton << "\n        <button class='step-button' "
                  << "onclick=\"document.getElementById('viewer').src='lut_state_"
                  << std::setw(3) << std::setfill('0') << instructionNumber << ".html'\">"
                  << "Step " << instructionNumber << "</button>";
        
        content.insert(insertPos, newButton.str());
        
        // Write the updated content back to the file
        std::ofstream indexFileOut("lut_visualization/index.html");
        if (!indexFileOut.is_open()) {
            std::cerr << "Error: Could not open index file for writing" << std::endl;
            return;
        }
        
        indexFileOut << content;
        indexFileOut.close();
    }
}

int main(int argc, char* argv[]) {
    std::string asmFilePath;
    
    if (argc > 1) {
        asmFilePath = argv[1];
    } else {
        asmFilePath = "output.asm";  // Default assembly file
    }
    
    std::cout << "Visualizing LUT programming for: " << asmFilePath << std::endl;
    visualizeLUTProgramming(asmFilePath);
    
    return 0;
} 