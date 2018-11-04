#include "winstate.h"

struct Rect Rect_init() {
  struct Rect ret;
  ret.xp = 0;
  ret.yp = 0;
  ret.widthp = 0;
  ret.heightp = 0;
}

struct WinState tiling_init() {
  struct WinState state;
  state.initial_geometries = g_hash_table_new(g_int64_hash, g_int64_equal);
  state.ctiled_geometries = g_hash_table_new(g_int64_hash, g_int64_equal);
  state.columns = 3;
  state.rows = 2;
  GHashTable* htable = g_hash_table_new(g_int_hash, g_int_equal);
  int a = 42;
  int b = 3;
  int c = 42;
  g_hash_table_insert(htable, &a, &b);

  int* result = g_hash_table_lookup(htable, &c);
  //g_print("%i\n", *(int*)(g_hash_table_lookup(htable, &c)));
  return state;
}
