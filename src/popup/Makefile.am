INCLUDES = -I$(top_srcdir)/include
LDFLAGS = -L$(top_srcdir)/lib -L../../lib 
AM_CPPFLAGS = -Wall $(GTK_CFLAGS)
#
bin_PROGRAMS = cid-popup
cid_popup_LDADD = -lcid_modem -lcid_util -lpthread $(GTK_LIBS)
cid_popup_SOURCES = \
        main.C 
