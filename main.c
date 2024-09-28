#include "BitManipulationUtility.c"
#include <stdbool.h>
#include <stdio.h>

#define POSITION(ROW, COL) (63 - (((ROW) - 1) * 8 + ((COL) - 1))) // Human-readable position

//bitboards
uint32_t p1PeonBoard;
uint32_t p1KingBoard;
uint32_t p2PeonBoard;
uint32_t p2KingBoard;

bool validateMove(uint32_t startPos, uint32_t endPos);
bool arrayContains(int array[], int size, int value);
bool isOccupied(uint32_t pos);
bool determinePieceAndPlayer(uint32_t pos, int *player, bool *isKing);
bool isValidHop(int moveDistance, int player, bool isKing, uint32_t startPos);
bool isValidMoveDistance(int moveDistance, int player, bool isKing);
bool checkHop(uint32_t intermediatePos, int player);
void resetGame();
int main()
{

    p1PeonBoard = 0b11111111111100000000000000000000;
    p1KingBoard = 0;
    p2PeonBoard = 0b00000000000001000000111111111111;
    p2KingBoard = 0;
    uint32_t a = 5;
    uint32_t b = 3;
    printf("Result of");
    uint32_t result = binaryAdd(a, b);

    printf("Result of binaryAdd(%u, %u) = %u\n", a, b, result);
    printf("\nResult of div: %u\n", binaryDivide(11, 5));

    if (validateMove(POSITION(3, 2), POSITION(5, 4)))
    {
        printf("Move is valid.\n");
    }
    else
    {
        printf("Move is invalid.\n");
    }

    return 0;
}


/// @brief Resets the board state for both players.
void resetGame() {
    p1PeonBoard = 0b11111111111100000000000000000000;
    p1KingBoard = 0;
    p2PeonBoard = 0b00000000000000000000111111111111;
    p2KingBoard = 0;
}

/// @brief Validates a move from startPos to endPos, determines piece and player from startPos.
/// @param startPos starting position index (0-63).
/// @param endPos ending position index (0-63).
/// @return true if the move is valid; false otherwise.
bool validateMove(uint32_t startPos, uint32_t endPos)
{
    // Check board boundaries
    if (startPos > 63 || endPos > 63)
        return false;

    // Check if both positions are on black tiles
    uint32_t startRow = (63 - startPos) / 8 + 1;
    uint32_t startCol = (63 - startPos) % 8 + 1;
    uint32_t endRow = (63 - endPos) / 8 + 1;
    uint32_t endCol = (63 - endPos) % 8 + 1;

    if ((startRow + startCol) % 2 == 0 || (endRow + endCol) % 2 == 0)
        return false;

    // re-interpret positions for 32 tiles
    uint32_t trueStartPos = (startPos / 8) * 4 + (startPos % 8) / 2;
    uint32_t trueEndPos = (endPos / 8) * 4 + (endPos % 8) / 2;

    int moveDistance = trueStartPos - trueEndPos;

    // Determine player and piece type
    int player;
    bool isKing;
    if (!determinePieceAndPlayer(trueStartPos, &player, &isKing))
        return false;

    // Check if the end position is occupied
    if (isOccupied(trueEndPos))
        return false;

    // Validate move distance
    if (!isValidMoveDistance(moveDistance, player, isKing))
        return false;

    // Check for hops if necessary
    bool isHop = false;
    if (player == 1 && isKing && (moveDistance == 9 || moveDistance == -7))
        isHop = true;
    else if (player == 1 && !isKing && (moveDistance == 9))
        isHop = true;
    else if (player == 2 && isKing && (moveDistance == -7 || moveDistance == 9))
        isHop = true;
    else if (player == 2 && !isKing && (moveDistance == -7 || moveDistance == -9))
        isHop = true;

    if (isHop)
    {
        return isValidHop(moveDistance, player, isKing, trueStartPos);
    }

    // If not a hop, it's a simple valid move
    return true;
}


/// @brief Checks if a position is occupied by any piece. Helper method for validateMove method.
/// @param pos position index (0-31).
/// @return true if occupied; false otherwise.
bool isOccupied(uint32_t pos)
{
    return getBitValue(p1PeonBoard, pos) ||
           getBitValue(p1KingBoard, pos) ||
           getBitValue(p2PeonBoard, pos) ||
           getBitValue(p2KingBoard, pos);
}

