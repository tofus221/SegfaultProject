#include <gtk/gtk.h>
#include <stdlib.h>
#include <err.h>
#include <stdio.h>
#include <time.h>
#include "engine.h"
#include "perlin.h"
#include <math.h>

#define W_WIDTH 700
#define W_HEIGHT 700

SDL_Surface *terrain;
Uint8 r = 0;
agentType **agents;
agentType *activeAgent;
size_t nbAgents = 1;
// Function to swap two numbers
void swap(char *x, char *y) {
    char t = *x; *x = *y; *y = t;
}
 
// Function to reverse `buffer[i…j]`
char* reverse(char *buffer, int i, int j)
{
    while (i < j) {
        swap(&buffer[i++], &buffer[j--]);
    }
 
    return buffer;
}
 
// Iterative function to implement `itoa()` function in C
char* itoa(int value, char* buffer, int base)
{
    // invalid input
    if (base < 2 || base > 32) {
        return buffer;
    }
 
    // consider the absolute value of the number
    int n = abs(value);
 
    int i = 0;
    while (n)
    {
        int r = n % base;
 
        if (r >= 10) {
            buffer[i++] = 65 + (r - 10);
        }
        else {
            buffer[i++] = 48 + r;
        }
 
        n = n / base;
    }
 
    // if the number is 0
    if (i == 0) {
        buffer[i++] = '0';
    }
 
    // If the base is 10 and the value is negative, the resulting string
    // is preceded with a minus sign (-)
    // With any other base, value is always considered unsigned
    if (value < 0 && base == 10) {
        buffer[i++] = '-';
    }
 
    buffer[i] = '\0'; // null terminate string
 
    // reverse the string and return it
    return reverse(buffer, 0, i - 1);
}

void update(simulation *sim)
{
    spawnFood(sim->foodHandler, W_WIDTH, W_HEIGHT);
    SDL_Surface *screen = sim->screen;
    SDL_BlitSurface(terrain, NULL, screen, NULL);
    for (struct agentLinkedList *al = sim->agentList->next; al != NULL;)
    {
        if (al->agent->type->energy <= 0.0f || al->agent->type->timeLeft < 0.0f)
        {
            struct agentLinkedList *temp = al;
            al = al->next;
            agent* res;
            popWithId(sim->agentList,temp->agent->id,&res);
            freeAgent(res);
            sim->popCount--;
            continue;
        }
        
        agentBehave(al->agent,sim);
        
        al->agent->type->timeLeft -= 1;
        al = al->next;
    }


    drawAgents(screen,sim->agentList);
    drawFood(sim->foodHandler, screen);
}

void init_simulation(GtkButton *button __attribute__((unused)), gpointer user_data __attribute__((unused)))
{
    terrain = perlin_surface(W_WIDTH, W_HEIGHT, 0.005);
    srand(time(0));
    simulation *sim = initEngine(W_WIDTH, W_HEIGHT, terrain);

    int infect[1] ={1}; 
    sickness* droopy_nose = createSickness("droop",0.5,1,0.01,infect,1,SDL_MapRGB(sim->screen->format, 0, 0, 0));
    for (size_t i = 0; i < nbAgents; i++)
    {
        Uint32 color = SDL_MapRGB(sim->screen->format, rand() % 255, rand() % 255, rand() % 255);
        for (size_t j = 0; j < 50; j++)
        {
            agentType *aType = createAgentType(agents[i]->name, agents[i]->typeId, agents[i]->lifeSpan, agents[i]->energy, agents[i]->speed, agents[i]->resistance, 
            agents[i]->hearingRange, agents[i]->asexual, agents[i]->birthRate, agents[i]->fertilityRate, agents[i]->birthCost, agents[i]->individualBirthCost, agents[i]->moveCost);
            aType->targetAmount = agents[i]->targetAmount;
            aType->targetsId = calloc(1, sizeof(int));
            aType->targetsId[0] = agents[i]->targetsId[0];
            aType->color = color;
            agent *a = createAgent(aType, rand() % W_WIDTH, rand() % W_HEIGHT);
            addSickness(a->SLL, droopy_nose);
            push(sim, a);
        }
        
    }

    run(sim, update);
    free(droopy_nose);
    
    free_simulation(sim);
    
    SDL_FreeSurface(terrain);
    
    SDL_Quit();
}

