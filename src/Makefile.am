lib_LTLIBRARIES = libobjloader.la

libobjloader_la_SOURCES = grammar.yy  lexer.h  obj_reader.ll  objloader.cpp  objloader.h  parser.h  token.h
# AM_CPPFLAGS = -Isrc/
AM_YFLAGS = -d

library_includedir = $(includedir)/$(PACKAGE)-$(VERSION)
library_include_HEADERS = objloader.h

