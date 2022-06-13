#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "agent.h"
#include "engine.h"
#include "pixelOp.h"
#include "perlin.h"
#include <math.h>

#define W_WIDTH 700
#define W_HEIGHT 700

SDL_Surface *terrain;
Uint8 r = 0;
void update(simulation *sim)
{
    SDL_Surface *screen = sim->screen;
    SDL_BlitSurface(terrain, NULL, screen, NULL);
    spawnFood(sim->foodHandler);
    
    drawAgents(screen,sim->agentList);
    drawFood(sim->foodHandler, screen);
}

int main()
{
    terrain = perlin_surface(W_WIDTH, W_HEIGHT, 0.005);
    srand(time(0));
    simulation *sim = initEngine(W_WIDTH, W_HEIGHT);


    run(sim, update);
    
    free_simulation(sim);
    
    SDL_FreeSurface(terrain);
    
    SDL_Quit();
    return 0;
}