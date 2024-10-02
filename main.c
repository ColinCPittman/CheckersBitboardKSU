#include "BitManipulationUtility.c"
#include <stdbool.h>
#include <stdio.h>

#define POSITION(ROW, COL) (63 - (((ROW) - 1) * 8 + ((COL) - 1))) // Allows users to input row and column index to make moves.
#define MENU_MAKE_MOVE 1                                          // input outcome, for more readable main method
#define MENU_END_TURN 2
#define MENU_DISPLAY_BOARD 3                                      // input outcome, for more readable mian method
#define MENU_EXIT 4                                               // input outcome, for more readable main method

// bitboards
uint32_t p1PeonBoard;
uint32_t p1KingBoard;
uint32_t p2PeonBoard;
uint32_t p2KingBoard;

char getPlayerChar(int player);
bool validateMove(uint32_t startPos, uint32_t endPos);
bool arrayContains(int array[], int size, int value);
bool isOccupied(uint32_t pos);
bool determinePieceAndPlayer(uint32_t pos, int *player, bool *isKing);
int validateHop(int player, bool isKing, int moveDistance, uint32_t startPos, int startCol);
bool validateNormalMove(int moveDistance, int player, bool isKing, int startCol);
bool checkHop(uint32_t hoppedPos, int player);
bool isHop(int player, bool isKing, int moveDistance);
void resetGame();
bool makeMove(uint32_t startPos, uint32_t endPos);
void promoteToKing(int player, uint32_t pos);
void displayBoard();
bool updateRowAndCol(int *row, int *col, uint32_t pos);
void convertSinglePositionTo32TileFormat(uint32_t *pos);
void convertPositionsTo32TileFormat(uint32_t *startPos, uint32_t *endPos);
bool updateRow(int *row, uint32_t pos);
bool updateCol(int *col, uint32_t pos);
int promptUserForInteger(const char *prompt);
int checkVictory();
bool promptToPlayAgain();


