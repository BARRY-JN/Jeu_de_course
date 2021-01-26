#include <gtk/gtk.h>
#include "util.h"

void refresh_area (GtkWidget *area)
{
	GdkWindow *win = gtk_widget_get_window (area);
	if (win == NULL)
		return;  // widget pas encore realise
	gdk_window_invalidate_rect (
	win,
	NULL,   // tout le window
	FALSE); // pas de sous-window
}

void set_status (GtkWidget *status, const char *format, ...)
{
	char buf[1000];
	va_list ap;
	va_start (ap, format);
	vsnprintf (buf, sizeof(buf)-1, format, ap);
	va_end (ap);
	buf[sizeof(buf)-1] = 0;
	gtk_statusbar_pop  (GTK_STATUSBAR(status), 0);
	gtk_statusbar_push (GTK_STATUSBAR(status), 0, buf);
}