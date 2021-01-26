#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <math.h>
#include "curve.h"
#include "game.h"
#include "collision.h"
#define M_PI 3.14159265359
#define sensibilite 10

int proche,proche_b,x_gen,y_gen,x_genb,y_genb,xb_gen,yb_gen,xb_genb,yb_genb;

void rotate_cars (Game *game, int num_car){
	
	/* GESTION DE LA DIRECTION */
	game->cars[num_car].angle+=game->cars[num_car].direction*sensibilite;
	if(game->cars[num_car].angle<0)
		game->cars[num_car].angle=360+game->cars[num_car].angle;
	game->cars[num_car].angle=((int) game->cars[num_car].angle) % 360;
	
}

double max(double a, double b){
	if(a>=b){
		return a;
	}else{
		return b;
	}
}

double min(double a, double b){
	if(a<=b){
		return a;
	}else{
		return b;
	}
}

void place_cars_on_start (Game *game){
	double x_v1, y_v1;
	int k = game->car_count,i;
	
	for(i=1;i<=k;i++){
		x_v1 = game->road.track_central.sample_x[(i-1)*10];
		y_v1 = game->road.track_central.sample_y[(i-1)*10];
		game->cars[i-1]=init_car(x_v1,y_v1,8,0.3);
	}
}

int point_in_rectangle(double x_rec,double y_rec,double x_rec_max,double y_rec_max,double x_point, double y_point){
	if(x_point>=x_rec&&x_point<=x_rec_max&&y_point>=y_rec&&y_point<=y_rec_max){
		return TRUE;
	}else{
		return FALSE;
	}
}

int rapproche_segment(double xc, double yc,double xa, double ya,double xb, double yb, double alpha){
	double dist1=compute_dist_point_to_line (xa, ya, xb, yb,xc, yc);
	double dist2=compute_dist_point_to_line (xa, ya, xb, yb,xc+cos(alpha), yc+sin(alpha));
	if(dist1>=dist2){
		return TRUE;
	}else{
		return FALSE;
	}
}

void progress_game_next_step (Game *game){
	
	int i;
	for(i=0;i<game->car_count;i++){
		rotate_cars (game, i);
		/* GESTION DE LA VITESSE */

		game->cars[i].accelerator+=(game->cars[i].max_accelerator/200);
		game->cars[i].speed+=game->cars[i].accelerator*game->cars[i].avance;

		//quand on n'accèlère pas, on freine et l'accélération diminue
		if(game->cars[i].avance==0&&game->cars[i].speed>0&&game->cars[i].accelerator>0.001){
			game->cars[i].speed-=(game->cars[i].accelerator/2);
			game->cars[i].accelerator-=(game->cars[i].max_accelerator/200);
		}

		if(game->cars[i].avance==0&&game->cars[i].speed<0&&game->cars[i].accelerator>0.001){
			game->cars[i].speed+=(game->cars[i].accelerator/2);
			game->cars[i].accelerator-=(game->cars[i].max_accelerator/200);
		}
		
		//on verifie que la vitesse ne soit pas dépassée
		if(game->cars[i].speed>game->cars[i].max_speed)
			game->cars[i].speed=game->cars[i].max_speed;

		if(game->cars[i].speed<-1*game->cars[i].max_speed)
			game->cars[i].speed=-1*game->cars[i].max_speed;

		/* GESTION DES COORDONNEES */

		game->cars[i].x+=cos(game->cars[i].angle*(M_PI/180))*game->cars[i].speed;
		game->cars[i].y+=sin(game->cars[i].angle*(M_PI/180))*game->cars[i].speed;

		if(test_collide_right (game, 0)==1){
			//if_collision(game);
			//printf("boum a droite\n");
		}
		
		if(test_collide_left (game, 0)==1){
			//printf("boum a gauche\n");
			//if_collision(game);
		}
		
		if(test_finish(game, i)==1){
			printf("victoire !\n");
		}
	}
	return;
}

void init_game(Game *game){
	game->car_count=0;
	game->state=GS_HELLO;
	game->score=0;
	game->mode=GM_NONE;
	game->countdown=5;
}

