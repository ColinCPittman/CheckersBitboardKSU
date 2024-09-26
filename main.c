#include "BitManipulationUtility.c"
#include <stdbool.h>
#define POSITION(ROW, COL) (((ROW) - 1) * 8 + ((COL) - 1)) // this is to make it easier toV
static uint32_t p1PeonBoard;
static uint32_t p1KingBoard;
static uint32_t p2PeonBoard;
static uint32_t p2KingBoard;

int main()
{
    p1PeonBoard = 0b11111111111100000000000000000000;
    p1KingBoard = 0;
    p2PeonBoard = 0b11111111111100000000000000000000;
    p2KingBoard = 0;

    uint32_t a = 5;
    uint32_t b = 3;
    printf("Result of");
    uint32_t result = binaryAdd(a, b);

    printf("Result of binaryAdd(%u, %u) = %u\n", a, b, result);
    printf("\nResult of div: %u", binaryDivide(11, 5));

    return 0;
}

bool validiateMove(uint32_t startPos, uint32_t endPos)
{
    if (startPos > 31 || endPos > 31)
    { // check if the positions are within the bounds of the board.
        return false;
    }
    bool isValid = true;

    int startRow = startPos / 8 + 1;
    int startCol = startPos % 8 + 1;
    int endRow = endPos / 8 + 1;
    int endCol = endPos % 8 + 1;

    bool isOnLeftEdge = (startCol == 1);
    bool isOnRightEdge = (startCol == 8);
    bool isOnEdge = isOnLeftEdge || isOnRightEdge;
    bool endPosOnOpposingEnd = (endRow == 8);

    int moveDistance = startPos - endPos; // distance that this function call is attempting to move the piece

    // determine the move is automatically invalid if the ending position of this move is occupied
    int endPosPiecesFound = 0;
    if (getBitValue(p1KingBoard, endPos))
        endPosPiecesFound++;
    if (getBitValue(p2KingBoard, endPos))
        endPosPiecesFound++;
    if (getBitValue(p1PeonBoard, endPos))
        endPosPiecesFound++;
    if (getBitValue(p2PeonBoard, endPos))
        endPosPiecesFound++;
    if (endPosPiecesFound != 0)
    {
        return false;
    }

    int validPeonDifferences[] = {4, 5, 7, 8, 9};                     // anything outside these values can be said to be an invalid move for a peon piece.
    int validKingDifferences[] = {4, 5, 7, 8, 9, -4, -5, -7, -8, -9}; // anything outside of these values can be said to be an invalid move for a king piece.

    int validPeonEdgeDifference[] = {4, 7, 8};                   // anything outside of these values can be said to be an invalid move for a peon piece that is along the edge of the board.
    int validLeftEdgeKingDifferences[] = {-4, -7, -8, 4, 8, 9};  // anything outside of these values can be said to be and invalid move for a king piece along the left edge of the board.
    int validRightEdgeKingDifferences[] = {-4, -8, -9, 4, 7, 8}; // anything outside of these values can be said to be an invalid move for a king piece along the right edge of the board.

    // Next determine player and piece type
    int player = 0;
    bool isKing = false;
    int startPosPiecesFound = 0;

    if (getBitValue(p1PeonBoard, startPos))
    {
        player = 1;
        isKing = false;
        startPosPiecesFound++;
    }
    if (getBitValue(p1KingBoard, startPos))
    {
        player = 1;
        isKing = true;
        startPosPiecesFound++;
    }
    if (getBitValue(p2PeonBoard, startPos))
    {
        player = 2;
        isKing = false;
        startPosPiecesFound++;
    }
    if (getBitValue(p2KingBoard, startPos))
    {
        player = 2;
        isKing = true;
        startPosPiecesFound++;
    }

    if (startPosPiecesFound != 1)
    {
        return false;
    }
    // Piece and player now determined, logic branches from here to dertermine valid moves for the possible cases
    if (player == 1)
    {
        if (isKing)
        {
            if (isOnRightEdge) //p1 king is on right edge
            {
                if (arrayContains(validRightEdgeKingDifferences, 6, moveDistance)) // check if the move distance is valid for a right edge king
                {
                    if ((moveDistance == 7 || moveDistance == 8) && (getBitValue(p2KingBoard, (startPos + 4)) + getBitValue(p2PeonBoard, (startPos + 4)) == 1)) // forward hop
                    {
                        return true;
                    }
                    else if ((moveDistance == -8 || moveDistance == -9) && (getBitValue(p2KingBoard, (startPos - 4)) + getBitValue(p2PeonBoard, (startPos - 4)) == 1)) // backward hop
                    { 
                        return true;
                    } 
                    else if(moveDistance == 4 || moveDistance == -4) // p1 king on right edge makes a simple diagonal move into unoccupied tile
                    {
                        return true;
                    }
                    else { //player 2's boards are invalid and contain more than 1 piece in the same tile, should not occur
                        return false;
                    }
                }
                else 
                {
                    return false;
                }
            }
            else if (isOnLeftEdge) //p1 king on left edge
            {
                if (arrayContains(validLeftEdgeKingDifferences, 6, moveDistance)) // check if the move distance is valid for a left edge king
                {
                    if ((moveDistance == 8 || moveDistance == 9) //distance is hop
                    && (getBitValue(p2KingBoard, (startPos + 4)) + getBitValue(p2PeonBoard, (startPos + 4)) == 1) //enemy piece in between
                    && (getBitValue(p1KingBoard, (startPos + 4)) + getBitValue(p1PeonBoard, (startPos + 4) == 0))) //not a friendly piece in between
                    {
                        return true;
                    }
                    else if ((moveDistance == -7 || moveDistance == -8) && (getBitValue(p2KingBoard, (startPos - 4)) + getBitValue(p2PeonBoard, (startPos - 4)) == 1)) // backward hop
                    { 
                        return true;
                    }
                    else if(moveDistance == 4 || moveDistance == -4) //p1 king on left edge makes a simple diagonal move into an unoccupied tile
                    {
                        return true;
                    }
                    else { //player 2's boards are invalid and contain more than 1 piece in the same tile, should not occur
                        return false;
                    }
                }
                else //moveDistance is not valid for p1 king on left edge
                {
                    return false;
                }
            }
            else // p1 king not on any edge
            {
                if (arrayContains(validKingDifferences, 10, moveDistance)) // check if the move distance is valid for a king not on any edge
                {
                    if ((moveDistance == 7 || moveDistance == 8 || moveDistance == 9) && (getBitValue(p2KingBoard, (startPos + 4)) + getBitValue(p2PeonBoard, (startPos + 4)) == 1)) // forward hop
                    {
                        return true;
                    }
                    else if ((moveDistance == -7 || moveDistance == -8 || moveDistance == -9) && (getBitValue(p2KingBoard, (startPos - 4)) + getBitValue(p2PeonBoard, (startPos - 4)) == 1)) // backward hop
                    { 
                        return true;
                    }
                    else  // moveDistance is 4 or -4, simple diagonal move into unoccupied tile
                    {
                        return true;
                    }
                }
                else 
                {
                    return false;
                }
            }
        }
        else // player 1 Peon
        {
            if (isOnEdge)
            {
                if (arrayContains(validPeonEdgeDifference, 3, moveDistance))
                {
                    if((moveDistance == 7 || moveDistance == 8) && (getBitValue(p2KingBoard, (startPos + 4)) + getBitValue(p2PeonBoard, (startPos + 4)) == 1)) {
                        return true;
                    }
                    else { //moveDistance is 4, normal hop into unoccupied spot
                        return true;
                    }
                }
            }
            else
            {
                if (!arrayContains(validPeonDifferences, 5, moveDistance))
                {
                    return false;
                }
            }

            // Determine the direction of movement for Player 1 Peon (assuming Player 1 moves upwards)
            if (moveDistance == 4 || moveDistance == 5) // Simple forward move
            {
                return true;
            }
            else if (moveDistance == 7 || moveDistance == 9) // Capture move (hop)
            {
                // Calculate the position of the piece being jumped over
                int jumpedPos = (startPos + endPos) / 2;
                // Check if the jumped position contains an opponent's piece
                if (getBitValue(p2PeonBoard, jumpedPos) || getBitValue(p2KingBoard, jumpedPos))
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else // Invalid move distance for Peon
            {
                return false;
            }
        }
    }
    else if (player == 2)
    {
        if (isKing)
        {
            if (isOnRightEdge)
            {
                if (arrayContains(validRightEdgeKingDifferences, 6, moveDistance)) // check if the move distance is valid for a right edge king
                {
                    if ((moveDistance == -7 || moveDistance == -8) && (getBitValue(p1KingBoard, (startPos + 4)) + getBitValue(p1PeonBoard, (startPos + 4)) == 1)) // forward hop
                    {
                        return true;
                    }
                    else if ((moveDistance == 8 || moveDistance == 9) && (getBitValue(p1KingBoard, (startPos - 4)) + getBitValue(p1PeonBoard, (startPos - 4)) == 1)) // backward hop
                    { 
                        return true;
                    }
                    else if (moveDistance == 4 || moveDistance == -4) // simple diagonal move
                    {
                        return true;
                    }
                    else // invalid move distance
                    {
                        return false;
                    }
                }
                else 
                {
                    return false;
                }
            }
            else if (isOnLeftEdge)
            {
                if (arrayContains(validLeftEdgeKingDifferences, 6, moveDistance)) // check if the move distance is valid for a left edge king
                {
                    if ((moveDistance == 8 || moveDistance == 9) && (getBitValue(p1KingBoard, (startPos + 4)) + getBitValue(p1PeonBoard, (startPos + 4)) == 1)) // forward hop
                    {
                        return true;
                    }
                    else if ((moveDistance == 4 || moveDistance == 5) && (getBitValue(p1KingBoard, (startPos - 4)) + getBitValue(p1PeonBoard, (startPos - 4)) == 1)) // backward hop
                    { 
                        return true;
                    }
                    else if (moveDistance == 4 || moveDistance == -4) // simple diagonal move
                    {
                        return true;
                    }
                    else // invalid move distance
                    {
                        return false;
                    }
                }
                else
                {
                    return false;
                }
            }
            else // Not on any edge
            {
                if (arrayContains(validKingDifferences, 10, moveDistance)) // check if the move distance is valid for a king not on any edge
                {
                    if ((moveDistance == 7 || moveDistance == 8 || moveDistance == 9) && (getBitValue(p1KingBoard, (startPos + 4)) + getBitValue(p1PeonBoard, (startPos + 4)) == 1)) // forward hop
                    {
                        return true;
                    }
                    else if ((moveDistance == -7 || moveDistance == -8 || moveDistance == -9) && (getBitValue(p1KingBoard, (startPos - 4)) + getBitValue(p1PeonBoard, (startPos - 4)) == 1)) // backward hop
                    { 
                        return true;
                    }
                    else if (moveDistance == 4 || moveDistance == -4) // simple diagonal move
                    {
                        return true;
                    }
                    else // invalid move distance
                    {
                        return false;
                    }
                }
                else 
                {
                    return false;
                }
            }
        }
        else // Player 2 Peon
        {
            if (isOnEdge)
            {
                if (!arrayContains(validPeonEdgeDifference, 3, moveDistance))
                {
                    return false;
                }
            }
            else
            {
                if (!arrayContains(validPeonDifferences, 5, moveDistance))
                {
                    return false;
                }
            }

            // Determine the direction of movement for Player 2 Peon (assuming Player 2 moves downwards)
            if (moveDistance == -4 || moveDistance == -5) // Simple forward move
            {
                return true;
            }
            else if (moveDistance == -7 || moveDistance == -9) // Capture move (hop)
            {
                // Calculate the position of the piece being jumped over
                int jumpedPos = (startPos + endPos) / 2;
                // Check if the jumped position contains an opponent's piece
                if (getBitValue(p1PeonBoard, jumpedPos) || getBitValue(p1KingBoard, jumpedPos))
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else // Invalid move distance for Peon
            {
                return false;
            }
        }
    }

    // If none of the valid conditions are met, the move is invalid
    return false;
}
/// @brief Helper method for checking if an int is contained in an int array to simplify some of the validateMove logic.
/// @param array int array to be checked
/// @param size size of the int array 'array'
/// @param value value to check for in the int array 'array'.
/// @return true if a match is found, otherwise false.
bool arrayContains(int array[], int size, int value)
{
    for (int i = 0; i < size; i++)
    {
        if (array[i] == value)
        {
            return true;
        }
    }
    return false;
}