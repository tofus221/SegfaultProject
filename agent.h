#ifndef SEGFAULTPROJECT_AGENT_H
#define SEGFAULTPROJECT_AGENT_H
#include "SDL/SDL.h"
#include "engine.h"
#include "sickness.h"

typedef struct agentType{
    char* name;
    int typeId;
    float lifeSpan;
    float timeLeft;
    float energy;
    float speed;
    int asexualReproduction;
    int birthRate;
    float birthCost;
    float individualBirthCost;
    float moveCost;
    float resistance;
    float hearingRange;
    int* targetsId;
    int targetAmount;
    float fertilityRate;


    Uint32 color;
} agentType;

typedef struct agent{
    int id;
    agentType* type;
    int Xpos;
    int Ypos;
    int wanderX;
    int wanderY;
    sicknessLinkedList* SLL;
}agent;

struct agentLinkedList{
    agent* agent;
    struct agentLinkedList* next;
};

agentType* createAgentType(char* name, int typeId, float lifeSpan, float energy, float speed, 
float resistance, float hRange, int birthRate, float fertility, float birthCost, float individualBirthCost, float moveCost);
agent* createAgent(agentType* type, int x, int y);
struct agentLinkedList* initLinkedList();
void push(simulation* sim, agent* agent);
int pop(struct agentLinkedList* list, agent** res);
int popWithId(struct agentLinkedList* list, int id, agent** res);
void freeLinkedList(struct agentLinkedList* agentLinkedList);
void drawAgents(SDL_Surface* screen, struct agentLinkedList* list);
int agentBehave(agent* mainAgent, simulation* sim);
int moveTowards(agent* agent, simulation* sim, int x, int y);
int canSeeAgent(agent* mainAgent, agent* targetAgent);
int tryMate(agent* mainAgent, simulation* sim);
int tryFeed(agent* mainAgent, simulation* sim);
void freeAgent(agent* agt);
void doInfect(agent* mainAgent, simulation* sim);
void doWander(agent* mainAgent, simulation* sim);
void reproduction(agent* agent1, agent* agent2, simulation* sim);

#endif //SEGFAULTPROJECT_AGENT_H
