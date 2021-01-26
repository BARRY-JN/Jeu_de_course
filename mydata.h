#ifndef MYDATA
#define MYDATA

#define MYDATA_MAGIC 0x46EA7E05
enum mode {EDIT_NONE,EDIT_ADD_CURVE,EDIT_MOVE_CURVE,EDIT_REMOVE_CURVE,EDIT_ADD_CONTROL,EDIT_MOVE_CONTROL,EDIT_REMOVE_CONTROL,EDIT_MOVE_CLIP,EDIT_RESET_CLIP,EDIT_LAST};

typedef struct {
	unsigned int magic;
	GtkWidget *window;
	//GtkWidget *image1;
	GtkWidget *area1;
	GtkWidget *status;
	GtkWidget *win_scale;
	GtkWidget *win_edit;
	GtkWidget *win_play;
	GtkWidget *vbox1;
	GtkWidget *vbox2;
	GtkWidget *hbox1;
	GtkWidget *scale1;
	GtkWidget *menu_bar;
	GtkWidget *scroll;
	GtkWidget *track_name_label;
	GtkWidget *nbr_joueur;
	GtkWidget *edit_radios[EDIT_LAST];
	GtkWidget *game_radios[GS_LOST+1];
	GdkPixbuf *pixbuf1;
	GdkPixbuf *pixbuf2;
	Curve_infos curve_infos;
	char *title;
	char *current_folder;
	char *track_name;
	int win_width;
	int win_height;
	int click_x;
	int click_y;
	int click_n;
	int clip_image;
	int show_edit;
	int flag_key_left[CAR_MAX], flag_key_right[CAR_MAX], flag_key_down[CAR_MAX],flag_key_up[CAR_MAX];
	int view_rendered;
	double rotate_angle;
	double scale1_value;
	double last_x, last_y;
	int edit_mode;
	Game game;
	guint timeout1;
} Mydata;

Mydata *get_mydata (gpointer data);
void init_mydata (Mydata *my);
void set_edit_mode (Mydata *my, int mode);
#endif
