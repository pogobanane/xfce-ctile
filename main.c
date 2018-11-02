#include <libwnck/libwnck.h>

#define WNCK_WINDOW_CHANGE_EVERYTHING (WNCK_WINDOW_CHANGE_X | WNCK_WINDOW_CHANGE_Y | WNCK_WINDOW_CHANGE_WIDTH | WNCK_WINDOW_CHANGE_HEIGHT)

#include "keybinds.c"

void print_windows(WnckScreen* screen) {
  WnckWindow* active_window;
  GList *window_l;

  active_window = wnck_screen_get_active_window(screen);
  for (window_l = wnck_screen_get_windows(screen); window_l != NULL; window_l = window_l->next)
    {
      WnckWindow *window = WNCK_WINDOW(window_l->data);
      g_print ("%s%s\n", wnck_window_get_name(window),
              window == active_window ? " (active)" : "");
    }
}

struct WnckScreen *open_wnck(int argc, char **argv) {
  WnckScreen* screen;

  gdk_init(&argc, &argv);
  screen = wnck_screen_get_default();
  wnck_screen_force_update(screen);
}

void close_wnck() {
  wnck_shutdown();
}

void move_active_window(int argc, char **argv) {
  WnckScreen* screen;
  WnckWindow *active_window;

  screen = open_wnck(argc, argv);
  
  active_window = wnck_screen_get_active_window(screen);
  g_print ("%s\n", wnck_window_get_name(active_window));

  //print_windows(screen);

  int xp, yp, widthp, heightp;
  wnck_window_get_geometry(active_window, &xp, &yp, &widthp, &heightp);
  g_print("%i, %i, %i, %i\n", xp, yp, widthp, heightp);
  wnck_window_set_geometry(active_window, WNCK_WINDOW_GRAVITY_SOUTH, WNCK_WINDOW_CHANGE_EVERYTHING, xp, yp+50, widthp, heightp);

  close_wnck();
}

int main (int argc, char **argv)
{
  struct XHandle handle = xhandle_init_hotkeys();

  while(true) {
    xhandle_wait_event(handle);
    move_active_window(argc, argv);

    g_print("%s\n", "waiting for strg shift y");
  }

  return 0;
}
