#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>

typedef int bool;
#define true 1
#define false 0

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
        bool shouldQuit = false;
        XNextEvent(dpy, &ev);
        switch(ev.type)
        {
            case KeyPress:
                printf("%s", "Hot key pressed!\n");
                XUngrabKey(dpy,keycode,modifiers,grab_window);
                shouldQuit = true;

            default:
                break;
        }

        if(shouldQuit)
            break;
    }

    XCloseDisplay(dpy);
    return 0;
}
