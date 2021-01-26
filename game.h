#ifndef GAME
#define GAME

#define PI 3.14159265358979323846
#define TAILLE_CHEMIN 2000
#define SAMPLE_MAX 10000
#define CAR_MAX 3

typedef enum { GS_HELLO, GS_EDIT, GS_PLAYING, GS_PAUSE, GS_WON, GS_LOST} Game_state;
typedef enum { GM_NONE, GM_SINGLE, GM_MULTI, GM_SERVER, GM_CLIENT} Game_mode;

typedef struct {
    int sample_count;
    double sample_x[SAMPLE_MAX], sample_y[SAMPLE_MAX];
} Track;

typedef struct {
    Curve curve_right, curve_central, curve_left;
    Track track_right, track_central, track_left;
} Road;

typedef struct {
    double x, y, radius;
    double angle, speed, max_speed, accelerator,max_accelerator;
    int direction, avance;
    //GdkPixbuf *pixbuf1;
} Car;

typedef struct {
    Game_state state;
    Game_mode mode;
    int score;
    Car cars[CAR_MAX];
    int car_count;
    Road road;     // par la suite un tableau par niveau
    int countdown;
} Game;

extern int proche,proche_b,x_gen,y_gen,x_genb,y_genb,xb_gen,yb_gen,xb_genb,yb_genb;

void rotate_cars (Game *game, int num_car);
double max(double a, double b);
double min(double a, double b);
int point_in_rectangle(double x_rec,double y_rec,double x_rec_max,double y_rec_max,double x_point, double y_point);
int rapproche_segment(double xc, double yc,double xa, double ya,double xb, double yb, double alpha);
void place_cars_on_start (Game *game);
Car init_car(double x, double y, double max, double acc);
void progress_game_next_step (Game *game);
void init_game(Game *game);
void init_road(Game *game, Curve *curve);
void compute_bezier_track (Curve *curve, Track *tra, double theta);
void compute_road_tracks (Road *road, double theta);
void draw_edit_track (cairo_t *cr, Track *l_track,Track *c_track, Track *r_track);
void draw_track (cairo_t *cr, Track *l_track,Track *c_track, Track *r_track);
void compute_normal_right (double xa, double ya, double xb, double yb, double *xn, double *yn);
void do_vector_product (double xa, double ya, double za,double xb, double yb, double zb,double *xn, double *yn, double *zn);
int compute_intersection (double xs1, double ys1, double xs2, double ys2,double xt1, double yt1, double xt2, double yt2,double *xp, double *yp);
double get_horiz_angle_rad (double xa, double ya, double xb, double yb);
int point_is_on_right (double xa, double ya, double xb, double yb, double xc, double yc);
double compute_dist_point_to_line (double xa, double ya, double xb, double yb,double xc, double yc);
double calculer_angle_reflechi(double xa, double ya, double xb, double yb, double angle);

#endif
