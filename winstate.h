#include <glib.h>

struct Rect {
  int xp, yp, widthp, heightp;
};


struct WinState {
  int columns, rows;
  struct GHashTable* windows;
  struct Rect initial_geometry;
  /*
  (x_state, y_state) represents the coordinates in this
  matrix:
  smarttiles = [
        ['top-left', 'top', 'top-right'],
        ['left', 'reset', 'right'],
        ['bottom-left', 'bottom', 'bottom-right']
    ]
  */
  int x_state, y_state;
};

struct Rect Rect_init();

struct WinState tiling_init();
