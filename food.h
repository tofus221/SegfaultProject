#ifndef SEGFAULTPROJECT_FOOD_H
#define SEGFAULTPROJECT_FOOD_H

#include "pixelOp.h"

#define NUMBEROFTREE 10
#define MAXIMUMFOODPERTREE 25


//l'idée est que au début de la simulation on fait spawn des arbre qui a leurs tour feront spawn de la nouriture autour d'eux.
typedef struct foodTree{
    int Xpos, Ypos;
    int currentFood; //le nombre de nourriture par terre
    float radius;
}foodTree;

typedef struct food{
    float energyToGive;
    foodTree* father; //just a reference dont free it.
    float lifeSpan;
    int Xpos, Ypos;
}food;

struct foodNode{
    food* food;
    struct foodNode* next;
};

typedef struct foodHandler{
    foodTree* trees[NUMBEROFTREE];
    struct foodNode* foodList;
}foodHandler;

foodHandler* initFoodHandler(int simWidth, int simHeight);
int popWithPos(struct foodNode* list, int x, int y);
void spawnFood(foodHandler* handler, int w, int h);
void drawFood(foodHandler* foodHandler, SDL_Surface* surface);
void freeFoodHandler(foodHandler* foodHandler);

#endif //SEGFAULTPROJECT_FOOD_H
