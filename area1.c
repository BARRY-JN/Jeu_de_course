#include <gtk/gtk.h>
#include <unistd.h>
#include <time.h>
//#include <gdk-pixbuf/gdk-pixbuf.h>
#include "util.h"
#include "curve.h"
#include "game.h"
#include "mydata.h"
#include "area1.h"
#include "font.h"
#define M_PI 3.14159265359

void set_anim1_mode (Mydata *my, int flag)
{
	if (flag) {
		if (my->timeout1 != 0) return;
		my->timeout1 = g_timeout_add (ANIM1_TIME, on_timeout1, my);

		set_status (my->status, "Animation 1 started");
	} else {
		if (my->timeout1 == 0) return;
		g_source_remove (my->timeout1);
		my->timeout1 = 0;

		set_status (my->status, "Animation 1 stopped");
	}
}

gboolean on_timeout1 (gpointer data)
{
	int i;
	Mydata *my = get_mydata(data);
    Game *game = &my->game;
		  
	for(i=0;i<game->car_count;i++){

		  game->cars[i].direction =
					(my->flag_key_left[i]  && !my->flag_key_right[i]) ? -1 :
					(my->flag_key_right[i] && !my->flag_key_left[i] ) ?  1 : 0;
			  
		if(my->flag_key_up[i]&&!my->flag_key_down[i]){
				game->cars[i].avance=1;
		}else{
			if(my->flag_key_down[i]&&!my->flag_key_up[i]){
				game->cars[i].avance=-1;
			}else{
			//on fait freiner la voiture quand on n'accélère pas
			game->cars[i].avance=-0;
			}
		}
	}
          
          progress_game_next_step (game);        	
          
          refresh_area (my->area1);
          return TRUE;
}

void draw_control_labels (cairo_t *cr, PangoLayout *layout, Curve_infos *ci){
	int i,j,x,y;
	/*
	w = gtk_widget_get_allocated_width (area),
	h = gtk_widget_get_allocated_height (area);
	*/
	font_set_name (layout, "Sans, 8");
	cairo_set_source_rgb (cr, 0.25, 0.25, 0.25);
	
	for(i=0;i<ci->curve_list.curve_count;i++){
		for(j=0;j<ci->curve_list.curves[i].control_count;j++){
			x=ci->curve_list.curves[i].controls[j].x;
			y=ci->curve_list.curves[i].controls[j].y;
			font_draw_text (cr, layout, FONT_TC,x,y-18,"%d",j);
		}
	}
}

void apply_image_transforms (Mydata *my){
	g_clear_object(&my->pixbuf2);
	
	if(my->pixbuf1==NULL)
		return;
	
	int largeur=0, hauteur=0;

	//gtk_range_set_value (GTK_RANGE(my->scale1),my->scale1_value);

	GdkPixbuf *tmp1=NULL;
	
	tmp1=gdk_pixbuf_rotate_simple (my->pixbuf1, my->rotate_angle);//rotation de 90°
	
	largeur=gdk_pixbuf_get_width(tmp1) * my->scale1_value;
	hauteur=gdk_pixbuf_get_height(tmp1) * my->scale1_value;
	
	my->pixbuf2 = gdk_pixbuf_scale_simple (tmp1,largeur,hauteur,GDK_INTERP_BILINEAR);

	set_status(my->status,"Image rotated");
	
	g_object_unref (G_OBJECT(tmp1));
}

void update_area1_with_transforms (Mydata *my){
	apply_image_transforms (my);
	if(my->pixbuf2==NULL)
		return;
	gtk_widget_set_size_request (my->area1, gdk_pixbuf_get_width(my->pixbuf2), gdk_pixbuf_get_height(my->pixbuf2));
	refresh_area (my->area1);
}

void draw_bezier_curve (cairo_t *cr, Control bez_points[4], double theta){
	double bx[4], by[4],x,y,t=0;
	int i;
	
	
	for(i=0;i<4;i++){
		bx[i]=bez_points[i].x;
		by[i]=bez_points[i].y;
	}
	
	for(t=0;t<=1;t+=theta){
		x=compute_bezier_cubic (bx, t);
		y=compute_bezier_cubic (by, t);
		cairo_line_to (cr,x,y);

	}
	//cairo_stroke (cr);

}