Car init_car(double x, double y, double max, double acc){
	Car car;
	car.x=x;
	car.y=y;
	car.radius=12;
	car.angle=0;
	car.avance=0;
	car.speed=0;
	car.max_speed=max;
	car.max_accelerator=acc;
	car.direction=0;
	car.accelerator=0;
	return car;
}
// Conversion d'une curve en track

void compute_bezier_track (Curve *curve, Track *tra, double theta)
{
    Control bez_points[4];

    tra->sample_count = 0;
    if (curve->control_count < 3) return;

    compute_bezier_points_prolong_first (curve, bez_points);
    sample_bezier_curve (bez_points, theta, tra->sample_x, tra->sample_y,
        &tra->sample_count, SAMPLE_MAX, 1);

    for (int k = 0; k < curve->control_count-3; k++) {
      compute_bezier_points_open (curve, k, bez_points);
      sample_bezier_curve (bez_points, theta, tra->sample_x, tra->sample_y,
          &tra->sample_count, SAMPLE_MAX, 0);
    }

    compute_bezier_points_prolong_last (curve, bez_points);
    sample_bezier_curve (bez_points, theta, tra->sample_x, tra->sample_y,
        &tra->sample_count, SAMPLE_MAX, 0);
}


// Conversion des 3 curves en 3 tracks

void compute_road_tracks (Road *road, double theta)
{
    compute_bezier_track (&road->curve_left,    &road->track_left,    theta);
    compute_bezier_track (&road->curve_central, &road->track_central, theta);
    compute_bezier_track (&road->curve_right,   &road->track_right,   theta);
}


// Affichage d'un track : appeler avant les fonctions :
//  cairo_set_source_rgb
//  cairo_set_line_width
//  cairo_set_dash


void draw_edit_track (cairo_t *cr, Track *l_track,Track *c_track, Track *r_track)
{
cairo_set_source_rgb (cr, 0, 0, 0);
cairo_set_line_width (cr, 1.5);

if (l_track->sample_count == 0)
	return;

cairo_move_to (cr, l_track->sample_x[0], l_track->sample_y[0]);
for (int k = 1; k < l_track->sample_count; k++){
  cairo_line_to (cr, l_track->sample_x[k], l_track->sample_y[k]);
}
cairo_stroke (cr);

if (c_track->sample_count == 0)
	return;

cairo_move_to (cr, c_track->sample_x[0], c_track->sample_y[0]);
for (int k = 1; k < c_track->sample_count; k++){
  cairo_line_to (cr, c_track->sample_x[k], c_track->sample_y[k]);
}
cairo_stroke (cr);

if (r_track->sample_count == 0)
	return;

cairo_move_to (cr, r_track->sample_x[0], r_track->sample_y[0]);
for (int k = 1; k < r_track->sample_count; k++){
  cairo_line_to (cr, r_track->sample_x[k], r_track->sample_y[k]);
}
cairo_stroke (cr);
}


