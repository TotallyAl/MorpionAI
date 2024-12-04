#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "aiagent.h"
#include "board.h"
#include "agent.h"
#include "Dict.h"

static bool isBoardValid(Board b)
{
    int countX = 0;
    int countO = 0;

    for (int i = 0; i < 9; i++)
    {
        if (b[i] == 'X')
            countX++;
        if (b[i] == 'O')
            countO++;
    }

    if (abs(countX - countO) > 1)
        return false;

    return true;
}

static void noEnd(Agent *agent, Board b, Player p)
{
    (void)agent, (void)b, (void)p;
    return;
}

static void noFree(void *p)
{
    (void)p;
    return;
}

static Move agentPlay(Agent *agent, Board b)
{

    bool debug = false;

    // Defines the logic that will make the agent play by computing the optimal move from the optimal score.
    if (debug)
        printf("Starting to play !\n");
    // Initializing condition variables
    int bestScore = -2; // We use -2 because it's different from -1, 0 and 1.
    Move bestMove = -1; // We define an optimal move that depends on the optimal score (Initialy, no optimal move.)

    // same logic as Minimax, but this time, we save the optimal move right away.
    for (Move m = 0; m < 9; m++)
        if (boardValidMove(b, m))
        {
            Board nextBoard = boardCopy(b);
            boardNext(nextBoard, m, agentGetPlayer(agent));
            if (debug)
                printf("Computing score\n");
            int score = -GetMinimaxScore(agent, nextBoard);
            boardFree(nextBoard);
            if (score > bestScore)
            {
                bestScore = score;
                bestMove = m;
            }
        }

    // printf("Number of keys in the memory: %zu\n", dictGetNbKeys(agentGetData(agent)));

    return bestMove;
}

Agent *createAiAgent(void) // Creates the agent
{
    Dict *memory = dictCreate(12000);
    if (!memory)
    {
        printf("Can't allocate the memory needed for the dictionnary\n");
        exit(1);
    }

    Agent *agent = agentCreate("Minimax agent", agentPlay, noEnd, noFree);

    agentSetData(agent, memory);
    return agent;
}

int GetMinimaxScore(Agent *agent, Board b)
{

    bool debug = false;

    // CHECKING TO AVOID USELESS COMPUTATION.
    if (boardWin(b) != E) // Someone wins
        return (boardWin(b) == agentGetPlayer(agent)) ? 1 : -1;

    if (boardIsFull(b)) // It's a tie
        return 0;

    // WE CHECK TO SEE IF THE BOARD HAS ALREADY BEEN COMPUTED OR NOT.
    // We get all the memory matches of the agent.

    if (debug)
        printf("Accessing memory\n");

    Dict *memory = agentGetData(agent);
    if (memory == NULL)
    {
        if (debug)
            printf("Memory is NULL\n");
        return -1;
    }
    // If the board exists, we return the score that is linked to the board.
    if (dictContains(memory, b))
    {
        int *score = dictSearch(memory, b);
        if (debug)
            printf("Done accessing memory\n");
        if (score != NULL)
            return *score;
    }
    else
    {
        if (debug)
            printf("Memory does not contain board.\n");
    }
    // We compute all the possible boards.
    int bestScore = -2; // We take an initial value that is different to -1, 0 and 1.
    for (Move m = 0; m < 9; m++)
        if (boardValidMove(b, m)) // We check if the move is valid.
        {
            // We compute the next board
            Board nextBoard = boardCopy(b);
            boardNext(nextBoard, m, agentGetPlayer(agent));

            // // We check if the board is valid or not.
            // if (!isBoardValid(nextBoard))
            // {
            //     boardFree(nextBoard);
            //     continue;
            // }

            int score = -GetMinimaxScore(agent, nextBoard); // We compute the negative score.
            boardFree(nextBoard);                           // We no longer need the board.
            if (score > bestScore)                          // We take the maximum of both results -> score and bestScore ==> We take the greatest.
                bestScore = score;
        }

    // We need to save the current board and it's score in the memory of the player.
    int *scoreSave = malloc(sizeof(int)); // -> We are saving a regular int
    scoreSave = &bestScore;
    printf("board:%s --> score: %d \n", b, bestScore);
    printf("---------\n");
    dictInsert(memory, b, scoreSave);

    return bestScore;
}