void generate_bezier_path (cairo_t *cr, Control bez_points[4], double theta, int is_first){
	double bx[4], by[4],x,y,t=0;
	int i;
	
	
	for(i=0;i<4;i++){
		bx[i]=bez_points[i].x;
		by[i]=bez_points[i].y;
	}
	
	for(t=0;t<=1;t+=theta){
		x=compute_bezier_cubic (bx, t);
		y=compute_bezier_cubic (by, t);
		if(is_first==TRUE&&t==0){
			cairo_move_to (cr,x,y);
		}else{
			cairo_line_to (cr,x,y);
		}

	}
}

void draw_bezier_curves_open (cairo_t *cr, Curve_infos *ci, double theta){
	Control bez_points[4];
	int k,i;
	cairo_set_line_width (cr, 1.5);
	cairo_set_source_rgb (cr, 1.0, 0.0, 1.0);
	
	for(k=0;k<ci->curve_list.curve_count;k++){
		for(i=0;i<ci->curve_list.curves[k].control_count-3;i++){ 
			compute_bezier_points_open (&ci->curve_list.curves[k],i,bez_points);
			draw_bezier_curve (cr, bez_points,theta);
		}
		cairo_stroke (cr);
	}
}

void draw_bezier_curves_close (cairo_t *cr, Curve_infos *ci, double theta){
	Control bez_points[4];
	int k,i;
	
	cairo_set_source_rgb (cr, 1.0, 0.0, 1.0);
	
	for(k=0;k<ci->curve_list.curve_count;k++){
		for(i=0;i<ci->curve_list.curves[k].control_count;i++){
			compute_bezier_points_close (&ci->curve_list.curves[k],i,bez_points);
			draw_bezier_curve (cr, bez_points,theta);
		}
		cairo_close_path (cr);
		cairo_stroke (cr);
		
		//cairo_set_line_width (cr, 1.5);
		//compute_bezier_points_close (&ci->curve_list.curves[k],0,bez_points);
		//draw_bezier_curve (cr, bez_points,theta);
	}
}

void draw_bezier_curves_fill (cairo_t *cr, Curve_infos *ci, double theta){
	Control bez_points[4];
	int k,i;
	cairo_set_line_width (cr, 1.5);
	cairo_set_source_rgb (cr, 1.0, 0.0, 1.0);
	
	for(k=0;k<ci->curve_list.curve_count;k++){
		for(i=0;i<ci->curve_list.curves[k].control_count;i++){
			compute_bezier_points_close (&ci->curve_list.curves[k],i,bez_points);
			generate_bezier_path (cr, bez_points, theta,i == 0 ? TRUE : FALSE);
		}
		cairo_close_path (cr);
		cairo_stroke_preserve (cr);
	}
	cairo_fill (cr);
}

void draw_bezier_curves_clip (cairo_t *cr, Curve_infos *ci, double theta, Mydata *my){
	Control bez_points[4];
	int k,i;
	cairo_set_line_width (cr, 1.5);
	
	for(k=0;k<ci->curve_list.curve_count;k++){
		
			if(my->pixbuf2!=NULL){
				gdk_cairo_set_source_pixbuf (cr,my->pixbuf2,ci->curve_list.curves[k].shift_x,ci->curve_list.curves[k].shift_y);
			}else{
				cairo_set_source_rgb (cr, 1.0, 0.0, 1.0);
			}
		
		for(i=0;i<ci->curve_list.curves[k].control_count;i++){
			compute_bezier_points_close (&ci->curve_list.curves[k],i,bez_points);
			
			generate_bezier_path (cr, bez_points, theta,i == 0 ? TRUE : FALSE);
		}
		cairo_close_path (cr);
		cairo_stroke_preserve (cr);
		cairo_fill (cr);
	}
}


void draw_bezier_curves_prolong (cairo_t *cr, Curve_infos *ci, double theta){
	Control bez_points[4];
	int k,i;
	cairo_set_line_width (cr, 1.5);
	cairo_set_source_rgb (cr, 1.0, 0.0, 1.0);
	
	for(k=0;k<ci->curve_list.curve_count;k++){
		if(ci->curve_list.curves[k].control_count<=3){
			if(k+1<ci->curve_list.curve_count){
				k++;
			}else{
				break;
			}
		}
		compute_bezier_points_prolong_first (&ci->curve_list.curves[k],bez_points);
		draw_bezier_curve (cr, bez_points,theta);
		for(i=0;i<ci->curve_list.curves[k].control_count-3;i++){
			compute_bezier_points_open (&ci->curve_list.curves[k],i,bez_points);
			draw_bezier_curve (cr, bez_points,theta);
		}
		compute_bezier_points_prolong_last (&ci->curve_list.curves[k],bez_points);
		draw_bezier_curve (cr, bez_points,theta);
		cairo_stroke (cr);
	}
}


