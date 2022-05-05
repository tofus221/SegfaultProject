#ifndef ENGINE_H
#define ENGINE_H

#include <SDL/SDL.h>
#include "agent.h"

typedef struct button
{
    int x;
    int y;
    int w;
    int h;
    Uint32 color;
} button;

typedef struct simulation
{
    SDL_Surface *screen;
    struct agentLinkedList* agentList;
    int popCount; //this isn't the current population, it is used to determine the id of every agent. It will only increase.
    button play;
    button speed;
    int tickInterval;
    int isPaused;
} simulation;


simulation *initEngine(int w, int h);
void free_simulation(simulation *sim);
void run(simulation *sim, void (*update)(simulation *));

#endif