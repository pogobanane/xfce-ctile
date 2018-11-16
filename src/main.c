#include <libwnck/libwnck.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

#include "winstate.h"
#include "wnckhandler.h"
#include "keybinds.c"

#define WNCK_WINDOW_CHANGE_EVERYTHING (WNCK_WINDOW_CHANGE_X | WNCK_WINDOW_CHANGE_Y | WNCK_WINDOW_CHANGE_WIDTH | WNCK_WINDOW_CHANGE_HEIGHT)


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

int main (int argc, char **argv)
{
  struct XHandle handle = xhandle_init_hotkeys();
  struct WinState win_state = tiling_init();

  while(true) {
    g_print("%s\n", "waiting for strg shift y");
    unsigned int keycode = xhandle_wait_event(handle);
    if (keycode == XKeysymToKeycode(handle.dpy,XK_Left)) {
      g_print("left\n");
    } else
    if (keycode == XKeysymToKeycode(handle.dpy,XK_Up)) {
      g_print("up\n");
    } else
    if (keycode == XKeysymToKeycode(handle.dpy,XK_Right)) {
      g_print("right\n");
    } else
    if (keycode == XKeysymToKeycode(handle.dpy,XK_Down)) {
      g_print("down\n");
    }
    //move_active_window(argc, argv);
    do_tiling(&win_state);
  }

  return 0;
}