void draw_bezier_polygons_open (cairo_t *cr, Curve_infos *ci){
Control bez_points[4];
int k,i;
	cairo_set_line_width (cr, 1.5);
	cairo_set_source_rgb (cr, 0, 1.0, 0);
	
	for(k=0;k<ci->curve_list.curve_count;k++){
		for(i=0;i<ci->curve_list.curves[k].control_count-3;i++){

			compute_bezier_points_open (&ci->curve_list.curves[k],i,bez_points);

			cairo_move_to (cr, bez_points[0].x, bez_points[0].y);
			cairo_line_to (cr, bez_points[1].x, bez_points[1].y);
			//cairo_close_path (cr);
			
			cairo_move_to (cr, bez_points[2].x, bez_points[2].y);
			cairo_line_to (cr, bez_points[3].x, bez_points[3].y);
			//cairo_close_path (cr);
		}
	}
	
	cairo_stroke (cr);
	
}


void draw_control_polygons (cairo_t *cr, Curve_infos *ci){
	double x,y,x_prec,y_prec;
	int i,j;
	x=-1;
	y=-1;
	cairo_set_line_width (cr, 1.5);
	
	for(i=0;i<ci->curve_list.curve_count;i++){
		//on dessine les courbes
		for(j=0;j<ci->curve_list.curves[i].control_count;j++){
			x_prec=x;
			y_prec=y;
			x=ci->curve_list.curves[i].controls[j].x;
			y=ci->curve_list.curves[i].controls[j].y;
			
			//gestion couleur courbe
			if(ci->current_curve==i){
				cairo_set_source_rgb (cr, 1.0, 1.0, 0);
			}else{
				cairo_set_source_rgb (cr, 0.5, 0.5, 0.5);
			}
			
			if(x_prec!=-1||y_prec!=-1){
				cairo_move_to(cr,x,y);
				cairo_line_to (cr,x_prec,y_prec);
			}
			cairo_stroke (cr);
		}
		//on dessine les traits
		for(j=0;j<ci->curve_list.curves[i].control_count;j++){
			x=ci->curve_list.curves[i].controls[j].x;
			y=ci->curve_list.curves[i].controls[j].y;

			//gestion couleur controles
			if(ci->current_control==j&&ci->current_curve==i){
				cairo_set_source_rgb (cr, 1.0, 0, 0);	
			}
			else{
				cairo_set_source_rgb (cr, 0, 0, 1.0);	
			}
			
			cairo_rectangle (cr, x-3, y-3, 6, 6);
			cairo_stroke (cr);
		}
		x=-1;
		y=-1;
	}
}

void copy_curve_infos_to_road (Curve *curve, Game *game){
	int i;
	double xd,yd;
	//recopie curve 0 dans curve_central

	game->road.curve_central.control_count = curve->control_count;
	//game->road.curve_central.shift_x = curve->shift_x;

	for(i=0;i<curve->control_count;i++){
		game->road.curve_central.controls[i].x = curve->controls[i].x;
		game->road.curve_central.controls[i].y = curve->controls[i].y;
	}

	//calcul curve_right

	game->road.curve_right.control_count = curve->control_count;

	//Calcul curve_left

	game->road.curve_left.control_count = curve->control_count;

	for(i=0;i<game->road.curve_central.control_count;i++){
		if(i+1==game->road.curve_central.control_count){
			compute_normal_right (game->road.curve_central.controls[i-1].x, game->road.curve_central.controls[i-1].y, game->road.curve_central.controls[i].x, game->road.curve_central.controls[i].y, &xd, &yd);
		}else{
			if(i==0){
				compute_normal_right (game->road.curve_central.controls[i].x, game->road.curve_central.controls[i].y, game->road.curve_central.controls[i+1].x, game->road.curve_central.controls[i+1].y, &xd, &yd);
			}else{
				compute_normal_right (game->road.curve_central.controls[i-1].x, game->road.curve_central.controls[i-1].y, game->road.curve_central.controls[i+1].x, game->road.curve_central.controls[i+1].y, &xd, &yd);
			}
		}
		
		game->road.curve_right.controls[i].x = game->road.curve_central.controls[i].x + (xd*40);
		game->road.curve_right.controls[i].y = game->road.curve_central.controls[i].y + (yd*40);
		game->road.curve_left.controls[i].x = game->road.curve_central.controls[i].x - (xd*40);
		game->road.curve_left.controls[i].y = game->road.curve_central.controls[i].y - (yd*40);
	}
}