int main()
{
    bool keepPlaying = true;

    while (keepPlaying)
    {
        resetGame();
        int currentPlayer = 1;
        bool gameOver = false;
        int choice;
        bool hasMadeMove = false;
        char mainMenu[256];
        while (!gameOver)
        {
            
            snprintf(mainMenu, sizeof(mainMenu), //update the menu to the current player
                     "\n----- MAIN MENU -----\n"
                     "1. Make a Move\n"
                     "2. End Player %c's Turn\n"
                     "3. Display Board\n"
                     "4. Exit\n",
                     getPlayerChar(currentPlayer));
            printf("%s", mainMenu);
            choice = promptUserForInteger("Enter your choice: ");

            switch (choice)
            {
            case MENU_MAKE_MOVE:
            {
                uint32_t startPos, endPos;
                int startRow, startCol, endRow, endCol;
                int player;
                bool isKing;
                int moveDistance = 0;
                bool performedHop = false;
                do
                {
                    if(hasMadeMove && !performedHop) { //interrupt the player if they try to move again when they are not allowed to
                    printf("\nNo valid move.");
                    break;
                    }
                    displayBoard();
                    if (!hasMadeMove)
                    {
                        printf("\nPlayer %c's Turn:\n", getPlayerChar(currentPlayer));

                        startRow = promptUserForInteger("Enter Start Row (1-8): ");
                        startCol = promptUserForInteger("Enter Start Column (1-8): ");

                        if (startRow < 1 || startRow > 8 || startCol < 1 || startCol > 8)
                        {
                            printf("Invalid row or column. Please enter values between 1 and 8.\n");
                            break;
                        }

                        startPos = POSITION(startRow, startCol);
                        int startPosCheck = startPos;
                        convertSinglePositionTo32TileFormat(&startPosCheck);
                        determinePieceAndPlayer(startPosCheck, &player, &isKing);
                        if (player != currentPlayer) {
                            printf("Invalid move, cannot move opponent's pieces. Try again.\n");
                            break;
                        }
                    }
                    else //the player has already performed a move, if it was a hop then inform the player ending indexes only needed.
                    {
                        if(performedHop) {
                        printf("\nPlayer %c performed a hop. You must choose a new end location for the same piece.\n", getPlayerChar(currentPlayer));
                        } 
                    }

                    endRow = promptUserForInteger("Enter End Row (1-8): ");
                    endCol = promptUserForInteger("Enter End Column (1-8): ");

                    if (endRow < 1 || endRow > 8 || endCol < 1 || endCol > 8)
                    {
                        printf("Invalid row or column. Please enter values between 1 and 8.\n");
                        continue;
                    }

                    endPos = POSITION(endRow, endCol);


                    if (makeMove(startPos, endPos)) // if move is valid
                    {
                        hasMadeMove = true;
                        convertPositionsTo32TileFormat(&startPos, &endPos);
                        int moveDistance = startPos - endPos;
                        startPos = endPos; // set for potential next hop

                        int victory = checkVictory();
                        if (victory == 1)
                        {
                            displayBoard();
                            printf("Player A is the last one standing.\n");
                            gameOver = true;
                            break;
                        }
                        else if (victory == 2)
                        {
                            displayBoard();
                            printf("Player B is the last one standing.\n");
                            gameOver = true;
                            break;
                        }

                        
                        int movedPlayer;
                        determinePieceAndPlayer(endPos, &movedPlayer, &isKing);

                        if (isHop(currentPlayer, isKing, moveDistance))
                        {
                            printf("Player %c performed a hop. You must choose a new end location for the same piece.\n", getPlayerChar(currentPlayer));
                            performedHop == true; // Continue allowing the same player to make another move
                        }
                        else
                        {
                            printf("Move completed.\n");
                        }
                    }
                    else
                    {
                        printf("Not a valid move.\n");
                    }

                } while (isHop(currentPlayer, isKing, moveDistance) && !gameOver);

                break;
            }
            case MENU_END_TURN:
            {
                if (hasMadeMove)
                {
                    printf("Ending Player %c's turn.\n", getPlayerChar(currentPlayer));
                    currentPlayer = (currentPlayer == 1) ? 2 : 1;
                    printf("It is now Player %c's turn.\n", getPlayerChar(currentPlayer));
                    hasMadeMove = false; // Reset for the next player's turn
                }
                else
                {
                    printf("You must make at least one move before ending your turn.\n");
                }
                break;
            }
            case MENU_DISPLAY_BOARD:
                displayBoard();
                char binaryBuffer[33], hexBuffer[9];

                toBinaryString(p1PeonBoard, binaryBuffer, sizeof(binaryBuffer));
                binaryToHexString(p1PeonBoard, hexBuffer, sizeof(hexBuffer));
                printf("Player A Peon Board (Binary): %s\nPlayer A Peon Board (Hex): %s\n", binaryBuffer, hexBuffer);

                toBinaryString(p1KingBoard, binaryBuffer, sizeof(binaryBuffer));
                binaryToHexString(p1KingBoard, hexBuffer, sizeof(hexBuffer));
                printf("Player A King Board (Binary): %s\nPlayer A King Board (Hex): %s\n", binaryBuffer, hexBuffer);

                toBinaryString(p2PeonBoard, binaryBuffer, sizeof(binaryBuffer));
                binaryToHexString(p2PeonBoard, hexBuffer, sizeof(hexBuffer));
                printf("Player B Peon Board (Binary): %s\nPlayer B Peon Board (Hex): %s\n", binaryBuffer, hexBuffer);

                toBinaryString(p2KingBoard, binaryBuffer, sizeof(binaryBuffer));
                binaryToHexString(p2KingBoard, hexBuffer, sizeof(hexBuffer));
                printf("Player B King Board (Binary): %s\nPlayer B King Board (Hex): %s\n", binaryBuffer, hexBuffer);

                break;

            case MENU_EXIT:
                printf("Exiting the game.\n");
                gameOver = true;
                keepPlaying = false; // Exit the outer loop as well
                break;

            default:
                printf("Invalid choice. Please select a valid option from the menu.\n");
            }
        }

        if (gameOver)
        {
            keepPlaying = promptToPlayAgain();
            if (keepPlaying)
            {
                printf("\nStarting a new game...\n");
            }
            else
            {
                printf("Exiting.\n");
            }
        }
    }

    return 0;
}


