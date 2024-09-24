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
    bool isValid = true;

    int startRow = startPos / 8 + 1;
    int startCol = startPos % 8 + 1;
    int endRow = endPos / 8 + 1;
    int endCol = endPos % 8 + 1;

    bool isOnLeftEdge = (startCol == 1);
    bool isOnRightEdge = (startCol == 8);
    bool isOnTopEdge = (startRow == 1);
    bool isOnBottomEdge = (startRow == 8);
    bool isOnEdge = isOnLeftEdge || isOnRightEdge || isOnTopEdge || isOnBottomEdge;
    bool endPosOnOpposingEnd = (endRow == 8);

    int moveDistance = startPos - endPos;                             // distance that this function call is attempting to move the piece
    int validPeonDifferences[] = {4, 5, 7, 8, 9};                     // anything outside these values can be said to be an invalid move for a peon piece.
    int validKingDifferences[] = {4, 5, 7, 8, 9, -4, -5, -7, -8, -9}; // anything outside of these values can be said to be an invalid move for a king piece.

    int validPeonEdgeDifference[] = {4, 7, 8};
    int validLeftEdgeKingDifferences[] = {-4, -7, -8, 4, 8, 9};
    int validRightEdgeKingDifferences[] = {-4, -8, -9, 4, 7, 8};

    // First determine player and piece type
    int player = 0;
    bool isKing = false;
    int piecesFound = 0;

    if (getBitValue(p1PeonBoard, startPos))
    {
        player = 1;
        isKing = false;
        piecesFound++;
    }
    if (getBitValue(p1KingBoard, startPos))
    {
        player = 1;
        isKing = true;
        piecesFound++;
    }
    if (getBitValue(p2PeonBoard, startPos))
    {
        player = 2;
        isKing = false;
        piecesFound++;
    }
    if (getBitValue(p2KingBoard, startPos))
    {
        player = 2;
        isKing = true;
        piecesFound++;
    }

    if (piecesFound != 1)
    {
        return false;
    }
    if (player == 1)
    {
        if (isKing)
        {
            if (isOnRightEdge) {

            }
        }
        else
        {
        }
    }
    else if (player == 2)
    {
        if (isKing)
        {
        }
        else
        {
            if(endPosOnOpposingEnd) {

            }
        }
    }
 
    return isValid;
}