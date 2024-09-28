#include <stdio.h>
#include <stdint.h>
#include <string.h>  // Added for strlen
#include <math.h>    // Added for pow

// Forward declaration for binaryToHexChar
char binaryToHexChar(uint8_t fourBitGroup);

/// @brief Sets a bit to 1 by using a bitwise OR with 1 shifted left by bitPosition.
/// @param data unsigned int to be modified
/// @param bitPosition position of the bit to set in the provided data
void setBit(uint32_t *data, uint32_t bitPosition)
{
    if (bitPosition <= 31)
    {
        *data |= (1 << bitPosition);
    }
}

/// @brief Clears a bit (sets to 0) by using a bitwise AND with the negation of 1 shifted left by bitPosition.
/// @param data unsigned int to be modified
/// @param bitPosition position of the bit to clear in the provided data
void clearBit(uint32_t *data, uint32_t bitPosition)
{
    if (bitPosition <= 31)
    {
        *data &= ~(1 << bitPosition);
    }
}

/// @brief Toggles the bit between 1 and 0 by using a bitwise XOR with 1 shifted left by bitPosition.
/// @param data unsigned int to be modified
/// @param bitPosition position of the bit to toggle in the provided data
void toggleBit(uint32_t *data, int bitPosition)
{
    if (bitPosition <= 31)
    {
        *data ^= (1 << bitPosition);
    }
}

/// @brief Returns the value of the bit at the specified bitPosition by shifting the target bit into the first position and taking the bitwise AND with the result.
/// @param data to have a bit value assessed
/// @param bitPosition position of the bit in data to asses, counting from 0 at the least significant bit.
/// @return value of the bit in the bitPosition of the paramater data
int getBitValue(uint32_t data, int bitPosition)
{
    if (bitPosition > 31)
    {
        return -1;
    }
    return (data >> bitPosition) & 1;
}

/// @brief Performs the binary addition of 'a' and 'b' iteratively by first identifying where the carry out would occur in the addition, and then adding without carrying out (via the bitwise XOR).
/// The carries are then shifted left and added to the result in the subsequent iteration. Repeats until all carries have been propagated and readded into the sum.
/// @param a the first addend
/// @param b the second addend
/// @return the result of adding 'a' and 'b'.
uint32_t binaryAdd(uint32_t a, uint32_t b)
{
    while (b != 0)
    {
        uint32_t carries = a & b;
        a = a ^ b;
        b = (carries << 1);
    }
    return a;
}

/// @brief Performs the binary subtraction of 'a' minus 'b' by taking the two's complement of 'b' and adding it to 'a'.
/// @param a minuend
/// @param b subtrahend 
/// @return the result of subtracting 'b' from 'a'. Note: If 'b' is greater than 'a' (the result would be negative) then 0 is returned.
uint32_t binarySubtract(uint32_t a, uint32_t b)
{
    if (b > a)  // Fixed the condition
    {
        return 0;
    }
    return binaryAdd(a, binaryAdd(~b, 1));
}

/// @brief Performs binary multiplication of 'a' and 'b' by checking each bit of the multiplier, and for each '1', it adds the multiplicand and shifts left once. Otherwise shift left only.
/// @param a multiplicand
/// @param b multiplier
/// @return the result of multiplying 'a' by 'b'
uint32_t binaryMultiply(uint32_t a, uint32_t b)
{
    uint32_t result = 0;
    while (b != 0)
    {
        if (getBitValue(b, 0) == 1)
        {
            result = binaryAdd(result, a);
        }
        a <<= 1;
        b >>= 1;
    }

    return result;
}


/// @brief Performs the binary division of 'a' divided by 'b'. Builds the quotient by iterating over the dividend's bits, checking if the divisor can be subtracted from the portion of digits of the dividend.
/// If the subtraction is possible, a 1 is added to the quotient at the corresponding bit position, otherwise zero.
/// @param a dividend 
/// @param b divisor
/// @return the quotient of a divided by b as an unsigned integer. Note: Returns 0 if b is 0 even though it is undefined.
uint32_t binaryDivide(uint32_t a, uint32_t b)
{
    if (b == 0)
    {
        return 0;
    }

    uint32_t result = 0;
    uint32_t temp = 0;

    for (int i = 31; i >= 0; i--)
    {
        temp = binaryAdd((temp << 1), getBitValue(a, i));
        if (temp >= b)
        {
            temp = binarySubtract(temp, b);
            setBit(&result, i);
        }
    }
    return result;
}

void toBinaryString(uint32_t data, char *buffer, size_t bufferSize)
{
    const size_t requiredSize = 33;
    //check if buffer is null before attemping to write to it.
    if (buffer == NULL) {
        return; // Alternatively, handle the error as needed
    }
    //if the buffer isn't large enough then just the update the char array in memory to null and return. 
    if (bufferSize < requiredSize) {
        if (bufferSize > 0) {
            buffer[0] = '\0';
        }
        return;
    }

    // Iterate over each bit, from the most significant bit (31) to the least (0)
    for (int i = 31; i >= 0; i--) {
        buffer[31 - i] = getBitValue(data, i) ? '1' : '0';
    }

    buffer[32] = '\0';
}


