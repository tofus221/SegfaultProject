#include "food.h"
#include <stdlib.h>
#include <math.h>
#include <SDL/SDL_video.h>

#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

struct foodNode* initFoodList(){
    struct foodNode* list = malloc(sizeof(struct foodNode));
    list->food = NULL;
    list->next = NULL;
    return list;
}

void pushFood(struct foodNode* foodNode, food* food){
    struct foodNode* newBlock = malloc(sizeof(struct foodNode));
    newBlock->next = NULL;
    if (foodNode->next != NULL)
    {
        newBlock->next = foodNode->next;
    }
    foodNode->next = newBlock;
    newBlock->food = food;
}

int popWithPos(struct foodNode* list, int x, int y){
    struct foodNode* parent = list;
    for (struct foodNode* curr = list->next; curr != NULL; parent = curr, curr = curr->next)
    {
        if (curr->food->Xpos == x)
        {
            if (curr->food->Ypos == y){
                parent->next = curr->next;
                free(curr);
                return 1;
            }
        }
    }
    return 0;
}

void freeFoodList(struct foodNode* foodList){
    if (foodList->next == NULL)
    {
        free(foodList);
        return;
    }
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
    res->Ypos = (rand() % (simHeight - (int) radius - 20)) + (int) radius + 20; //le 20 est une sécurité, pour eviter de pas ce mettre de pixel en dehors du screen.
    res->Xpos = (rand() % (simWidth - (int) radius - 20)) + (int) radius + 20;
    return res;
}

foodHandler* initFoodHandler(int simWidth, int simHeight){
    foodHandler* res = malloc(sizeof(foodHandler));
    res->foodList = initFoodList();
    for (int i = 0; i < NUMBEROFTREE; ++i) {
        res->trees[i] = newTree(50, simWidth, simHeight);
    }
    return res;
}

food* initFood(foodTree* father, int w, int h){
    food* res = malloc(sizeof(food));
    res->father = father;
    res->lifeSpan = 10;
    res->energyToGive = (rand() % 50) + 10;
    do {
        res->Xpos = father->Xpos - (int)father->radius + (rand() % (int)father->radius*2);
        res->Ypos = father->Ypos - (int)father->radius + (rand() % (int)father->radius*2);

        res->Xpos = MIN(MAX(0, res->Xpos), w);
        res->Ypos = MIN(MAX(0, res->Ypos), h);
    } while (sqrt(pow(res->Xpos - father->Xpos, 2) + pow(res->Ypos - father->Ypos, 2)) > father->radius);
    return res;
}

void spawnFood(foodHandler* handler, int w, int h){
    food* toSpawn;
    for (int i = 0; i < NUMBEROFTREE; ++i) {
        if (handler->trees[i]->currentFood < MAXIMUMFOODPERTREE){
            toSpawn = initFood(handler->trees[i], w, h);
            handler->trees[i]->currentFood++;
            pushFood(handler->foodList, toSpawn);
        }
    }
}

void freeFoodHandler(foodHandler* foodHandler){
    for (int i = 0; i < NUMBEROFTREE; ++i) {
        free(foodHandler->trees[i]);
    }
    freeFoodList(foodHandler->foodList);
    free(foodHandler);
};

void drawFruit(food* curr, SDL_Surface* screen, SDL_Surface* image){
    SDL_Rect fruit = {curr->Xpos, curr->Ypos, 11, 16};
    SDL_BlitSurface(image, NULL, screen, &fruit);
};

void drawTree(foodTree* curr, SDL_Surface* screen, SDL_Surface* image){
    SDL_Rect tree = {curr->Xpos, curr->Ypos, 23, 40};
    SDL_BlitSurface(image, NULL, screen, &tree);
};

void drawFood(foodHandler* foodHandler, SDL_Surface* surface){
    SDL_Surface *fruitImage = load_image("assets/fruit.png");
    SDL_Surface *treeImage = load_image("assets/tree.png");
    for (int i = 0; i < NUMBEROFTREE; ++i){
        drawTree(foodHandler->trees[i], surface, treeImage);
    }

    struct foodNode* curr = foodHandler->foodList;
    if (curr->next == NULL)
        return;
    curr = curr->next;
    for (; curr != NULL; curr = curr->next){
        drawFruit(curr->food, surface, fruitImage);
    }

    SDL_FreeSurface(fruitImage);
    SDL_FreeSurface(treeImage);
}