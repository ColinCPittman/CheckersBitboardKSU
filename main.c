#include "BitManipulationUtility.c"

int main() {
    uint32_t a = 5;
    uint32_t b = 3; 
    printf("Result of");
    uint32_t result = binaryAdd(a, b);
    
    printf("Result of binaryAdd(%u, %u) = %u\n", a, b, result);


    return 0;
}