void Save_Settings(GtkButton *button __attribute__((unused)), gpointer builder)
{
    GtkTreeIter iter;
    GtkComboBox* agentCombo = GTK_COMBO_BOX(gtk_builder_get_object(builder, "Agent"));
    GtkListStore* agent = GTK_LIST_STORE(gtk_builder_get_object(builder, "AgentList"));
    GtkEntry* name = GTK_ENTRY(gtk_builder_get_object(builder, "NameEntry"));
    GtkComboBox* foodTarget = GTK_COMBO_BOX(gtk_builder_get_object(builder, "FTCombo"));
    GtkListStore* foodTargetList = GTK_LIST_STORE(gtk_builder_get_object(builder, "FoodList"));
    GtkAdjustment* FoodRange = GTK_ADJUSTMENT(gtk_builder_get_object(builder, "RangeData"));
    GtkAdjustment* speed = GTK_ADJUSTMENT(gtk_builder_get_object(builder, "SpeedData"));
    GtkEntry* lifeTime = GTK_ENTRY(gtk_builder_get_object(builder, "LTimeEntry"));
    GtkRadioButton* hot = GTK_RADIO_BUTTON(gtk_builder_get_object(builder, "Hot"));
    GtkRadioButton* cold = GTK_RADIO_BUTTON(gtk_builder_get_object(builder, "Cold"));
    GtkRadioButton* asexual = GTK_RADIO_BUTTON(gtk_builder_get_object(builder, "Asexual"));
    GtkAdjustment* maxChildren = GTK_ADJUSTMENT(gtk_builder_get_object(builder, "ChildrenData"));
    GtkAdjustment* fertilityRate = GTK_ADJUSTMENT(gtk_builder_get_object(builder, "FertilityData"));
    GtkAdjustment* MovingEnergy = GTK_ADJUSTMENT(gtk_builder_get_object(builder, "MoveCostData"));
    GtkAdjustment* birthCost = GTK_ADJUSTMENT(gtk_builder_get_object(builder, "BirthCostData"));
    GtkAdjustment* IndividualBirthCost = GTK_ADJUSTMENT(gtk_builder_get_object(builder, "ChildCostData"));

    strcpy(activeAgent->name, gtk_entry_get_text(name));
    int id = gtk_combo_box_get_active(agentCombo);
    

    activeAgent->lifeSpan = atoi(gtk_entry_get_text(lifeTime));
    activeAgent->timeLeft = atoi(gtk_entry_get_text(lifeTime));
    activeAgent->speed = gtk_adjustment_get_value(speed);
    activeAgent->resistance = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(hot)) ? 0 : (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cold)) ? 1 : 0.5);
    activeAgent->hearingRange = gtk_adjustment_get_value(FoodRange);;
    activeAgent->asexual = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(asexual));
    activeAgent->birthRate =  gtk_adjustment_get_value(maxChildren);
    activeAgent->birthCost = gtk_adjustment_get_value(birthCost);
    activeAgent->individualBirthCost = gtk_adjustment_get_value(IndividualBirthCost);
    activeAgent->moveCost = gtk_adjustment_get_value(MovingEnergy);
    activeAgent->fertilityRate = gtk_adjustment_get_value(fertilityRate);

    int target = gtk_combo_box_get_active(foodTarget);
    
    if(target == 0)
    {
        activeAgent->targetAmount = 0;
        activeAgent->targetsId[0] = target;
    }
    else
    {
        activeAgent->targetAmount = 1;
        activeAgent->targetsId[0] = target;
    }

    char buffer[5];
    GtkTreePath *path1 = gtk_tree_path_new_from_string(itoa(id, buffer, 10));
    gtk_tree_model_get_iter(GTK_TREE_MODEL(agent), &iter, path1);
    gtk_tree_path_free(path1);
    gtk_list_store_set(agent, &iter, 0, activeAgent->name, -1);

    GtkTreePath *path2 = gtk_tree_path_new_from_string(itoa(id + 1, buffer, 10));
    gtk_tree_model_get_iter(GTK_TREE_MODEL(foodTargetList), &iter, path2);
    gtk_tree_path_free(path2);
    gtk_list_store_set(foodTargetList, &iter, 0, activeAgent->name, -1);
}

