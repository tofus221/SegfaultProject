#include <stdlib.h>
#include <memory.h>
#include "agent.h"

//create a Agent Type via the interface
agentType* createAgentType(char* name, float lifeSpan, float energy, float speed, float resistance){
    agentType* newAgent = malloc(sizeof (agentType));
    memset(newAgent, 0, sizeof(agentType));
    newAgent->name = name;
    newAgent->lifeSpan = lifeSpan;
    newAgent->energy = energy;
    newAgent->speed = speed;
    newAgent->resistance = resistance;
    return newAgent;
}

//actual agent in the simulation
agent* createAgent(agentType* type, int x, int y){
    agent* newAgent = malloc(sizeof (agent));
    memset(newAgent, 0, sizeof (agent));
    newAgent->type = type;
    newAgent->Xpos = x;
    newAgent->Ypos = y;
    return newAgent;
}

void moveAgent(agent* agent, int addX, int addY){
    agent->Xpos += addX;
    agent->Ypos += addY;
}