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
    while (sim->popCount < 500)
    {
        agentType* aType = createAgentType("sheep", 2, 100,100,3,6,100, 1, 1, 1);
        aType->targetAmount = 0;
        aType->color = SDL_MapRGB(screen->format, 0, 255, 0);
        push(sim,createAgent(aType,rand() % (screen->w),rand() % (screen->h)));
    }
    
    for (struct agentLinkedList *al = sim->agentList->next; al != NULL;)
    {
        
        
        if (al->agent->type->typeId == 1)
        {
            if (al->agent->type->energy <= 0.0f || al->agent->type->lifeSpan < 0.0f)
            {
                struct agentLinkedList *temp = al;
                al = al->next;
                agent* res;
                popWithId(sim->agentList,temp->agent->id,&res);
                freeAgent(res);
                sim->popCount--;
                continue;
            }
            
            int res = agentBehave(al->agent,sim);
            
            al->agent->type->lifeSpan -= 1;
        }
        else
        {
            doWander(al->agent,sim);
        }
        //printf("pop = %i\n",sim->popCount);
        al = al->next;
    }
    
    drawAgents(screen,sim->agentList);
}

int main()
{
    terrain = perlin_surface(W_WIDTH, W_HEIGHT, 0.005);
    srand(time(0));
    simulation *sim = initEngine(W_WIDTH, W_HEIGHT);
    for (size_t i = 0; i < 50; i++)
    {
        agentType* aType = createAgentType("wolf", 1, 1000,1000,4,6, 100, 1, 1, 1);
        aType->targetAmount = 1;
        aType->targetsId = calloc(aType->targetAmount, sizeof(int));
        aType->targetsId[0] = 2;
        aType->color = SDL_MapRGB(sim->screen->format, 255, 0, 0);
        push(sim,createAgent(aType,rand() % (sim->screen->w),rand() % (sim->screen->h)));
    }
    run(sim, update);
    
    free_simulation(sim);
    
    SDL_FreeSurface(terrain);
    
    SDL_Quit();
    return 0;
}