void Add_agents(GtkButton *button __attribute__((unused)), gpointer builder)
{
    GtkComboBox* ComboAgent = GTK_COMBO_BOX(gtk_builder_get_object(builder, "Agent"));
    GtkListStore* agent = GTK_LIST_STORE(gtk_builder_get_object(builder, "AgentList"));
    GtkEntry* name = GTK_ENTRY(gtk_builder_get_object(builder, "NameEntry"));
    GtkListStore* foodTarget = GTK_LIST_STORE(gtk_builder_get_object(builder, "FoodList"));
    GtkAdjustment* FoodRange = GTK_ADJUSTMENT(gtk_builder_get_object(builder, "RangeData"));
    GtkAdjustment* speed = GTK_ADJUSTMENT(gtk_builder_get_object(builder, "SpeedData"));
    GtkEntry* lifeTime = GTK_ENTRY(gtk_builder_get_object(builder, "LTimeEntry"));
    GtkRadioButton* hot = GTK_RADIO_BUTTON(gtk_builder_get_object(builder, "Hot"));
    GtkRadioButton* medium = GTK_RADIO_BUTTON(gtk_builder_get_object(builder, "Tempered"));
    GtkRadioButton* cold = GTK_RADIO_BUTTON(gtk_builder_get_object(builder, "Cold"));
    GtkRadioButton* asexual = GTK_RADIO_BUTTON(gtk_builder_get_object(builder, "Asexual"));
    GtkRadioButton* sexual = GTK_RADIO_BUTTON(gtk_builder_get_object(builder, "Sexual"));
    GtkAdjustment* maxChildren = GTK_ADJUSTMENT(gtk_builder_get_object(builder, "ChildrenData"));
    GtkAdjustment* fertilityRate = GTK_ADJUSTMENT(gtk_builder_get_object(builder, "FertilityData"));
    GtkAdjustment* MovingEnergy = GTK_ADJUSTMENT(gtk_builder_get_object(builder, "MoveCostData"));
    GtkAdjustment* birthCost = GTK_ADJUSTMENT(gtk_builder_get_object(builder, "BirthCostData"));
    GtkAdjustment* IndividualBirthCost = GTK_ADJUSTMENT(gtk_builder_get_object(builder, "ChildCostData"));
    
    nbAgents++;
    agents = realloc(agents, nbAgents * sizeof(agentType *));
    char *s = malloc(30);
    sprintf(s, "Agent %ld", nbAgents);
    gtk_entry_set_text(name, s);
    
    GtkTreeIter tmp;
    gtk_list_store_append(foodTarget, &tmp);
    gtk_list_store_set(foodTarget, &tmp, 0, s, -1);

    gtk_list_store_append(agent, &tmp);
    gtk_list_store_set(agent, &tmp, 0, s, -1);

    gtk_adjustment_set_value(FoodRange, 1);
    gtk_adjustment_set_value(speed, 1);
    gtk_entry_set_text(lifeTime, "200");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(hot), TRUE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(medium), FALSE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cold), FALSE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(sexual), TRUE);
    gtk_adjustment_set_value(maxChildren, 1);
    gtk_adjustment_set_value(fertilityRate, 50);
    gtk_adjustment_set_value(MovingEnergy, 1);
    gtk_adjustment_set_value(birthCost, 5);
    gtk_adjustment_set_value(IndividualBirthCost, 1);

    agents[nbAgents - 1] = createAgentType(s, nbAgents - 1, atoi(gtk_entry_get_text(lifeTime)), 100, gtk_adjustment_get_value(speed), 0, gtk_adjustment_get_value(FoodRange),
    0 ,gtk_adjustment_get_value(maxChildren), gtk_adjustment_get_value(fertilityRate), gtk_adjustment_get_value(birthCost), gtk_adjustment_get_value(IndividualBirthCost),
    gtk_adjustment_get_value(MovingEnergy));
    agents[nbAgents - 1]->targetAmount = 0;
    agents[nbAgents - 1]->targetsId = calloc(1, sizeof(int));
    
    activeAgent = agents[nbAgents - 1];

    gtk_combo_box_set_active_iter(ComboAgent, &tmp);
}

