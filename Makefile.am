bin_PROGRAMS = spinat

spinat_SOURCES = src/grammar.yy  src/lexer.h  src/obj_reader.ll  src/objfile_logger.h  src/objloader.cpp  src/objloader.h  src/parser.h  src/test.cpp  src/token.h
AM_CPPFLAGS = -Isrc/
AM_YFLAGS = -d


.PHONY: doc
doc: 
	cd doc; doxygen doxyfile

# include $(top_srcdir)/aminclude.am
# SUBDIRS = src
