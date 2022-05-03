#ifndef PERLIN_H
#define PERLIN_H

float perlin(float x, float y);
void init_gradients(int w, int h);
void free_gradients();

#endif