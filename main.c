#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "agent.h"
#include "sickness.h"
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
        agentType* aType = createAgentType("sheep", 2, 200, 150, 3, 0.5, 40, 2, 300, 50);
        aType->targetAmount = 0;
        aType->color = SDL_MapRGB(screen->format, 0, 255, 0);
        push(sim,createAgent(aType,rand() % (screen->w),rand() % (screen->h)));
    }
    
    for (struct agentLinkedList *al = sim->agentList->next; al != NULL;)
    {
        
        
        if (al->agent->type->typeId == 1)
        {
            if (al->agent->type->energy <= 0.0f || al->agent->type->timeLeft < 0.0f)
            {
                struct agentLinkedList *temp = al;
                al = al->next;
                agent* res;
                popWithId(sim->agentList,temp->agent->id,&res);
                freeAgent(res);
                sim->popCount--;
                continue;
            }
            
            agentBehave(al->agent,sim);
            
            al->agent->type->timeLeft -= 1;
        }
        else
        {
            doWander(al->agent,sim);
        }
        al = al->next;
    }


    drawAgents(screen,sim->agentList);
    drawFood(sim->foodHandler, screen);
}

int main()
{
    
    terrain = perlin_surface(W_WIDTH, W_HEIGHT, 0.005);
    srand(time(0));
    simulation *sim = initEngine(W_WIDTH, W_HEIGHT, terrain);

    int infect[1] ={1}; 
    sickness* droopy_nose = createSickness("droop",0.5,1,0.01,infect,1,SDL_MapRGB(sim->screen->format, 0, 0, 0));
    for (size_t i = 0; i < 50; i++)
    {
        agentType* aType = createAgentType("wolf", 1, 200, 150, 4, 0.5, 40, 2, 300, 50);
        aType->targetAmount = 1;
        aType->targetsId = calloc(aType->targetAmount, sizeof(int));
        aType->targetsId[0] = 2;
        aType->color = SDL_MapRGB(sim->screen->format, 255, 0, 0);
        agent* wolf = createAgent(aType,rand() % (sim->screen->w),rand() % (sim->screen->h));
        addSickness(wolf->SLL,droopy_nose);
        push(sim, wolf);
    }
    run(sim, update);
    free(droopy_nose);
    
    free_simulation(sim);
    
    SDL_FreeSurface(terrain);
    
    SDL_Quit();
    return 0;
}