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

static void noEnd(Agent *agent, Board board, Player player)
{

    (void)agent, (void)player, (void)board;
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

int GetMinimaxScore(Agent *agent, Board b)
{

    // We first check to see if there is a winner.
    if (boardWin(b) != E)
        return (boardWin(b) == agentGetPlayer(agent)) ? 1 : -1; // If the winner is the agent passed in parameter -> 1 if not -1.

    // We check if there is a tie, by looking if the board is full or not.
    if (boardIsFull(b))
        return 0;

    // If we reach here, it means we either don't have a clear winner, or we don't have tie.
    // We are now going to look after the optimal score.
    // We first check if the board exists in the memory(dictionnary) of our agent, if yes -> return the score, if not -> continue
    Dict *memory = agentGetData(agent);

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
            Board nextBoard = boardCopy(b);
            boardNext(nextBoard, m, agentGetPlayer(agent));

            // We compute the score of that new board.
            int score = -GetMinimaxScore(agent, nextBoard);

            // We compare the score. We keep the highest score.
            if (score > bestScore)
                bestScore = score;
        }
    }
    // We save boards as they come and go.
    int *scoreSave = malloc(sizeof(int));
    *scoreSave = bestScore;
    dictInsert(memory, b, scoreSave);

    return bestScore;
}
