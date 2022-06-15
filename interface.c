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
int nbAgents = 1;
void update(simulation *sim)
{
    spawnFood(sim->foodHandler);
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

void call_perlin_noise(GtkButton *button, gpointer user_data)
{
    terrain = perlin_surface(W_WIDTH, W_HEIGHT, 0.005);
    srand(time(0));
    simulation *sim = initEngine(W_WIDTH, W_HEIGHT, terrain);

    int infect[1] ={1}; 
    sickness* droopy_nose = createSickness("droop",0.5,1,0.01,infect,1,SDL_MapRGB(sim->screen->format, 0, 0, 0));
    for (size_t i = 0; i < 10; i++)
    {
        agentType* aType = createAgentType("wolf", 1, 200, 150, 4, 0.5, 40, 2, 300, 50);
        aType->targetAmount = 1;
        aType->targetsId = calloc(aType->targetAmount, sizeof(int));
        aType->targetsId[0] = 2;
        aType->color = SDL_MapRGB(sim->screen->format, 255, 0, 0);
        agent* wolf = createAgent(aType,rand() % (sim->screen->w),rand() % (sim->screen->h));
        addSickness(wolf->SLL,droopy_nose);
        push(sim, wolf);
    }

    for (size_t i = 0; i < 600; i++)
    {
        agentType* aType = createAgentType("sheep", 2, 200, 150, 3, 0.5, 100, 2, 300, 50);
        aType->targetAmount = 0;
        aType->color = SDL_MapRGB(sim->screen->format, 0, 255, 0);
        agent* sheep = createAgent(aType,rand() % (sim->screen->w),rand() % (sim->screen->h));
        push(sim, sheep);
    }

    run(sim, update);
    free(droopy_nose);
    
    free_simulation(sim);
    
    SDL_FreeSurface(terrain);
    
    SDL_Quit();
}

void Save_Settings(GtkButton *button, gpointer builder)
{
    GtkEntry* name = GTK_ENTRY(gtk_builder_get_object(builder, "NameEntry"));
    GtkComboBox* foodTarget = GTK_ENTRY(gtk_builder_get_object(builder, "FTCombo"));
    GtkScale* FoodRange = GTK_SCALE(gtk_builder_get_object(builder, "FRangScale"));
    GtkScale* speed = GTK_SCALE(gtk_builder_get_object(builder, "SupidoScale"));
    GtkEntry* lifeTime = GTK_ENTRY(gtk_builder_get_object(builder, "LTimeEntry"));
    GtkRadioButton* hot = GTK_RADIO_BUTTON(gtk_builder_get_object(builder, "Hot"));
    GtkRadioButton* cold = GTK_RADIO_BUTTON(gtk_builder_get_object(builder, "Cold"));
    GtkScale* maxChildren = GTK_SCALE(gtk_builder_get_object(builder, "ChildrenScale"));
    GtkScale* fertilityRate = GTK_SCALE(gtk_builder_get_object(builder, "FertilityScale"));
    GtkScale* MovingEnergy = GTK_SCALE(gtk_builder_get_object(builder, "MoveCostScale"));
    GtkScale* birthCost = GTK_SCALE(gtk_builder_get_object(builder, "BirthCostScale"));
    GtkScale* IndividualBirthCost = GTK_SCALE(gtk_builder_get_object(builder, "ChildCostScale"));
    GtkButton* save = GTK_BUTTON(gtk_builder_get_object(builder, "Save"));

    activeAgent->name = gtk_entry_get_text(name);
    activeAgent->lifeSpan = atoi(gtk_entry_get_text(lifeTime));
    activeAgent->timeLeft = atoi(gtk_entry_get_text(lifeTime));
    activeAgent->speed = gtk_scale_button_get_value(speed);
    activeAgent->resistance = gtk_toggle_button_get_active(hot) ? 0 : (gtk_toggle_button_get_active(cold) ? 1 : 0.5);
    activeAgent->hearingRange = gtk_scale_button_get_value(FoodRange);;
    activeAgent->birthRate =  gtk_scale_button_get_value(maxChildren);
    activeAgent->birthCost = gtk_scale_button_get_value(birthCost);
    activeAgent->individualBirthCost = gtk_scale_button_get_value(IndividualBirthCost);
    activeAgent->moveCost = gtk_scale_button_get_value(MovingEnergy);
    activeAgent->fertilityRate = gtk_scale_button_get_value(fertilityRate);
}

void Add_agents(GtkButton *button, gpointer user_data)
{
    nbAgents++;
    agents = realloc(agents, nbAgents * sizeof(agentType *));
    activeAgent = agents[nbAgents - 1];


    
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
    GtkComboBox* agent = GTK_ENTRY(gtk_builder_get_object(builder, "Agent"));
    GtkButton* AddAgent = GTK_BUTTON(gtk_builder_get_object(builder, "AddAgent"));
    GtkEntry* name = GTK_ENTRY(gtk_builder_get_object(builder, "NameEntry"));
    GtkComboBox* foodTarget = GTK_ENTRY(gtk_builder_get_object(builder, "FTCombo"));
    GtkScale* FoodRange = GTK_SCALE(gtk_builder_get_object(builder, "FRangScale"));
    GtkScale* speed = GTK_SCALE(gtk_builder_get_object(builder, "SupidoScale"));
    GtkEntry* lifeTime = GTK_ENTRY(gtk_builder_get_object(builder, "LTimeEntry"));
    GtkScale* maxChildren = GTK_SCALE(gtk_builder_get_object(builder, "ChildrenScale"));
    GtkScale* fertilityRate = GTK_SCALE(gtk_builder_get_object(builder, "FertilityScale"));
    GtkScale* MovingEnergy = GTK_SCALE(gtk_builder_get_object(builder, "MoveCostScale"));
    GtkScale* birthCost = GTK_SCALE(gtk_builder_get_object(builder, "BirthCostScale"));
    GtkScale* IndividualBirthCost = GTK_SCALE(gtk_builder_get_object(builder, "ChildCostScale"));
    GtkButton* save = GTK_BUTTON(gtk_builder_get_object(builder, "Save"));
    
    agents[0] = createAgentType(gtk_entry_get_text(name), 0, atoi(gtk_entry_get_text(lifeTime)), 100, gtk_scale_button_get_value(speed), 0, gtk_scale_button_get_value(FoodRange),
    gtk_scale_button_get_value(maxChildren), gtk_scale_button_get_value(fertilityRate), gtk_scale_button_get_value(birthCost), gtk_scale_button_get_value(IndividualBirthCost),
    gtk_scale_button_get_value(MovingEnergy));
    
    activeAgent = agents[0];

	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(run, "clicked", G_CALLBACK(call_perlin_noise), NULL);

    g_signal_connect(save, "clicked", G_CALLBACK(Save_Settings), builder);
    g_signal_connect(AddAgent, "clicked", G_CALLBACK(Add_agents), builder);


	gtk_widget_show(window);
    gtk_main();
    free(agents);
}
