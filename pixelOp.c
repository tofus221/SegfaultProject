#include "pixelOp.h"

static inline
Uint8* pixel_ref(SDL_Surface *surf, unsigned x, unsigned y)
{
    int bpp = surf->format->BytesPerPixel;
    return (Uint8*)surf->pixels + y * surf->pitch + x * bpp;
}

void put_pixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel)
{
    Uint8 *p = pixel_ref(surface, x, y);

    switch(surface->format->BytesPerPixel)
    {
        case 1:
            *p = pixel;
            break;

        case 2:
            *(Uint16 *)p = pixel;
            break;

        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            }
            else
            {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;

        case 4:
            *(Uint32 *)p = pixel;
            break;
    }
}

Uint32 get_pixel(SDL_Surface *surface, unsigned x, unsigned y)
{
    Uint8 *p = pixel_ref(surface, x, y);

    switch (surface->format->BytesPerPixel)
    {
        case 1:
            return *p;

        case 2:
            return *(Uint16 *)p;

        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;

        case 4:
            return *(Uint32 *)p;
    }

    return 0;
}

/*SDL_Surface *blur(SDL_Surface *screen)
{
    SDL_Surface *newScreen = SDL_CreateRGBSurface(0, screen->w, screen->h, 32, 0, 0, 0, 0);

    for (int x = 0; x < screen->w; x++)
    {
        for (int y = 0; y < screen->h; y++)
        {
            Uint8 alpha = 0;
            Uint8 sum = 0;
            for (int i = -1; i < 2; i++)
            {
                for (int j = -1; j < 2; j++)
                {
                    if(x + i >= 0 &&
                    x + i < screen->w &&
                    y + j >= 0 &&
                    y + j < screen->h)
                    {
                        Uint32 pixel = get_pixel(screen, x + i, y + j);
                        SDL_GetRGB(pixel, screen->format, &alpha, &alpha, &alpha);
                        sum += alpha;
                    }
                }
                
            }
            Uint8 average = 255; //sum/9;
            Uint32 color = SDL_MapRGB(screen->format, average, average, average);
            put_pixel(newScreen, x, y, color);
        }
        
    }
    
    return newScreen;
}*/
int minimum(int x,int y)
{
    if (x>y)
    {
        return y;
    }
    else
        return x;
}
int maximum(int x,int y)
{
    if (x>y)
        return x;
    else 
        return y;
}


static Uint32 average(SDL_Surface *surface, int i, int j, int n)
{
    const int initial_h = maximum(j - n, 0);
    const int initial_w = maximum(i- n, 0);
    const int final_h = minimum(j + n, surface->h - 1);
    const int final_w = minimum(i + n, surface->w - 1);
    const int nb_pixel = ((final_h - initial_h) * (final_w -initial_w));
    //const Uint32 *p = surface->pixels;


    Uint32 sum_r = 0, sum_g = 0, sum_b = 0;
    SDL_Color color;
    for (int x = initial_w; x < final_w; x++)
        for(int y = initial_h; y < final_h; y++)
        {
            Uint32 pixel = get_pixel(surface, x, y);
            SDL_GetRGB(pixel, surface->format, &color.r, &color.g, &color.b);
            sum_r += color.r;
            sum_g += color.g;
            sum_b += color.b;
        }



    if (nb_pixel==0)
    {
        return SDL_MapRGB(surface->format, 0, 0, 0);
    }
    else
        return SDL_MapRGB(surface->format, sum_r / nb_pixel, sum_g / nb_pixel, sum_b / nb_pixel);
}

SDL_Surface *blur(SDL_Surface * img)
{
    SDL_Surface* newIMG = SDL_CreateRGBSurface(0, img->w, img->h, 32, 0, 0, 0 ,0);

    int width = img->w;
    int height = img->h;
    for (int i=0;i<width;i++)
    {
	    for (int j=0;j<height;j++)
	    {
            Uint32 pixel = average(img, i, j, 1);
		    put_pixel(newIMG, i, j, pixel);
	    }
    }
	return newIMG;
}