

void tile_right(struct WinState state, WnckScreen* screen) {
  WnckWindow* active;
  active = wnck_screen_get_active_window(screen);

  g_print("%i x %i\n", wnck_screen_get_width(screen), wnck_screen_get_height(screen));

}

struct Strut {
  int top, bot, left, right;
};

// looks for the maximum "docking height" of all children of this window
struct Strut max_strut(Display* display, Window window)
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
struct Rect compute_usable(WnckScreen* screen) {
      Display* display = XOpenDisplay(NULL);
      Window window = RootWindow(display, DefaultScreen(display));
      struct Strut strut = max_strut(display, window);
      g_print("%i, %i, %i, %i\n", strut.top, strut.bot, strut.left, strut.right);
      struct Rect usable;
      usable.xp = 0 + strut.left; // screen.left_side + ...
      usable.yp = 0 + strut.top; // screen.top_side + ...
      usable.widthp = wnck_screen_get_width(screen) - strut.left - strut.right;
      usable.heightp = wnck_screen_get_height(screen) - strut.top - strut.bot;
      g_print("%i, %i, %i, %i\n", usable.xp, usable.yp, usable.widthp, usable.heightp);
      return usable;
}
