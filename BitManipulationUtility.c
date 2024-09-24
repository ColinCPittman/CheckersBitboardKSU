#include <stdio.h>
#include <stdint.h>

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
/// @return the result of subtracting 'b' from 'a', or -1 if 'b' is greater than 'a' (the result would be negative).
uint32_t binarySubtract(uint32_t a, uint32_t b)
{
    if (!(b > a))
    {
        return -1;
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
/// @return the quotient of a divided by b as an unsigned integer. Returns -1 if b is 0 to indicate undefined.
uint32_t binaryDivide(uint32_t a, uint32_t b)
{
    if (b == 0)
    {
        return -1;
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


void toHexString(uint32_t data, char *buffer, size_t bufferSize)
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
        uint8_t groupOfFour = (data >> i) & 00001111;
        buffer[(28 - i)/4] = binaryToHex(groupOfFour);
    }

    buffer[8] = '\0';
}

/// @brief Helper method for toHexString, looks at a group of 4 bits and returns its corresponding ascii character value for binary to hexadecimal conversion.
/// @param fourBitGroup 8 bit int out of necessity, only first 4 bits should contain a value to be assessed.
/// @return the char value of fourBitGroup in hexadecimal
char binaryToHex(uint8_t fourBitGroup) {
    if(fourBitGroup < 10) {
        return binaryAdd(fourBitGroup, 48);
    } else if(fourBitGroup < 16) {
        return binaryAdd(binarySubtract(fourBitGroup,10), 65);
    }
    return '?';
}

/// @brief Converts a decimal string to an integer by summing the values of each char's coefficient (multiplied by 10) to the digits respective power(*2^i).
/// @param decimalString decimal string to interpret
/// @return Converted uint32_t integer. Returns -1 on invalid input.
uint32_t fromDecimalString(const char *decimalString){
     if (decimalString == NULL) {
        return -1;
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

/// @brief Converts a binary string to a uint32_t integer.
/// @param binaryStr Input binary string.
/// @return Converted uint32_t integer. Returns 0 on invalid input.
uint32_t binaryToDecimal(const char *binaryStr){
    uint32_t result = 0;
    int length = strlen(binaryStr);
    for(int i=0; i<length-1;i++){
        if(getBitValue(binaryStr, length-i) == '1'){
            setBit(result,i);
        } 
    }
    return result;
}

/// @brief Converts a hexadecimal string to an integer.
/// @param hexStr Input hexadecimal string.
/// @return Converted uint32_t integer. Returns 0 on invalid input.
uint32_t hexToDecimal(const char *hexStr);

/// @brief Converts a binary string directly to a hexadecimal string.
/// @param binaryStr Input binary string.
/// @param hexBuffer Output buffer for hexadecimal string.
/// @param bufferSize Size of the output buffer.
/// @return 0 on success, -1 on failure.
int binaryToHexString(const char *binaryStr, char *hexBuffer, size_t bufferSize);

/// @brief Converts a hexadecimal string directly to a binary string.
/// @param hexStr Input hexadecimal string.
/// @param binaryBuffer Output buffer for binary string.
/// @param bufferSize Size of the output buffer.
/// @return 0 on success, -1 on failure.
int hexToBinaryString(const char *hexStr, char *binaryBuffer, size_t bufferSize);
