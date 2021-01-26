#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include "util.h"
#include "curve.h"
#include "game.h"
#include "mydata.h"
#include "area1.h"
#include "gui.h"

void export_track_to_file(Game game, char const *filename){
	FILE *f = fopen(filename, "w");
	int i,x,y;
	
	if (f == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}

	fprintf(f, "%s\n","<Track>");
	
	for(i=0;i<game.road.curve_central.control_count;i++){
		x = (int) game.road.curve_central.controls[i].x;
		y = (int) game.road.curve_central.controls[i].y;
		fprintf(f, "%d %d\n", x, y);
	}
	fprintf(f, "%s\n","</Track>");
	
	fclose(f);
}

void import_track_from_file(Mydata *my, char const *filename){
	
	while(my->curve_infos.curve_list.curve_count>=1){
		my->curve_infos.current_curve=my->curve_infos.curve_list.curve_count-1;
		remove_curve(&my->curve_infos);
	}
	
	FILE *f = fopen(filename, "r");
	int x,y,read_track=FALSE;
	char * line = NULL;
    size_t len = 0;
	
	if (f == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}

	add_curve (&my->curve_infos);
	//pour lire le fichier ligne par ligne
	while (getline(&line, &len, f) != -1) {
		if(strcmp(line,"<Track>\n")==0){
			read_track=TRUE;
			if(getline(&line, &len, f)==-1){
				fclose(f);
				return;
			}
		}
		if(strcmp(line,"</Track>\n")==0){
			read_track=FALSE;
		}
		if(read_track==TRUE){
			sscanf(line, "%d %d", &x, &y);
			add_control (&my->curve_infos, (double) x, (double) y);
		}
    }
	
	//my->game.road.curve_central.control_count=i;
	fclose(f);
	if (line)
        free(line);
}

void on_save_track_button_clicked (GtkButton *button, gpointer user_data)
{
	Mydata *my = get_mydata(user_data);
	
	GtkWidget *dialog;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
	gint res;

	dialog = gtk_file_chooser_dialog_new ("Enregistrer la route",GTK_WINDOW (my->window),action,"Annuler",GTK_RESPONSE_CANCEL,"Enregistrer",GTK_RESPONSE_ACCEPT,NULL);
	
	if(my->current_folder!=NULL)
	{
		gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER (dialog),my->current_folder);
	}
	
	res = gtk_dialog_run (GTK_DIALOG (dialog));
	
	if (res == GTK_RESPONSE_ACCEPT){
	    char *filename;
	    g_free(my->current_folder);
	    my->current_folder = gtk_file_chooser_get_current_folder (GTK_FILE_CHOOSER (dialog));
	    filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
	    set_status(my->status,"Sauvegarde de la route...");
		export_track_to_file(my->game,filename);
		set_status(my->status,"Route sauvegardée");
	    g_free (filename);
	}
	
	gtk_widget_destroy (dialog);
}

void on_load_track_button_clicked (GtkButton *button, gpointer user_data)
{
	Mydata *my = get_mydata(user_data);
	
	//gtk_image_set_from_file (GTK_IMAGE(my->image1),"tux2.gif");
	GtkWidget *dialog;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
	gint res;

	dialog = gtk_file_chooser_dialog_new ("Ouvrir une route",GTK_WINDOW (my->window),action,"Annuler",GTK_RESPONSE_CANCEL,"Ouvrir",GTK_RESPONSE_ACCEPT,NULL);
	
	if(my->current_folder!=NULL)
	{
		gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER (dialog),my->current_folder);
	}
	
	set_status(my->status,"debug");
	res = gtk_dialog_run (GTK_DIALOG (dialog));
	
	if (res == GTK_RESPONSE_ACCEPT)
	{
	    char *filename;
		GFile *trackname;
	    g_free(my->current_folder);
		
	    my->current_folder = gtk_file_chooser_get_current_folder (GTK_FILE_CHOOSER (dialog));
	    filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		trackname = gtk_file_chooser_get_file (GTK_FILE_CHOOSER (dialog));
		
		gtk_label_set_text (GTK_LABEL(my->track_name_label),g_file_get_basename (trackname));
		
	    set_status(my->status,"Chargement de la route...");
		import_track_from_file(my,filename);
		set_status(my->status,"Route chargée !");
		
	    g_free (filename);
		g_object_unref (trackname);
	}
	
	set_edit_mode (my, EDIT_ADD_CONTROL);
	gtk_widget_destroy (dialog);
}

