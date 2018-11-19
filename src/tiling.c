#include "farey_sequence.c"

#include <glib.h>
#include <string.h>
#include <libwnck/libwnck.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <stdlib.h>

#include "tiling.h"

#define WNCK_WINDOW_CHANGE_EVERYTHING (WNCK_WINDOW_CHANGE_X | WNCK_WINDOW_CHANGE_Y | WNCK_WINDOW_CHANGE_WIDTH | WNCK_WINDOW_CHANGE_HEIGHT)

struct TilingState* tiling_state_new() {
  struct TilingState* ts = malloc(sizeof(struct TilingState));
  memset(ts, 0, sizeof(struct TilingState));
  ts->smartx = 1;
  ts->smarty = 1;
  return ts;
}

void tiling_state_destroy(void* ptr) {
  struct TilingState* s = (struct TilingState*) ptr;
  free(s);
}

struct Strut {
  int top, bot, left, right;
};

// looks for the maximum "docking height" of all children of this window
static struct Strut max_strut(Display* display, Window window)
{
    // from https://stackoverflow.com/a/4019312
    struct Strut ret;
    ret.top = 0;
    ret.bot = 0;
    ret.left = 0;
    ret.right = 0;
    Window w;
    Window* children;
    unsigned int n_children;

    XQueryTree(display, window, &w, &w, &children, &n_children);

    // looks for each one of the children
    int i;
    for(i=0; i<n_children; i++)
    {
        // this is the property we're looking for
        Atom strut = XInternAtom(display, "_NET_WM_STRUT_PARTIAL",
                False);
        Atom type_return;
        int actual_type;
        unsigned long nitems, bytes;
        unsigned char* data = NULL;

        // load window attributes (we only want to know about the
        //                         windows where y = 0)
        XWindowAttributes xwa;
        XGetWindowAttributes(display, window, &xwa);

        // load the property _NET_WM_STRUT_PARTIAL:
        // [0]:left, [1]:right, [2]:top, [3]:bottom, [...]:left_start_y, left_end_y,
        // right_start_y, right_end_y, top_start_x, top_end_x, bottom_start_x,
        // bottom_end_x,CARDINAL[12]/32
        int s = XGetWindowProperty(display, window, strut, 0, LONG_MAX,
                False,
                XA_CARDINAL, &type_return, &actual_type,
                &nitems, &bytes, (unsigned char**)&data);
        if(s == Success)
        {
            Atom *state = (Atom *) data;
            if(nitems > 0) {
              if(state[0] > ret.left) {
                ret.left = state[0];
              }
              if(state[1] > ret.right) {
                ret.right = state[1];
              }
              if(state[2] > ret.top) {
                ret.top = state[2];
              }
              if(state[3] > ret.bot) {
                ret.bot = state[3];
              }
            }
        }

        // recursively, traverse the tree of all children of children
        struct Strut children_strut = max_strut(display, children[i]);
        if(children_strut.left > ret.left) {
           ret.left = children_strut.left;
        }
        if(children_strut.right > ret.right) {
          ret.right = children_strut.right;
        }
        if(children_strut.top > ret.top) {
          ret.top = children_strut.top;
        }
        if(children_strut.bot > ret.bot) {
          ret.bot = children_strut.bot;
        }
    }

    return ret;
}

/* returns Rect of usable part of screen (which is not used up by taskbars etc.)
*/
static struct Rect compute_usable(WnckScreen* screen) {
      Display* display = XOpenDisplay(NULL);
      Window window = RootWindow(display, DefaultScreen(display));
      struct Strut strut = max_strut(display, window);
      //g_print("%i, %i, %i, %i\n", strut.top, strut.bot, strut.left, strut.right);
      struct Rect usable;
      usable.xp = 0 + strut.left; // screen.left_side + ...
      usable.yp = 0 + strut.top; // screen.top_side + ...
      usable.widthp = wnck_screen_get_width(screen) - strut.left - strut.right;
      usable.heightp = wnck_screen_get_height(screen) - strut.top - strut.bot;
      //g_print("%i, %i, %i, %i\n", usable.xp, usable.yp, usable.widthp, usable.heightp);
      return usable;
}

void tile_left(struct WinState* state, WnckScreen* screen) {
  WnckWindow* active = wnck_screen_get_active_window(screen);
  struct Rect final_tiled_geometry;
  struct Rect usable = compute_usable(screen);
  final_tiled_geometry.xp = usable.xp;
  final_tiled_geometry.yp = usable.yp;
  final_tiled_geometry.widthp = usable.widthp / 2;
  final_tiled_geometry.heightp = usable.heightp;
  wnck_window_set_geometry(active, WNCK_WINDOW_GRAVITY_SOUTH,
    WNCK_WINDOW_CHANGE_EVERYTHING,
    final_tiled_geometry.xp, final_tiled_geometry.yp,
    final_tiled_geometry.widthp, final_tiled_geometry.heightp);
}

