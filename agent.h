#ifndef SEGFAULTPROJECT_AGENT_H
#define SEGFAULTPROJECT_AGENT_H
#include "SDL/SDL.h"
#include "sickness.h"
#include "food.h"


typedef struct agentType{
    char* name;
    int typeId;
    float lifeSpan;
    float minLifeSpanForReprod;
    float timeLeft;
    float energy;
    float speed;
    int asexual;
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
float resistance, float hRange, int asexual,int birthRate, float fertility, float birthCost, float individualBirthCost, float moveCost);
agent* createAgent(agentType* type, int x, int y);
struct agentLinkedList* initLinkedList();
void push(struct agentLinkedList* list, agent* agent);
int popWithId(struct agentLinkedList* list, int id, agent** res);
void freeLinkedList(struct agentLinkedList* agentLinkedList);
void drawAgents(SDL_Surface* screen, struct agentLinkedList* list);
int agentBehave(agent* mainAgent, struct agentLinkedList* list, foodHandler* foodHandler,SDL_Surface* screen, SDL_Surface* terrain);
int moveTowards(agent* agent, SDL_Surface* terrain, SDL_Surface* screen, int x, int y);
int canSeeAgent(agent* mainAgent, agent* targetAgent);
int tryMate(agent* mainAgent, struct agentLinkedList* list, SDL_Surface* screen, SDL_Surface* terrain);
int tryFeedAgent(agent* mainAgent, struct agentLinkedList* list,SDL_Surface* screen, SDL_Surface* terrain);
void freeAgent(agent* agt);
void doInfect(agent* mainAgent, struct agentLinkedList* list);
void doWander(agent* mainAgent, SDL_Surface* screen, SDL_Surface* terrain);
void reproduction(agent* agent1, agent* agent2, struct agentLinkedList* list);

#endif //SEGFAULTPROJECT_AGENT_H
