AC_INIT(src)
AM_INIT_AUTOMAKE(mycid,0.0.1)
AM_CONFIG_HEADER(include/config.h)
AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB
#AC_PROG_CPP
AC_STDC_HEADERS

##
echo "blah, blah, blah"
echo "this is a subliminal message...."
##
PKG_CHECK_MODULES(GTK, glib >= 1.2.10 gtk+ >= 1.2.10)
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)
#
AC_OUTPUT(Makefile include/Makefile lib/Makefile src/Makefile \
src/console/Makefile \
src/popup/Makefile \
src/console2/Makefile) 
##
##
##
echo ""
echo "--- Now run make ---"
echo ""
