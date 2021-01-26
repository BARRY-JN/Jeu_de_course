#ifndef GUI
#define GUI

void export_track_to_file(Game game, char const *filename);
void play_init(gpointer user_data);
void editing_init(gpointer user_data);
void window_init(GtkApplication* app, gpointer user_data);
void layout_init(gpointer user_data);
void status_init(gpointer user_data);
void win_scale_init (gpointer user_data);

#endif