#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([doomsday-tetris-2], [1.0], [eemil.visakorpi@gmail.com])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([ac_config.h])
AC_CONFIG_MACRO_DIRS([m4])

# Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET


# Checks for libraries.
LT_INIT
AC_ENABLE_SHARED
AC_DISABLE_STATIC
AC_PROG_LIBTOOL(libtool)

AC_CHECK_LIB([SDL2], [SDL_Init], [], [AC_MSG_ERROR([SDL2 was not found])])
AC_CHECK_LIB([SDL2_ttf], [TTF_OpenFont], [], [AC_MSG_ERROR([SDL2 ttf was not found])])
AC_CHECK_LIB([pq], [PQconnectdb], [], [AC_MSG_ERROR([pq-fe was not found in libm])])
AC_CHECK_LIB([fontconfig], [FcInitLoadConfigAndFonts], [], [AC_MSG_ERROR([Fontconfig was not found])])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([pow sqrt])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
