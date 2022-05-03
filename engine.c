#include <SDL/SDL_mouse.h>
#include <err.h>
#include "engine.h"
#include "agent.h"

#define FAST 13
#define MEDIUM 25
#define SLOW 50

static int TICK_INTERVAL = MEDIUM;

static Uint32 next_time;
static int loop = 1;

simulation *initEngine()
{
    SDL_Surface *screen;
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) == -1)
    {
        errx(1, "couldn't initialize sdl: %s", SDL_GetError());
    }
    atexit(SDL_Quit);

    screen = SDL_SetVideoMode(500, 500, 32, SDL_HWSURFACE);
    if(screen == NULL)
    {
        errx(1, "couldn't set 1920x1080x8 video mode: %s", SDL_GetError());
    }
    
    simulation *sim = malloc(sizeof(simulation));
    sim->screen = screen;
    sim->agentList = initLinkedList();
    sim->popCount = 0;
    return sim;
}

void free_simulation(simulation *sim)
{
    SDL_FreeSurface(sim->screen);
    freeLinkedList(sim->agentList);
    free(sim);
}

void getEvents()
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                loop = 0;
                break;
        }
    }
}

Uint32 time_left()
{
    Uint32 now = SDL_GetTicks();
    if(next_time <= now)
    {
        return 0;
    }
    else
    {
        return next_time - now;
    }
}

void run(simulation *sim, void (*update)(simulation *))
{
    next_time = SDL_GetTicks();
    
    while(loop)
    {
        getEvents();

        Uint32 white = SDL_MapRGB(sim->screen->format, 255, 255, 255);
        SDL_FillRect(sim->screen, NULL, white);

        (*update)(sim);
        
        SDL_UpdateRect(sim->screen, 0, 0, 0, 0);

        SDL_Delay(time_left());
        next_time += TICK_INTERVAL;
    }
}

void foo(simulation *sim);