///@brief Prompts the user to decide whether to play again or quit after a game ends
/// @return true if the user wants to play again; false otherwise
bool promptToPlayAgain()
{
    int choice;
    printf("\nWould you like to play again?\n");
    printf("1. Yes\n");
    printf("2. No\n");
    choice = promptUserForInteger("Enter your choice: ");

    while (choice != 1 && choice != 2)
    {
        printf("Invalid choice. Please enter 1 for Yes or 2 for No.\n");
        choice = promptUserForInteger("Enter your choice: ");
    }

    return (choice == 1);
}

/// @brief Displays the current state of the board in an 8x8 ASCII grid.
void displayBoard()
{
    printf("  ---------------------------------\n");
    printf("    1 2 3 4 5 6 7 8\n");
    int bitIndex = 0;

    for (int row = 1; row <= 8; row++)
    {
        printf("%d | ", row); // print row number
        for (int col = 1; col <= 8; col++)
        {
            if ((row + col) % 2 == 1)
            { // if it is odd then it's playable

                int currentBitIndex = bitIndex;
                int bitPos = 31 - currentBitIndex; // printing in reverse order since we are starting our printing from the top right

                // check all four bitboards for a piece at this position
                int piecesFound = 0;
                char piece = '.';
                if (getBitValue(p1PeonBoard, bitPos))
                {
                    piecesFound++;
                    piece = 'a';
                }
                if (getBitValue(p1KingBoard, bitPos))
                {
                    piecesFound++;
                    piece = 'A';
                }
                if (getBitValue(p2PeonBoard, bitPos))
                {
                    piecesFound++;
                    piece = 'b';
                }
                if (getBitValue(p2KingBoard, bitPos))
                {
                    piecesFound++;
                    piece = 'B';
                }

                if (piecesFound > 1)
                {
                    piece = 'E'; // E for error
                }

                printf("%c ", piece);
                bitIndex++;
            }
            else
            { // it is a white tile we can skip checking and print dot.

                printf(". ");
            }
        }
        printf("|\n");
    }
    printf("  -------------------\n");
    printf("    1 2 3 4 5 6 7 8\n");
}

/// @brief Executes a move from startPos to endPos if it's valid.
///        Updates the board state accordingly.
/// @param startPos Starting position index (0-63).
/// @param endPos Ending position index (0-63).
/// @return true if the move was successfully made; false otherwise.
bool makeMove(uint32_t startPos, uint32_t endPos)
{
    if (!validateMove(startPos, endPos))
    {
        return false;
    }

    int startCol;
    updateCol(&startCol, startPos); // used in determining valid hop
    int endRow;
    updateRow(&endRow, endPos); // get row number to determine if a promotion will be needed

    // re-interpret positions for 32 tile index positions and consider black tiles only.
    convertPositionsTo32TileFormat(&startPos, &endPos);

    int moveDistance = startPos - endPos;

    int player = 0;
    bool isKing;
    if (!determinePieceAndPlayer(startPos, &player, &isKing))
        return false;
    if (player == 1)
    {
        if (isHop(player, isKing, moveDistance))
        {
            int hoppedTileIndex = validateHop(player, isKing, moveDistance, startPos, startCol);
            if (hoppedTileIndex)
            {
                // having verified there is only 1 enemy piece in hoppedTileIndex already with validateHop
                // we can safely clear both boards of the opponent without determined which board the hopped piece was on
                clearBit(&p2PeonBoard, hoppedTileIndex);
                clearBit(&p2KingBoard, hoppedTileIndex);
                if (isKing)
                {
                    clearBit(&p1KingBoard, startPos); // clear player 1's king piece's original position
                    setBit(&p1KingBoard, endPos);     // set player 1's king piece's new position
                }
                else
                {
                    clearBit(&p1PeonBoard, startPos); // clear player 1's non-king piece's original position
                    setBit(&p1PeonBoard, endPos);     // set player 1's non-king piece's new position
                }
            }
            else
            {
                return false; // validateHop indicated invalid
            }
        }
        else // move is valid and not a hop
        {
            if (isKing)
            {
                clearBit(&p1KingBoard, startPos); // clear player 1's king piece's original position
                setBit(&p1KingBoard, endPos);     // set player 1's king piece's new position
            }
            else
            {
                clearBit(&p1PeonBoard, startPos); // clear player 1's non-king piece's original position
                setBit(&p1PeonBoard, endPos);     // set player 1's non-king piece's new position
            }
        }
    }
    else if (player == 2)
    {
        if (isHop(player, isKing, moveDistance))
        {
            int hoppedTileIndex = validateHop(player, isKing, moveDistance, startPos, startCol);
            if (hoppedTileIndex)
            {
                // having verified there is only 1 enemy piece in hoppedTileIndex already with validateHop
                // we can safely clear both boards of the opponent without determined which board the hopped piece was on
                clearBit(&p1PeonBoard, hoppedTileIndex);
                clearBit(&p1KingBoard, hoppedTileIndex);
                if (isKing)
                {
                    clearBit(&p2KingBoard, startPos); // clear player 2's king piece's original position
                    setBit(&p2KingBoard, endPos);     // set player 2's king piece's new position
                }
                else
                {
                    clearBit(&p2PeonBoard, startPos); // clear player 2's non-king piece's original position
                    setBit(&p2PeonBoard, endPos);     // set player 2's non-king piece's new position
                }
            }
            else
            {
                return false; // validateHop indicated invalid
            }
        }
        else // move is valid and not a hop
        {
            if (isKing)
            {
                clearBit(&p2KingBoard, startPos); // clear player 2's king piece's original position
                setBit(&p2KingBoard, endPos);     // set player 2's king piece's new position
            }
            else
            {
                clearBit(&p2PeonBoard, startPos); // clear player 2's non-king piece's original position
                setBit(&p2PeonBoard, endPos);     // set player 2's non-king piece's new position
            }
        }
    }

    if ((endRow == 8 && player == 1 && !isKing) || (endRow == 1 && player == 2 && !isKing)) promoteToKing(player, endPos);

    return true;
}

