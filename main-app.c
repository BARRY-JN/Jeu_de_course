#include <stdio.h>
#include <gtk/gtk.h>
#include <string.h>

#include "curve.h"
#include "game.h"
#include "mydata.h"
#include "menus.h"
#include "util.h"
#include "area1.h"
#include "gui.h"

void on_app_activate (GtkApplication* app, gpointer user_data)
{
	Mydata *my = get_mydata(user_data);//Recuperation des donnees fourre tout
	
	window_init(app,my);
	menus_init(my);
	area1_init(my);
	status_init(my);
	
	play_init(my);
	editing_init(my);
	layout_init(my);
	win_scale_init(my);

	
	
	/* AFFICHAGE DE L'INTERFACE */
	
	g_object_set (gtk_settings_get_default(),"gtk-shell-shows-menubar", FALSE, NULL);
	gtk_widget_show_all (my->window);
	//gtk_widget_hide (my->frame1);
}

int main (int argc, char *argv[])
{
	Mydata my;
	int status;
	
	init_mydata(&my);
	GtkApplication *app;
	
	app = gtk_application_new (NULL, G_APPLICATION_FLAGS_NONE);
	g_signal_connect (app, "activate",G_CALLBACK(on_app_activate), &my);
	status = g_application_run (G_APPLICATION(app), argc, argv);
	g_object_unref (app);
	return status;
}

