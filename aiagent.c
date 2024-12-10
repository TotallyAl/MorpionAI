#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "aiagent.h"
#include "board.h"
#include "agent.h"
#include "Dict.h"

static int computeBoardsScores(Player player, Board board, Dict *memory)
{
    // We get the potential winner of the board to check if there is a winner or a tie.
    Player winner = boardWin(board);

    // Does the player passed as parameter win ?
    if (winner == player && winner != E)
    {
        dictInsert(memory, board, (void *)1);
        return 1;
    }

    // Does the opponent win ?
    if (winner != player && winner != E)
    {
        dictInsert(memory, board, (void *)-1);
        return -1;
    }

    if (winner == E && boardIsFull(board))
    {
        dictInsert(memory, board, (void *)0);
        return 0;
    }

    // We generate all the boards.
    int maxScore = -2;
    int score;
    for (Move m = 0; m < 9; m++)
    {
        if (boardValidMove(board, m))
        {
            // We play the move as the player passed by the parameters
            Board nextBoard = boardNext(boardCopy(board), m, player);

            if (dictContains(memory, nextBoard))
            {
                int *tempScore = (int *)dictSearch(memory, nextBoard);
                score = *tempScore;
            }

            else
            {
                score = -computeBoardsScores(boardGetPlayer(nextBoard), nextBoard, memory);
                int *scorePtr = malloc(sizeof(int));
                *scorePtr = score;
                dictInsert(memory, nextBoard, (void *)scorePtr);
            }

            if (score > maxScore)
                maxScore = score;
        }
    }

    return maxScore;
}

static Move AIPlay(Agent *agent, Board board)
{
    int maxScore = -2;
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
            int *tempScore = (int *)dictSearch(agentGetData(agent), next);
            score = *tempScore;
            if (score > maxScore)
            {
                for (int k = 0; k < 9; k++)
                    moves[k] = -2;
                maxScore = score;
                bestMove = m;
                moves_index = 0;
                moves[moves_index] = (int)m;
            }
            else if (score == maxScore)
                moves[moves_index++] = m;
        }
    }
    if (moves_index > 1) // If there is only one
    {
        int r = rand() % moves_index;
        while (moves[r] == -2)
            r = rand() % moves_index;
        // printf("Picking a random move: %d\n", moves[r]);
        return moves[r];
    }
    // printf("bestMove: %d\n", bestMove);
    return bestMove;
}

static void AIEnd(Agent *agent, Board board, Player player)
{
    (void)agent, (void)board, (void)player;
    return;
}

static void AIFree(void *param)
{
    dictFree((Dict *)param);
    return;
}

Agent *createAiAgent(void) // Creates the agent
{

    Dict *memory = dictCreate(12000);

    // Board initialBoard = malloc(10 * sizeof(char));

    // for (int k = 0; k < 9; k++)
    //     initialBoard[k] = E;

    // initialBoard[9] = '\0';

    Board initialBoard = boardMakeEmpty();

    if (!memory)
    {
        printf("Dict couldn't be created, exiting.\n");
        free(initialBoard);
        dictFree(memory);
        exit(1);
    }

    Agent *minimaxAI = agentCreate("Minimax AI", AIPlay, AIEnd, AIFree);
    if (!minimaxAI)
    {
        printf("Failed to create minimax agent, exiting.\n");
        free(initialBoard);
        dictFree(memory);
        exit(1);
    }

    agentSetData(minimaxAI, memory);

    GetMinimaxScore(minimaxAI, initialBoard);

    free(initialBoard);

    return minimaxAI;
}

int GetMinimaxScore(Agent *agent, Board b)
{
    computeBoardsScores(agentGetPlayer(agent), b, agentGetData(agent));
    return -42;
}
