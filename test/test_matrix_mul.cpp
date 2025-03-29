#include <iostream>

int main() {
    // Matrix declarations with explicit dimensions
    Matrix A(3, 2);
    Matrix B(2, 4);
    Matrix C(3, 4);
    
    // Matrix multiplication
    C = A * B;
    
    return 0;
} 