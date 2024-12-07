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
            score = GetMinimaxScore(agent, next);
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
    (void)board, (void)player, (void)agent;
    dictIterate(agentGetData(agent), dictPrint);
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
    Dict *memory = dictCreate(3 * 3 * 3 * 3 * 3 * 3 * 3 * 3 * 3);
    if (!memory)
    {
        printf("Can't allocate the memory needed for the dictionnary\n");
        exit(1); // Error, close program.
    }

    Agent *agent = agentCreate("Minimax agent", AIPlay, noEnd, noFree);

    // We link the dictionnary to the agent.
    agentSetData(agent, memory);
    return agent;
}

static int GetMinimaxScoreCopy(Dict *memory, Board b, Player player)
{
    printf("A\n");

    // We first check to see if there is a winner.
    if (boardWin(b) != E)
    {
        int s = (boardWin(b) == player) ? 1 : -1;
        int *scorePtr = malloc(sizeof(int));
        *scorePtr = s;
        dictInsert(memory, b, (void *)scorePtr);
        return s; // If the winner is the agent passed in parameter -> 1 if not -1.
    }

    // We check if there is a tie, by looking if the board is full or not.
    if (boardIsFull(b) && boardWin(b) == E)
    {
        int *scorePtr = malloc(sizeof(int));
        *scorePtr = 0;
        dictInsert(memory, b, (void *)scorePtr);
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

    printf("B\n");

    if (dictContains(memory, b))
    {
        printf("AA\n");
        // The key exists, so it means the board is in the dictionnary.
        int *scorePtr = (int *)dictSearch(memory, b); // This returns the score associated to the board.
        int score = *scorePtr;
        return score;
    }

    printf("C\n");

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

            /*
                THIS IS PROBABLY WHERE THE BUG IS.
                I NEED TO FIND A WAY TO SWAP AGENTS.

            */
            int score = -GetMinimaxScoreCopy(memory, next, boardGetPlayer(next));

            // We compare the score. We keep the highest score.
            printf("Is %d > %d ?\n", score, bestScore);
            if (score > bestScore)
                bestScore = score;
        }
    }
    // We save boards as they come and go.
    printf("Saving board\n");
    int *scoreSave = malloc(sizeof(int));
    *scoreSave = bestScore;
    printf("ScoreSave:%d\n", *scoreSave);
    dictInsert(memory, b, (void *)scoreSave);
    printf("saved board\n");

    return bestScore;
}

int GetMinimaxScore(Agent *agent, Board b)
{
    return GetMinimaxScoreCopy(agentGetData(agent), b, agentGetPlayer(agent));
}
