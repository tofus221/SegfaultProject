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
void update(simulation *sim)
{
    SDL_BlitSurface(terrain, NULL, sim->screen, NULL);
}

void call_perlin_noise(GtkButton *button, gpointer user_data)
{
    terrain = perlin_surface(W_WIDTH, W_HEIGHT, 0.005);
    simulation *sim = initEngine(W_WIDTH, W_HEIGHT);

    run(sim, update);

    free_simulation(sim);
    
    SDL_FreeSurface(terrain);
	SDL_Quit();
}

void setupGtk()
{
	
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

	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(run, "clicked", G_CALLBACK(call_perlin_noise), NULL);

	gtk_widget_show(window);
	gtk_main();
}
