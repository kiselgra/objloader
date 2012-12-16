#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(libobjloader, 0.0.1, BUG-REPORT-ADDRESS)

	#doxygen
# DX_HTML_FEATURE(ON)
#	DX_CHM_FEATURE(OFF)
#	DX_CHI_FEATURE(OFF)
#	DX_MAN_FEATURE(OFF)
#	DX_RTF_FEATURE(OFF)
#	DX_XML_FEATURE(OFF)
#	DX_PDF_FEATURE(OFF)
#	DX_PS_FEATURE(OFF)
#	DX_INIT_DOXYGEN(objloader, doxygen.cfg)

AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/objloader.cpp])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_FILES(Makefile src/Makefile doc/Makefile lib-config-src/Makefile test/Makefile libobjloader.pc)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LEX
AC_PROG_YACC
AC_PROG_LIBTOOL
AC_PROG_AWK
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB



# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([inttypes.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
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
AC_CHECK_FUNCS([memset])

## http://stackoverflow.com/questions/5298830/how-to-include-m4-files-in-autoconf
AC_CONFIG_MACRO_DIR([.])
AC_LANG([C++])
AX_CXX_COMPILE_STDCXX_11

AC_OUTPUT
