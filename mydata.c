#include <gtk/gtk.h>
#include "curve.h"
#include "game.h"
#include "mydata.h"


Mydata *get_mydata (gpointer data)
{
	if (data == NULL) {
		fprintf (stderr, "get_mydata: NULL data\n"); return NULL;
	}
	if (((Mydata *)data)->magic != MYDATA_MAGIC) {
		fprintf (stderr, "get_mydata: bad magic number\n"); return NULL;
	}
	return data;
}

void init_mydata (Mydata *my)
{
	//Initialise des widget, des propriétés et autres qui sont regroupé dans une même
	//structure et peuvent etres utilisees dans tout le programme
	//Mydata *my = get_mydata(user_data);
	int i=0;
	my->magic = MYDATA_MAGIC;
	my->title = "Jeu de course";
	my->track_name = NULL;
	my->win_width = 800;
	my->win_height = 600;
	my->window=NULL;
	//my->image1 = NULL;
	my->area1 = NULL;
	my->current_folder=NULL;
	my->status=NULL;
	my->pixbuf1=NULL;
	my->pixbuf2=NULL;
	my->vbox1=NULL;
	my->vbox2=NULL;
	my->hbox1=NULL;
	my->menu_bar=NULL;
	my->scroll=NULL;
	my->win_scale=NULL;
	my->track_name_label=NULL;
	my->nbr_joueur=NULL;
	my->rotate_angle=0;
	my->scale1_value=1;
	my->click_x=0;
	my->click_y=0;
	my->click_n=0;
	my->last_x=0;
	my->last_y=0;
	for(i=0;i<CAR_MAX;i++){
		my->flag_key_left[i] = 0;
		my->flag_key_right[i] = 0;
		my->flag_key_up[i] = 0;
		my->flag_key_down[i] = 0;
	}
	my->show_edit=FALSE;
	my->clip_image=FALSE;
	my->scale1=NULL;
	my->edit_mode=EDIT_ADD_CURVE;
	my->timeout1 = 0;
	my->view_rendered=FALSE;
	init_curve_infos (&my->curve_infos);
	init_game (&my->game);
}

void set_edit_mode (Mydata *my, int mode){
if(mode>=1&&mode<=6)
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (my->edit_radios[mode-1]), TRUE);
}
