#include <stdlib.h>
#include <memory.h>
#include <err.h>
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
    struct agentLinkedList* toAdd = malloc(sizeof(struct agentLinkedList));
    toAdd->next = NULL;
    toAdd->agent = agentToAdd;
    while (list->next != NULL)
        list = list->next;
    list->next = toAdd;
}

//pops an agent at the index i or the last one if index is NULL and return it.
agent* pop(struct agentLinkedList* list, size_t i){
    if (list->agent == NULL)
        errx(2, "agent.c: you are trying to pop an empty list");

    struct agentLinkedList* parent;
    if (i == NULL){
        while (list->next != NULL){
            parent = list;
            list = list->next;
        }
        parent->next = NULL;
        return list->agent;
    } else {
        for (size_t j = 0; j < i; j++){
            parent = list;
            list = list->next;
        }
        parent->next = NULL;
        return list->agent;
    }
}