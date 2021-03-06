#ifndef PIXELOP_H
#define PIXELOP_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <err.h>

void put_pixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel);
Uint32 get_pixel(SDL_Surface *surface, unsigned x, unsigned y);
SDL_Surface* load_image(char *path);

#endif