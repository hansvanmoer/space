#
# Main application makefile
#

bin_PROGRAMS=space
space_SOURCES=IO.cpp Application.cpp Data.cpp Settings.cpp Window.cpp Module.cpp Script.cpp Graphics.cpp Feature.cpp Texture.cpp Orbit.cpp Star.cpp Session.cpp ResourceIndexer.cpp MapGenerator.cpp main.cpp
space_CPPFLAGS=-DRUNTIME_DATA_PATH -std=c++11 -I../core -I../json -I/usr/include/python3.4
space_LDADD=$(top_srcdir)/src/core/libcore.a $(top_srcdir)/src/json/libjson.a -lGL -lsfml-graphics -lsfml-window -lsfml-system -lboost_python-py34 -lpython3.4m
