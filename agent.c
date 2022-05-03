#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <math.h>
#include <time.h>
#include "agent.h"
#include "SDL/SDL.h"
#include "engine.h"
#include "time.h"

#define GENETICDRIFT 0.01
#define REPRODUCTION_THRESHOLD 200.0

//create an Agent Type, meant to be used with the interface.
agentType* createAgentType(char* name, float lifeSpan, float energy, float speed, float resistance, float hRange){
    agentType* newAgent = malloc(sizeof (agentType));
    memset(newAgent, 0, sizeof(agentType));
    newAgent->name = name;
    newAgent->lifeSpan = lifeSpan;
    newAgent->energy = energy;
    newAgent->speed = speed;
    newAgent->resistance = resistance;
    newAgent->hearingRange = hRange;
    return newAgent;
}

//create an actual agent in the simulation.
agent* createAgent(agentType* type, int x, int y){
    agent* newAgent = malloc(sizeof (agent));
    memset(newAgent, 0, sizeof (agent));
    newAgent->type = type;
    newAgent->Xpos = x;
    newAgent->Ypos = y;
    return newAgent;
}

void freeAgentType(agentType* agentT)
{
    free(agentT->targets);
    free(agentT->name);
    free(agentT);
}

void freeAgent(agent* agt)
{
    freeAgentType(agt->type);
    free(agt);
}

void moveAgent(agent* agent, int addX, int addY){
    agent->Xpos += addX;
    agent->Ypos += addY;
}

//init a linked list to store all the agents currently in the simulation
//contains a sentinel at the beginning.
struct agentLinkedList* initLinkedList(){
    struct agentLinkedList* list = malloc(sizeof(struct agentLinkedList));
    list->agent = NULL;
    list->next = NULL;
    return list;
}

//push an agent to the list.
void push(simulation* simulation1, agent* agentToAdd){
    struct agentLinkedList* tmp = simulation1->agentList;
    struct agentLinkedList* toAdd = malloc(sizeof(struct agentLinkedList));
    toAdd->next = NULL;
    toAdd->agent = agentToAdd;
    while (tmp->next != NULL)
        tmp = tmp->next;
    tmp->next = toAdd;
    simulation1->popCount++;
    agentToAdd->id = simulation1->popCount;
}

//pops an agent in the list and return it in the res argument.
//return 0 if everything went well, -1 otherwise.
int pop(struct agentLinkedList* list, agent* res){
    if (list->next == NULL) {
        printf("pop: you are trying to pop an empty list\n");
        return -1;
    }
    struct agentLinkedList* tmp = list;
    struct agentLinkedList* parent;
    while (tmp->next != NULL){
        parent = tmp;
        tmp = tmp->next;
    }
    parent->next = NULL;
    res = tmp->agent;
    free(tmp);
    return 0;

}

//pops an agents in the list with the following id and return it in the res argument
//return 0 if everything went well, -1 otherwise.
int popWithId(struct agentLinkedList* list, int id, agent* res){
    if (list->next == NULL) {
        printf("popWithId: you are trying to pop an empty list.\n");
        return -1;
    }

    struct agentLinkedList* tmp = list->next; //skip the sentinel
    struct agentLinkedList* parent = list;
    while (tmp->agent->id != id && tmp->next != NULL){
        parent = tmp;
        tmp = tmp->next;
    }
    if (tmp->next == NULL && tmp->agent->id != id) {
        printf("popWithId: no agents with this ID have been found\n");
        return -1;
    }
    parent->next = tmp->next;
    res = tmp->agent;
    free(tmp);
    return 0;
}

//return the agent with the ID given without popping it off the list. return NULL if the list is empty or no agent with ID is found.
agent* peekWithID(struct agentLinkedList* list, int id){
    if (list->next == NULL){
        printf("peekWithID: The list is empty.\n");
        return NULL;
    }

    struct agentLinkedList* tmp = list->next;
    while (tmp->next != NULL){
        if (tmp->agent->id == id)
            return tmp->agent;
        tmp = tmp->next;
    }
    if (tmp->agent->id != id) {
        printf("peekWithID: No agents with this ID was found.\n");
        return NULL;
    }
    return tmp->agent;
}

