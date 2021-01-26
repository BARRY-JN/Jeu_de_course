#include <gtk/gtk.h>
#include "util.h"
#include "curve.h"
#include "game.h"
#include "mydata.h"
#include "area1.h"
#include "menus.h"

void on_item_edition_activate(GtkCheckMenuItem *check_menu_item, gpointer user_data){
	Mydata *my = get_mydata(user_data);
	gtk_window_present (GTK_WINDOW (my->win_edit));
	my->show_edit=TRUE;

}
void on_item_niveaux_activate(GtkMenuItem *menu_item, gpointer user_data){
	//Mydata *my = get_mydata(user_data);
}
void on_item_online_activate(GtkMenuItem *menu_item, gpointer user_data){
	//Mydata *my = get_mydata(user_data);
}

void on_item_jouer_activate(GtkMenuItem *menu_item, gpointer user_data){

//lorsqu'on passe en mode jeu

Mydata *my = get_mydata(user_data);
gtk_window_present (GTK_WINDOW (my->win_play));
my->game.mode=GM_SINGLE;

}

void on_item_load_activate(GtkMenuItem *menu_item, gpointer user_data)
{
	Mydata *my = get_mydata(user_data);
	const char *nom = gtk_menu_item_get_label(menu_item);
	printf ("Le menu-item %s a ete active\n", nom);
	
	//gtk_image_set_from_file (GTK_IMAGE(my->image1),"tux2.gif");
	GtkWidget *dialog;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
	gint res;

	dialog = gtk_file_chooser_dialog_new ("Ouvrir un fichier",GTK_WINDOW (my->window),action,"Annuler",GTK_RESPONSE_CANCEL,"Ouvrir",GTK_RESPONSE_ACCEPT,NULL);
	
	if(my->current_folder!=NULL)
	{
		gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER (dialog),my->current_folder);
	}
	
	res = gtk_dialog_run (GTK_DIALOG (dialog));
	
	if (res == GTK_RESPONSE_ACCEPT)
	  {
	    char *filename;
	    g_free(my->current_folder);
	    my->current_folder = gtk_file_chooser_get_current_folder (GTK_FILE_CHOOSER (dialog));
	    filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
	    set_status(my->status,"Chargement de l'image...");
	    //gtk_image_set_from_file (GTK_IMAGE(my->image1),filename);
	    
		g_clear_object(&my->pixbuf1);
		my->pixbuf1=gdk_pixbuf_new_from_file(filename,NULL);
		if(my->pixbuf1==NULL)
		{
			set_status(my->status,"Chargement echoue, ce n'est pas une image.");
			//gtk_image_set_from_icon_name(GTK_IMAGE(my->image1), "image-missing", GTK_ICON_SIZE_DIALOG);
		}
		else
		{
			set_status(my->status,"Chargement reussi: image %d x %d",gdk_pixbuf_get_width(my->pixbuf1),gdk_pixbuf_get_height(my->pixbuf1));
			
			my->scale1_value=1.0;
			gtk_range_set_value (GTK_RANGE(my->scale1),1);
			my->rotate_angle=0;
			
			update_area1_with_transforms(my);
			//gtk_image_set_from_pixbuf(GTK_IMAGE(my->image1), my->pixbuf1);
		}
		
	    g_free (filename);
	  }
	
	gtk_widget_destroy (dialog);
}

void on_item_about_activate(GtkMenuItem *menu_item, gpointer user_data)
{
	Mydata *my = get_mydata(user_data);
	char *auteurs[] = {"Alio David <adresse@email>","Barry Jean-Noel <adresse@email>", NULL};
	gtk_show_about_dialog (GTK_WINDOW (my->window),"program-name", my->title,"version","2.4","website","j.mp/igraph","authors",auteurs,"logo-icon-name","face-cool",NULL);
	
}

void on_item_quit_activate(GtkMenuItem *menu_item, gpointer user_data)
{
	Mydata *my = get_mydata(user_data);
	printf ("Fermeture de la fenetre et du programme\n");
	gtk_widget_destroy(my->track_name_label);
	gtk_widget_destroy(my->win_scale);
	gtk_widget_destroy(my->win_edit);
	gtk_widget_destroy(my->win_play);
	gtk_widget_destroy(my->window);
	
}



void on_item_rotate_activate(GtkMenuItem *menu_item, gpointer user_data)
{
	Mydata *my = get_mydata(user_data);
	(my->rotate_angle)+=90;
	
	if(my->rotate_angle >= 360){
		(my->rotate_angle-=360);
	}
	
	update_area1_with_transforms (my);
	set_status(my->status,"Image rotated");

}

void on_item_bgcolor_activate(GtkMenuItem *menu_item, gpointer user_data)
{
	gchar *texte;
	gint rep;
	GdkRGBA col;
	Mydata *my = get_mydata(user_data);
	
	GtkWidget *color = gtk_color_chooser_dialog_new ("Background color",NULL);
	rep=gtk_dialog_run(GTK_DIALOG (color));
	if(rep==GTK_RESPONSE_OK){
		gtk_color_chooser_get_rgba (GTK_COLOR_CHOOSER(color),&col);
		texte=gdk_rgba_to_string (&col);
		set_status(my->status,"Selected Bg Color: %s",texte);
		gtk_widget_override_background_color (GTK_WIDGET(my->area1),GTK_STATE_FLAG_NORMAL,&col);
		g_free(texte);
	}
	gtk_widget_destroy(color);
}