void change_settings(GtkComboBox *agent, gpointer builder)
{
    GtkEntry* name = GTK_ENTRY(gtk_builder_get_object(builder, "NameEntry"));
    GtkComboBox* foodTarget = GTK_COMBO_BOX(gtk_builder_get_object(builder, "FTCombo"));
    GtkAdjustment* FoodRange = GTK_ADJUSTMENT(gtk_builder_get_object(builder, "RangeData"));
    GtkAdjustment* speed = GTK_ADJUSTMENT(gtk_builder_get_object(builder, "SpeedData"));
    GtkEntry* lifeTime = GTK_ENTRY(gtk_builder_get_object(builder, "LTimeEntry"));
    GtkRadioButton* hot = GTK_RADIO_BUTTON(gtk_builder_get_object(builder, "Hot"));
    GtkRadioButton* medium = GTK_RADIO_BUTTON(gtk_builder_get_object(builder, "Tempered"));
    GtkRadioButton* cold = GTK_RADIO_BUTTON(gtk_builder_get_object(builder, "Cold"));
    GtkRadioButton* asexual = GTK_RADIO_BUTTON(gtk_builder_get_object(builder, "Asexual"));
    GtkRadioButton* sexual = GTK_RADIO_BUTTON(gtk_builder_get_object(builder, "Sexual"));
    GtkAdjustment* maxChildren = GTK_ADJUSTMENT(gtk_builder_get_object(builder, "ChildrenData"));
    GtkAdjustment* fertilityRate = GTK_ADJUSTMENT(gtk_builder_get_object(builder, "FertilityData"));
    GtkAdjustment* MovingEnergy = GTK_ADJUSTMENT(gtk_builder_get_object(builder, "MoveCostData"));
    GtkAdjustment* birthCost = GTK_ADJUSTMENT(gtk_builder_get_object(builder, "BirthCostData"));
    GtkAdjustment* IndividualBirthCost = GTK_ADJUSTMENT(gtk_builder_get_object(builder, "ChildCostData"));
    
    char buffer[10];
    int id = gtk_combo_box_get_active(agent);
    activeAgent = agents[id];
    gtk_entry_set_text(name, activeAgent->name);
    gtk_combo_box_set_active(foodTarget, activeAgent->targetsId[0]);
    gtk_adjustment_set_value(FoodRange, activeAgent->hearingRange);
    gtk_adjustment_set_value(speed, activeAgent->speed);
    gtk_entry_set_text(lifeTime, itoa(activeAgent->lifeSpan, buffer, 10));

    if(activeAgent->resistance == 0)
    {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(hot), TRUE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(medium), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cold), FALSE);
    }
    else if(activeAgent->resistance == 1)
    {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(hot), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(medium), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cold), TRUE);
    }
    else
    {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(hot), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(medium), TRUE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cold), FALSE);
    }
    if (activeAgent->asexual){
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(asexual), TRUE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(sexual), FALSE);
    }
    else
    {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(asexual), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(sexual), TRUE);
    }

    gtk_adjustment_set_value(maxChildren, activeAgent->birthRate);
    gtk_adjustment_set_value(fertilityRate, activeAgent->fertilityRate);
    gtk_adjustment_set_value(MovingEnergy, activeAgent->moveCost);
    gtk_adjustment_set_value(birthCost, activeAgent->birthCost);
    gtk_adjustment_set_value(IndividualBirthCost, activeAgent->individualBirthCost);
}

