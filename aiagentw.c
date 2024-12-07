#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "aiagent.h"
#include "board.h"
#include "agent.h"
#include "Dict.h"

static void dictPrint(const char *key, void *value)
{
    FILE *file = fopen("boards.txt", "a");
    if (file == NULL)
    {
        perror("Failed to open file");
        return;
    }
    fprintf(file, "Board: %s\nScore : %d\n", key, (int)(intptr_t)value);
    fclose(file);
}

static Move agentPlay(Agent *agent, Board board)
{
    // Basic variabes.
    int bestScore = -2;
    Move bestMove = -1;

    for (Move m = 0; m < 9; m++)
        if (boardValidMove(board, m))
        {
            Board nextBoard = boardCopy(board);
            boardNext(nextBoard, m, agentGetPlayer(agent));

            int score = -GetMinimaxScore(agent, board);

            if (score > bestScore)
            {
                bestScore = score;
                bestMove = m;
            }
        }

    return bestMove;
}

static Move AIPlay(Agent *agent, Board board)
{
    int bestScore = -2;
    Move bestMove = -1;
    int moves[9];
    int score;
    int moves_index = 0;

    for (int k = 0; k < 9; k++)
        moves[k] = -2;

    for (Move m = 0; m < 9; m++)
    {
        if (boardValidMove(board, m))
        {
            Board next = boardNext(boardCopy(board), m, boardGetPlayer(board));
            score = (int)dictSearch(agentGetData(agent), next);
            if (score > bestScore)
            {
                for (int k = 0; k < 9; k++)
                    moves[k] = -2;
                bestScore = score;
                bestMove = m;
                moves_index = 0;
                moves[moves_index] = (int)m;
            }
            if (score == bestScore)
                moves[moves_index++] = m;
        }
    }

    if (moves_index > 1) // If there is only one
    {
        int r = rand() % moves_index;
        while (moves[r] == -2)
            r = rand() % moves_index;
        printf("Picking a random move: %d\n", moves[r]);
        return moves[r];
    }
    printf("bestMove: %d\n", bestMove);
    return bestMove;
}

static void noEnd(Agent *agent, Board board, Player player)
{
    (void)player, (void)agent, (void)board;
    dictIterate(agentGetData(agent), dictPrint);
    return;
}

static void noFree(void *p)
{
    (void)p;
    return;
}

// Now we generate all possible boards in advance.
static int generateDicts(Player player, Board b, Dict *memory)
{
    Player winner = boardWin(b);

    if (winner == player && winner != E)
    {
        dictInsert(memory, b, (void *)1);
        return 1;
    }

    if (boardIsFull(b) && winner == E)
    {
        dictInsert(memory, b, (void *)0);
        return 0;
    }

    if (winner != player && winner != E)
    {
        dictInsert(memory, b, (void *)-1);
        return -1;
    }

    int bestScore = -2;
    int score;
    for (Move m = 0; m < 9; m++)
    {
        if (boardValidMove(b, m))
        {
            Board nextBoard = boardNext(boardCopy(b), m, boardGetPlayer(b));

            if (dictContains(memory, nextBoard))
            {
                score = dictSearch(memory, nextBoard);
            }
            else
            {
                score = -generateDicts(boardGetPlayer(nextBoard), nextBoard, memory);
                dictInsert(memory, nextBoard, (void *)score);
            }

            if (score > bestScore)
            {
                bestScore = score;
            }
        }
    }

    return bestScore;
}

Agent *createAiAgent(void) // Creates the agent
{
    // We create a dictionnary of size 12000. That value was given to us by the teacher.
    Dict *memory = dictCreate(12000);

    Board board = malloc(10 * sizeof(char));
    for (int i = 0; i < 9; i++)
        board[i] = E;

    if (!memory)
    {
        printf("Can't allocate the memory needed for the dictionnary\n");
        exit(1); // Error, close program.
    }

    generateDicts(X, board, memory);

    Agent *agent = agentCreate("AI", AIPlay, noEnd, noFree);

    // We link the dictionnary to the agent.
    agentSetData(agent, memory);
    return agent;
}

int GetMinimaxScore(Agent *agent, Board b)
{
    Dict *memory = agentGetData(agent);
    if (!dictContains(memory, b))
    {
        return 0;
    }
    return -(int)dictSearch(memory, b);
}