void freeAgentTypes(struct agentLinkedList* agentLinkedList){
    if (agentLinkedList->next == NULL)
        return;
    agentType** typeList = malloc(sizeof(agentType*));
    int i = 0;
    int toAdd = 1;
    struct agentLinkedList* tmp = agentLinkedList->next;
    while (tmp != NULL){
        agent* agent = tmp->agent;
        for (int j = 0; j < i; ++j) {
            if (typeList[j] == agent->type) {
                toAdd = 0;
                break;
            }
        }
        if (toAdd){
            i++;
            typeList = realloc(typeList, sizeof(agentType)*i);
            typeList[i-1] = agent->type;
        }
        toAdd = 1;
        tmp = tmp->next;
    }

    for (int j = 0; j < i; ++j) {
        free(typeList[j]);
    }
    free(typeList);
};

//free the linked list.
void freeLinkedList(struct agentLinkedList* agentLinkedList){
    if (agentLinkedList->next == NULL)
        return;
    struct agentLinkedList* parent;
    freeAgentTypes(agentLinkedList);
    while (agentLinkedList->next != NULL){
        parent = agentLinkedList;
        agentLinkedList = agentLinkedList->next;
        freeAgent(parent->agent);
        free(parent);
    }
    free(agentLinkedList->agent);
    free(agentLinkedList);
}

void drawAgents(SDL_Surface* screen, struct agentLinkedList* list){
    if (list->next == NULL) //check if the list is empty
        return;
    Uint32 color = SDL_MapRGB(screen->format, 255, 0, 0); //temporary, should be really cool to be able to put an image instead
    SDL_Rect rect;
    rect.w = 2;
    rect.h = 2;

    struct agentLinkedList* tmp = list->next; // skip the sentinel
    agent* agent;
    while (tmp->next != NULL){
        agent = tmp->agent;
        rect.x = (Sint16)agent->Xpos; //weird cast but my IDE seems more fine with it than without, might just be fine tho
        rect.y = (Sint16)agent->Ypos;
        SDL_FillRect(screen, &rect, color);
        tmp = tmp->next;
    }
    agent = tmp->agent;
    rect.x = (Sint16)agent->Xpos;
    rect.y = (Sint16)agent->Ypos;
    SDL_FillRect(screen, &rect, color);
}

float geneticDrift(float value){
    srand(time(0));
    float newValue = value;
    float drift = value * GENETICDRIFT;
    if (rand()%2)
        newValue += drift;
    else
        newValue -= drift;
    return newValue;
}

agentType* reproductionWithGeneticDrift(agent* agent1, agent* agent2){
    float newLifeSpan, newEnergy, newSpeed, newResistance, newHrange;
    agentType* agentType1 = agent1->type;
    agentType* agentType2 = agent2->type;
    if (rand()%2){
        newLifeSpan = geneticDrift(agentType1->lifeSpan);
        newEnergy = geneticDrift(agentType1->energy);
        newSpeed = geneticDrift(agentType1->speed);
        newResistance = geneticDrift(agentType1->resistance);
        newHrange = geneticDrift(agentType1->hearingRange);
    } else{
        newLifeSpan = geneticDrift(agentType2->lifeSpan);
        newEnergy = geneticDrift(agentType2->energy);
        newSpeed = geneticDrift(agentType2->speed);
        newResistance = geneticDrift(agentType2->resistance);
        newHrange = geneticDrift(agentType2->hearingRange);
        
    }
    return createAgentType(agentType1->name, newLifeSpan, newEnergy, newSpeed, newResistance, newHrange);
}

agentType* normalReproduction(agent* agent1, agent* agent2){ //not nice..
    float newLifeSpan, newEnergy, newSpeed, newResistance, newHrange;
    agentType* agentType1 = agent1->type;
    agentType* agentType2 = agent2->type;
    for (int i = 0; i < 5; i++) {
        switch (i) {
            case 0:
                if (rand()%2)
                    newLifeSpan = agentType1->lifeSpan;
                else
                    newLifeSpan = agentType2->lifeSpan;
                break;
            case 1:
                if (rand()%2)
                    newEnergy = agentType1->energy;
                else
                    newEnergy= agentType2->energy;
                break;
            case 2:
                if (rand()%2)
                    newSpeed = agentType1->speed;
                else
                    newSpeed = agentType2->speed;
                break;
            case 3:
                if (rand()%2)
                    newResistance = agentType1->resistance;
                else
                    newResistance = agentType2->resistance;
                break;
            default:
                if (rand()%2)
                    newHrange = agentType1->hearingRange;
                else
                    newHrange = agentType2->hearingRange;
                break;
        }
    }

    return createAgentType(agentType1->name, newLifeSpan, newEnergy, newSpeed, newResistance, newHrange);
}

