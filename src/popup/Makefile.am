INCLUDES = -I$(top_srcdir)/include
CPPFLAGS = -g -Wall `gtk-config --cflags`
LDFLAGS = -L$(top_srcdir)/lib `gtk-config --libs` -lpthread
#
bin_PROGRAMS = cid-popup
cid_popup_LDADD = -lcid_modem -lcid_util
cid_popup_SOURCES = \
        main.C 
