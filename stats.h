#ifndef STATS_H
#define STATS_H

#include "engine.h"

typedef struct SimList
{
    struct SimList *next;
    agentType *ATs;
} SimList;

SimList *initSL();
void freeSL(SimList *sl);
void pushATs(SimList *l, agentType *ATs, int nbAgents);
void computeAverages(struct agentLinkedList *agentList, int nbAgents, agentType *ATs);
void saveStats(SimList *sl, int nbAgents);


#endif