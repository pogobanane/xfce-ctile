#include <glib.h>

struct Rect {
  int xp, yp, widthp, heightp;
};

/*
TODO: GHashTables are never cleaned up because we do
not recognize when windows are closed.
Proposal: scrape the list of windows to remove closed ones
from the Tables after a tiling if there was no scrape for
the last 10 minutes
*/
struct WinState {
  int columns,
  rows;
  GHashTable* initial_geometries; // last user defined geometry
  GHashTable* ctiled_geometries; // last geometry applied by ctile
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

void dump_hash_table(GHashTable* table);

struct WinState tiling_init();