void on_scale1_value_changed(GtkRange *range, gpointer user_data){
	Mydata *my = get_mydata(user_data);
	my->scale1_value=gtk_range_get_value (range);
	update_area1_with_transforms(my);
	set_status(my->status,"Scale %f",my->scale1_value);
}

void on_edit_radio_toggled (GtkToggleButton *togglebutton, gpointer user_data)
{
	Mydata *my = get_mydata(user_data);
	set_anim1_mode (my, FALSE);
	my->game.state = GS_EDIT;
	refresh_area (my->area1);
	
	my->edit_mode = GPOINTER_TO_INT(g_object_get_data (G_OBJECT(togglebutton), "mode"));
}

void on_view_check_button_toggled (GtkToggleButton *togglebutton, gpointer user_data)
{
	Mydata *my = get_mydata(user_data);
	my->view_rendered=gtk_toggle_button_get_active (togglebutton);
	refresh_area (my->area1);
}

void on_play_game(GtkButton *togglebutton, gpointer user_data){
	Mydata *my = get_mydata(user_data);
	Game *game = &my->game;

	my->game.state=GS_PLAYING;
	my->game.countdown=3;
	
	my->game.car_count = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON(my->nbr_joueur));
	
	// Ces 3 fonctions à implémenter
	copy_curve_infos_to_road (&my->curve_infos.curve_list.curves[0],&my->game);
	compute_road_tracks (&game->road, 0.02);
	place_cars_on_start (game);

	//game->mode=GM_SINGLE;
	my->show_edit=FALSE; //pour arréter l'édition
	
	refresh_area (my->area1);
	gtk_widget_hide (my->win_play);
}

void play_init(gpointer user_data){
	Mydata *my = get_mydata(user_data);
	
	GdkRGBA color;
	color.red = 0.5;
	color.green = 0.5;
	color.blue = 0.5;
	color.alpha = 1.0;

	GtkWidget *vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 8);
	GtkWidget *nbr_label = gtk_label_new ("Nombre de joueurs :");
	
	if(my->nbr_joueur==NULL){
		my->nbr_joueur = gtk_spin_button_new_with_range (1,CAR_MAX,1);
	}

	GtkWidget *Piste_label = gtk_label_new ("Sélectionnez un niveau :");
	gtk_label_set_markup (GTK_LABEL(Piste_label),"<span foreground=\"blue\"><b>Sélectionnez un niveau :</b></span>");
	
	GtkWidget *selectbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_widget_override_background_color (selectbox1,
                                      GTK_STATE_NORMAL,
                                      &color);
	
	if(my->track_name_label==NULL){
		my->track_name_label = gtk_label_new ("Aucune course sélectionnée");
	}
	GtkWidget *load_button = gtk_button_new_with_label ("Charger");
	
	GtkWidget *start_game = gtk_button_new_with_label ("Jouer !");
	
	g_signal_connect (G_OBJECT(load_button), "clicked",G_CALLBACK(on_load_track_button_clicked), my);
	g_signal_connect(G_OBJECT(start_game), "clicked", G_CALLBACK(on_play_game), my);
	//GtkWidget *separe=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
	
	my->win_play = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (my->win_play), "Démarrer une course");
	gtk_window_set_default_size (GTK_WINDOW (my->win_play), 300, 200);
	
	gtk_box_pack_start (GTK_BOX (vbox1), nbr_label, FALSE, FALSE, 8);
	gtk_box_pack_start (GTK_BOX (vbox1), my->nbr_joueur, FALSE, FALSE, 8);
	gtk_box_pack_start (GTK_BOX (vbox1), Piste_label, FALSE, FALSE, 8);
	
	gtk_box_pack_start (GTK_BOX (selectbox1), my->track_name_label, TRUE, FALSE, 8);
	gtk_box_pack_start (GTK_BOX (selectbox1), load_button, TRUE, FALSE, 8);
	
	gtk_container_add (GTK_CONTAINER (vbox1),selectbox1);
	
	gtk_box_pack_start (GTK_BOX (vbox1), start_game, FALSE, FALSE, 8);
	
	gtk_container_add (GTK_CONTAINER (my->win_play),vbox1);
	
	g_signal_connect(G_OBJECT(my->win_play), "delete-event", G_CALLBACK(gtk_widget_hide_on_delete), my);
	
	gtk_widget_show_all (my->win_play);
	gtk_widget_hide (my->win_play);
}

