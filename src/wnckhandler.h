
void *open_wnck(int argc, char **argv);

void close_wnck();

void do_tiling(struct WinState* win_state, void (*tile_somehow)(struct WinState*, WnckScreen*));