void on_item_clip_activate(GtkCheckMenuItem *check_menu_item, gpointer user_data){
	Mydata *my = get_mydata(user_data);
	my->clip_image=gtk_check_menu_item_get_active (check_menu_item);
	
	if(my->clip_image==TRUE){
		set_status(my->status,"Clipping is on");
	}else{
		set_status(my->status,"Clipping is off");
	}
	
	refresh_area(my->area1);
}

void on_show_scale (GtkMenuItem *menu_item,gpointer user_data){
	Mydata *my = get_mydata(user_data);
	gtk_window_present (GTK_WINDOW (my->win_scale));
}

void menus_init(gpointer user_data){
	Mydata *my = get_mydata(user_data);
	GtkWidget *item_file, *item_tools, *item_help, *sub_file, *item_load, *item_quit, *sub_tools, *sub_help, *item_rotate, *item_bgcolor, *item_about, *item_scale, *item_clip;
	GtkWidget *item_mode, *sub_mode, *item_jouer ,*item_edition, *item_niveaux, *item_online;
	
	my->menu_bar = gtk_menu_bar_new ();
	
	item_file = gtk_menu_item_new_with_label ("File");
	item_mode = gtk_menu_item_new_with_label ("Mode");
	item_tools = gtk_menu_item_new_with_label ("Tools");
	item_help = gtk_menu_item_new_with_label ("Help");
	
	gtk_menu_shell_append (GTK_MENU_SHELL(my->menu_bar),item_file);
	gtk_menu_shell_append (GTK_MENU_SHELL(my->menu_bar),item_mode);
	gtk_menu_shell_append (GTK_MENU_SHELL(my->menu_bar),item_tools);
	gtk_menu_shell_append (GTK_MENU_SHELL(my->menu_bar),item_help);
	
	sub_file = gtk_menu_new ();
	item_load = gtk_menu_item_new_with_label ("Load");
	item_quit = gtk_menu_item_new_with_label ("Quit");
	gtk_menu_shell_append (GTK_MENU_SHELL(sub_file),item_load);
	gtk_menu_shell_append (GTK_MENU_SHELL(sub_file),item_quit);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM(item_file), sub_file);
	
	sub_mode = gtk_menu_new ();
	item_jouer = gtk_menu_item_new_with_label ("Jouer");
	item_edition = gtk_menu_item_new_with_label ("Edition");
	item_niveaux = gtk_menu_item_new_with_label ("Niveaux");
	item_online = gtk_menu_item_new_with_label ("Online");
	gtk_menu_shell_append (GTK_MENU_SHELL(sub_mode),item_jouer);
	gtk_menu_shell_append (GTK_MENU_SHELL(sub_mode),item_edition);
	gtk_menu_shell_append (GTK_MENU_SHELL(sub_mode),item_niveaux);
	gtk_menu_shell_append (GTK_MENU_SHELL(sub_mode),item_online);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM(item_mode), sub_mode);
	
	sub_tools = gtk_menu_new ();
	item_rotate = gtk_menu_item_new_with_label ("Rotate");
	item_bgcolor = gtk_menu_item_new_with_label ("Bg color");
	item_scale = gtk_menu_item_new_with_label ("Scale");
	item_clip = gtk_check_menu_item_new_with_label ("Clip");
	gtk_menu_shell_append (GTK_MENU_SHELL(sub_tools),item_rotate);
	gtk_menu_shell_append (GTK_MENU_SHELL(sub_tools),item_bgcolor);
	gtk_menu_shell_append (GTK_MENU_SHELL(sub_tools),item_scale);
	gtk_menu_shell_append (GTK_MENU_SHELL(sub_tools),item_clip);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM(item_tools), sub_tools);
	
	sub_help = gtk_menu_new ();
	item_about = gtk_menu_item_new_with_label ("About");	
	gtk_menu_shell_append (GTK_MENU_SHELL(sub_help),item_about);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM(item_help), sub_help);
	
	
	/* GESTION DES EVENEMENTS */
	
	g_signal_connect (G_OBJECT(item_load), "activate",G_CALLBACK(on_item_load_activate), my);
	g_signal_connect (G_OBJECT(item_quit), "activate",G_CALLBACK(on_item_quit_activate), my);
	
	g_signal_connect (G_OBJECT(item_jouer), "activate",G_CALLBACK(on_item_jouer_activate), my);
	g_signal_connect (G_OBJECT(item_edition), "activate",G_CALLBACK(on_item_edition_activate), my);
	g_signal_connect (G_OBJECT(item_niveaux), "activate",G_CALLBACK(on_item_niveaux_activate), my);
	g_signal_connect (G_OBJECT(item_online), "activate",G_CALLBACK(on_item_online_activate), my);
	
	g_signal_connect (G_OBJECT(item_rotate), "activate",G_CALLBACK(on_item_rotate_activate), my);
	g_signal_connect (G_OBJECT(item_bgcolor), "activate",G_CALLBACK(on_item_bgcolor_activate), my);
	g_signal_connect (G_OBJECT(item_scale), "activate",G_CALLBACK(on_show_scale), my);
	g_signal_connect (G_OBJECT(item_clip), "activate",G_CALLBACK(on_item_clip_activate), my);
	
	g_signal_connect (G_OBJECT(item_about), "activate",G_CALLBACK(on_item_about_activate), my);
}
