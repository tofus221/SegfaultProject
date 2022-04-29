#include <stdlib.h>
#include <memory.h>
#include <err.h>
#include "agent.h"

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

//actual agent in the simulation
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

//pops an agent in the list and return it.
agent* pop(struct agentLinkedList* list){
    if (list->next == NULL)
        errx(2, "pop: you are trying to pop an empty list");
    struct agentLinkedList* tmp = list;
    struct agentLinkedList* parent;
    while (tmp->next != NULL){
        parent = tmp;
        tmp = tmp->next;
    }
    parent->next = NULL;
    agent* toReturn = tmp->agent;
    free(tmp);
    return toReturn;

}

//pops an agents in the list with the following id and return it.
agent* popWithId(struct agentLinkedList* list, int id){
    if (list->next == NULL)
        errx(2, "popWithId: you are trying to pop an empty list");

    struct agentLinkedList* tmp = list->next; //skip the sentinel
    struct agentLinkedList* parent = list;
    while (tmp->agent->id != id && tmp->next != NULL){
        parent = tmp;
        tmp = tmp->next;
    }
    if (tmp->next == NULL && tmp->agent->id != id)
        errx(2, "popWithId: no agents with that ID found");
    parent->next = tmp->next;
    agent* toReturn = tmp->agent;
    free(tmp);
    return toReturn;
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