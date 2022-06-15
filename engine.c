#include <SDL/SDL_mouse.h>
#include <err.h>
#include "engine.h"
#include "agent.h"
#include "pixelOp.h"

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define FAST 13
#define MEDIUM 25
#define SLOW 50

SDL_Surface *PLAY;
SDL_Surface *PAUSE;
SDL_Surface *STOP;
SDL_Surface *X1;
SDL_Surface *X2;
SDL_Surface *X3;

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

simulation *initEngine(int w, int h, SDL_Surface *terrain)
{

    loop = 1;
    SDL_Surface *renderer;
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) == -1)
    {
        errx(1, "couldn't initialize sdl: %s", SDL_GetError());
    }
    atexit(SDL_Quit);

    renderer = SDL_SetVideoMode(w, h, 32, SDL_HWSURFACE);
    if(renderer == NULL)
    {
        errx(1, "couldn't set 1920x1080x8 video mode: %s", SDL_GetError());
    }
    
    SDL_Surface *screen = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);

    BLACK  = SDL_MapRGB(renderer->format, 0,   0,   0);
    WHITE  = SDL_MapRGB(renderer->format, 255, 255, 255);
    RED    = SDL_MapRGB(renderer->format, 255, 0,   0);
    GREEN  = SDL_MapRGB(renderer->format, 0,   255, 0);
    BLUE   = SDL_MapRGB(renderer->format, 0,   0,   255);
    YELLOW = SDL_MapRGB(renderer->format, 255, 255, 0);
    PURPLE = SDL_MapRGB(renderer->format, 255, 0,   255);
    CYAN   = SDL_MapRGB(renderer->format, 0,   255, 255);
    PLAY = load_image("assets/play.png");
    PAUSE = load_image("assets/pause.png");
    X1 = load_image("assets/x1.png");
    X2 = load_image("assets/x2.png");
    X3 = load_image("assets/x3.png");
    STOP = load_image("assets/stop.png");

    simulation *sim = malloc(sizeof(simulation));
    sim->renderer = renderer;
    sim->screen = screen;
    sim->terrain = terrain;
    sim->agentList = initLinkedList();
    sim->foodHandler = initFoodHandler(screen->w, screen->h);
    sim->popCount = 0;
    sim->tickInterval = SLOW;
    sim->isPaused = 0;

    int dim = MIN(w, h) / 15;
    button play = {x: 10, y: 10, w: 50, h: 50, image: PAUSE};
    button speed = {x: dim + 20, y: 10, w: 50, h: 50, image: X1};
    button stop = {x: w - dim - 10, y: 10, w: 50, h: 50, image: STOP};
    sim->play = play;
    sim->speed = speed;
    sim->stop = stop;
    return sim;
}

void free_simulation(simulation *sim)
{
    SDL_FreeSurface(sim->renderer);
    SDL_FreeSurface(sim->screen);
    freeLinkedList(sim->agentList);
    freeFoodHandler(sim->foodHandler);
    SDL_FreeSurface(PLAY);
    SDL_FreeSurface(PAUSE);
    SDL_FreeSurface(STOP);
    SDL_FreeSurface(X1);
    SDL_FreeSurface(X2);
    SDL_FreeSurface(X3);
    free(sim);
}

void toggle_play(simulation *sim)
{
    sim->isPaused = !(sim->isPaused);
    if(sim->isPaused)
        sim->play.image = PLAY;
    else
        sim->play.image = PAUSE;
}

void toggle_speed(simulation *sim)
{
    switch (sim->tickInterval)
    {
    case FAST:
        sim->speed.image = X1;
        sim->tickInterval = SLOW;
        break;

    case MEDIUM:
        sim->speed.image = X3;
        sim->tickInterval = FAST;
        break;

    case SLOW:
        sim->speed.image = X2;
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
    button stop = sim->stop;

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
    else if(x >= stop.x && x <= stop.x + stop.w &&
        y >= stop.y && y <= stop.y + stop.h)
    {
        loop = 0;
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
    SDL_Surface *renderer = sim->renderer;
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

    SDL_Rect btn3 = {
        sim->stop.x,
        sim->stop.y,
        sim->stop.w,
        sim->stop.h
    };

    SDL_BlitSurface(sim->play.image, NULL, renderer, &btn1);
    SDL_BlitSurface(sim->speed.image, NULL, renderer, &btn2);
    SDL_BlitSurface(sim->stop.image, NULL, renderer, &btn3);
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
        SDL_BlitSurface(sim->screen, NULL, sim->renderer, NULL);
        display_buttons(sim);
        SDL_UpdateRect(sim->renderer, 0, 0, 0, 0);
        
        SDL_Delay(time_left());
        next_time += sim->tickInterval;
    }

	
}