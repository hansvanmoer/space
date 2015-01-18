#
# Main application makefile
#

bin_PROGRAMS=space
space_SOURCES=Path.cpp Settings.cpp main.cpp
space_CPPFLAGS= -DNO_THROW='throw()' -std=c++11 -lsfml-window -I../core -I../json
space_LDADD=$(top_srcdir)/src/core/libcore.a $(top_srcdir)/src/json/libjson.a