void draw_car (cairo_t *cr, Mydata *my){
	int i;
	for(i=0;i<my->game.car_count;i++){
		cairo_set_line_width (cr, 1.5);
		
		if(i==0)
			cairo_set_source_rgb (cr, 1.0, 0, 1.0);
		if(i==1)
			cairo_set_source_rgb (cr, 0, 0, 1.0);
		if(i>=2)
			cairo_set_source_rgb (cr, 1.0, 0, 0);
		
		
		//GdkPixbuf *pix = gdk_pixbuf_new_from_file("",NULL);
		//gdk_cairo_set_source_pixbuf (cr,my->pixbuf1,car.x-50, car.y-50);
		
		cairo_identity_matrix (cr);
		cairo_translate (cr, my->game.cars[i].x, my->game.cars[i].y);
		cairo_rotate (cr, my->game.cars[i].angle * (M_PI/180));
		cairo_translate (cr, -1*my->game.cars[i].x, -1*my->game.cars[i].y);
		cairo_arc(cr, my->game.cars[i].x, my->game.cars[i].y, my->game.cars[i].radius, 45*(M_PI/180), 315*(M_PI/180));
		cairo_identity_matrix (cr); 
		
		//printf("x:%f y:%f r:%f\n",car.x, car.y, car.radius);
		cairo_fill (cr);
	}
}

void draw_countdown (cairo_t *cr, Mydata *my){
	
	PangoLayout *layout = pango_cairo_create_layout (cr);
	font_set_name (layout, "Sans, 42");
	cairo_set_source_rgb (cr, 1, 0, 0);
	
	if(my->game.countdown<0){
		return;
	}
	
	if(my->game.countdown>0){
		font_draw_text (cr, layout, FONT_BL,50,100,"%d",my->game.countdown);
	}
	
	if(my->game.countdown==0){
		font_draw_text (cr, layout, FONT_BL,50,100,"Partez !");
	}
	cairo_stroke (cr);
	refresh_area (my->area1);
	g_object_unref (layout);
}

void draw_pause (cairo_t *cr, Mydata *my){
	PangoLayout *layout = pango_cairo_create_layout (cr);
	font_set_name (layout, "Sans, 42");
	cairo_set_source_rgb (cr, 1, 0, 0);
	font_draw_text (cr, layout, FONT_BL,50,100,"Pause");
	cairo_stroke (cr);
	g_object_unref (layout);
}

gboolean on_area1_draw (GtkWidget *area, cairo_t *cr, gpointer user_data){
	int pixbuf_h,pixbuf_l;
	Mydata *my = get_mydata(user_data);
	
	PangoLayout *layout = pango_cairo_create_layout (cr);
	
	if(my->pixbuf2!=NULL){
		pixbuf_l=gdk_pixbuf_get_width(my->pixbuf2);
		pixbuf_h=gdk_pixbuf_get_height(my->pixbuf2);
		gdk_cairo_set_source_pixbuf (cr,my->pixbuf2,0, 0);
		
		cairo_rectangle (cr, 0, 0, pixbuf_l, pixbuf_h);
		cairo_fill (cr);
	}

	
	cairo_set_source_rgb (cr, 0, 0, 1.0);
	
switch (my->game.state) {
	case GS_EDIT:
		copy_curve_infos_to_road (&my->curve_infos.curve_list.curves[0],&my->game);
        compute_road_tracks (&my->game.road, 0.02);
		if(my->view_rendered==TRUE){
			draw_track (cr, &my->game.road.track_left, &my->game.road.track_central, &my->game.road.track_right);
		}else{
			draw_edit_track (cr, &my->game.road.track_left, &my->game.road.track_central, &my->game.road.track_right);
			draw_control_polygons (cr, &my->curve_infos);
			draw_control_labels(cr, layout, &my->curve_infos);
			draw_bezier_polygons_open (cr, &my->curve_infos);
		}
		
	break;
	case GS_PLAYING:
		draw_track (cr, &my->game.road.track_left, &my->game.road.track_central, &my->game.road.track_right);
		
		draw_car (cr,my);
		if(my->game.countdown>=-1){
			draw_countdown(cr,my);
			sleep(1);
			my->game.countdown--;
			if(my->game.countdown==-1){
				set_anim1_mode (my, TRUE);
			}
		}
	break;
	case GS_PAUSE:
		draw_track (cr, &my->game.road.track_left, &my->game.road.track_central, &my->game.road.track_right);
		draw_car (cr,my);
		draw_pause (cr, my);
	break;
	default:
	
	break;
	}
	
	g_object_unref (layout);

	return TRUE;
}

