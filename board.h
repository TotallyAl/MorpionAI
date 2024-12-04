#ifndef _BOARD_H_
#define _BOARD_H_

#include <stdbool.h>

/**
 * @brief Represent a whole board by a null terminated string (NB: strlen(Board) is always 9)
 */
typedef char *Board;

/**
 * @brief enum to represent a cell of the board
 *
 * @enum X : the first player
 * @enum O : the second player
 * @enum E : an empty cell
 */
typedef enum
{
    X = 'X',
    O = 'O',
    E = ' ',
} Player;

/**
 * @brief enum to represent all possible moves in a human readable way
 */
typedef enum
{
    P11 = 0,
    P12 = 1,
    P13 = 2,
    P21 = 3,
    P22 = 4,
    P23 = 5,
    P31 = 6,
    P32 = 7,
    P33 = 8,
} Move;

/**
 * @brief create an empty board
 * 
 * @returns a newly allocated board with E in all its cell
 */
Board boardMakeEmpty(void);

/**
 * @brief deallocate the memory associated to a board
 */
void boardFree(Board b);

/**
 * @brief create (allocate) a new board with the same content as this once
 * 
 * @param b original board
 * 
 * @returns copy of the original board
 */
Board boardCopy(Board b);

/**
 * @brief shows the board on the standard output
 */
void boardPrint(Board b);

/**
 * @brief detect if a player has won the game
 * 
 * NOTE E is returned if the game is not finished, and if the game is a tie.
 * You may need to use boardWin and boardIsFull to accurately detect a game over.
 * 
 * @param b board to evaluate
 * 
 * @returns the winning player (if any) or E
 */
Player boardWin(Board b);

/**
 * @brief detect if a board is full
 * 
 * NOTE a game can be over even if it is not full.
 * You may need to use boardWin and boardIsFull to accurately detect a game over.
 * 
 * @param b board to evaluate
 * 
 * @returns true if there are no empty cell left
 */
bool boardIsFull(Board b);

/**
 * @brief update the board to make a move
 * 
 * NOTE there is no verification that the player can play on this board.
 * It is possible to fill a grid with X using this method, use it appropriately.
 * 
 * @param b board to be updated
 * @param m move to take
 * @param p player to make the move (X or O)
 * 
 * @returns the same board that was received in argument
 */
Board boardNext(Board b, Move m, Player p);

/**
 * @brief asses the validity of a move on a board
 * 
 * @param b board in which the move should be assessed
 * @param m move to be made on the board
 * 
 * @returns true if the move is legal
 */
bool boardValidMove(Board b, Move m);

/**
 * @brief detects the next player of the board
 * 
 * @returns the next player to play on the board
 */
Player boardGetPlayer(Board b);

#endif // _BOARD_H_