void binaryToHexString(uint32_t data, char *buffer, size_t bufferSize)
{
    const size_t requiredSize = 9;
    //check if buffer is null before attemping to write to it.
    if (buffer == NULL) {
        return; // Alternatively, handle the error as needed
    }
    //if the buffer isn't large enough then just the update the char array in memory to null and return. 
    if (bufferSize < requiredSize) {
        if (bufferSize > 0) {
            buffer[0] = '\0';
        }
        return;
    }

    // Iterate over every 4 bits and pass it to helper method binaryToHex to get its char value in hexadecimal, then adds it to the buffer.
    for (int i = 28; i >= 0; i-=4) {
        uint8_t groupOfFour = (data >> i) & 0x0F;  // Changed 00001111 to 0x0F for clarity
        buffer[(28 - i)/4] = binaryToHexChar(groupOfFour);
    }

    buffer[8] = '\0';
}

/// @brief Helper method for binaryToHexString, looks at a group of 4 bits and returns its corresponding ascii character value for binary to hexadecimal conversion.
/// @param fourBitGroup 8 bit int out of necessity, only first 4 bits should contain a value to be assessed.
/// @return the char value of fourBitGroup in hexadecimal
char binaryToHexChar(uint8_t fourBitGroup) {
    if (fourBitGroup < 10) {
        return (char)binaryAdd(fourBitGroup, 48);  // '0' starts at ASCII 48
    } else if (fourBitGroup < 16) {
        return (char)binaryAdd(binarySubtract(fourBitGroup, 10), 65);  // 'A' starts at ASCII 65
    }
    return '?';  // Error case
}

/// @brief Converts a decimal string to an integer by summing the values of each char's coefficient (multiplied by 10) to the digits respective power(*2^i).
/// @param decimalString decimal string to interpret
/// @return Converted uint32_t integer. Returns 0 on invalid input.
uint32_t fromDecimalString(const char *decimalString){
     if (decimalString == NULL) {
        return 0;
    }

    uint32_t result = 0;
    int i = 0;

    while (decimalString[i] != '\0') {
        char c = decimalString[i];

        // checking if char is a digit
        if (c < '0' || c > '9') {
            return 0;
        }
        //this shifts the previous result of the value in base 10 left one digit place and appends the value of the next digit to the right of it.
        result = binaryAdd(binaryMultiply(result, 10),(c - '0'));

        i++;
    }

    return result;
}

/// @brief Helper method for bitboard that reverses the order of bits in a 32 bit unsigned int. 
/// This simplifies the piece move validatioin logic because both player's boards can be initialized with the pieces at the same side 
/// and moves can be interpreted as the same direction for both players.
uint32_t reverseBits(uint32_t originalBoard) {
    uint32_t reversed = 0;
    for(int i = 0; i < 32; i++) {
        reversed <<= 1;
        reversed |= (originalBoard & 1);
        originalBoard >>= 1;
    }
    return reversed;
}

/// @brief Converts a binary string to a uint32_t integer.
/// @param binaryStr input binary string.
/// @return converted uint32_t integer. Returns 0 on invalid input.
uint32_t binaryToDecimal(const char *binaryStr){
    uint32_t result = 0;
    int length = strlen(binaryStr);
    for(int i=0; i<length;i++){
        if(binaryStr[i] == '1'){
            setBit(&result, i);  // Passed the address of result
        } else if(binaryStr[i] == '0') {
            // doing nothing here keeps the bit set to zero
        } else {
            return 0; //string should only contain 1s or 0s, so abort if it is not one of those two
        }
    }
    return result;
}

/// @brief Converts a hexadecimal string to an integer.
/// @param hexStr Input hexadecimal string.
/// @return Converted uint32_t integer. Returns 0 on invalid input.
uint32_t hexToDecimal(const char *hexStr){
    uint32_t result = 0;
    int i=0;
    int len = strlen(hexStr);  // Store the length to avoid multiple calls
    while(hexStr[i] != '\0'){
        if(hexStr[i] >= 'A' && hexStr[i] <= 'F') {
            result = binaryAdd(result, binaryMultiply((hexStr[i] - 55), pow(16, (len - 1) - i)));
        } else if(hexStr[i] >= '0' && hexStr[i] <= '9') {
            result = binaryAdd(result, binaryMultiply((hexStr[i] - '0'), pow(16, (len - 1) - i)));
        } else {
            return 0; // string does not contain 0-9 or A-F indicates invalid hexStr
        }
        i++;
    }
    return result;
}
