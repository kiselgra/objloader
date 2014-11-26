lib_LTLIBRARIES = libobjloader.la
bin_PROGRAMS = obj2bobj
noinst_PROGRAMS = pobj2obj obj2pobj

libobjloader_la_SOURCES = obj-grammar.cpp obj-grammar.hpp \
						  mtl-grammar.cpp mtl-grammar.hpp \
						  lexer.h  \
						  default.cpp default.h \
						  obj_reader.cpp \
						  mtl_reader.cpp \
						  objloader.cpp  objloader.h  \
						  parser.h  token.h

obj2pobj_SOURCES = $(libobjloader_la_SOURCES) pobj.h pobj.cpp obj2pobj.cpp
obj2pobj_CPPFLAGS = $(AM_CPPFLAGS)

pobj2obj_SOURCES = $(libobjloader_la_SOURCES) pobj.h pobj.cpp pobj2obj.cpp
pobj2obj_CPPFLAGS = $(AM_CPPFLAGS)

obj2bobj_SOURCES = $(libobjloader_la_SOURCES) tobinary.cpp
obj2bobj_CPPFLAGS = $(AM_CPPFLAGS)

# obj lexer and grammar
obj-grammar.hpp: obj-grammar.cpp
obj-grammar.cpp: obj-grammar.yy
	bison -p libobj -o $@ -d $<

obj_reader.cpp: obj_reader.ll
	flex -P libobj -o $@ $<

# mtl lexer and grammar
mtl-grammar.hpp: mtl-grammar.cpp
mtl-grammar.cpp: mtl-grammar.yy
	bison -p mtl -o $@ -d $<

mtl_reader.cpp: mtl_reader.ll
	flex -P mtl -o $@ $<

bison_output_files = mtl-grammar.cpp mtl-grammar.hpp obj-grammar.cpp obj-grammar.hpp
flex_output_files = obj_reader.cpp mtl_reader.cpp


# AM_CPPFLAGS = -Isrc/
AM_CPPFLAGS = -D WITH_DEFAULT_IMPLEMENTATION
AM_YFLAGS = -d -plibobj
AM_LFLAGS = -Plibobj -o lex.yy.c

#libobjloader_la_LIBADD = -l3dmath
#obj2pobj_LDFLAGS = -l3dmath
#pobj2obj_LDFLAGS = -l3dmath

library_includedir = $(includedir)/$(PACKAGE)-$(VERSION)
library_include_HEADERS = objloader.h default.h

MAINTAINERCLEANFILES = $(bison_output_files) $(flex_output_files)

EXTRA_DIST = mtl-grammar.yy mtl_reader.ll obj-grammar.yy obj_reader.ll

library_nonversioned_includedir = $(includedir)/$(PACKAGE)

install-data-hook:
	@test -L $(library_nonversioned_includedir) && echo "   !!!   The nonversioned library include dir <$(library_nonversioned_includedir)> is already covered by a link. You'll have to update yourself". || true
	test -L $(library_nonversioned_includedir) || ln -s $(library_includedir) $(library_nonversioned_includedir)


