#ifndef _RLAGENT_H_
#define _RLAGENT_H_

#include "agent.h"

/**
 * @brief create an agent that plays using the reinforcement learning algorithm
 * 
 * @returns a newly created agent
 */
Agent *createRlAgent(void);

/**
 * @brief update the training move of the agent
 * 
 * @param agent agent to be updated
 * @param training true if the agent should begin training, false if it should end
 * 
 */
void setTrainingModeRlAgent(Agent *agent, bool training);

/**
 * @brief testing method that computes the score of a move in a board
 * 
 * @param agent current agent
 * @param b board in which the agent can play
 * @param m valid move
 * 
 * @returns the score of the move
 */
float getMoveScoreRlAgent(Agent *agent, Board b, Move m);

#endif // _RLAGENT_H_
