#include <math.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct
{
    float x, y;
} vector2;

vector2 *gradients;
int cols;

float interpolate(float a0, float a1, float w)
{
    return (a1 - a0) * w + a0;
}

void init_gradients(int w, int h)
{
    gradients = calloc((w+1) * (h+1), sizeof(vector2));
    cols = w;
    for(int x = 0; x < w+1; x++)
    {
        for(int y = 0; y < w+1; y++)
        {
            float random = ((float)rand() / (float)RAND_MAX) * 2 * M_PI;
            gradients[y * cols + x].x = cosf(random);
            gradients[y * cols + x].y = sinf(random);
        }
    }
}

void free_gradients()
{
    free(gradients);
}

float dotGradient(int ix, int iy, float x, float y)
{
    vector2 gradient = gradients[iy * cols + ix];

    float dx = x - (float)ix;
    float dy = y - (float)iy;

    return dx*gradient.x + dy*gradient.y;
}

float perlin(float x, float y)
{
    int x0 = (int)floorf(x);
    int x1 = x0 + 1;
    int y0 = (int)floorf(y);
    int y1 = y0 + 1;

    float sx = x - (float)x0;
    float sy = y - (float)y0;

    float n0, n1, ix0, ix1, value;

    n0 = dotGradient(x0, y0, x, y);
    n1 = dotGradient(x1, y0, x, y);
    ix0 = interpolate(n0, n1, sx);

    n0 = dotGradient(x0, y1, x, y);
    n1 = dotGradient(x1, y1, x, y);
    ix1 = interpolate(n0, n1, sx);

    value = interpolate(ix0, ix1, sy);
    return value;
}