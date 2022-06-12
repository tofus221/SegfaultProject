#include "food.h"
#include <stdlib.h>
#include <math.h>

struct foodNode* initFoodList(){
    struct foodNode* list = malloc(sizeof(struct foodNode));
    list->food = NULL;
    list->next = NULL;
    return list;
}

void push(struct foodNode* foodNode, food* food){
    struct foodNode* newBlock = malloc(sizeof(struct foodNode));
    newBlock->next = NULL;
    if (foodNode->next != NULL)
    {
        newBlock->next = foodNode->next;
    }
    foodNode->next = newBlock;
    newBlock->food = food;
}

int popWithPos(struct foodNode* list, int x, int y, food** res){
    struct foodNode* parent = list;
    for (struct foodNode* curr = list->next; curr != NULL; parent = curr, curr = curr->next)
    {
        if (curr->food->Xpos == x)
        {
            if (curr->food->Ypos == y){
                parent->next = curr->next;
                *res = curr->food;
                free(curr);
                return 1;
            }
        }
    }
    return 0;
}

void freeFoodList(struct foodNode* foodList){
    if (foodList->next == NULL)
        return;
    struct foodNode* parent;
    while (foodList != NULL){
        parent = foodList;
        foodList = foodList->next;
        if (parent->food != NULL)
            free(parent->food);
        free(parent);
    }
}

foodTree* newTree(float radius,int simWidth, int simHeight){
    foodTree* res = malloc(sizeof(foodTree));
    res->radius = radius;
    res->currentFood = 0;
    res->Ypos = (rand() % (simHeight - (int) radius - 4)) + (int) radius + 4; //le 4 est la taille du sprite du fruit
    res->Xpos = rand() % (simWidth - (int) radius - 3) + (int) radius + 3; //pareil pour le 3, sa evite de check tout le temp c'est pratique
}

foodHandler* initFoodHandler(int simWidth, int simHeight){
    foodHandler* res = malloc(sizeof(foodHandler));
    res->foodList = initFoodList();
    for (int i = 0; i < NUMBEROFTREE; ++i) {
        res->trees[i] = newTree(25, simWidth, simHeight);
    }
    return res;
}

food* initFood(foodTree* father){
    food* res = malloc(sizeof(food));
    res->father = father;
    father->currentFood++;
    res->lifeSpan = 10;
    res->energyToGive = (rand() % 10) + 1;
    do {
        res->Xpos = father->Xpos - (int)father->radius + (rand() % (int)father->radius*2);
        res->Ypos = father->Ypos - (int)father->radius + (rand() % (int)father->radius*2);
    } while (sqrt(pow(res->Xpos - father->Xpos, 2) + pow(res->Ypos - father->Ypos, 2)) > father->radius);
    return res;
}

void spawnFood(foodHandler* handler){
    food* toSpawn;
    for (int i = 0; i < NUMBEROFTREE; ++i) {
        toSpawn = initFood(handler->trees[i]);
        push(handler->foodList, toSpawn);
    }
}

void freeFoodHandler(foodHandler* foodHandler){
    for (int i = 0; i < NUMBEROFTREE; ++i) {
        free(foodHandler->trees[i]);
    }
    freeFoodList(foodHandler->foodList);
};

void drawFruit(food* curr, SDL_Surface* surface){
    int x = curr->Xpos;
    int y = curr->Ypos;

    Uint32 Green = SDL_MapRGB(surface->format, 12, 102, 36);
    Uint32 Red = SDL_MapRGB(surface->format, 196, 4, 36);

    put_pixel(surface, x, y, Green);
    put_pixel(surface, x+1, y+1, Red);
    put_pixel(surface, x, y+2, Red);
    put_pixel(surface, x+1, y+2, Red);
    put_pixel(surface, x+2, y+2, Red);
    put_pixel(surface, x+1, y+3, Red);
};

void drawTree(foodTree* curr, SDL_Surface* surface){
    int x = curr->Xpos;
    int y = curr->Ypos;

    Uint32 Green = SDL_MapRGB(surface->format, 51, 152, 75);
    Uint32 Red = SDL_MapRGB(surface->format, 255, 3, 66);
    Uint32 Brown = SDL_MapRGB(surface->format, 93, 44, 40);

    put_pixel(surface, x+1, y, Green);
    put_pixel(surface, x+2, y, Green);
    put_pixel(surface, x+3, y, Green);
    put_pixel(surface, x, y+1, Green);
    put_pixel(surface, x+2, y+1, Green);
    put_pixel(surface, x+3, y+1, Green);
    put_pixel(surface, x+4, y+1, Green);
    put_pixel(surface, x, y+2, Green);
    put_pixel(surface, x+1, y+2, Green);
    put_pixel(surface, x+2, y+2, Green);
    put_pixel(surface, x+4, y+2, Green);
    put_pixel(surface, x+1, y+3, Green);
    put_pixel(surface, x+2, y+3, Green);
    put_pixel(surface, x+3, y+3, Green);

    put_pixel(surface, x+1, y+1, Red);
    put_pixel(surface, x+3, y+2, Red);
    put_pixel(surface, x, y+3, Red);
    put_pixel(surface, x+4, y+3, Red);

    put_pixel(surface, x+1, y+4, Brown);
    put_pixel(surface, x+2, y+4, Brown);
    put_pixel(surface, x+3, y+4, Brown);
    put_pixel(surface, x+1, y+5, Brown);
    put_pixel(surface, x+2, y+5, Brown);
    put_pixel(surface, x+3, y+5, Brown);
    put_pixel(surface, x+1, y+6, Brown);
    put_pixel(surface, x+2, y+6, Brown);
    put_pixel(surface, x+3, y+6, Brown);
    put_pixel(surface, x+1, y+7, Brown);
    put_pixel(surface, x+2, y+7, Brown);
    put_pixel(surface, x+3, y+7, Brown);
};

void drawFood(foodHandler* foodHandler, SDL_Surface* surface){
    for (int i = 0; i < NUMBEROFTREE; ++i) {
        drawTree(foodHandler->trees[i], surface);
    }

    for (struct foodNode* curr = foodHandler->foodList; curr != NULL; curr = curr->next){
        drawFruit(curr->food, surface);
    }
}