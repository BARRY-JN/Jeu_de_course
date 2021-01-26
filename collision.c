#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <math.h>
#include "curve.h"
#include "game.h"
#include "collision.h"

int proche,proche_b,x_gen,y_gen,x_genb,y_genb,xb_gen,yb_gen,xb_genb,yb_genb;

int test_collide_right (Game *game, int voiture){
/* GESTION DES COLLISIONS */
	int i,collision=0;
	double xa,ya,xb,yb;
	proche=0;
	int first_point=-1;
	
	for(i=0;i<game->road.track_right.sample_count-1;i++){
		
		if(i>=game->road.track_right.sample_count){
			xa=game->road.track_right.sample_x[i-1];
			ya=game->road.track_right.sample_y[i-1];
			xb=game->road.track_right.sample_x[i];
			yb=game->road.track_right.sample_y[i];
		}else{
			xa=game->road.track_right.sample_x[i];
			ya=game->road.track_right.sample_y[i];
			xb=game->road.track_right.sample_x[i+1];
			yb=game->road.track_right.sample_y[i+1];
		}
	
		
		if(point_in_rectangle(min(xa,xb)-game->cars[voiture].radius, min(ya,yb)-game->cars[voiture].radius, max(xa,xb)+game->cars[voiture].radius, max(ya,yb)+game->cars[voiture].radius, game->cars[voiture].x, game->cars[voiture].y)==TRUE){
			//x_gen = min(xa,xb)-game->cars[0].radius;
			//y_gen = min(ya,yb)-game->cars[0].radius;
			//x_genb = (max(xa,xb)+game->cars[0].radius) - x_gen;
			//y_genb = (max(ya,yb)+game->cars[0].radius) - y_gen;
			collision=1;
			if(first_point==-1){
				x_gen=xa;
				y_gen=ya;
				first_point=0;
			}
			x_genb=xb;
			y_genb=yb;
			if(rapproche_segment(game->cars[voiture].x, game->cars[voiture].y,xa,ya,xb, yb, game->cars[voiture].angle)==TRUE){
				if(compute_dist_point_to_line (xa, ya, xb, yb,game->cars[voiture].x, game->cars[voiture].y)<=game->cars[voiture].radius){
					//proche=1;
					//if_collision(game);
				}
			}
		}
		
	}
	
	if(collision==1){
		return 1;
	}else{
		return 0;
	}
}

int test_collide_left (Game *game, int voiture){
	int i,collision=0;;
	double xa,ya,xb,yb;
	int first_point=-1;
	
	for(i=game->road.track_left.sample_count-1;i>=0;i--){

		if(i+1==game->road.track_left.sample_count){
			xa=game->road.track_left.sample_x[i-1];
			ya=game->road.track_left.sample_y[i-1];
			xb=game->road.track_left.sample_x[i];
			yb=game->road.track_left.sample_y[i];
		}else{
			xa=game->road.track_left.sample_x[i];
			ya=game->road.track_left.sample_y[i];
			xb=game->road.track_left.sample_x[i+1];
			yb=game->road.track_left.sample_y[i+1];
		}
		if(point_in_rectangle(min(xa,xb)-game->cars[voiture].radius, min(ya,yb)-game->cars[voiture].radius, max(xa,xb)+game->cars[voiture].radius, max(ya,yb)+game->cars[voiture].radius, game->cars[voiture].x, game->cars[voiture].y)==TRUE){
				if(first_point==-1){
					xb_gen=xa;
					yb_gen=ya;
					first_point=0;
				}
				xb_genb=xb;
				yb_genb=yb;
				collision=1;
			if(rapproche_segment(game->cars[voiture].x, game->cars[voiture].y,xa,ya,xb, yb, game->cars[voiture].angle)==TRUE){
				if(compute_dist_point_to_line (xa, ya, xb, yb,game->cars[voiture].x, game->cars[voiture].y)<=game->cars[voiture].radius){
					proche_b=1;
					//if_collision(game);
				}
			}
		}
	}
	if(collision==1){
		return 1;
	}else{
		return 0;
	}
}

void if_collision(Game *game){
	game->cars[0].speed-=1;
	if(game->cars[0].speed<1)
	{
		game->cars[0].speed=1;
	}
	game->cars[0].accelerator=0;
}

int test_finish(Game *game, int voiture){
	double xa,ya,xb,yb;
	int final;
	final=game->road.track_left.sample_count-1;
	xa=game->road.track_left.sample_x[final];
	ya=game->road.track_left.sample_y[final];
	xb=game->road.track_right.sample_x[final];
	yb=game->road.track_right.sample_y[final];
	printf("v: %f\n", compute_dist_point_to_line (xa, ya, xb, yb,game->cars[voiture].x, game->cars[voiture].y));
	
	if(compute_dist_point_to_line (xa, ya, xb, yb,game->cars[voiture].x, game->cars[voiture].y)<1){
		//printf("arrivee\n");
		return 1;
	}
	
	return 0;
}
