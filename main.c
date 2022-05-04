#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "engine.h"
#include "pixelOp.h"
#include "agent.h"

int x = 180, y = 328;
int dx = 1, dy = 2;

void update(simulation *sim)
{
    SDL_Surface *screen = sim->screen;
    Uint32 color = SDL_MapRGB(screen->format, 255, 255, 255);

    x += dx;
    y += dy;

    if(x < 0)
    {
        x = 0;
        dx *= -1;
    }
    if(x >= screen->w)
    {
        x = screen->w;
        dx *= -1;
    }
    if(y < 0)
    {
        y = 0;
        dy *= -1;
    }
    if(y >= screen->h)
    {
        y = screen->h;
        dy *= -1;
    }
    
    while (sim->popCount < 500)
    {
        agentType* aType = createAgentType("sheep",100,100,3,6,100);
        aType->targetAmount = 0;
        aType->color = SDL_MapRGB(sim->screen->format, 0, 255, 0);
        push(sim,createAgent(aType,rand() % (sim->screen->w),rand() % (sim->screen->h)));
    }
    
    //put_pixel(screen, x, y, color);
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
    simulation *sim = initEngine();
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
    return 0;
}