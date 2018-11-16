#include <libwnck/libwnck.h>
#include <stdlib.h>
#include <string.h>
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

static void save_user_defined_geometry(struct WinState* state, WnckScreen* screen) {
  WnckWindow* active;
  u_int64_t xid;
  struct Rect geometry;
  active = wnck_screen_get_active_window(screen);
  xid = wnck_window_get_xid(active);
  wnck_window_get_geometry(active,
  &geometry.xp, &geometry.yp, &geometry.widthp,
  &geometry.heightp);

  //g_print("%i x %i\n", wnck_screen_get_width(screen), wnck_screen_get_height(screen));

  // save user defined "initial" geometry
  // did ctile move this window already?
  gpointer initial_geometry = g_hash_table_lookup(state->initial_geometries, &xid);
  if(initial_geometry == NULL) {
    g_print("new window found: xid %i\n", xid);
    // insert geometry
    struct Rect* value = malloc(sizeof(struct Rect));
    memcpy(value, &geometry, sizeof(struct Rect));
    //u_int64_t* key = malloc(sizeof(u_int64_t));
    //*key = xid;
    g_hash_table_insert(state->initial_geometries, &xid, value);
  } else {
    struct Rect* ctiled_geometry = (struct Rect*)g_hash_table_lookup(state->ctiled_geometries, &xid);
    if(ctiled_geometry != NULL) {
      if(0 != memcmp(ctiled_geometry, &geometry, sizeof(struct Rect))) {
        // ctiled_geometry != active window's geometry
        // => geometry has been changed by the user since ctile
        // changed it last: save user's geometry
        g_print("ctiled geometry was: %i, %i, %i, %i\n", ctiled_geometry->xp, ctiled_geometry->yp, ctiled_geometry->widthp, ctiled_geometry->heightp);
        g_print("Detected new user geometry %i, %i, %i, %i\n", geometry.xp, geometry.yp, geometry.widthp, geometry.heightp);
        struct Rect* value = malloc(sizeof(struct Rect));
        memcpy(value, &geometry, sizeof(struct Rect));
        g_hash_table_insert(state->initial_geometries, &xid, value);
      }
    }
  }
}

static void save_ctile_defined_geometry(struct WinState* state, WnckScreen* screen) {
  WnckWindow* active = wnck_screen_get_active_window(screen);
  u_int64_t xid = wnck_window_get_xid(active);
  u_int64_t* xidp = malloc(sizeof(u_int64_t));
  *xidp = xid;
  struct Rect* i = malloc(sizeof(struct Rect));
  wnck_window_get_geometry(active,
  &i->xp, &i->yp, &i->widthp,
  &i->heightp);
  g_print("window actually is: %i %i %i %i\n", i->xp, i->yp, i->widthp, i->heightp);
  // add outcome geometry of tiling to WinState state
  g_hash_table_insert(state->ctiled_geometries, xidp, i);
}

/*
takes care of handling wnck and saving window positions done
by the user instead of ctile
*/
void do_tiling(struct WinState* win_state) {
  WnckScreen* screen = open_wnck();

  save_user_defined_geometry(win_state, screen);
  tile_right(win_state, screen);

  // check actual window bounds (window might alter our instructions)
  close_wnck();
  screen = open_wnck();

  save_ctile_defined_geometry(win_state, screen);

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
