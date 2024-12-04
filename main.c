#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "board.h"
#include "agent.h"
#include "aiagent.h"
#include "rlagent.h"

static void print_usage(void)
{
    printf("Usage:\n");
    printf("  - to play a single game: tictactoe -s <player X> <player O>\n");
    printf("  - to play a tournament: tictactoe -t <nb games> <player X> <player O>\n");
    printf("      (to pretrain the RL agent add the option -p <nb steps> [<training player>])");
    printf("Player can be human, random, ai or rl\n");
}

static Agent *getPlayer(const char *name)
{
    if (strcmp(name, "human") == 0)
        return createHumanAgent();
    else if (strcmp(name, "random") == 0)
        return createRandomAgent();
    else if (strcmp(name, "ai") == 0)
        return createAiAgent();
    else if (strcmp(name, "rl") == 0)
        return createRlAgent();
    else
    {
        printf("'%s' is not a valid agent name.\n", name);
        print_usage();
        exit(1);
    }
}

int main(int argc, char const *argv[])
{
    srand(time(NULL));

    if (argc == 1)
    {
        print_usage();
        return 1;
    }

    Agent *agentX = NULL;
    const char *agentXname;
    Agent *agentO = NULL;
    const char *agentOname;

    bool pretrain = false;
    int nbPretrainGames = 0;
    Agent *trainingAgent = NULL;
    const char *trainingAgentName;

    bool singleGame = true;
    int nbrounds = 0;

    // Analysis of the command line arguments

    if (strcmp(argv[1], "-s") == 0 && argc >= 4)
    {
        singleGame = true;
        nbrounds = 1;
        agentX = getPlayer(argv[2]);
        agentO = getPlayer(argv[3]);
        agentXname = argv[2];
        agentOname = argv[3];

        if (strcmp(argv[2], "rl") == 0 || strcmp(argv[3], "rl") == 0)
        {
            // pretraining ?
            if (argc > 4 && strcmp(argv[4], "-p") == 0)
            {
                pretrain = true;
                nbPretrainGames = atoi(argv[5]);
                if (argc >= 7)
                {
                    if (strcmp(argv[6], "human") != 0)
                    {
                        trainingAgent = getPlayer(argv[6]);
                        trainingAgentName = argv[6];
                    }
                    else
                    {
                        printf("Human can not be used for pretraining a rl agent.\n");
                        exit(1);
                    }
                }
                else if (strcmp(argv[2], "human") == 0 || strcmp(argv[3], "human") == 0)
                {
                    printf("You need to provide a pretraining agent in a rl vs human game.\n");
                    exit(1);
                }
            }
        }
    }
    else if (strcmp(argv[1], "-t") == 0 && argc >= 5)
    {
        singleGame = false;
        if (strcmp(argv[3], "human") == 0 || strcmp(argv[4], "human") == 0)
        {
            printf("Humans can not participate to tournaments. Sorry.\n");
            exit(1);
        }

        nbrounds = atoi(argv[2]);
        agentX = getPlayer(argv[3]);
        agentO = getPlayer(argv[4]);
        agentXname = argv[3];
        agentOname = argv[4];

        if (strcmp(argv[3], "rl") == 0 || strcmp(argv[4], "rl") == 0)
        {
            if (argc > 5 && strcmp(argv[5], "-p") == 0)
            {
                pretrain = true;
                nbPretrainGames = atoi(argv[6]);
                if (argc >= 8)
                {
                    trainingAgent = getPlayer(argv[7]);
                    trainingAgentName = argv[7];
                }
            }
        }
    }
    else
    {
        print_usage();
        return 1;
    }

    // Pre-training of the RL agent(s)

    if (pretrain)
    {
        printf("Pretraining the rl agent(s) for %d games:\n", nbPretrainGames);
        if (trainingAgent == NULL)
        { // no specific training player
            printf("%s against %s...", agentXname, agentOname);
            for (int i = 0; i < nbPretrainGames; i++)
                agentPlayGame(agentX, agentO, false);
            printf("Done.\n");
        }
        else
        { // one specific training player
            if (strcmp(agentXname, "rl") == 0)
            {
                printf("%s against %s...", agentXname, trainingAgentName);
                for (int i = 0; i < nbPretrainGames; i++)
                    agentPlayGame(agentX, trainingAgent, false);
            }
            if (strcmp(agentOname, "rl") == 0)
            {
                printf("%s against %s...", trainingAgentName, agentOname);
                for (int i = 0; i < nbPretrainGames; i++)
                    agentPlayGame(trainingAgent, agentO, false);
            }
            printf("Done.\n");
        }
        // if rl agents are trained, they are not anymore in the tournament.
        if (strcmp(agentXname, "rl") == 0)
            setTrainingModeRlAgent(agentX, false);
        if (strcmp(agentOname, "rl") == 0)
            setTrainingModeRlAgent(agentO, false);
    }

    if (singleGame)
    {
        printf("\nStarting a game between %s and %s\n\n", agentGetName(agentX), agentGetName(agentO));
        agentPlayGame(agentX, agentO, true);
    }
    else
    {
        printf("Tournament: %s against %s for %d games...\n", agentGetName(agentX), agentGetName(agentO), nbrounds);
        int nbwinX = 0;
        int nbwinO = 0;
        int nbdraws = 0;
        for (int i = 0; i < nbrounds; i++)
        {
            Player p = agentPlayGame(agentX, agentO, true); // Modification du verbose a true.
            if (p == X)
                nbwinX++;
            else if (p == O)
                nbwinO++;
            else
                nbdraws++;
        }
        printf("Win of %s (as X): %d\n", agentGetName(agentX), nbwinX);
        printf("Win of %s (as O): %d\n", agentGetName(agentO), nbwinO);
        printf("Number of draws  : %d\n", nbdraws);
    }
    return 0;
}