gboolean on_area1_key_press (GtkWidget *area,GdkEvent *event, gpointer data){
	Mydata *my = get_mydata(data);
	
	GdkEventKey *evk = &event->key;
	//printf ("%s: GDK_KEY_%s\n",__func__, gdk_keyval_name(evk->keyval));

	switch (evk->keyval) {
		case GDK_KEY_q :
			gtk_widget_destroy(my->track_name_label);
			gtk_widget_destroy(my->win_scale);
			gtk_widget_destroy(my->win_edit);
			gtk_widget_destroy(my->win_play);
			gtk_widget_destroy(my->window);
		break;
		case GDK_KEY_p :
		if(my->game.state != GS_PAUSE){
			my->game.state = GS_PAUSE;
			set_anim1_mode (my, FALSE);
		}else{
			my->game.state = GS_PLAYING;
			set_anim1_mode (my, TRUE);
		}
		refresh_area (my->area1);
		break;
		case GDK_KEY_Left  : my->flag_key_left[0]  = 1; break;
        case GDK_KEY_Right : my->flag_key_right[0] = 1; break;
        case GDK_KEY_Up  : my->flag_key_up[0]  = 1; break;
        case GDK_KEY_Down  : my->flag_key_down[0]  = 1; break;
		case GDK_KEY_a  : my->flag_key_left[1]  = 1; break;
        case GDK_KEY_e : my->flag_key_right[1] = 1; break;
        case GDK_KEY_z  : my->flag_key_up[1]  = 1; break;
        case GDK_KEY_s  : my->flag_key_down[1]  = 1; break;
		case GDK_KEY_y  : my->flag_key_left[2]  = 1; break;
        case GDK_KEY_i : my->flag_key_right[2] = 1; break;
        case GDK_KEY_u  : my->flag_key_up[2]  = 1; break;
        case GDK_KEY_j  : my->flag_key_down[2]  = 1; break;
	}
	return TRUE; 
}

gboolean on_area1_key_release (GtkWidget *area,GdkEvent *event, gpointer data){
	Mydata *my = get_mydata(data);
	
	GdkEventKey *evk = &event->key;
	switch (evk->keyval) {
		case GDK_KEY_Left  : my->flag_key_left[0]  = 0; break;
        case GDK_KEY_Right : my->flag_key_right[0] = 0; break;
        case GDK_KEY_Up  : my->flag_key_up[0]  = 0; break;
        case GDK_KEY_Down  : my->flag_key_down[0]  = 0; break;
		case GDK_KEY_a  : my->flag_key_left[1]  = 0; break;
        case GDK_KEY_e : my->flag_key_right[1] = 0; break;
        case GDK_KEY_z  : my->flag_key_up[1]  = 0; break;
        case GDK_KEY_s  : my->flag_key_down[1]  = 0; break;
		case GDK_KEY_y  : my->flag_key_left[2]  = 0; break;
        case GDK_KEY_i : my->flag_key_right[2] = 0; break;
        case GDK_KEY_u  : my->flag_key_up[2]  = 0; break;
        case GDK_KEY_j  : my->flag_key_down[2]  = 0; break;
	}
	return TRUE; 
}

gboolean on_area1_button_press (GtkWidget *area,GdkEvent *event, gpointer data){
	Mydata *my = get_mydata(data);
	
	GdkEventButton *evb = &event->button;
	//printf ("%s: %d %.1f %.1f\n",__func__, evb->button, evb->x, evb->y);
	my->click_x=evb->x;
	my->click_y=evb->y;
	my->click_n=evb->button;
	
	if(my->click_n==1&&my->show_edit==TRUE){
		switch (my->edit_mode)
		{
			case EDIT_ADD_CURVE :
				if(add_curve (&my->curve_infos)<0)
					return TRUE;
				set_edit_mode (my, EDIT_ADD_CONTROL);
				add_control (&my->curve_infos, my->click_x, my->click_y);
			break;
			
			case EDIT_MOVE_CURVE :
				find_control(&my->curve_infos, my->click_x, my->click_y);
			break;
			
			case EDIT_REMOVE_CURVE :
				if(find_control(&my->curve_infos, my->click_x, my->click_y)!=-1){
					remove_curve(&my->curve_infos);
				}
			break;
			
			case EDIT_ADD_CONTROL :
				add_control(&my->curve_infos, my->click_x, my->click_y);
			break;
			
			case EDIT_MOVE_CONTROL :
				find_control(&my->curve_infos, my->click_x, my->click_y);
			break;
			
			case EDIT_REMOVE_CONTROL :
			if(find_control(&my->curve_infos, my->click_x, my->click_y)!=-1){
				remove_control(&my->curve_infos);
			}
			break;
			
			case EDIT_MOVE_CLIP :
				find_control(&my->curve_infos, my->click_x, my->click_y);
			break;
			
			case EDIT_RESET_CLIP :
				find_control(&my->curve_infos, my->click_x, my->click_y);
				reset_shift(&my->curve_infos);
			break;
			
			
		}
	}
	refresh_area(my->area1);
	return TRUE; 
}

