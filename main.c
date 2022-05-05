#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "engine.h"
#include "pixelOp.h"
#include "agent.h"
#include "perlin.h"
#include <math.h>

#define W_WIDTH 700
#define W_HEIGHT 700

SDL_Surface *terrain;
Uint8 r = 0;
void update(simulation *sim)
{
    SDL_Surface *screen = sim->screen;
    Uint32 color = SDL_MapRGB(screen->format, 255, 255, 255);

   
    while (sim->popCount < 500)
    {
        agentType* aType = createAgentType("sheep",100,100,3,6,100);
        aType->targetAmount = 0;
        aType->color = SDL_MapRGB(sim->screen->format, 0, 255, 0);
        push(sim,createAgent(aType,rand() % (sim->screen->w),rand() % (sim->screen->h)));
    }
    
    for (struct agentLinkedList *al = sim->agentList->next; al != NULL;)
    {
        
        
        if (strcmp(al->agent->type->name, "wolf") == 0)
        {
            if (al->agent->type->energy <= 0.0f)
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
        }
        else
        {
            doWander(al->agent,sim);
        }
        printf("pop = %i\n",sim->popCount);
        al = al->next;
    }
    
    drawAgents(sim->screen,sim->agentList);
}

int main()
{
    srand(time(0));
    simulation *sim = initEngine(W_WIDTH, W_HEIGHT);
    for (size_t i = 0; i < 50; i++)
    {
        agentType* aType = createAgentType("wolf",1000,1000,4,6,100);
        aType->targetAmount = 1;
        aType->targets = calloc(aType->targetAmount, sizeof(char*));
        aType->targets[0] = "sheep";
        aType->color = SDL_MapRGB(sim->screen->format, 255, 0, 0);
        push(sim,createAgent(aType,rand() % (sim->screen->w),rand() % (sim->screen->h)));
    }
    run(sim, update);

    free_simulation(sim);
    
    SDL_FreeSurface(terrain);
    return 0;
}