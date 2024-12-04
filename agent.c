#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "board.h"
#include "agent.h"

// agent structure

struct Agent_t
{
    char *name;
    Player player;
    void *data;
    Move (*play)(Agent *, Board);
    void (*end)(Agent *, Board, Player);
    void (*freeData)(void *);
};

// static functions
static Move randomAgentPlay(Agent *agent, Board b);
static void noEnd(Agent *agent, Board b, Player p);
static Move humanAgentPlay(Agent *agent, Board b);

// Main play loop.

Player agentPlayGame(Agent *agentX, Agent *agentO, bool verbose)
{
    if (agentX == agentO)
    {
        printf("The same agent can not play against itself.\n");
        exit(0);
    }
    Board b = boardMakeEmpty();
    if (verbose)
        boardPrint(b);

    agentSetPlayer(agentX, X);
    agentSetPlayer(agentO, O);
    Player winner = E;

    while (!boardIsFull(b))
    {
        if (verbose)
            printf("%s (X)'s turn.\n", agentX->name);
        Move m = agentX->play(agentX, b);
        if (verbose)
            printf("Move: %u\n", m);
        boardNext(b, m, X);
        if (verbose)
            boardPrint(b);
        if (boardWin(b) == X)
        {
            if (verbose)
                printf("%s (X) wins!\n", agentX->name);
            winner = X;
            break;
        }
        if (boardIsFull(b))
            break;
        if (verbose)
            printf("%s (O)'s turn.\n", agentO->name);
        m = agentO->play(agentO, b);
        if (verbose)
            printf("Move: %u\n", m);
        boardNext(b, m, O);
        if (verbose)
            boardPrint(b);
        if (boardWin(b) == O)
        {
            if (verbose)
                printf("%s (O) wins!\n", agentO->name);
            winner = O;
            break;
        }
    }
    if (winner == E && verbose)
        printf("Board is full. Draw!\n");
    agentX->end(agentX, b, winner);
    agentO->end(agentO, b, winner);
    boardFree(b);
    return winner;
}

// Random agent: an agent that plays fully at random

static Move randomAgentPlay(Agent *agent, Board b)
{
    (void)agent;

    int moves[9] = {0};
    int numLegalMoves = 0;
    for (int m = 0; m < 9; m++)
    {
        if (boardValidMove(b, m))
        {
            moves[numLegalMoves++] = m;
        }
    }
    return moves[rand() % numLegalMoves];
}

static void noEnd(Agent *agent, Board b, Player winner)
{
    (void)agent;
    (void)b;
    (void)winner;
    return;
}

static void noFree(void *data)
{
    (void)data;
}

Agent *createRandomAgent(void)
{
    return agentCreate("Random agent", randomAgentPlay, noEnd, noFree);
}

// Human agent: an agent played by the player. Should be used only if verbose is true.

static Move humanAgentPlay(Agent *agent, Board b)
{
    (void)agent;

    char ans[10];
    int m;

    while (1)
    {
        printf("Enter a valid position: ");
        scanf("%s", ans);
        m = atoi(ans);
        if (m >= 0 && m < 9)
        {
            if (boardValidMove(b, m))
                return m;
            else
            {
                printf("Cell %u is not empty. Try again.\n", m);
            }
        }
        else
        {
            printf("Invalid position. Try again.\n");
        }
    }
    return 0;
}

Agent *createHumanAgent(void)
{
    Agent *agent = malloc(sizeof(Agent));
    agent->name = "Human agent";
    agent->play = humanAgentPlay;
    agent->end = noEnd;
    agent->freeData = noFree;
    return agent;
}

// generic function to free an agent

void agentFree(Agent *agent)
{
    agent->freeData(agent->data);
    free(agent);
}

void agentSetPlayer(Agent *agent, Player player)
{
    agent->player = player;
}

Player agentGetPlayer(Agent *agent)
{
    return agent->player;
}

char *agentGetName(Agent *agent)
{
    return agent->name;
}

void agentSetData(Agent *agent, void *data)
{
    agent->data = data;
}

void *agentGetData(Agent *agent)
{
    return agent->data;
}

// To build your own agent

Agent *agentCreate(char *name, Move (*play)(Agent *, Board),
                   void (*end)(Agent *, Board, Player),
                   void (*freeData)(void *))
{
    Agent *agent = malloc(sizeof(Agent));
    if (agent == NULL)
    {
        fprintf(stderr, "agentCreate: allocation problem\n");
        exit(1);
    }
    agent->name = name;
    agent->player = X;
    agent->play = play;
    agent->end = end;
    agent->freeData = freeData;
    return agent;
}
