#include <stdio.h>
#include <time.h>
#include "engine.h"
#include "pixelOp.h"
#include "perlin.h"
#include <math.h>

SDL_Surface *terrain;
Uint8 r = 0;
void update(simulation *sim)
{
    SDL_BlitSurface(terrain, NULL, sim->screen, NULL);
}

int main()
{
    srand(time(NULL));
    terrain = SDL_CreateRGBSurface(0, 500, 500, 32, 0, 0, 0 ,0);

    init_gradients(500, 500);
    for(int x = 0; x < 500; x++)
    {
        for (int y = 0; y < 500; y++)
        {
            float p = (perlin(x, y) + 1) / 2;
            int r = floorf(p * 255.0);
            printf("%d\n", r);
            Uint32 color = SDL_MapRGB(terrain->format, r, r, r);
            put_pixel(terrain, x, y, color);
        }
        
    }
    free_gradients();

    simulation *sim = initEngine(500, 500);

    run(sim, update);

    free_simulation(sim);
    
    SDL_FreeSurface(terrain);
    return 0;
}