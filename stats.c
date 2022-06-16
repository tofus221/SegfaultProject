#include "stats.h"

SimList *initSL()
{
    SimList *sl = malloc(sizeof(SimList));
    sl->ATs = NULL;
    sl->next = NULL;
    return sl;
}

void freeSL(SimList *sl)
{
    SimList *prev = sl;
    sl = sl->next;

    while(sl != NULL)
    {
        free(prev->ATs);
        free(prev);
        prev = sl;
        sl = sl->next;
    }
    free(prev->ATs);
    free(prev);
}

agentType *copyAT(agentType *aType, int nbAgents)
{
    agentType *at = calloc(nbAgents, sizeof(agentType));
    for (int i = 0; i < nbAgents; i++)
    {
        at[i].name = aType[i].name;
        at[i].speed = aType[i].speed;
        at[i].resistance = aType[i].resistance;
        at[i].hearingRange = aType[i].hearingRange;
        at[i].fertilityRate = aType[i].fertilityRate;
    }

    return at;
}

void pushATs(SimList *l, agentType *ATs, int nbAgents)
{
    while (l->next != NULL)
    {
        l = l->next;
    }

    l->next = malloc(sizeof(SimList));
    l = l->next;
    l->ATs = copyAT(ATs, nbAgents);
    l->next = NULL;
}

void computeAverages(struct agentLinkedList *agentList, int nbAgents, agentType *ATs)
{
    int sums[nbAgents];
    memset(sums, 0, nbAgents * sizeof(int));

    agentList = agentList->next;

    for (int i = 0; i < nbAgents; i++)
    {
        ATs[i].speed = 0;
        ATs[i].resistance = 0;
        ATs[i].hearingRange = 0;
        ATs[i].fertilityRate = 0;
    }
    

    while(agentList != NULL)
    {
        agentType *at = agentList->agent->type;
        int id = at->typeId;
        sums[id]++;
        ATs[id].speed += at->speed;
        ATs[id].resistance += at->resistance;
        ATs[id].hearingRange += at->hearingRange;
        ATs[id].fertilityRate += at->fertilityRate;

        agentList = agentList->next;
    }

    for (int i = 0; i < nbAgents; i++)
    {
        ATs[i].speed /= sums[i];
        ATs[i].resistance /= sums[i];
        ATs[i].hearingRange /= sums[i];
        ATs[i].fertilityRate /= sums[i];
    }
}

void saveStats(SimList *sl, int nbAgents)
{
    FILE *f = fopen("species_infos.csv", "w");
    fputs("time (ticks);", f);
    if(sl->next != NULL)
    {    for (int i = 0; i < nbAgents; i++)
        {
            fputs(sl->next->ATs[i].name, f);
            fputs(";Speed;Climat Preference;Food Range;Fertility Rate;", f);
        }
    }
    
    fputs("\n", f);

    sl = sl->next;
    size_t ticks = 0;
    while (sl != NULL)
    {
        char s[1000];
        sprintf(s, "%ld;;", ticks);
        
        for (int i = 0; i < nbAgents; i++)
        {
            char s1[1000];
            sprintf(s1, "%f;%f;%f;%f;;", sl->ATs[i].speed, sl->ATs[i].resistance, sl->ATs[i].hearingRange, sl->ATs[i].fertilityRate);
            strcat(s, s1);
        }
        strcat(s, "\n");
        fputs(s, f);
        sl = sl->next;
        ticks += 100;
    }
    fclose(f);
}