void tile_right(struct WinState* state, WnckScreen* screen) {
  WnckWindow* active = wnck_screen_get_active_window(screen);
  struct Rect final_tiled_geometry;
  struct Rect usable = compute_usable(screen);
  final_tiled_geometry.xp = usable.widthp / 2;
  final_tiled_geometry.yp = usable.yp;
  final_tiled_geometry.widthp = usable.widthp / 2;
  final_tiled_geometry.heightp = usable.heightp;
  wnck_window_set_geometry(active, WNCK_WINDOW_GRAVITY_SOUTH,
    WNCK_WINDOW_CHANGE_EVERYTHING,
    final_tiled_geometry.xp, final_tiled_geometry.yp,
    final_tiled_geometry.widthp, final_tiled_geometry.heightp);
}

void tiling_right_cycle_width(struct WinState* wstate, WnckScreen* screen) {
  // get important values
  WnckWindow* active = wnck_screen_get_active_window(screen);
  u_int64_t xid = wnck_window_get_xid(active);
  struct TilingState* tstate = (struct TilingState*) g_hash_table_lookup(wstate->tiling_states, &xid);
  if(tstate == NULL) {
    g_print("ERROR: wnckhandler didn't create a tiling state for this window");
    return;
  }
  struct Rect usable = compute_usable(screen);
  // compute new window geometry
  struct Rect final_tiled_geometry;

  final_tiled_geometry.widthp = usable.widthp * farey_indexed(wstate->columns, tstate->dimension_cycle);
  final_tiled_geometry.xp = usable.widthp - final_tiled_geometry.widthp + usable.xp;
  final_tiled_geometry.yp = usable.yp;
  final_tiled_geometry.heightp = usable.heightp;

  tstate->dimension_cycle = (tstate->dimension_cycle + 1) % wstate->columns;
  // set geometry
  wnck_window_set_geometry(active, WNCK_WINDOW_GRAVITY_SOUTH,
    WNCK_WINDOW_CHANGE_EVERYTHING,
    final_tiled_geometry.xp, final_tiled_geometry.yp,
    final_tiled_geometry.widthp, final_tiled_geometry.heightp);
}

void tiling_left_cycle_width(struct WinState* wstate, WnckScreen* screen) {
  // get important values
  WnckWindow* active = wnck_screen_get_active_window(screen);
  u_int64_t xid = wnck_window_get_xid(active);
  struct TilingState* tstate = (struct TilingState*) g_hash_table_lookup(wstate->tiling_states, &xid);
  if(tstate == NULL) {
    g_print("ERROR: wnckhandler didn't create a tiling state for this window");
    return;
  }
  struct Rect usable = compute_usable(screen);
  // compute new window geometry
  struct Rect final_tiled_geometry;

  final_tiled_geometry.widthp = usable.widthp * farey_indexed(wstate->columns, tstate->dimension_cycle);
  final_tiled_geometry.xp = usable.xp;
  final_tiled_geometry.yp = usable.yp;
  final_tiled_geometry.heightp = usable.heightp;

  tstate->dimension_cycle = (tstate->dimension_cycle + 1) % wstate->columns;
  // set geometry
  wnck_window_set_geometry(active, WNCK_WINDOW_GRAVITY_SOUTH,
    WNCK_WINDOW_CHANGE_EVERYTHING,
    final_tiled_geometry.xp, final_tiled_geometry.yp,
    final_tiled_geometry.widthp, final_tiled_geometry.heightp);
}

void tiling_reset(struct WinState* wstate, WnckScreen* screen) {
}

// returns NULL on error
static struct TilingState* tiling_get_tstate(struct WinState* wstate, WnckScreen* screen) {
  // get important values
  WnckWindow* active = wnck_screen_get_active_window(screen);
  u_int64_t xid = wnck_window_get_xid(active);
  struct TilingState* tstate = (struct TilingState*) g_hash_table_lookup(wstate->tiling_states, &xid);
  return tstate;
}

static void tiling_smart(struct WinState* wstate, WnckScreen* screen, int deltax, int deltay) {
  struct TilingState* tstate = tiling_get_tstate(wstate, screen);
  if(tstate == NULL) {
    g_print("ERROR: wnckhandler didn't create a tiling state for this window");
    return;
  }

  typedef void (*tfunc)(struct WinState*, WnckScreen*);
  tfunc functions[3][3] = 
    {{tiling_reset, tiling_reset, tiling_reset},
    {tiling_left_cycle_width, tiling_reset, tiling_right_cycle_width},
    {tiling_reset, tiling_reset, tiling_reset}};

  int x = tstate->smartx + deltax;
  int y = tstate->smarty + deltay;
  if (0 <= x && x < wstate->columns && 0 <= y && y < 3) {
    tstate->smartx = x;
    tstate->smarty = y;
  }
  functions[tstate->smarty][tstate->smartx](wstate, screen);

}

void tiling_smart_left(struct WinState* wstate, WnckScreen* screen) {
  tiling_smart(wstate, screen, -1, 0);
}

void tiling_smart_top(struct WinState* wstate, WnckScreen* screen) {
  tiling_smart(wstate, screen, 0, -1);
}

void tiling_smart_right(struct WinState* wstate, WnckScreen* screen) {
  tiling_smart(wstate, screen, 1, 0);
}

void tiling_smart_bot(struct WinState* wstate, WnckScreen* screen) {
  tiling_smart(wstate, screen, 0, 1);
}
