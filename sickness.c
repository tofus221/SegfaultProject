#include "sickness.h"
#include <SDL/SDL.h>

sickness* createSickness(char* name, float infectiousness, float severity, float mortality, int* canInfect, size_t canInfectAmount, Uint32 color)
{
    sickness* sick = malloc(sizeof(sickness));
    if (sick == NULL)
        errx(1,"Not Enough Memory");
    sick->name = name;
    sick->infectiousness = infectiousness;
    sick->severity = severity;
    sick->mortality = mortality;
    sick->canInfectID = canInfect;
    sick->canInfectAmount = canInfectAmount;
    sick->color = color;
    return sick;    
}

sicknessLinkedList* initSLL()
{
    sicknessLinkedList* SLL = malloc(sizeof(sicknessLinkedList));
    if (SLL == NULL)
        errx(1,"Not Enough Memory");
    SLL->next = NULL;
    return SLL;
}

void addSickness(sicknessLinkedList* SLL, sickness* sick)
{
    if (hasSickness(SLL,sick))
        return;
    sicknessLinkedList* newEl = malloc(sizeof(sicknessLinkedList));
    if (newEl == NULL)
        errx(1,"Not Enough Memory");
    newEl->data = sick;
    newEl->next = SLL->next;
    SLL->next = newEl;
}

int hasSickness(sicknessLinkedList* SLL, sickness* sick)
{
    for (sicknessLinkedList* curr = SLL->next; curr != NULL; curr = curr->next)
    {
        if(strcmp(sick->name,curr->data->name) == 0)
            return 1;
    }
    return 0;
}
