#ifndef ENGINE_H
#define ENGINE_H

#include <SDL/SDL.h>

typedef struct simulation
{
    SDL_Surface *screen;
} simulation;

simulation *initEngine();
void free_simulation(simulation *sim);
void run(simulation *sim, void (*update)(simulation *));

#endif