build:
	gcc -DWNCK_I_KNOW_THIS_IS_UNSTABLE `pkg-config --cflags x11 libwnck-3.0` main.c `pkg-config --libs x11 libwnck-3.0`
