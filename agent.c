#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <math.h>
#include "agent.h"
#include "sickness.h"
#include "pixelOp.h"
#include <SDL/SDL.h>
#include "engine.h"
#include "food.h"

#define GENETICDRIFT 0.01
#define REPRODUCTION_THRESHOLD 200.0f
#define AGENT_SIZE 4

#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

//create an Agent Type, meant to be used with the interface.
agentType* createAgentType(char* name, int typeId, float lifeSpan, float energy, float speed, 
float resistance, float hRange, int birthRate, float fertility, float birthCost, float individualBirthCost, float moveCost){
    agentType* newAgent = malloc(sizeof(agentType));
    memset(newAgent, 0, sizeof(agentType));
    newAgent->name = name;
    newAgent->typeId = typeId;
    newAgent->lifeSpan = lifeSpan;
    newAgent->timeLeft = lifeSpan;
    newAgent->energy = energy;
    newAgent->speed = speed;
    newAgent->resistance = resistance;
    newAgent->hearingRange = hRange;
    newAgent->birthRate = birthRate;
    newAgent->birthCost = birthCost;
    newAgent->individualBirthCost = individualBirthCost;
    newAgent->moveCost = moveCost;
    newAgent->fertilityRate = fertility;
    return newAgent;
}

//create an actual agent in the simulation.
agent* createAgent(agentType* type, int x, int y){
    agent* newAgent = malloc(sizeof (agent));
    memset(newAgent, 0, sizeof(agent));
    newAgent->type = type;
    newAgent->Xpos = x;
    newAgent->Ypos = y;
    newAgent->wanderX = -1;
    newAgent->wanderY = -1;
    newAgent->SLL = initSLL();
    return newAgent;
}

void freeAgentType(agentType* agentT)
{

    free(agentT->targetsId);
    free(agentT);
}

void freeAgent(agent* agt)
{
    freeAgentType(agt->type);
    freeSLL(agt->SLL);
    free(agt);
}

void moveAgent(agent* agent, int addX, int addY, SDL_Surface *screen){
    agent->Xpos += addX;
    agent->Ypos += addY;
    agent->Xpos = MIN(MAX(0, agent->Xpos), screen->w);
    agent->Ypos = MIN(MAX(0, agent->Ypos), screen->h);
}

//init a linked list to store all the agents currently in the simulation
//contains a sentinel at the beginning.
struct agentLinkedList* initLinkedList(){
    struct agentLinkedList* list = malloc(sizeof(struct agentLinkedList));
    list->agent = NULL;
    list->next = NULL;
    return list;
}

int getNewID(simulation* sim)
{
    int attemptID = rand();
    int isValid = 0;
    while(!isValid)
    {
        struct agentLinkedList* list = sim->agentList;
        isValid = 1;
        for (struct agentLinkedList* curr = list->next; curr != NULL && isValid; curr = curr->next)
        {
            isValid = curr->agent->id != attemptID;
        }
    }
    return attemptID;
    
}


//push an agent to the list.
void push(simulation* sim, agent* agent){
    struct agentLinkedList* list = sim->agentList;
    struct agentLinkedList* newBlock = malloc(sizeof(struct agentLinkedList));
    newBlock->next = NULL;
    if (list->next != NULL)
    {
        newBlock->next = list->next;
    }
    list->next = newBlock;
    newBlock->agent = agent;
    agent->id = getNewID(sim);
    sim->popCount++;
}

//pops an agent in the list and return it in the res argument.
//return 0 if everything went well, -1 otherwise.
int pop(struct agentLinkedList* list, agent** res){
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
    *res = tmp->agent;
    free(tmp);
    return 0;

}


