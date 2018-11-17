#include <libwnck/libwnck.h>

#include "winstate.h"

// remember to update tiling_state_destroy, if you update this!
struct TilingState {
  int dimension_cycle; // represents the tiling_cycle_dimensions state
};

struct TilingState* tiling_state_new();

void tiling_state_destroy(void* ptr);

void tile_right(struct WinState* state, WnckScreen* screen);

void tile_left(struct WinState* state, WnckScreen* screen);
