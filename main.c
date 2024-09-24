#include "BitManipulationUtility.c"
#include <stdbool.h>
#define POSITION(ROW, COL) ( ((ROW) - 1) * 8 + ((COL) - 1) ) //this is to make it easier toV
int main() {
    uint32_t a = 5;
    uint32_t b = 3; 
    printf("Result of");
    uint32_t result = binaryAdd(a, b);
    
    printf("Result of binaryAdd(%u, %u) = %u\n", a, b, result);
    printf("\nResult of div: %u" + binaryDivide(11,5));

    return 0;
}

bool validiateMove(uint32_t startPos, uint32_t endPos) {
    int startRow = startPos / 8 + 1;
    int startCol = startPos % 8 + 1;
    int endRow = endPos / 8 + 1;
    int endCol = endPos % 8 + 1;
    
}