//pops an agents in the list with the following id and return it in the res argument
//return 0 if everything went well, -1 otherwise.
int popWithId(struct agentLinkedList* list, int id, agent** res){
    struct agentLinkedList* parent = list;
    for (struct agentLinkedList* curr = list->next; curr != NULL; parent = curr, curr = curr->next)
    {
        if (curr->agent->id == id)
        {
            parent->next = curr->next;
            *res = curr->agent;
            free(curr);
            return 1;
        }
        
    }
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

//free the linked list.
void freeLinkedList(struct agentLinkedList* agentLinkedList){
    if (agentLinkedList->next == NULL)
        return;
    struct agentLinkedList* parent;
    while (agentLinkedList != NULL){
        parent = agentLinkedList;
        agentLinkedList = agentLinkedList->next;
        if (parent->agent != NULL)
            freeAgent(parent->agent);
        free(parent);
    }
}

void drawAgents(SDL_Surface* screen, struct agentLinkedList* list){
    if (list->next == NULL) //check if the list is empty
        return;
    //Uint32 color = SDL_MapRGB(screen->format, 255, 0, 0); temporary, should be really cool to be able to put an image instead

    list = list->next; // skip the sentinel
    while (list != NULL){
        SDL_Rect rect;
        rect.w = AGENT_SIZE;
        rect.h = AGENT_SIZE;
        agent* agent = list->agent;
        rect.x = (Sint16)agent->Xpos; //weird cast but my IDE seems more fine with it than without, might just be fine tho
        rect.y = (Sint16)agent->Ypos;
        SDL_FillRect(screen, &rect, agent->type->color);
        list = list->next;
    }
}

float geneticDrift(float value){
    //srand(time(0));
    float newValue = value;
    float drift = value * GENETICDRIFT;
    if (rand()%2)
        newValue += drift;
    else
        newValue -= drift;
    return newValue;
}

agentType* reproductionWithGeneticDrift(agent* agent1, agent* agent2){
    float newEnergy, newSpeed, newResistance, newHrange , newFertilityRate;
    newEnergy = agent1->type->energy/2;
    agentType* agentType1 = agent1->type;
    agentType* agentType2 = agent2->type;
    if (rand()%2){
        newSpeed = geneticDrift(agentType1->speed);
        newResistance = geneticDrift(agentType1->resistance);
        newHrange = geneticDrift(agentType1->hearingRange);
        newFertilityRate = geneticDrift(agent1->type->fertilityRate);
    } else{
        newSpeed = geneticDrift(agentType2->speed);
        newResistance = geneticDrift(agentType2->resistance);
        newHrange = geneticDrift(agentType2->hearingRange);
        newFertilityRate = geneticDrift(agent1->type->fertilityRate);
    }
    return createAgentType(agentType1->name, agentType1->typeId, 200, newEnergy,
                           newSpeed, newResistance, newHrange, agent1->type->birthRate, newFertilityRate,
                           agent1->type->birthCost, agent1->type->individualBirthCost, agent1->type->moveCost);
}

agentType* normalReproduction(agent* agent1, agent* agent2){ //not nice..
    float newLifeSpan, newEnergy, newSpeed, newResistance, newHrange, newFertilityRate;
    newEnergy = agent1->type->energy/2;
    agentType* agentType1 = agent1->type;
    agentType* agentType2 = agent2->type;
    if (rand()%2)
        newLifeSpan = agentType1->lifeSpan;
    else
        newLifeSpan = agentType2->lifeSpan;
    if (rand()%2)
        newSpeed = agentType1->speed;
    else
        newSpeed = agentType2->speed;
    if (rand()%2)
        newResistance = agentType1->resistance;
    else
        newResistance = agentType2->resistance;
    if (rand()%2)
        newHrange = agentType1->hearingRange;
    else
        newHrange = agentType2->hearingRange;
    if (rand()%2)
        newFertilityRate = agentType1->fertilityRate;
    else
        newFertilityRate = agentType2->fertilityRate;

    return createAgentType(agentType1->name, agentType1->typeId, newLifeSpan, newEnergy,
                           newSpeed, newResistance, newHrange, agent1->type->birthRate, agent1->type->fertilityRate,
                           agent1->type->birthCost, agent1->type->individualBirthCost, agent1->type->moveCost);
}

void reproduction(agent* agent1, agent* agent2, simulation* sim){
    agent1->type->energy -= agent1->type->birthCost;
    agent2->type->energy -= agent2->type->birthCost;
    for (int i = 0; i < agent1->type->birthRate; ++i) {
        if (rand() % 100 <= agent1->type->fertilityRate){
            agentType* newType;
            if (agent1->type->asexualReproduction || rand()%2){ //pick if there will be drift or not
                newType = reproductionWithGeneticDrift(agent1, agent2);
            }
            else {
                newType = normalReproduction(agent1, agent2);
            }
            agent* newAgent = createAgent(newType, agent1->Xpos, agent1->Ypos);
            //newAgent->type->energy = REPRODUCTION_THRESHOLD; //TEMPORARY, idk why this is here lol
            newAgent->type->targetAmount = agent1->type->targetAmount;
            newAgent->type->targetsId = calloc(agent1->type->targetAmount, sizeof(int));
            for (int i = 0; i < agent1->type->targetAmount; i++)
            {
                newAgent->type->targetsId[i] = agent1->type->targetsId[i];
            }

            newAgent->type->color = agent1->type->color;
            push(sim, newAgent);

            agent1->type->energy -= agent1->type->individualBirthCost;
        }
    }
};



float energyCost(float actionCost, agent* mainAgent, simulation* sim)
{
    float cost = actionCost;
    for (sicknessLinkedList* curr = mainAgent->SLL->next; curr != NULL; curr = curr->next )
    {
        cost += actionCost * curr->data->severity;
    }
    Uint8 r = 0;
    Uint8 g = 0;
    Uint8 b = 0;
    SDL_GetRGB(get_pixel(sim->terrain, mainAgent->Xpos,mainAgent->Ypos),sim->terrain->format,&r, &g, &b);
    float res = (float)(r + g + b) / (3 * 256);
    cost += (mainAgent->type->resistance - res) * (mainAgent->type->resistance - res) * actionCost;
    return cost;
}


int moveTowards(agent* agent, simulation* sim, int x, int y)
{
    if (agent->type->speed <= 0)
    {
        return 0;
    }
    
    double xOFF = (double)x-agent->Xpos;
    double yOFF = (double)y-agent->Ypos;
    double distance = sqrt(xOFF*xOFF + yOFF*yOFF);
    if (distance > (double)agent->type->speed)
    {
        double ratio = (double)agent->type->speed / distance;
        moveAgent(agent, (int)(xOFF * ratio), (int)(yOFF * ratio), sim->screen);
        agent->type->energy -= energyCost(0.1,agent,sim); //agent->type->speed * agent->type->speed;
        return 0;
    }
    agent->Xpos = x;
    agent->Ypos = y;
    agent->type->energy -= energyCost(0.1,agent,sim);  //(float) distance * distance;
    return 1;
}


int canSeeAgent(agent* mainAgent, agent* targetAgent)
{
    int xOffset = targetAgent->Xpos - mainAgent->Xpos;
    int yOffset = targetAgent->Ypos - mainAgent->Ypos;
    double distance = sqrt((double)xOffset*xOffset + yOffset*yOffset);
    return distance <= (double)mainAgent->type->hearingRange;
}


int tryFeedAgent(agent* mainAgent, simulation* sim)
{
    agent* target = NULL;
    double minDist = 0;
    for (struct agentLinkedList* curr = sim->agentList->next; curr != NULL; curr = curr->next)
    {
        agent* currAgent = curr->agent;
        if (mainAgent == currAgent || (!canSeeAgent(mainAgent,currAgent)) || currAgent->type->timeLeft <= 0)
        {
            continue;
        }

        for (int i = 0; i < mainAgent->type->targetAmount; i++)
        {
            if (mainAgent->type->targetsId[i] == currAgent->type->typeId)
            {
                int xOffset = currAgent->Xpos - mainAgent->Xpos;
                int yOffset = currAgent->Ypos - mainAgent->Ypos;
                double distance = sqrt((double)xOffset*xOffset + yOffset*yOffset);
                if (target == NULL || minDist > distance)
                {
                    minDist = distance;
                    target = currAgent;
                }
            }
        }
    }
    if (target != NULL)
    {
        if(moveTowards(mainAgent,sim,target->Xpos,target->Ypos))
        {
            mainAgent->type->energy += target->type->energy;
            popWithId(sim->agentList,target->id,&target);
            sim->popCount--;
            freeAgent(target);
        }
        return 1;
    }
    
    return 0;
}

int canSeeFruit(agent* mainAgent, food* fruit){
    int xOffset = fruit->Xpos - mainAgent->Xpos;
    int yOffset = fruit->Ypos - mainAgent->Ypos;
    double distance = sqrt((double)xOffset*xOffset + yOffset*yOffset);
    return distance <= (double)mainAgent->type->hearingRange;
}

int tryFeedFruit(agent* mainAgent, simulation* sim){
    food* target = NULL;
    double minDist = 0;
    for (struct foodNode* curr = sim->foodHandler->foodList->next; curr != NULL; curr = curr->next)
    {
        food* currFruit = curr->food;
        if (!canSeeFruit(mainAgent, currFruit) || currFruit->lifeSpan == 0)
        {
            continue;
        }

        int xOffset = currFruit->Xpos - mainAgent->Xpos;
        int yOffset = currFruit->Ypos - mainAgent->Ypos;
        double distance = sqrt((double)xOffset*xOffset + yOffset*yOffset);
        if (target == NULL || minDist > distance)
        {
            minDist = distance;
            target = currFruit;
        }

    }
    if (target != NULL)
    {
        if(moveTowards(mainAgent, sim, target->Xpos, target->Ypos))
        {
            mainAgent->type->energy += target->energyToGive;
            popWithPos(sim->foodHandler->foodList, target->Xpos, target->Ypos);
            target->father->currentFood--;
            free(target);
        }
        return 1;
    }

    return 0;
}

int tryMate(agent* mainAgent, simulation* sim)
{
    agent* target = NULL;
    double minDist = 0;
    if (mainAgent->type->asexualReproduction && mainAgent->type->energy >= REPRODUCTION_THRESHOLD){
        reproduction(mainAgent, mainAgent, sim);
        return 1;
    }
    for (struct agentLinkedList* curr = sim->agentList->next; curr != NULL; curr = curr->next)
    {
        agent* currAgent = curr->agent;
        if (mainAgent == currAgent || (!canSeeAgent(mainAgent,currAgent) || currAgent->type->timeLeft <= 0))
        {
            continue;
        }
        if ((mainAgent->type->typeId == currAgent->type->typeId) && (currAgent->type->energy > REPRODUCTION_THRESHOLD))
        {
            int xOffset = currAgent->Xpos - mainAgent->Xpos;
            int yOffset = currAgent->Ypos - mainAgent->Ypos;
            double distance = sqrt((double)xOffset*xOffset + yOffset*yOffset);
            if (target == NULL || minDist > distance)
            {
                minDist = distance;
                target = currAgent;
            }
        }
    }
    if (target != NULL)
    {
        if (moveTowards(mainAgent, sim, target->Xpos, target->Ypos))
        {
            reproduction(mainAgent,target,sim);
        }
        return 1;
    }
    
    return 0;
}

int canInfectAgent(agent* mainAgent, agent* targetAgent)
{
    double xOffset = (double)targetAgent->Xpos - (double)mainAgent->Xpos;
    double yOffset = (double)targetAgent->Ypos - (double)mainAgent->Ypos;
    double distance = sqrt(xOffset*xOffset + yOffset*yOffset);
    return distance <= (double)3;
}


void doInfect(agent* mainAgent, simulation* sim)
{
    
    for (struct agentLinkedList* curr = sim->agentList->next; curr != NULL; curr = curr->next)
    {
        agent* currAgent = curr->agent;
        if (mainAgent == currAgent || (!canInfectAgent(mainAgent,currAgent) || currAgent->type->timeLeft <= 0))
        {
            continue;
        }
        for (struct sicknessLinkedList* sickList = mainAgent->SLL->next; sickList != NULL; sickList = sickList->next)
        {
        
            sickness* sick = sickList->data;
            for (size_t i = 0; i < sick->canInfectAmount; i++)
            {
                if (sick->mortality > (float)rand() / (float)(RAND_MAX))
                {
                    mainAgent->type->timeLeft = 0;
                }
                
                if (currAgent->type->typeId == sick->canInfectID[i])
                {
                    if((float)rand() / (float)(RAND_MAX) < (double)(sick->infectiousness))
                    {
                        addSickness(currAgent->SLL,sick);
                    }
                }
            }
            
        
        }
    }
}



// makes an agent behave; returns 2 when trying to mate, 1 when trying to feed, and 0 when wandering.
int agentBehave(agent* mainAgent, simulation* sim)
{  
    //printf("behave\n");
    doInfect(mainAgent,sim);
    if (mainAgent->type->energy > REPRODUCTION_THRESHOLD)
    {
        if (tryMate(mainAgent, sim))
        {
            //printf("mated\n");
            return 2;
        }
    }
    if ((mainAgent->type->targetAmount == 0 && tryFeedFruit(mainAgent, sim)) || tryFeedAgent(mainAgent, sim))
    {
        return 1;
    }


    doWander(mainAgent,sim);

    return 0;
    
}


void doWander(agent* mainAgent, simulation* sim)
{
    if (mainAgent->wanderX == mainAgent->Xpos || mainAgent->wanderX == -1 || mainAgent->wanderY == mainAgent->Ypos)
    {
        int newX = (mainAgent->Xpos - 100) + (rand() % 200);
        int newY = (mainAgent->Ypos - 100) + (rand() % 200);
        mainAgent->wanderX = MIN(MAX(0, newX), sim->screen->w);
        mainAgent->wanderY = MIN(MAX(0, newY), sim->screen->h);
    }
    moveTowards(mainAgent, sim, mainAgent->wanderX,mainAgent->wanderY);
}