#include <stdio.h>
#include "engine.h"
#include "pixelOp.h"

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
    put_pixel(screen, x, y, color);
}

int main()
{
    simulation *sim = initEngine();

    run(sim, update);

    free_simulation(sim);
    return 0;
}