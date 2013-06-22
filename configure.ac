#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([compass], [0.1], [tierney@cs.nyu.edu])
AC_CONFIG_SRCDIR([prop_logic_main.cc])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_CXX
AC_PROG_INSTALL
AM_PROG_AR
AC_PROG_LIBTOOL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_YACC
AM_INIT_AUTOMAKE
AM_PROG_LEX

# Checks for libraries.
AC_CHECK_LIB([fl], [main])
AC_CHECK_LIB([bdd], [main])

# Thrift (serialization and rpc library).
AC_ARG_WITH([thrift-libdir],
        [AS_HELP_STRING([--with-thrift-libdir=LIB_DIR],
        [location of the protocol buffers libraries, defaults to /usr/lib])],
        [THRIFT_LIBDIR="$withval"],
        [THRIFT_LIBDIR='/usr/lib'])
AC_SUBST([THRIFT_LIBDIR])

LDFLAGS="$LDFLAGS -L$THRIFT_LIBDIR"

AC_CHECK_LIB([thrift], [main], [], [AC_MSG_ERROR([cannot find thrift library])])

AC_ARG_WITH([thrift],
        [AS_HELP_STRING([--with-thrift=PATH],
        [location of the thrift protocol compiler binary, defaults to thrift])],
        [THRIFT="$withval"],
    [THRIFT='thrift'])
AC_SUBST([THRIFT])

# Checks for header files.
AC_FUNC_ALLOCA
AC_CHECK_HEADERS([inttypes.h libintl.h malloc.h stddef.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset strdup])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