void setupGtk()
{
	agents = calloc(sizeof(agentType *), 1);
	gtk_init(NULL,NULL);


	GtkBuilder* builder = gtk_builder_new();
	GError* error = NULL;

	if (gtk_builder_add_from_file(builder, "interface.glade", &error) == 0)
    {
        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error(&error);
        return;
    }

	GtkWidget* window = GTK_WIDGET(gtk_builder_get_object(builder,"window"));
	GtkButton* run = GTK_BUTTON(gtk_builder_get_object(builder, "Run"));
    GtkComboBox* agent = GTK_COMBO_BOX(gtk_builder_get_object(builder, "Agent"));
    GtkButton* AddAgent = GTK_BUTTON(gtk_builder_get_object(builder, "AddAgent"));
    GtkEntry* name = GTK_ENTRY(gtk_builder_get_object(builder, "NameEntry"));
    GtkComboBox* foodTarget = GTK_COMBO_BOX(gtk_builder_get_object(builder, "FTCombo"));
    GtkAdjustment* FoodRange = GTK_ADJUSTMENT(gtk_builder_get_object(builder, "RangeData"));
    GtkAdjustment* speed = GTK_ADJUSTMENT(gtk_builder_get_object(builder, "SpeedData"));
    GtkEntry* lifeTime = GTK_ENTRY(gtk_builder_get_object(builder, "LTimeEntry"));
    GtkAdjustment* maxChildren = GTK_ADJUSTMENT(gtk_builder_get_object(builder, "ChildrenData"));
    GtkAdjustment* fertilityRate = GTK_ADJUSTMENT(gtk_builder_get_object(builder, "FertilityData"));
    GtkAdjustment* MovingEnergy = GTK_ADJUSTMENT(gtk_builder_get_object(builder, "MoveCostData"));
    GtkAdjustment* birthCost = GTK_ADJUSTMENT(gtk_builder_get_object(builder, "BirthCostData"));
    GtkAdjustment* IndividualBirthCost = GTK_ADJUSTMENT(gtk_builder_get_object(builder, "ChildCostData"));
    GtkButton* save = GTK_BUTTON(gtk_builder_get_object(builder, "SaveSettings"));
    
    char s[30];
    strcpy(s, gtk_entry_get_text(name));
    agents[0] = createAgentType(s, 0, atoi(gtk_entry_get_text(lifeTime)), 30, gtk_adjustment_get_value(speed), 0, gtk_adjustment_get_value(FoodRange), 0,
    gtk_adjustment_get_value(maxChildren), gtk_adjustment_get_value(fertilityRate), gtk_adjustment_get_value(birthCost), gtk_adjustment_get_value(IndividualBirthCost),
    gtk_adjustment_get_value(MovingEnergy));
    agents[0]->targetAmount = 0;
    agents[0]->targetsId = calloc(1, sizeof(int));
    activeAgent = agents[0];

	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(run, "clicked", G_CALLBACK(init_simulation), NULL);

    g_signal_connect(save, "clicked", G_CALLBACK(Save_Settings), builder);
    g_signal_connect(AddAgent, "clicked", G_CALLBACK(Add_agents), builder);

    g_signal_connect(agent, "changed", G_CALLBACK(change_settings), builder);
    gtk_combo_box_set_active(agent, 0);
    gtk_combo_box_set_active(foodTarget, 0);
	gtk_widget_show(window);
    gtk_main();
    free(agents);
}
