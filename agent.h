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
    int id;
    agentType* type;
    int Xpos;
    int Ypos;
}agent;

struct agentLinkedList{
    agent* agent;
    struct agentLinkedList* next;
};

agentType* createAgentType(char* name, float lifeSpan, float energy, float speed, float resistance);
agent* createAgent(int id, agentType* type, int x, int y);
void moveAgent(agent* agent, int addX, int addY);
struct agentLinkedList* initLinkedList();
void push(struct agentLinkedList* list, agent* agentToAdd);
agent* pop(struct agentLinkedList* list);
agent* popWithId(struct agentLinkedList* list, int id);
void freeLinkedList(struct agentLinkedList* agentLinkedList);

#endif //SEGFAULTPROJECT_AGENT_H