gboolean on_area1_button_release (GtkWidget *area,GdkEvent *event, gpointer data){
	Mydata *my = get_mydata(data);
	
	//GdkEventButton *evb = &event->button;
	//printf ("%s: %d %.1f %.1f\n",__func__, evb->button, evb->x, evb->y);
	my->click_n=0;
	refresh_area(my->area1);
	return TRUE; 
}

gboolean on_area1_motion_notify (GtkWidget *area,GdkEvent *event, gpointer data){
	Mydata *my = get_mydata(data);
	
	GdkEventMotion *evm = &event->motion;
	my->last_x=my->click_x;
	my->last_y=my->click_y;
	
	my->click_x=evm->x;
	my->click_y=evm->y;

	if(my->click_n==1&&my->show_edit==TRUE){
		switch (my->edit_mode){			
			case EDIT_ADD_CURVE :

			break;
			
			case EDIT_MOVE_CURVE :
				move_curve(&my->curve_infos, my->click_x-my->last_x, my->click_y-my->last_y);
			break;
			
			case EDIT_REMOVE_CURVE : ; break;
			
			case EDIT_ADD_CONTROL : ;

			break;
			
			case EDIT_MOVE_CONTROL :
				move_control(&my->curve_infos, my->click_x-my->last_x, my->click_y-my->last_y);
			break;
			
			case EDIT_REMOVE_CONTROL : ; break;
			
			case EDIT_MOVE_CLIP :
				move_curve(&my->curve_infos, my->click_x-my->last_x, my->click_y-my->last_y);
				move_shift(&my->curve_infos, my->click_x-my->last_x, my->click_y-my->last_y);
			break;
			
		}
		
		refresh_area(my->area1);
	}
	return TRUE; 
}

gboolean on_area1_enter_notify (GtkWidget *area,GdkEvent *event, gpointer data){
	Mydata *my = get_mydata(data);
	
	//GdkEventMotion *evm = &event->motion;
	//printf ("%s: %.1f %.1f\n", __func__, evm->x, evm->y);
	gtk_widget_grab_focus (my->area1);
	return TRUE; 
}

gboolean on_area1_leave_notify (GtkWidget *area,GdkEvent *event, gpointer data){
	//Mydata *my = get_mydata(data);
	
	//GdkEventMotion *evm = &event->motion;
	//printf ("%s: %.1f %.1f\n", __func__, evm->x, evm->y);
	return TRUE; 
}

void area1_init(gpointer user_data){
	Mydata *my = get_mydata(user_data);
	
	my->area1=gtk_drawing_area_new ();
	//my->image1 = gtk_image_new();
	
	//gtk_container_add (GTK_CONTAINER (scroll), my->image1);
	
	gtk_widget_add_events (my->area1,GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK |GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK |GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK | GDK_POINTER_MOTION_MASK);
	gtk_widget_set_can_focus (my->area1, TRUE);
	
	g_signal_connect (my->area1, "motion-notify-event",G_CALLBACK (on_area1_motion_notify), my);
	g_signal_connect (my->area1, "enter-notify-event",G_CALLBACK (on_area1_enter_notify), my);
	g_signal_connect (my->area1, "leave-notify-event",G_CALLBACK (on_area1_leave_notify), my);
	g_signal_connect (my->area1, "key-press-event",G_CALLBACK (on_area1_key_press), my);
	g_signal_connect (my->area1, "key-release-event",G_CALLBACK (on_area1_key_release), my);
	g_signal_connect (my->area1, "button-press-event",G_CALLBACK (on_area1_button_press), my);
	g_signal_connect (my->area1, "button-release-event",G_CALLBACK (on_area1_button_release), my);
	g_signal_connect (my->area1, "draw",G_CALLBACK (on_area1_draw), my);
}