void editing_init(gpointer user_data){
	int i;
	Mydata *my = get_mydata(user_data);
	char *noms[8] = {"Add road", "Move road", "Remove road","Add control", "Move control", "Remove control","Move clip","Reset clip"};
	GtkWidget *save_button = gtk_button_new_with_label ("Sauvegarder la route");
	//my->frame1 = gtk_frame_new ("Editing");
	my->vbox2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	
	my->win_edit = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (my->win_edit), "Level editing");
	gtk_window_set_default_size (GTK_WINDOW (my->win_edit), 250, 400);
	
	GtkWidget *radios[8];
	
	GtkWidget *separe=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
	GtkWidget *affiche=gtk_check_button_new_with_label ("Afficher le rendu");
	
	for (i = 0; i < 8; i++) {
		radios[i] = gtk_radio_button_new_with_label_from_widget (i == 0 ? NULL : GTK_RADIO_BUTTON(radios[0]),noms[i]);
		my->edit_radios[i]=radios[i];
		g_object_set_data (G_OBJECT(my->edit_radios[i]), "mode",
GINT_TO_POINTER(i+1));
		g_signal_connect (radios[i], "toggled",G_CALLBACK(on_edit_radio_toggled), my);

		gtk_box_pack_start (GTK_BOX (my->vbox2), radios[i], FALSE, FALSE, 0);
		
	}

	gtk_box_pack_start (GTK_BOX (my->vbox2), separe, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (my->vbox2), affiche, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (my->vbox2), save_button, FALSE, FALSE, 0);
	
	g_signal_connect (affiche, "toggled",G_CALLBACK(on_view_check_button_toggled), my);
	g_signal_connect (save_button, "clicked",G_CALLBACK(on_save_track_button_clicked), my);
	
	gtk_container_add (GTK_CONTAINER (my->win_edit), my->vbox2);
	
	g_signal_connect(G_OBJECT(my->win_edit), "delete-event", G_CALLBACK(gtk_widget_hide_on_delete), my);
	
	gtk_widget_show_all (my->win_edit);
	gtk_widget_hide (my->win_edit);
}

void window_init(GtkApplication* app, gpointer user_data){
	Mydata *my = get_mydata(user_data);
	my->window = gtk_application_window_new (app);//my->window contient une nouvelle instance de fenetre
	
	gtk_window_set_title (GTK_WINDOW (my->window), my->title);//On définie la propriété titre de la fenetre
	gtk_window_set_default_size (GTK_WINDOW (my->window),my->win_width, my->win_height);//On définie la taille de la fenetre
}

void layout_init(gpointer user_data){
	Mydata *my = get_mydata(user_data);

	my->vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 4);
	my->hbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 2);
	my->scroll = gtk_scrolled_window_new(NULL,NULL);
	
	gtk_container_add (GTK_CONTAINER (my->window), my->vbox1);
	gtk_container_add (GTK_CONTAINER (my->scroll), my->area1);
	//gtk_container_add (GTK_CONTAINER (my->frame1), my->vbox2);
	
	gtk_box_pack_start (GTK_BOX (my->vbox1), my->menu_bar, FALSE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (my->vbox1), my->hbox1, TRUE, TRUE, 0);
	//gtk_box_pack_start (GTK_BOX (my->hbox1), my->frame1, FALSE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (my->hbox1), my->scroll, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (my->vbox1), my->status, FALSE, TRUE, 0);
}

void status_init(gpointer user_data){
	Mydata *my = get_mydata(user_data);
	my->status = gtk_statusbar_new();
	
	set_status(my->status,"Welcome in game !");
}


void win_scale_init (gpointer user_data){
	Mydata *my = get_mydata(user_data);
	GtkWidget *box1, *scale_label;

	my->win_scale=gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (my->win_scale), "Image scale");
	gtk_window_set_default_size (GTK_WINDOW (my->win_scale), 250, 80);
	
	scale_label = gtk_label_new ("Scale :");
	
	my->scale1 = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL,0.02,20.0,0.02);
	gtk_range_set_value (GTK_RANGE(my->scale1),1);
	g_signal_connect(G_OBJECT(my->scale1), "value-changed", G_CALLBACK(on_scale1_value_changed), my);
	
	box1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,0);
	gtk_container_add (GTK_CONTAINER (my->win_scale), box1);
	
	gtk_box_pack_start (GTK_BOX (box1), scale_label, FALSE, TRUE, 10);
	gtk_box_pack_start (GTK_BOX (box1), my->scale1, TRUE, TRUE, 10);
	
	g_signal_connect(G_OBJECT(my->win_scale), "delete-event", G_CALLBACK(gtk_widget_hide_on_delete), my);

	gtk_widget_show_all (my->win_scale);
	gtk_widget_hide (my->win_scale);
}
