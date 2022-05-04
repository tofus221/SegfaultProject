#ifndef SEGFAULTPROJECT_AGENT_H
#define SEGFAULTPROJECT_AGENT_H
#include "SDL/SDL.h"
#include "engine.h"

typedef struct agentType{
    char* name;
    float lifeSpan;
    float energy;
    //float foodDetectionRadius;
    //food to add when i'll know how to implement, might do a file just for the food search
    float speed;
    char* reproductionType;
    float resistance;
    float hearingRange;
    char** targets;
    int targetAmount;
    int dying;
    Uint32 color;
} agentType;

typedef struct agent{
    int id;
    agentType* type;
    int Xpos;
    int Ypos;
}agent;

struct agentLinkedList{
    agent* agent;
    struct agentLinkedList* next;
};

agentType* createAgentType(char* name, float lifeSpan, float energy, float speed, float resistance, float hRange);
agent* createAgent(agentType* type, int x, int y);
void moveAgent(agent* agent, int addX, int addY);
struct agentLinkedList* initLinkedList();
void push(simulation* simulation1, agent* agentToAdd);
int pop(struct agentLinkedList* list, agent* res);
int popWithId(struct agentLinkedList* list, int id, agent* res);
void freeLinkedList(struct agentLinkedList* agentLinkedList);
void drawAgents(SDL_Surface* screen, struct agentLinkedList* list);
int agentBehave(agent* mainAgent, simulation* sim);
int moveTowards(agent* agent, int x, int y);
int canSeeAgent(agent* mainAgent, agent* targetAgent);
int tryMate(agent* mainAgent, simulation* sim);
void freeAgent(agent* agt);

#endif //SEGFAULTPROJECT_AGENT_H
