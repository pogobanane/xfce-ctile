AC_INIT([ctile], [0.1])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
AC_CONFIG_FILES([
	Makefile
	src/Makefile
])
AC_SUBST(MY_PKG_LIBS)
AC_SUBST(MY_PKG_CFLAGS)
if pkg-config x11 libwnck-3.0; then
	MY_PKG_LIBS=`pkg-config --libs x11 libwnck-3.0`
	MY_PKG_CFLAGS=`pkg-config --cflags x11 libwnck-3.0`
else
	AC_MSG_ERROR(not all libs found: x11, libwnck-3.0)
fi
LIBWNCK_CFLAGS="-DWNCK_I_KNOW_THIS_IS_UNSTABLE"
AC_SUBST(LIBWNCK_CFLAGS)
AC_OUTPUT
