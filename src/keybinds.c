#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>

typedef int bool;
#define true 1
#define false 0

struct XHandle {
  Display* dpy;
  Window root;
  XEvent ev;
  unsigned int modifiers;
  int kc1, kc2, kc3, kc4; // keycodes
  Window grab_window;
  Bool owner_events;
  int pointer_mode;
  int keyboard_mode;
};

struct XHandle xhandle_init_hotkeys() {
  struct XHandle handle;
  handle.dpy = XOpenDisplay(0);
  handle.root = DefaultRootWindow(handle.dpy);
  handle.modifiers = ControlMask | ShiftMask;
  handle.kc1 = XKeysymToKeycode(handle.dpy,XK_Left);
  handle.kc2 = XKeysymToKeycode(handle.dpy,XK_Up);
  handle.kc3 = XKeysymToKeycode(handle.dpy,XK_Right);
  handle.kc4 = XKeysymToKeycode(handle.dpy,XK_Down);
  handle.grab_window = handle.root;
  handle.owner_events = False;
  handle.pointer_mode = GrabModeAsync;
  handle.keyboard_mode = GrabModeAsync;
  XGrabKey(handle.dpy, handle.kc1, handle.modifiers, handle.grab_window, handle.owner_events, handle.pointer_mode,
           handle.keyboard_mode);
  XGrabKey(handle.dpy, handle.kc2, handle.modifiers, handle.grab_window, handle.owner_events, handle.pointer_mode,
          handle.keyboard_mode);
  XGrabKey(handle.dpy, handle.kc3, handle.modifiers, handle.grab_window, handle.owner_events, handle.pointer_mode,
           handle.keyboard_mode);
  XGrabKey(handle.dpy, handle.kc4, handle.modifiers, handle.grab_window, handle.owner_events, handle.pointer_mode,
          handle.keyboard_mode);
  XSelectInput(handle.dpy, handle.root, KeyPressMask );
  return handle;
}

// returns a keycode. Note: XKeysymToKeycode(handle.dpy,XK_Left) with keysyms
// from X11/keysymdef.h.
unsigned int xhandle_wait_event(struct XHandle handle) {
  while (true) {
    XNextEvent(handle.dpy, &(handle.ev));
    switch(handle.ev.type)
    {
        case KeyPress:
            printf("Hot key pressed! Keycode: %x\n", handle.ev.xkey.keycode);
            return handle.ev.xkey.keycode;
        default:
            break;
    }
  }
}

void xhandle_close(struct XHandle handle) {
  XUngrabKey(handle.dpy,handle.kc1,handle.modifiers,handle.grab_window);
  XUngrabKey(handle.dpy,handle.kc2,handle.modifiers,handle.grab_window);
  XUngrabKey(handle.dpy,handle.kc3,handle.modifiers,handle.grab_window);
  XUngrabKey(handle.dpy,handle.kc4,handle.modifiers,handle.grab_window);
  XCloseDisplay(handle.dpy);
}

int init_keys()
{
    Display*    dpy     = XOpenDisplay(0);
    Window      root    = DefaultRootWindow(dpy);
    XEvent      ev;

    unsigned int    modifiers       = ControlMask | ShiftMask;
    int             keycode         = XKeysymToKeycode(dpy,XK_Y); // strg shift y
    Window          grab_window     =  root;
    Bool            owner_events    = False;
    int             pointer_mode    = GrabModeAsync;
    int             keyboard_mode   = GrabModeAsync;

    XGrabKey(dpy, keycode, modifiers, grab_window, owner_events, pointer_mode,
             keyboard_mode);

    XSelectInput(dpy, root, KeyPressMask );

    while(true)
    {
        XNextEvent(dpy, &ev);
        switch(ev.type)
        {
            case KeyPress:
                printf("%s", "Hot key pressed!\n");

            default:
                break;
        }
    }

    XUngrabKey(dpy,keycode,modifiers,grab_window);
    XCloseDisplay(dpy);
    return 0;
}
