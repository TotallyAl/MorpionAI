
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "board.h"

Board boardMakeEmpty(void)
{
    char *b = malloc(10 * sizeof(char));
    for (int i = 0; i < 9; i++)
        b[i] = E;
    b[9] = '\0';
    return b;
}

void boardFree(Board b)
{
    free(b);
}

Board boardCopy(Board b)
{
    Board newb = malloc(10 * sizeof(b));
    strcpy(newb, b);
    return newb;
}

void boardPrint(Board b)
{
    printf("_______\n");
    printf("|%c|%c|%c|\n", b[P11], b[P12], b[P13]);
    printf("-------\n");
    printf("|%c|%c|%c|\n", b[P21], b[P22], b[P23]);
    printf("-------\n");
    printf("|%c|%c|%c|\n", b[P31], b[P32], b[P33]);
    printf("-------\n");
}

Player boardWin(Board b)
{
    if (b[P11] != E && b[P11] == b[P12] && b[P11] == b[P13])
        return b[P11];
    else if (b[P21] != E && b[P21] == b[P22] && b[P21] == b[P23])
        return b[P21];
    else if (b[P31] != E && b[P31] == b[P32] && b[P31] == b[P33])
        return b[P31];
    else if (b[P11] != E && b[P11] == b[P21] && b[P11] == b[P31])
        return b[P11];
    else if (b[P12] != E && b[P12] == b[P22] && b[P12] == b[P32])
        return b[P12];
    else if (b[P13] != E && b[P13] == b[P23] && b[P13] == b[P33])
        return b[P13];
    else if (b[P11] != E && b[P11] == b[P22] && b[P11] == b[P33])
        return b[P11];
    else if (b[P13] != E && b[P13] == b[P22] && b[P13] == b[P31])
        return b[P13];
    else
        return E;
}

bool boardIsFull(Board b)
{
    for (int i = 0; b[i] != '\0'; i++)
        if (b[i] == E)
            return false;
    return true;
}

Board boardNext(Board b, Move m, Player p)
{
    if (!boardValidMove(b, m))
    {
        printf("boardNext: Invalid move, cell %d is not empty\n", m);
        exit(1);
    }
    b[m] = p;
    return b;
}

Player boardGetPlayer(Board b)
{
    if (boardIsFull(b))
        return E;
    int nbX = 0;
    int nbO = 0;
    for (int i = 0; i < 9; i++)
    {
        if (b[i] == X)
            nbX++;
        else if (b[i] == O)
            nbO++;
    }
    if (nbX == nbO)
        return X;
    else if (nbX == nbO + 1)
        return O;
    else
    {
        printf("boardGetPlayer: the board is invalid.\n");
        exit(1);
    }
}

bool boardValidMove(Board b, Move m)
{
    return (b[m] == E);
}
