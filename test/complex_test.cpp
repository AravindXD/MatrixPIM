#include <iostream>

int main() {
    // Matrix declarations with larger dimensions
    Matrix A(8, 6);  // 8x6 matrix
    Matrix B(6, 10); // 6x10 matrix
    Matrix C(8, 10); // 8x10 matrix
    
    // Set input matrices as inputs
    A.setInput(true); 
    B.setInput(true);
    
    // Set output matrix
    C.setOutput(true);
    
    // Matrix multiplication
    C = A * B;
    
    return 0;
} 