void reproduction(agent* agent1, agent* agent2, simulation* sim){
    srand(time(NULL)); //initialize the random generator
    agentType* newType;
    if (rand()%2){ //pick if there will be drift or not
        printf("geneticDrift\n");
        newType = reproductionWithGeneticDrift(agent1, agent2);
    }
    else {
        printf("normal\n");
        newType = normalReproduction(agent1, agent2);
    }
    agent* newAgent = createAgent(newType, agent1->Xpos, agent2->Ypos);
    push(sim, newAgent);
};


int moveTowards(agent* agent, int x, int y)
{
    double xOFF = (double)x-agent->Xpos;
    double yOFF = (double)y-agent->Ypos;
    double distance = sqrt(xOFF*xOFF + yOFF*yOFF);
    if (distance > (double)agent->type->speed)
    {
        double ratio = (double)agent->type->speed / distance;
        moveAgent(agent, (int)(xOFF * ratio), (int)(yOFF * ratio));
        agent->type->energy -= agent->type->speed * agent->type->speed;
        return 0;
    }
    agent->Xpos = x;
    agent->Ypos = y;
    agent->type->energy -= (float) distance * distance;
    return 1;
}


int canSeeAgent(agent* mainAgent, agent* targetAgent)
{
    int xOffset = targetAgent->Xpos - mainAgent->Xpos;
    int yOffset = targetAgent->Ypos - mainAgent->Ypos;
    double distance = sqrt((double)xOffset*xOffset + yOffset*yOffset);
    return distance <= mainAgent->type->hearingRange;
}

int tryFeed(agent* mainAgent, simulation* sim)
{
    for (struct agentLinkedList* curr = sim->agentList->next; curr != NULL; curr = curr->next)
    {
        agent* currAgent = curr->agent;
        if (mainAgent == currAgent)
        {
            continue;
        }

        for (int i = 0; i < mainAgent->type->targetAmount; i++)
        {
            if (strcmp(mainAgent->type->targets[i],currAgent->type->name))
            {
                if(moveTowards(mainAgent,currAgent->Xpos,currAgent->Ypos))
                {
                    mainAgent->type->energy += currAgent->type->energy;
                    popWithId(sim->agentList,currAgent->id,currAgent);
                    freeAgent(currAgent);
                    sim->popCount--;
                }
                return 1;
            }
        }
    }
    return 0;
}

int tryMate(agent* mainAgent, simulation* sim)
{
    for (struct agentLinkedList* curr = sim->agentList->next; curr != NULL; curr = curr->next)
    {
        agent* currAgent = curr->agent;
        if (mainAgent == currAgent)
        {
            continue;
        }

        if (strcmp(mainAgent->type->name,currAgent->type->name) && (currAgent->type->energy > REPRODUCTION_THRESHOLD))
        {
            if (moveTowards(mainAgent, currAgent->Xpos, currAgent->Ypos))
            {
                reproduction(mainAgent,currAgent,sim);
                mainAgent->type->energy -= REPRODUCTION_THRESHOLD / 2;
                currAgent->type->energy -= REPRODUCTION_THRESHOLD / 2;
            }
            return 1;
        }
    }
    return 0;
}

// makes an agent behave; returns 2 when trying to mate, 1 when trying to feed, and 0 when wandering.
int agentBehave(agent* mainAgent, simulation* sim)
{
    srand(time(0));
    if (mainAgent->type->energy > REPRODUCTION_THRESHOLD)
    {
        if (tryMate(mainAgent, sim))
        {
            return 2;
        }
    }
    if (tryFeed(mainAgent, sim))
    {
        return 1;
    }
    moveTowards(mainAgent,rand(),rand());
    return 1;
    
}