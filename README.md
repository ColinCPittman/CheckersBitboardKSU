# Bitboard Checkers (C)

A performance-oriented, 2-player checkers game written in C that utilizes bitboards for game state representation and custom low-level arithmetic.

## Project Features
* Maps the 32 playable squares of an 8x8 board to bits in uint32_t integers to reduce memory overhead.
* Includes a custom library (```BitManipulationUtility.c```) that implements arithmetic addition, subtraction, multiplication, and division using only bitwise logic (AND, OR, XOR, and shifts).
* Implements standard checkers rules (diagonal movement, king promotion, and capturing (hopping) logic, etc).
* Has an interactive ASCII board and real-time binary/hexadecimal data views of the internal bitboard state for debugging.

## Technical Implementation
* Instead of standard operators, binary addition (identifying carries via XOR/AND) and subtraction (two's complement) are uses for computation efficiency.
* Translates user-friendly grid coordinates (row 1-8, col 1-8) into 64-tile and 32-tile bit indexes for internal processing.

## Running
1. Compile the project using `gcc`:
   ```bash
   gcc main.c -o checkers
