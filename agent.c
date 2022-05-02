#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "agent.h"
#include "SDL/SDL.h"

//create an Agent Type, meant to be used with the interface.
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

//create an actual agent in the simulation.
agent* createAgent(int id, agentType* type, int x, int y){
    agent* newAgent = malloc(sizeof (agent));
    memset(newAgent, 0, sizeof (agent));
    newAgent->id = id;
    newAgent->type = type;
    newAgent->Xpos = x;
    newAgent->Ypos = y;
    return newAgent;
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
void push(struct agentLinkedList* list, agent* agentToAdd){
    struct agentLinkedList* tmp = list;
    struct agentLinkedList* toAdd = malloc(sizeof(struct agentLinkedList));
    toAdd->next = NULL;
    toAdd->agent = agentToAdd;
    while (tmp->next != NULL)
        tmp = tmp->next;
    tmp->next = toAdd;
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

//free the linked list.
void freeLinkedList(struct agentLinkedList* agentLinkedList){
    struct agentLinkedList* parent;
    while (agentLinkedList->next != NULL){
        parent = agentLinkedList;
        agentLinkedList = agentLinkedList->next;
        free(parent->agent);
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

    list = list->next; // skip the sentinel
    while (list->next != NULL){
        agent* agent = list->agent;
        rect.x = (Sint16)agent->Xpos; //weird cast but my IDE seems more fine with it than without, might just be fine tho
        rect.y = (Sint16)agent->Ypos;
        SDL_FillRect(screen, &rect, color);
        list = list->next;
    }
}