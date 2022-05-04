#include <stdio.h>
#include <time.h>
#include "engine.h"
#include "perlin.h"
#include <math.h>

#define W_WIDTH 700
#define W_HEIGHT 700

SDL_Surface *terrain;
Uint8 r = 0;
void update(simulation *sim)
{
    SDL_BlitSurface(terrain, NULL, sim->screen, NULL);
}

int main()
{
    terrain = perlin_surface(W_WIDTH, W_HEIGHT, 0.005);
    simulation *sim = initEngine(W_WIDTH, W_HEIGHT);

    run(sim, update);

    free_simulation(sim);
    
    SDL_FreeSurface(terrain);
    return 0;
}