/// @brief Promotes a peon to a king given the player and position.
///        Determines piece and owner to promote from position.
/// @param pos Position index (0-31) of the peon to be promoted.
void promoteToKing(int player, uint32_t pos)
{
    if (player == 1)
    {
        clearBit(&p1PeonBoard, pos);
        setBit(&p1KingBoard, pos);
    }
    else
    {
        clearBit(&p2PeonBoard, pos);
        setBit(&p2KingBoard, pos);
    }
}
/// @brief Resets the board state for both players.
void resetGame()
{
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
    // check board boundaries using the 64 tile index positions
    if (startPos > 63 || endPos > 63)
        return false;

    // initalized row and column values using 64 tile index positions.
    int startRow;
    int startCol;
    int endRow;
    int endCol;
    updateRowAndCol(&startRow, &startCol, startPos);
    updateRowAndCol(&endRow, &endCol, endPos);

    if ((startRow + startCol) % 2 == 0 || (endRow + endCol) % 2 == 0)
        return false; // if positions are white tiles

    // re-interpret positions for 32 tile index positions and consider black tiles only.
    convertPositionsTo32TileFormat(&startPos, &endPos);

    int moveDistance = startPos - endPos;

    // determine player and piece type
    int player;
    bool isKing;
    if (!determinePieceAndPlayer(startPos, &player, &isKing))
        return false;

    // check if the end position is occupied
    if (isOccupied(endPos))
        return false;

    if (isHop(player, isKing, moveDistance)) // check if it is a hop
    {
        return validateHop(player, isKing, moveDistance, startPos, startCol); // return true if the hop is valid
    }
    else
    {
        return validateNormalMove(moveDistance, player, isKing, startCol); // return true if the normal move is valid.
    }

    return false; // otherwise move is not valid.
}

/// @brief Simple check to see if all pieces have been eliminated from a players board, to be called after every capture.
/// @return '2' if player 2 won, '1' if player 1 won, and '0' if neither player has won.
int checkVictory()
{
    if (p1KingBoard + p1PeonBoard == 0)
    {
        return 2; // player 2 won
    }
    if (p2KingBoard + p2PeonBoard == 0)
    {
        return 1; // player 1 won
    }
    return 0; // no player has won yet.
}

/// @brief Checks if the move is a hop given the player, piece, and (0-32) move distance.
/// @param player player of the piece being checked
/// @param isKing type of piece being checked
/// @param moveDistance distanced of move being checked
/// @return true the distance matches a valid hop distance for the player and piece type.