/// @brief Determines the player and piece type at a given position. 
/// @param pos position index (0-31).
/// @param player pointer to store the player number (1 or 2).
/// @param isKing pointer to store if the piece is a king.
/// @return true if exactly one piece is found; false otherwise.
bool determinePieceAndPlayer(uint32_t pos, int *player, bool *isKing)
{
    int piecesFound = 0;
    if (getBitValue(p1PeonBoard, pos))
    {
        *player = 1;
        *isKing = false;
        piecesFound++;
    }
    if (getBitValue(p1KingBoard, pos))
    {
        *player = 1;
        *isKing = true;
        piecesFound++;
    }
    if (getBitValue(p2PeonBoard, pos))
    {
        *player = 2;
        *isKing = false;
        piecesFound++;
    }
    if (getBitValue(p2KingBoard, pos))
    {
        *player = 2;
        *isKing = true;
        piecesFound++;
    }
    return piecesFound == 1;
}

/// @brief Checks if a move is a valid hop over an opponent's piece. Helper method for validateMove method.
/// @param moveDistance the calculated move distance.
/// @param player the player making the move.
/// @param isKing whether the moving piece is a king.
/// @param startPos the starting position index.
/// @return true if the hop is valid; false otherwise.
bool isValidHop(int moveDistance, int player, bool isKing, uint32_t startPos)
{
    uint32_t intermediatePos;
    if (moveDistance > 0)
    {
        intermediatePos = isKing ? startPos - 4 : startPos - 5;
    }
    else
    {
        intermediatePos = isKing ? startPos + 4 : startPos + 5;
    }
    return checkHop(intermediatePos, player);
}

/// @brief Checks if the intermediate position during a hop contains exactly one enemy piece and no friendly pieces. 
/// Helper method for validateMove method.
/// @param intermediatePos the intermediate position index.
/// @param player the player making the move.
/// @return true if the hop crosses over an enemy piece; false otherwise.
bool checkHop(uint32_t intermediatePos, int player)
{
    if (player == 1)
    {
        return (getBitValue(p2KingBoard, intermediatePos) || getBitValue(p2PeonBoard, intermediatePos)) &&
               !(getBitValue(p1KingBoard, intermediatePos) || getBitValue(p1PeonBoard, intermediatePos));
    }
    else
    {
        return (getBitValue(p1KingBoard, intermediatePos) || getBitValue(p1PeonBoard, intermediatePos)) &&
               !(getBitValue(p2KingBoard, intermediatePos) || getBitValue(p2PeonBoard, intermediatePos));
    }
}

/// @brief Validates if the move distance is allowed for the player's piece.Helper method for validateMove method.
/// @param moveDistance the calculated move distance.
/// @param player the player making the move.
/// @param isKing Whether the moving piece is a king.
/// @return true if the move distance is valid; false otherwise.
bool isValidMoveDistance(int moveDistance, int player, bool isKing)
{
    int validPeonDifferences_p1[] = {4, 5, 7, 9};
    int validKingDifferences_p1[] = {4, 5, 7, 9, -3, -4, -7, -9};
    int validPeonDifferences_p2[] = {-4, -5, -7, -9};
    int validKingDifferences_p2[] = {-4, -5, -7, -9, 3, 4, 7, 9};

    if (player == 1)
    {
        if (isKing)
            return arrayContains(validKingDifferences_p1, sizeof(validKingDifferences_p1) / sizeof(int), moveDistance);
        else
            return arrayContains(validPeonDifferences_p1, sizeof(validPeonDifferences_p1) / sizeof(int), moveDistance);
    }
    else
    {
        if (isKing)
            return arrayContains(validKingDifferences_p2, sizeof(validKingDifferences_p2) / sizeof(int), moveDistance);
        else
            return arrayContains(validPeonDifferences_p2, sizeof(validPeonDifferences_p2) / sizeof(int), moveDistance);
    }
}

/// @brief Utility function to check if an array contains a specific value.
/// @param array the array to search.
/// @param size the size of the array.
/// @param value the value to find.
/// @return true if the value is found; false otherwise.
bool arrayContains(int array[], int size, int value)
{
    for (int i = 0; i < size; i++)
    {
        if (array[i] == value)
            return true;
    }
    return false;
}