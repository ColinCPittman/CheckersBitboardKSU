#include <stdio.h>
#include <stdint.h>

/// @brief Sets a bit to 1 by using a bitwise OR with 1 shifted left by bitPosition.
/// @param data unsigned int to be modified
/// @param bitPosition position of the bit to set in the provided data
void setBit(uint32_t* data, int bitPosition) {
    *data |= (1 << bitPosition); 
}

/// @brief Clears a bit (sets to 0) by using a bitwise AND with the negation of 1 shifted left by bitPosition.
/// @param data unsigned int to be modified
/// @param bitPosition position of the bit to clear in the provided data
void clearBit(uint32_t* data, int bitPosition) {
    *data &= ~(1 << bitPosition); 
}

/// @brief Toggles the bit between 1 and 0 by using a bitwise XOR with 1 shifted left by bitPosition.
/// @param data unsigned int to be modified
/// @param bitPosition position of the bit to clear in the provided data
void toggleBit(uint32_t* data, int bitPosition) {
    *data ^= (1 << bitPosition);
}

/// @brief Returns the value of the bit at the specified bitPosition by shifting the target bit into the first position and taking the bitwise AND with the result.
/// @param data to have a bit value assessed
/// @param bitPosition position of the bit in data to asses, counting from 0 at the least significant bit.
/// @return value of the bit in the bitPosition of the paramater data
int getBitValue(uint32_t data, int bitPosition) {
        return (data >> bitPosition) & 1; 
}

uint32_t binaryAdd(uint32_t a, uint32_t b) {
    if(a==0) {
        return b;
    }
    while(b != 0) {
        uint32_t carries = a & b;
        a = a ^ b;
        b = (carries << 1);
    }
    return a; 
}

uint32_t binarySubtract(uint32_t a, uint32_t b) {
    uint32_t complement = binaryAdd(~b,1);
    return binaryAdd(a, complement);
}

uint32_t binaryMultiply(uint32_t a, uint32_t b) {
    uint32_t result = 0;
    while(b!=0) {
        if(getBitValue(a,0) == 1) {
            result += a;
        }
        a <<= 1;
        b>>= 1;
    }

    return result;
}

// Function to perform binary division
uint32_t binaryDivide(uint32_t a, uint32_t b) {
uint32_t result = 0;
    //for (int i = sizeof(a) * 8 - 1; i>=0; i--) ?
        //uint32_t rollingDiff = 0; ?
        // uint32_t aCopy ? Need to looking into this more, lacking clarity on the variables needed here, have to deepen my understanding and come back
    
    return 0; 
}

void toBinaryString(uint32_t data, char* buffer, size_t bufferSize) {
    // TODO: Implement conversion to binary string
}

// Function to convert an integer to its hexadecimal string representation
void toHexString(uint32_t data, char* buffer, size_t bufferSize) {
    // TODO: Implement conversion to hexadecimal string
}

// Function to convert a decimal string to an integer
uint32_t fromDecimalString(const char* str) {
    // TODO: Implement conversion from decimal string
    return 0; // Placeholder return value
}
