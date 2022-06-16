#ifndef ENGINE_H
#define ENGINE_H

#include <SDL/SDL.h>
#include "food.h"


typedef struct button
{
    int x;
    int y;
    int w;
    int h;
    SDL_Surface *image;
} button;

typedef struct simulation
{
    SDL_Surface *screen;
    SDL_Surface *terrain;
    SDL_Surface *renderer;
    struct agentLinkedList* agentList;
    struct agentType *agents;
    //this isn't the current population, it is used to determine the id of every agent. It will only increase.
    foodHandler* foodHandler;
    button play;
    button speed;
    button stop;
    int tickInterval;
    int isPaused;
} simulation;


simulation *initEngine(int w, int h, SDL_Surface *terrain);
void free_simulation(simulation *sim);
void run(simulation *sim, void (*update)(simulation *));

#include "agent.h"
#endif