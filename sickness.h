#ifndef SEGFAULTPROJECT_SICKNESS_H
#define SEGFAULTPROJECT_SICKNESS_H
#include <SDL/SDL.h>
#include <err.h>

typedef struct sickness{
    char* name;
    float infectiousness; // chance to spread infection;
    float severity; //increases energy consumption
    float mortality; // reduces lifespan
    int* canInfectID;
    size_t canInfectAmount;
    Uint32 color;
} sickness;


typedef struct sicknessLinkedList{
    sickness* data;
    struct sicknessLinkedList* next;
}sicknessLinkedList;

sicknessLinkedList* initSLL();
void addSickness(sicknessLinkedList* SLL, sickness* sick);
int hasSickness(sicknessLinkedList* SLL, sickness* sick);
sickness* createSickness(char* name, float infectiousness, float severity, float mortality, int* canInfect, size_t canInfectAmount, Uint32 color);
#endif