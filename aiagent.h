#ifndef _AIAGENT_H_
#define _AIAGENT_H_

#include "agent.h"

/**
 * @brief create an agent that plays using the Minimax algorithm
 * 
 * @returns a newly created agent
 */
Agent *createAiAgent(void);


/**
 * @brief testing method that computes the score of a board, assuming the
 * other player also uses the Minimax algorithm
 * 
 * @param agent current agent
 * @param b board in which the agent can play
 */
int GetMinimaxScore(Agent *agent, Board b);

#endif // _AIAGENT_H_
