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

    
    drawAgents(screen,sim->agentList);
}

int main()
{
    terrain = perlin_surface(W_WIDTH, W_HEIGHT, 0.005);
    srand(time(0));
    simulation *sim = initEngine(W_WIDTH, W_HEIGHT);

    agentType *test = createAgentType("r", 1, 2 ,500,2 ,2,100,5,2,2);
    agent *agent1 = createAgent(test, 200, 200);
    agent *agent2 = createAgent(test, 210, 210);
    push(sim, agent1);
    push(sim, agent2);

    printf("reprod1\n");
    reproduction(agent1, agent2, sim);
    printf("\n");

    printf("reprod2\n");

    reproduction(agent1, agent2, sim);

    run(sim, update);
    
    free_simulation(sim);
    
    SDL_FreeSurface(terrain);
    
    SDL_Quit();
    return 0;
}