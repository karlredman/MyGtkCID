INCLUDES = -I$(top_srcdir)/include
LDFLAGS = -L$(top_srcdir)/lib -L../../lib 
AM_CPPFLAGS = -Wall
#
bin_PROGRAMS = cid-console
cid_console_LDADD = -lcid_modem -lcid_util -lpthread
cid_console_SOURCES = \
        main.C 
