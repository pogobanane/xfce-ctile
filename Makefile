build:
	gcc -DWNCK_I_KNOW_THIS_IS_UNSTABLE `pkg-config --cflags libwnck-3.0` main.c `pkg-config --libs libwnck-3.0`
