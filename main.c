#include <libwnck/libwnck.h>

#define WNCK_WINDOW_CHANGE_EVERYTHING (WNCK_WINDOW_CHANGE_X | WNCK_WINDOW_CHANGE_Y | WNCK_WINDOW_CHANGE_WIDTH | WNCK_WINDOW_CHANGE_HEIGHT)


void move_active_window(WnckScreen* screen) {
  WnckWindow *active_window;

  active_window = wnck_screen_get_active_window(screen);

  int xp, yp, widthp, heightp;
  wnck_window_get_geometry(active_window, &xp, &yp, &widthp, &heightp);
  g_print("%i, %i, %i, %i\n", xp, yp, widthp, heightp);

  wnck_window_set_geometry(active_window, WNCK_WINDOW_GRAVITY_SOUTH, WNCK_WINDOW_CHANGE_EVERYTHING, xp, yp+50, widthp, heightp);
  wnck_window_get_geometry(active_window, &xp, &yp, &widthp, &heightp);
  g_print("%i, %i, %i, %i\n", xp, yp, widthp, heightp);

}


int main (int argc, char **argv)
{
  WnckScreen *screen;
  GList *window_l;

  gdk_init(&argc, &argv);

  screen = wnck_screen_get_default();

  wnck_screen_force_update(screen);

  move_active_window(screen);

  for (window_l = wnck_screen_get_windows(screen); window_l != NULL; window_l = window_l->next)
    {
      WnckWindow *window = WNCK_WINDOW(window_l->data);
      g_print ("%s%s\n", wnck_window_get_name(window),
              /*window == active_window false ? " (active)" :*/ "");
    }

  return 0;
}