bool isHop(int player, bool isKing, int moveDistance)
{
    if (player == 1 && isKing && (moveDistance == 9 || moveDistance == -7))
        return true;
    else if (player == 1 && !(isKing) && (moveDistance == 9))
        return true;
    else if (player == 2 && isKing && (moveDistance == -7 || moveDistance == 9))
        return true;
    else if (player == 2 && !(isKing) && (moveDistance == -7 || moveDistance == -9))
        return true;
    else
        return false;
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
/// @return true if exactly one piece is found; false otherwise. If player and is unchanged, then no pieces were found.
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

/// @brief Checks if a move is a valid hop over an opponent's piece. Returns the index of the piece being hopped over, if valid.
/// @param moveDistance the calculated move distance.
/// @param player the player making the move.
/// @param isKing whether the moving piece is a king.
/// @param startPos the starting position index (0-32).
/// @param startCol the column of the starting piece
/// @return index of piece being hopped over if the hop is valid; false if the move, startCol, or board states are not valid.
int validateHop(int player, bool isKing, int moveDistance, uint32_t startPos, int startCol)
{
    uint32_t hoppedPos;

    if (!isKing)
    {
        switch (startCol) // for columns 1 through 8
        {
        case 1:
            if (player == 1) // Column 1, player 1 peon.
            {
                if (moveDistance == 9)
                {
                    hoppedPos = startPos - 4;
                }
                else
                {
                    return false;
                }
            }
            else // Column 1, player 2 peon.
            {
                if (moveDistance == -7)
                {
                    hoppedPos = startPos + 4;
                }
                else
                {
                    return false;
                }
            }
            break;

        case 2:

            if (player == 1) // Column 2, player 1 peon.
            {
                if (moveDistance == 9)
                {
                    hoppedPos = startPos - 5;
                }
                else
                {
                    return false;
                }
            }
            else // Column 2, player 2 peon.
            {
                if (moveDistance == -7)
                {
                    hoppedPos = startPos + 3;
                }
                else
                {
                    return false;
                }
            }
            break;
        case 5: // logic is the same for odd numbered positions that are not on an edge
        case 3:
            if (player == 1) // Column 3, player 1 peon.
            {
                if (moveDistance == 7 || moveDistance == 9)
                {
                    hoppedPos = (moveDistance == 7) ? (startPos - 3) : (startPos - 4);
                }
                else
                {
                    return false;
                }
            }
            else // Column 3, player 2 peon.
            {
                if (moveDistance == -7 || moveDistance == -9)
                {
                    hoppedPos = (moveDistance == -7) ? (startPos + 4) : (startPos + 5);
                }
                else
                {
                    return false;
                }
            }
            break;
        case 6: // even cases have the same movement logic for peices not on an edge
        case 4:
            if (player == 1) // Column 4, player 1 peon.
            {
                if (moveDistance == 7 || moveDistance == 9)
                {
                    hoppedPos = (moveDistance == 7) ? (startPos - 4) : (startPos - 5);
                }
                else
                {
                    return false;
                }
            }
            else // Column 4, player 2 peon.
            {
                if (moveDistance == -7 || moveDistance == -9)
                {
                    hoppedPos = (moveDistance == -7) ? (startPos + 3) : (startPos + 4);
                }
                else
                {
                    return false;
                }
            }
            break;

        case 7:

            if (player == 1) // Column 7, player 1 peon.
            {
                if (moveDistance == 7)
                {
                    hoppedPos = (startPos - 3);
                }
                else
                {
                    return false;
                }
            }
            else // Column 7, player 2 peon.
            {
                if (moveDistance == -9)
                {
                    hoppedPos = startPos + 5;
                }
                else
                {
                    return false;
                }
            }
            break;

        case 8:

            if (player == 1) // Column 8, player 1 peon.
            {
                if (moveDistance == 7)
                {
                    hoppedPos = startPos - 4;
                }
                else
                {
                    return false;
                }
            }
            else // Column 8, player 2 peon.
            {
                if (moveDistance == -9)
                {
                    hoppedPos = startPos + 4;
                }
                else
                {
                    return false;
                }
            }
            break;

        default: // column number is invalid.
            return false;
        }
    }
    else // it is a king piece, player does not need to be consdiered for these as both directions are valid for both players.
    {
        switch (startCol)
        {
        case 1: // Column 1

            if (moveDistance == 9 || moveDistance == -7)
            {
                hoppedPos = (moveDistance == 9) ? (startPos - 4) : (startPos + 4);
            }
            else
            {
                return false;
            }

            break;

        case 2: // Column 2
            if (moveDistance == 9 || moveDistance == -7)
            {
                hoppedPos = (moveDistance == 9) ? (startPos - 5) : (startPos + 3);
            }
            else
            {
                return false;
            }

            break;
        case 5: // Column 5, same logic as case 3.
        case 3: // Column 3
            if (moveDistance == -9 || moveDistance == -7 || moveDistance == 7 || moveDistance == 9)
            {
                if (moveDistance > 0)
                {
                    hoppedPos = (moveDistance == 7) ? (startPos - 3) : (startPos - 4);
                }
                else
                {
                    hoppedPos = (moveDistance == -7) ? (startPos + 4) : (startPos + 5);
                }
            }
            else
            {
                return false;
            }

            break;
        case 6: // Column 6, same logic as case 4.
        case 4: // Column 4
            if (moveDistance == -9 || moveDistance == -7 || moveDistance == 7 || moveDistance == 9)
            {
                if (moveDistance > 0)
                {
                    hoppedPos = (moveDistance == 7) ? (startPos - 4) : (startPos - 5);
                }
                else
                {
                    hoppedPos = (moveDistance == -7) ? (startPos + 3) : (startPos + 4);
                }
            }
            else
            {
                return false;
            }

            break;
        case 7: // Column 7
            if (moveDistance == -9 || moveDistance == 7)
            {
                hoppedPos = (moveDistance == 7) ? (startPos - 3) : (startPos + 5);
            }
            else
            {
                return false;
            }
            break;

        case 8: // Column 8
            if (moveDistance == 7 || moveDistance == -9)
            {
                hoppedPos = (moveDistance == 7) ? (startPos - 4) : (startPos + 4);
            }
            else
            {
                return false;
            }

            break;

        default:
            return false;
        }

        if (hoppedPos < 0 || hoppedPos > 31) // this should not occur as the validate hop method already checks to ensure position and moveDistance is valid
        {
            return false;
        }

        bool enemyOccupied = false;
        bool friendlyOccupied = false;

        if (player == 1)
        {
            // check if enemy (player 2) occupies the hopped position
            enemyOccupied = (getBitValue(p2PeonBoard, hoppedPos) + getBitValue(p2KingBoard, hoppedPos)) == 1;
            // check if friendly (player 1) occupies the hopped position
            friendlyOccupied = getBitValue(p1PeonBoard, hoppedPos) || getBitValue(p1KingBoard, hoppedPos);
        }
        else
        {
            // check if enemy (player 1) occupies the hopped position
            enemyOccupied = (getBitValue(p1PeonBoard, hoppedPos) + getBitValue(p1KingBoard, hoppedPos)) == 1;
            // check if friendly (player 2) occupies the hopped position
            friendlyOccupied = getBitValue(p2PeonBoard, hoppedPos) || getBitValue(p2KingBoard, hoppedPos);
        }

        // Validate that exactly one enemy piece is present and no friendly pieces
        if (enemyOccupied && !friendlyOccupied)
        {
            return hoppedPos;
        }

        return false;
    }
}

/// @brief Validates if the move distance is allowed for the player's piece based on the current column.
/// @param moveDistance the distance of the move.
/// @param player the player that owns the piece making the move (1 or 2).
/// @param isKing whether the piece being moved is a king.
/// @param column the current column of the piece being moved (1-8).
/// @return true if the move distance is valid; false otherwise.
bool validateNormalMove(int moveDistance, int player, bool isKing, int startCol)
{
    if (!isKing)
    {
        switch (startCol)
        {
        case 8: // edge columns have the same movement logic for pawns
        case 1:

            if (moveDistance == 4 || moveDistance == -4)
            {
                return (player == 1) ? (moveDistance == 4) : (moveDistance == -4);
            }
            break;
        case 6: // even cases of pieces not against the edge have the same logic
        case 4:
        case 2:
            if (moveDistance == 4 || moveDistance == 5 || moveDistance == -3 || moveDistance == -4)
            {
                return (player == 1) ? (moveDistance > 0) : (moveDistance < 0);
            }
            break;
        case 7: // odd cases not against the edge have the same logic
        case 5:
        case 3:
            if (moveDistance == 3 || moveDistance == 4 || moveDistance == -4 || moveDistance == -5)
            {
                return (player == 1) ? (moveDistance > 0) : (moveDistance < 0);
            }
            break;
        default:
            return false;
        }
    }
    else
    {
        switch (startCol)
        {
        case 8: // edge columns have the same movement logic for pawns
        case 1:

            if (moveDistance == 4 || moveDistance == -4)
            {
                return true;
            }
            break;
        case 6: // even cases of pieces not against the edge have the same logic
        case 4:
        case 2:
            if (moveDistance == 4 || moveDistance == 5 || moveDistance == -3 || moveDistance == -4)
            {
                return true;
            }
            break;
        case 7: // odd cases not against the edge have the same logic
        case 5:
        case 3:
            if (moveDistance == 3 || moveDistance == 4 || moveDistance == -4 || moveDistance == -5)
            {
                return true;
            }
            break;

        default:
            return false;
        }
    }
    // If none of the conditions are met, the move distance is invalid
    return false;
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

/// @brief Converts startPos and endPos from 0-63 to 0-31 for compatability with board validation.
/// @param startPos Pointer to the start position index (0-63).
/// @param endPos Pointer to the end position index (0-63).
void convertPositionsTo32TileFormat(uint32_t *startPos, uint32_t *endPos)
{
    if (startPos != NULL)
    {
        *startPos = (*startPos / 8) * 4 + ((*startPos % 8) / 2);
    }
    if (endPos != NULL)
    {
        *endPos = (*endPos / 8) * 4 + ((*endPos % 8) / 2);
    }
}

/// @brief Converts a position from 0-63 to 0-31 for compatability with board validation.
/// @param startPos Pointer to the start position index (0-63).
/// @param endPos Pointer to the end position index (0-63).
void convertSinglePositionTo32TileFormat(uint32_t *pos)
{
    if (pos != NULL)
    {
        *pos = (*pos / 8) * 4 + ((*pos % 8) / 2);
    }
}

/// @brief Takes a 0-63 position and updates row and column numbers accordinly.
/// @param row row value to be updated
/// @param col column value to be updated
/// @param pos position to determine row and col values.
/// @return ture if sucessful, or false if the position is invalid.
bool updateRowAndCol(int *row, int *col, uint32_t pos)
{
    if (pos >= 64)
    {
        return false;
    }

    *row = (63 - pos) / 8 + 1;
    *col = (63 - pos) % 8 + 1;

    return true;
}
/// @brief Takes a 0-63 position and updates the row column number accordinly.
/// @param row row value to be updated
/// @param pos position to determine row and col values.
/// @return ture if sucessful, or false if the position is invalid.
bool updateRow(int *row, uint32_t pos)
{
    if (pos >= 64)
    {
        return false;
    }

    *row = (63 - pos) / 8 + 1;

    return true;
}
/// @brief Takes a 0-63 position and updates the column number accordinly.
/// @param col column value to be updated
/// @param pos position to determine row and col values.
/// @return ture if sucessful, or false if the position is invalid.
bool updateCol(int *col, uint32_t pos)
{
    if (pos >= 64)
    {
        return false;
    }

    *col = (63 - pos) % 8 + 1;

    return true;
}

/// @brief Displays the given prompt message and then gets a valid integer from the user.
/// @param prompt the message to display to the user.
/// @return the integer entered by the user.
int promptUserForInteger(const char *prompt)
{
    int input;
    char c;
    while (1)
    {
        printf("%s", prompt);

        if (scanf("%d", &input) == 1)
        {
            while ((c = getchar()) != '\n' && c != EOF)
                ;
            return input;
        }
        else
        {
            while ((c = getchar()) != '\n' && c != EOF)
                ;
            printf("Invalid input, please enter a valid integer.\n");
        }
    }
}

/// @brief Simple method that interprets 1 as A and 2 and B to make the menu prompt consistent with the letters being used to represent their pieces on the board.
/// @param player number of the current player
/// @return A or B depending on the value of player (1 or 2), otherwise returns '?'.
char getPlayerChar(int player) {
    switch (player) {
        case 1:
            return 'A';
        case 2:
            return 'B';
        default:
            return '?'; 
    }
}