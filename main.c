#include <stdio.h>
#include <time.h>
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
    SDL_BlitSurface(terrain, NULL, sim->screen, NULL);
}

int main()
{
    srand(time(NULL));
    terrain = SDL_CreateRGBSurface(0, W_WIDTH, W_HEIGHT, 32, 0, 0, 0 ,0);

    init_gradients(W_WIDTH, W_HEIGHT);
    float xoff = 0.0;
    for(int x = 0; x < W_WIDTH; x++)
    {
        float yoff = 0.0;
        for (int y = 0; y < W_HEIGHT; y++)
        {
            float p = (perlin(xoff, yoff) + 1) / 2;
            int r = floorf(p * 255.0);
            Uint32 color = SDL_MapRGB(terrain->format, r, r, r);
            put_pixel(terrain, x, y, color);
            yoff += 0.01;
        }
        xoff += 0.01;
    }
    free_gradients();

    simulation *sim = initEngine(W_WIDTH, W_HEIGHT);

    run(sim, update);

    free_simulation(sim);
    
    SDL_FreeSurface(terrain);
    return 0;
}