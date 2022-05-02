#include <SDL/SDL_mouse.h>
#include <err.h>
#include "engine.h"

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define FAST 13
#define MEDIUM 25
#define SLOW 50

Uint32 BLACK;
Uint32 WHITE;
Uint32 RED;
Uint32 GREEN;
Uint32 BLUE;
Uint32 YELLOW;
Uint32 PURPLE;
Uint32 CYAN;

static Uint32 next_time;
static int loop = 1;

simulation *initEngine(int w, int h)
{
    SDL_Surface *screen;
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) == -1)
    {
        errx(1, "couldn't initialize sdl: %s", SDL_GetError());
    }
    atexit(SDL_Quit);

    screen = SDL_SetVideoMode(w, h, 32, SDL_HWSURFACE);
    if(screen == NULL)
    {
        errx(1, "couldn't set 1920x1080x8 video mode: %s", SDL_GetError());
    }
    
    BLACK  = SDL_MapRGB(screen->format, 0,   0,   0);
    WHITE  = SDL_MapRGB(screen->format, 255, 255, 255);
    RED    = SDL_MapRGB(screen->format, 255, 0,   0);
    GREEN  = SDL_MapRGB(screen->format, 0,   255, 0);
    BLUE   = SDL_MapRGB(screen->format, 0,   0,   255);
    YELLOW = SDL_MapRGB(screen->format, 255, 255, 0);
    PURPLE = SDL_MapRGB(screen->format, 255, 0,   255);
    CYAN   = SDL_MapRGB(screen->format, 0,   255, 255);
    
    simulation *sim = malloc(sizeof(simulation));
    sim->screen = screen;
    sim->tickInterval = SLOW;
    sim->isPaused = 0;

    int dim = MIN(w, h) / 15;
    button play = {x: 10, y: 10, w: dim, h: dim, color: CYAN};
    button speed = {x: dim + 20, y: 10, w: dim, h: dim, color: GREEN};
    sim->play = play;
    sim->speed = speed;
    return sim;
}

void free_simulation(simulation *sim)
{
    SDL_FreeSurface(sim->screen);
    free(sim);
}

void toggle_play(simulation *sim)
{
    sim->isPaused = !(sim->isPaused);
    if(sim->isPaused)
        sim->play.color = BLUE;
    else
        sim->play.color = CYAN;
}

void toggle_speed(simulation *sim)
{
    switch (sim->tickInterval)
    {
    case FAST:
        sim->speed.color = GREEN;
        sim->tickInterval = SLOW;
        break;

    case MEDIUM:
        sim->speed.color = RED;
        sim->tickInterval = FAST;
        break;

    case SLOW:
        sim->speed.color = YELLOW;
        sim->tickInterval = MEDIUM;
        break;
    
    default:
        break;
    }
}

void on_click(simulation *sim)
{
    int x, y;
    SDL_GetMouseState(&x, &y);

    button play = sim->play;
    button speed = sim->speed;

    if(x >= play.x && x <= play.x + play.w &&
        y >= play.y && y <= play.y + play.h)
    {
        toggle_play(sim);
    }
    else if(x >= speed.x && x <= speed.x + speed.w &&
        y >= speed.y && y <= speed.y + speed.h)
    {
        toggle_speed(sim);
    }
}

void getEvents(simulation *sim)
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                loop = 0;
                break;
            
            case SDL_MOUSEBUTTONUP:
                on_click(sim);
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

void display_buttons(simulation *sim)
{
    SDL_Surface *screen = sim->screen;
    SDL_Rect btn1 = {
        sim->play.x,
        sim->play.y,
        sim->play.w,
        sim->play.h
    };

    SDL_Rect btn2 = {
        sim->speed.x,
        sim->speed.y,
        sim->speed.w,
        sim->speed.h
    };
    SDL_FillRect(screen, &btn1, sim->play.color);
    SDL_FillRect(screen, &btn2, sim->speed.color);
}

void run(simulation *sim, void (*update)(simulation *))
{
    next_time = SDL_GetTicks();
    
    while(loop)
    {
        getEvents(sim);

        if(!sim->isPaused)
        {
            SDL_FillRect(sim->screen, NULL, BLACK);

            (*update)(sim);
            
        }
        display_buttons(sim);
        SDL_UpdateRect(sim->screen, 0, 0, 0, 0);
        
        SDL_Delay(time_left());
        next_time += sim->tickInterval;
    }
}