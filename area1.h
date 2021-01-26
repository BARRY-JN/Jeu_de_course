#ifndef AREA1
#define AREA1

#define ANIM1_TIME 20

void draw_car (cairo_t *cr, Mydata *my);
void draw_pause (cairo_t *cr, Mydata *my);
void copy_curve_infos_to_road (Curve *curve, Game *game);
void set_anim1_mode (Mydata *my, int flag);
gboolean on_timeout1 (gpointer data);
void generate_bezier_path (cairo_t *cr, Control bez_points[4], double theta, int is_first);
void draw_control_labels (cairo_t *cr, PangoLayout *layout, Curve_infos *ci);
void apply_image_transforms (Mydata *my);
void update_area1_with_transforms (Mydata *my);
void draw_bezier_curves_open (cairo_t *cr, Curve_infos *ci, double theta);
void draw_bezier_curves_close (cairo_t *cr, Curve_infos *ci, double theta);
void draw_bezier_curves_fill (cairo_t *cr, Curve_infos *ci, double theta);
void draw_bezier_curves_clip (cairo_t *cr, Curve_infos *ci, double theta, Mydata *my);
void draw_bezier_curves_prolong (cairo_t *cr, Curve_infos *ci, double theta);
void draw_bezier_polygons_open (cairo_t *cr, Curve_infos *ci);
void draw_control_polygons (cairo_t *cr, Curve_infos *ci);
void area1_init(gpointer user_data);

#endif
