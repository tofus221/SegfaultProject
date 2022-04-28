#ifndef ENGINE_H
#define ENGINE_H

#include <SDL/SDL.h>
#include "agent.h"

typedef struct simulation
{
    SDL_Surface *screen;
    agent* agentList;

} simulation;

simulation *initEngine();
void free_simulation(simulation *sim);
void run(simulation *sim, void (*update)(simulation *));

#endif