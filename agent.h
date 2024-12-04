#ifndef _AGENT_H_
#define _AGENT_H_

#include "board.h"

// Agent

/**
 * @brief Represents an agent structure in an opaque way.
 */
typedef struct Agent_t Agent;

/**
 * @brief Create a new agent.
 *
 * @param name name of the agent (used when printing)
 * @param play function that selects a move from a board
 * @param end function that is called at the end of a game with the last board and the winner player
 * @param freeData function that deallocate the memory holding the data (see agentGetData)
 *
 * @returns the created agent (exit on allocation failure)
 */
Agent *agentCreate(char *name, Move (*play)(Agent *, Board),
                   void (*end)(Agent *, Board, Player),
                   void (*freeData)(void *));

/**
 * @brief deallocate the memory used by the agent and its data
 *
 * @param agent agent to free
 */
void agentFree(Agent *agent);

/**
 * @brief set the player for which this agent plays
 * 
 * See agentGetPlayer
 * 
 * @param agent agent to be updated
 * @param player player to use
 */
void agentSetPlayer(Agent *agent, Player player);

/**
 * @brief get the player for which this agent plays
 * 
 * See agentSetPlayer
 * 
 * @returns the player
 */
Player agentGetPlayer(Agent *agent);

/**
 * @brief get the name of the agent
 * 
 * @returns the name
 */
char *agentGetName(Agent *agent);

/**
 * @brief update the data of the agent
 * 
 * See agentGetData
 * 
 * @param agent agent to be updated
 * @param data data to use
 */
void agentSetData(Agent *agent, void *data);


/**
 * @brief get the data of the agent
 * 
 * See agentSetData
 * 
 * @returns a generic pointer to the data
 */
void *agentGetData(Agent *agent);

// Implemented agent types


/**
 * @brief create a human agent that plays interactively in the terminal
 * 
 * @returns a newly created agent
 */
Agent *createHumanAgent(void);

/**
 * @brief create a random agent that plays by selecting random valid move
 * 
 * @returns a newly created agent
 */
Agent *createRandomAgent(void);

// Game simulation

/**
 * @brief simulate a complete game between two agents
 * 
 * @param agentX agent that plays the X
 * @param agentO agent that plays the O
 * @param verbose if true, print messages during the game
 * 
 * @returns the winning player, or E in case of a tie
 */
Player agentPlayGame(Agent *agentX, Agent *agentO, bool verbose);

#endif // _AGENT_H_
