#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "aiagent.h"
#include "board.h"
#include "agent.h"
#include "Dict.h"

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

static Move AiAgentPlay(Agent *agent, Board board)
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
            score = GetMinimaxScore(agent, next);
            if (score > bestScore)
            {
                for (int k = 0; k < 9; k++)
                    moves[k] = -2;
                bestScore = score;
                bestMove = m;
                moves_index = 0;
                moves[moves_index] = (int)m;
                moves_index++;
            }
            if (score == bestScore)
            {
                moves[moves_index] = m;
                moves_index++;
            }
        }
    }

    if (moves[0] != -2) // If there is only one
    {
        printf("Picking a random move\n");
        int r = rand() % moves_index;
        while (moves[r] == -2)
            r = rand() % moves_index;
        return moves[r];
    }

    return bestMove;
}

static void noEnd(Agent *agent, Board board, Player player)
{
    // (void)board, (void)player, (void)agent;
    (void)player, (void)agent;
    // printf("The game ended\n");
    // printf("Board: |%s|\n", board);
    // printf("Winner: %c\n", boardWin(board));
    // boardPrint(board);
    return;
}

static void noFree(void *p)
{
    (void)p;
    return;
}

Agent *createAiAgent(void) // Creates the agent
{
    // We create a dictionnary of size 12000. That value was given to us by the teacher.
    Dict *memory = dictCreate(12000);
    if (!memory)
    {
        printf("Can't allocate the memory needed for the dictionnary\n");
        exit(1); // Error, close program.
    }

    Agent *agent = agentCreate("Minimax agent", agentPlay, noEnd, noFree);

    // We link the dictionnary to the agent.
    agentSetData(agent, memory);
    return agent;
}

static int GetMinimaxScoreCopy(Player player, Board b, Dict *memory)
{
    Player winner = boardWin(b);

    // We first check to see if there is a winner.
    if (winner == player)
    {
        dictInsert(memory, b, (void *)1);
        return 1;
    }

    // We check if the opponent wins
    if (winner != player && winner != E)
    {
        dictInsert(memory, b, (void *)-1);
        return -1;
    }

    // We check if there is a tie, by looking if the board is full or not.
    if (boardIsFull(b) && winner == E)
    {
        dictInsert(memory, b, (void *)0);
        return 0;
    }

    // If we reach here, it means we either don't have a clear winner, or we don't have tie.
    // We are now going to look after the optimal score.
    // We first check if the board exists in the memory(dictionnary) of our agent, if yes -> return the score, if not -> continue

    if (memory == NULL)
    {
        printf("There is no memory linked to the agent.\n");
        exit(1);
    }

    if (dictContains(memory, b))
    {
        // The key exists, so it means the board is in the dictionnary.
        int *score = dictSearch(memory, b); // This returns the score associated to the board.
        if (score != NULL)
            return *score;
    }

    // The board couldn't be found inside the dictionnary.

    int bestScore = -2; // We choose a number that is different from -1, 0 and 1.
    for (Move m = 0; m < 9; m++)
    {
        if (boardValidMove(b, m))
        {
            // We create a new board that will contain the change.
            // Board nextBoard = boardCopy(b);
            // boardNext(nextBoard, m, agentGetPlayer(agent));

            Board next = boardNext(boardCopy(b), m, boardGetPlayer(b));

            // We compute the score of that new board.
            // printf("board: %s\n", next);
            int score = -GetMinimaxScoreCopy(boardGetPlayer(next), next, memory);

            // We compare the score. We keep the highest score.
            if (score > bestScore)
                bestScore = score;
        }
    }
    // We save boards as they come and go.
    int *scoreSave = malloc(sizeof(int));
    *scoreSave = bestScore;
    dictInsert(memory, b, scoreSave);
    // printf("Board:%s||Score: %d\n", b, *scoreSave);

    return bestScore;
}

int GetMinimaxScore(Agent *agent, Board b)
{
    return GetMinimaxScoreCopy(agentGetPlayer(agent), b, agentGetData(agent));
}
