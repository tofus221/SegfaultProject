#ifndef SEGFAULTPROJECT_AGENT_H
#define SEGFAULTPROJECT_AGENT_H

typedef struct agentType{
    char* name;
    float lifeSpan;
    float energy;
    //float foodDetectionRadius;
    //food to add when i'll know how to implement, might do a file just for the food search
    float speed;
    //reproduction too
    float resistance;
} agentType;

typedef struct agent{
    agentType* type;
    int Xpos;
    int Ypos;
}agent;

#endif //SEGFAULTPROJECT_AGENT_H
