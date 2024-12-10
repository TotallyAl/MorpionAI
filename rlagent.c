#include <stdlib.h>
#include "aiagent.h"
#include "board.h"
#include "agent.h"
#include "rlagent.h"
#include "Dict.h"
#include "LinkedList.h"

#define EPSILON 0.25

static Move rlPlay(Agent *agent, Board board);
static void rlEnd(Agent *agent, Board board, Player player);
static void rlFree(void *param);
static void analyseBoards(Dict *memory, Board board);
static Move bestAverageMoveRandom(List *llTuples);

typedef struct tupleMoveScore
{
    Move move;
    float score;
    unsigned movesUsed;
} tupleMoveScore;

typedef struct RlAgentData
{
    Dict *memory;    // Memory of the agent (Go see comment under)
    bool training;   // Training mode
    Move moves[5];   // List of moves played. We set the list size to 5 because we can't exceed 5 moves in tictactoe.
    Board boards[5]; // We list all of the boards that happend in order, so that it allows us to set back to update the score;
    unsigned movesPlayed;
} RlAgentData;

/*
The agent will look like this:

memory = {
    "board1": [(move, score), (move, score), ...],
    "board2": [(move, score), (move, score), ...],
    ...
    }

• (move, score) -> struct tupleMoveScore
• [] -> linked list.

*/

// Returns a list of the best moves by looking at their averageScore
static Move bestAverageMoveRandom(List *llTuples)
{
    Node *node = llHead(llTuples);
    if (node == NULL)
        printf("NODE NULL\n");
    float bestScore = -1e6;
    Move *bestMoves[9];
    unsigned movesIndex = 0;
    Move bestMove;

    while (node != NULL)
    {
        tupleMoveScore *tuple = llData(node);
        float score = tuple->score / tuple->movesUsed;

        if (score > bestScore)
        {
            bestScore = score;
            movesIndex = 0;
            bestMoves[movesIndex++] = &tuple->move;
        }

        else if (bestScore == score)
            bestMoves[movesIndex++] = &tuple->move;

        node = llNext(node);
    }

    unsigned randomIndex = rand() % movesIndex;

    bestMove = *bestMoves[randomIndex];
    return bestMove;
}

static Move rlPlay(Agent *agent, Board board)
{
    RlAgentData *data = agentGetData(agent);
    bool training = data->training;

    float probability = (float)rand() / (float)RAND_MAX;
    Move move = -1;

    if (training && probability < EPSILON)
    {
        // printf("Random move\n");
        unsigned possibleMoves = 0;
        Move moves[9];

        for (Move k = 0; k < 9; k++)
            if (boardValidMove(board, k))
                moves[possibleMoves++] = k;

        move = moves[rand() % possibleMoves];

        data->moves[data->movesPlayed] = move;
        data->boards[data->movesPlayed] = boardCopy(board);
        data->movesPlayed++;
    }

    else
    {
        // printf("Not picking a random move\n");
        if (dictContains(data->memory, board))
        {
            List *llTuples = dictSearch(data->memory, board);
            // Node *node = llHead(llTuples); // When we call llHead, there is a crash.
            // if (node == NULL)
            // printf("NODE IS NULL BEFORE BEING CALLED\n");
            move = bestAverageMoveRandom(llTuples); // This doesn't cause the head to be null.
            // printf("after crash\n");
            if (training)
            {
                data->moves[data->movesPlayed] = move;
                data->boards[data->movesPlayed] = boardCopy(board);
                data->movesPlayed++;
            }
        }
    }

    return move;
}

static void rlEnd(Agent *agent, Board board, Player player)
{
    RlAgentData *data = agentGetData(agent);
    bool training = data->training;

    if (!training)
        return;

    for (unsigned i = 0; i < data->movesPlayed; i++)
    {
        Board boardUsed = data->boards[i];
        List *llTuples = dictSearch(data->memory, boardUsed);
        Node *node = llHead(llTuples);

        while (node != NULL)
        {
            tupleMoveScore *tuple = llData(node);

            if (tuple->move == data->moves[i])
            {
                if (player == agentGetPlayer(agent))
                    tuple->score++;
                if (player != agentGetPlayer(agent) && player != E)
                    tuple->score--;

                tuple->movesUsed++;
                break;
            }
            node = llNext(node);
        }

        data->moves[i] = -1;
        boardFree(data->boards[i]);
        data->boards[i] = NULL;
    }
    data->movesPlayed = 0;
    (void)board;
}

static void rlFree(void *param)
{
    (void)param;
    printf("freeing !\n");
    return;
}

static void analyseBoards(Dict *memory, Board board)
{

    Player winner = boardWin(board);

    if (winner == O || winner == X || (boardIsFull(board) && winner == E))
        return;

    int validMoves = 0;
    int moves[9] = {0};
    Player currentPlayer = boardGetPlayer(board);

    for (Move m = 0; m < 9; m++)
    {

        if (boardValidMove(board, m))
        {
            moves[validMoves++] = m; // We save all valid moves

            Board nextBoard = boardNext(boardCopy(board), m, currentPlayer);

            analyseBoards(memory, nextBoard); // We generate all boards
            boardFree(nextBoard);             // We free the board after because it is usless once saved.
        }
    }
    if (!dictContains(memory, board))
    {

        List *llTuples = llCreateEmpty();
        for (int k = 0; k < validMoves; k++)
        {
            tupleMoveScore *tuple = malloc(sizeof(tupleMoveScore));
            tuple->move = moves[k];
            tuple->score = 0; // Initially, the score should be 0.
            tuple->movesUsed = 1;
            llInsertLast(llTuples, (void *)tuple);
        }

        dictInsert(memory, board, (void *)llTuples);
    }
}

Agent *createRlAgent(void)
{
    Dict *memory = dictCreate(12000);
    Board initialBoard = boardMakeEmpty();

    Agent *rlAgent = agentCreate("RL Agent", rlPlay, rlEnd, rlFree);

    RlAgentData *data = malloc(sizeof(RlAgentData));
    data->memory = memory;
    data->training = true;
    data->movesPlayed = 0;
    for (int i = 0; i < 5; i++)
    {
        // data->boards[i] = boardMakeEmpty();
        data->boards[i] = NULL;
    }

    analyseBoards(data->memory, initialBoard);
    agentSetData(rlAgent, data);

    return rlAgent;
}

void setTrainingModeRlAgent(Agent *agent, bool training)
{
    RlAgentData *data = agentGetData(agent);
    data->training = training;
    return;
}

float getMoveScoreRlAgent(Agent *agent, Board b, Move m)
{
    (void)agent, (void)b, (void)m;
    return 0.0;
}
