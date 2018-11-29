#include <gdk/gdk.h>
#include <stdlib.h>
#include <string.h>
#include "tiling.h"

#define WNCK_WINDOW_CHANGE_EVERYTHING (WNCK_WINDOW_CHANGE_X | WNCK_WINDOW_CHANGE_Y | WNCK_WINDOW_CHANGE_WIDTH | WNCK_WINDOW_CHANGE_HEIGHT)

/* returns WnckScreen*
*/
static WnckScreen* open_wnck() {
  g_print("sasdfadfasdasdadsdasdasdasd\n");
  WnckScreen* screen;
  gboolean succ = gdk_init_check(NULL, NULL); // int argc, char **argv
  screen = wnck_screen_get_default();
  wnck_screen_force_update(screen);

  GdkScreen* gdkscreen = gdk_screen_get_default();

  if (!succ) {
    g_print("ERROR: could not init gdk");
  }
  GdkDisplayManager* dm = gdk_display_manager_get();
  GSList* display1;
  display1 = gdk_display_manager_list_displays(dm);
  while(1) {
  	if(display1 == NULL) {
  		compute_usable(screen);
  		break;
  	}
	GdkDisplay* display = (GdkDisplay*) display1->data;
	display1 = display1->next;
	int n_monitors = gdk_display_get_n_monitors(display);
	g_print("%i monitors\n", n_monitors);
	int i;
	for(i=0; i<n_monitors; i++) {
		GdkMonitor* monitor = gdk_display_get_monitor(display, i);	
		GdkRectangle rectangle;
		gdk_monitor_get_geometry(monitor, &rectangle);
		g_print("gdk rect: %i, %i, %i, %i\n",rectangle.x, rectangle.y, rectangle.width, rectangle.height);
		GdkRectangle workspace;
		gdk_monitor_get_workarea(monitor, &workspace);
		g_print("gdk strutted rect: %i, %i, %i, %i\n",workspace.x, workspace.y, workspace.width, workspace.height);
	}
  }

  GdkWindow* active = gdk_screen_get_active_window(gdkscreen);
  struct Rect usable;
  gdk_window_get_geometry(active, &usable.xp, &usable.yp, 
  	&usable.widthp, &usable.heightp);
  u_int64_t xid = gdk_x11_window_get_xid(active);
  g_print("active window %i: %i, %i, %i, %i\n", xid, usable.xp, usable.yp, usable.widthp, usable.heightp);


  		exit(0);
  gdk_display_manager_get_default_display(dm);
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
    u_int64_t* key = malloc(sizeof(u_int64_t));
    *key = xid;
    g_hash_table_insert(state->initial_geometries, key, value);
    // insert empty tiling state
    struct TilingState* ts = tiling_state_new();
    key = malloc(sizeof(u_int64_t));
    *key = xid;
    g_hash_table_insert(state->tiling_states, key, ts);
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
        // no need to malloc xid, because xid already exists in table and
        // therefor won't get replaced
        u_int64_t* key = malloc(sizeof(u_int64_t));
        *key = xid;
        g_hash_table_insert(state->initial_geometries, key, value);
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
void do_tiling(struct WinState* win_state, void (*tile_somehow)(struct WinState*, WnckScreen*)) {
  WnckScreen* screen = open_wnck();

  save_user_defined_geometry(win_state, screen);
  tile_somehow(win_state, screen);

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