void draw_track (cairo_t *cr, Track *l_track,Track *c_track, Track *r_track)
{
int k;
double dash[2]={10,5};

cairo_set_source_rgb (cr, 0.75, 0.75, 1);
cairo_set_line_width (cr, 2);

    if (l_track->sample_count == 0) return;
	
    cairo_move_to (cr, l_track->sample_x[0], l_track->sample_y[0]);
for (k = 1; k < l_track->sample_count; k++){
	
	cairo_line_to (cr, l_track->sample_x[k], l_track->sample_y[k]);
}
  
for (k = r_track->sample_count-1; k>=0; k--){

      cairo_line_to (cr, r_track->sample_x[k], r_track->sample_y[k]);
}

cairo_close_path (cr);
//cairo_stroke_preserve(cr);
cairo_fill_preserve (cr);

if(proche_b==1){
	cairo_set_source_rgb (cr, 1, 0, 0);
}else{
	if(proche==1){
		cairo_set_source_rgb (cr, 0, 0, 1);
	}else{
		cairo_set_source_rgb (cr, 0.75, 0.75, 1);
	}
}	

cairo_stroke(cr);

//tracer collision droite
cairo_set_line_width (cr, 2);
cairo_set_source_rgb (cr, 1, 0, 0);

cairo_move_to (cr,x_gen,y_gen);
cairo_line_to (cr,x_genb,y_genb);
cairo_stroke(cr);

//tracer collision gauche
cairo_set_source_rgb (cr, 0, 0, 1);

cairo_move_to (cr,xb_gen,yb_gen);
cairo_line_to (cr,xb_genb,yb_genb);
cairo_stroke(cr);

cairo_set_line_width (cr, 2);
cairo_set_dash (cr,dash,1,1);
cairo_set_line_width (cr, 2);
cairo_set_source_rgb (cr, 1, 1, 1);
cairo_move_to (cr, c_track->sample_x[0], c_track->sample_y[0]);
for (k = 0; k < c_track->sample_count; k++){
		cairo_line_to (cr, c_track->sample_x[k], c_track->sample_y[k]);
}
cairo_stroke(cr);
cairo_set_dash (cr,dash,0,1);

}

void compute_normal_right (double xa, double ya, double xb, double yb, double *xn, double *yn){
double x,y,n;

x=ya-yb;
y=xb-xa;
n=sqrt((x*x)+(y*y));

if(n==0){
	*xn=0;
 	*yn=0;
}else{
	*xn=x/n;
	*yn=y/n;
}

}

void do_vector_product (double xa, double ya, double za,double xb, double yb, double zb,double *xn, double *yn, double *zn){

	*xn = (ya*zb)-(yb*za);
	*yn = -1*((xa*zb)-(xb*za));
	*zn = (xa*yb)-(xb*ya);

}

int compute_intersection (double xs1, double ys1, double xs2, double ys2,double xt1, double yt1, double xt2, double yt2,double *xp, double *yp){
	double as,bs,cs=0,at,bt,ct=0,xi,yi,zi=0;
	do_vector_product(xs1, ys1, 1, xs2, ys2, 1, &as, &bs, &cs);
	do_vector_product(xt1, yt1, 1, xt2, yt2, 1, &at, &bt, &ct);
	do_vector_product(as, bs, cs, at, bt, ct, &xi, &yi, &zi);
	
	if(zi==0){
		return 0;
	}
	
	*xp=xi/zi;
	*yp=yi/zi;
	return 1;
	
}
            
double get_horiz_angle_rad (double xa, double ya, double xb, double yb){
	double dx,dy,n;
	dy=yb-ya;
	dx=xb-xa;
	n=sqrt((dx*dx)+(dy*dy));

	if(n==0){
		return 0;
	}
	
	if(dy>0){
		return acos(dx/n);
	}else{
		return -1*acos(dx/n);
	}


}

int point_is_on_right (double xa, double ya, double xb, double yb, double xc, double yc){
	double A,B,C,D,det;
	A=xc-xa;
	B=xb-xa;
	C=yc-ya;
	D=yb-ya;
	det=A*D-B*C;
	if(det<0){
		return TRUE;
	}else{
		return FALSE;
	}
}
            
double compute_dist_point_to_line (double xa, double ya, double xb, double yb,double xc, double yc){
	double A,B,C,D, E, F,num,denum;
	A=(yb-ya)*xc;
	B=(xb-xa)*yc;
	C=xb*ya;
	D=yb*xa;
	num=abs(A-B+C-D);

	E=(yb-ya)*(yb-ya);
	F=(xb-xa)*(xb-xa);
	denum=sqrt(E+F);
	if(denum==0)
		return 0;	
	return num/denum;
}

double calculer_angle_reflechi(double xa, double ya, double xb, double yb, double angle){
double angle_ref,Beta=0;

angle_ref = 2*angle + PI - Beta;

if(angle_ref<0){
	angle_ref+=(2*PI);
}else{
	if(angle_ref>(2*PI))
		angle_ref+=(-2*PI);
}
return angle_ref;
}
