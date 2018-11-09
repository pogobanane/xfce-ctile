#include <libwnck/libwnck.h>
#include "tiling.h"

#define WNCK_WINDOW_CHANGE_EVERYTHING (WNCK_WINDOW_CHANGE_X | WNCK_WINDOW_CHANGE_Y | WNCK_WINDOW_CHANGE_WIDTH | WNCK_WINDOW_CHANGE_HEIGHT)

/* returns WnckScreen*
*/
static WnckScreen* open_wnck() {
  WnckScreen* screen;
  gboolean succ = gdk_init_check(NULL, NULL); // int argc, char **argv
  if (!succ) {
    g_print("ERROR: could not init gdk");
  }
  screen = wnck_screen_get_default();
  wnck_screen_force_update(screen);

  return screen;
}

static void close_wnck() {
  wnck_shutdown();
}

/*
takes care of handling wnck and saving window positions done
by the user instead of ctile
*/
void do_tiling(struct WinState* win_state) {
  WnckScreen* screen = open_wnck();
  tile_right(win_state, screen);
  // tests
  close_wnck();
  screen = open_wnck();
  WnckWindow* active = wnck_screen_get_active_window(screen);
  struct Rect i;
  wnck_window_get_geometry(active,
  &i.xp, &i.yp, &i.widthp,
  &i.heightp);
  g_print("window actually is: %i %i %i %i\n", i.xp, i.yp, i.widthp, i.heightp);

  close_wnck();
}

/* this is a demo only */
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
