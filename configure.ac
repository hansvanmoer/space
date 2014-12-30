#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([space], [0.1], [hansvm.projects@gmail.com])
AC_LANG(C++)
AC_CONFIG_SRCDIR([src/main/main.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/main/Makefile src/core/Makefile src/json/Makefile])

AM_INIT_AUTOMAKE
AC_PROG_RANLIB

# Checks for programs.
AC_PROG_CXX

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

# Check for SFML

# Check for SFML Window library

SAVED_LDFLAGS=$LDFLAGS
LDFLAGS="$LDFLAGS -lsfml-window"

AC_LINK_IFELSE(
    [AC_LANG_PROGRAM([#include <SFML/Window/VideoMode.hpp>],
    [sf::VideoMode::getDesktopMode();])],
    [AC_MSG_RESULT([checking for SFML window library... yes])],
    [AC_MSG_ERROR([No compatible version of the SFML Window library found.])])

LDFLAGS=$SAVED_LDFLAGS

AC_OUTPUT
