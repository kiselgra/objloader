#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(objloader, 0.0.1, BUG-REPORT-ADDRESS)

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
AC_CONFIG_SRCDIR([src/objfile_logger.h])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_FILES(Makefile)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LEX
AC_PROG_YACC

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

AC_OUTPUT
