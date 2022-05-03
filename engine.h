#ifndef ENGINE_H
#define ENGINE_H

#include <SDL/SDL.h>
//#include "agent.h"

typedef struct simulation
{
    SDL_Surface *screen;
    struct agentLinkedList* agentList;
    int popCount; //this isn't the current population, it is used to determine the id of every agent. It will only increase.
} simulation;

simulation *initEngine();
void free_simulation(simulation *sim);
void run(simulation *sim, void (*update)(simulation *));

#endif