# $Id: Makefile 134 2007-12-16 22:11:45Z kai $
BIN=spinat

build: $(BIN)

.phony: build clean abgabe dot_to_ps

# ggf hier programme spezifizieren
CXX=g++
YACC=byacc
LEX=flex

GRAMMAR=src/grammar.y
LEXFILE=src/obj_reader.lex

# keine wildcards, sonst kommen uns noch die yacc/lex dateien in die quere
SRC=$(shell find src -name "*.cpp")
HDR=$(shell find src -name "*.h")
SUBDIRS=src $(shell find src -maxdepth 1 -type d -regex ".*/[^.].*")

YACC_SRC=build/y.tab.c
YACC_HDR=build/y.tab.h
LEX_SRC=build/lex.yy.c

ABGEBEN=$(BIN) src Makefile README.txt examples doxygen.cfg
ABGABE=curr

CFLAGS+= -I src

# was wird gebaut
# OBJS=$(addprefix build/, $(patsubst %.c,%.o, $(notdir $(YACC_SRC) $(LEX_SRC))) $(patsubst %.cpp,%.o, $(notdir $(SRC))))
OBJS=$(addprefix build/, $(patsubst %.c,%.o, $(notdir $(YACC_SRC) $(LEX_SRC))) $(patsubst %.cpp,%.o, $(shell echo $(SRC) | sed -e 's:/:_:g')))

LALA=
$(foreach directory,$(SUBDIRS), $(eval LALA+=$(shell echo $(directory) |  sed -e 's:/:_:g' | sed -e 's:.*:build/&_:')))

info:
	@echo "SUBDIRS: $(SUBDIRS)"
	@echo "OBJ:     $(OBJS)"
	@echo "CFLAGS:  $(CFLAGS)"
	@echo "SUBDIRS: $(SUBDIRS)"
	@echo "LALA:    $(LALA)"

filelist:
	@echo $(SRC) $(HDR)

# yacc'en
$(YACC_SRC) $(YACC_HDR): $(GRAMMAR)
	@echo YACC
	@mkdir -p build
	@$(YACC) -d $<
	@mv y.tab.c $(YACC_SRC)
	@mv y.tab.h $(YACC_HDR)
	@sed -e 's/getenv();/getenv(const char*);/' -i $(YACC_SRC)		# yeah : (

# lex'en
$(LEX_SRC): $(LEXFILE)
	@echo LEX
	@lex --outfile $(LEX_SRC) $<

# kompilieren der generierten dateien in build
build/%.o: build/%.c
	@echo CXX $<
	@$(CXX) $(CFLAGS) -c $< -o $@

# kompilieren der normalen quellen in den einzelnen subdirs
.SECONDARYEXPANSION:

define CXX_DirectoryTarget
$(2)%.o: $(1)/%.cpp
	@echo  "CXX $$<"
	@$(CXX) $(CFLAGS) -o $$@ -c $(1)/$$*.cpp
endef

# $(foreach directory,$(SUBDIRS),$(eval $(call CXX_DirectoryTarget,$(directory),$(shell echo $(directory) |  sed -e 's:/:_:g'))))
$(foreach directory,$(SUBDIRS),\
          $(eval $(call CXX_DirectoryTarget,\
                        $(directory), \
                        $(shell echo $(directory) |  sed -e 's:/:_:g' | sed -e 's:.*:build/&_:'))))
# $(eval LALA+=$(shell echo $(directory) |  sed -e 's:/:_:g' | sed -e 's:.*:build/&_:'))

# abhängigkeiten finden
DEPS=$(SRC)
depend: $(YACC_SRC) $(LEX_SRC) $(DEPS)
	@echo DEP
	@rm -f build/depend
	@for f in $(DEPS); do \
		SUB=`echo $$f | sed -e 's:/:_:g' | sed -e 's/\(.*\.\)cpp/\1o/'`; \
		$(CXX) $(CFLAGS) -MM $$f | sed -e "s/.*:/build\/$$SUB:/" >> build/depend; \
	done;
	@$(CXX) $(CFLAGS) -MM $(YACC_SRC) | sed -e "s/.*:/build\/&/" >> build/depend
	@$(CXX) $(CFLAGS) -MM $(LEX_SRC) | sed -e "s/.*:/build\/&/" >> build/depend
	

-include build/depend
	
# linken
$(BIN): depend $(OBJS)
	@echo LD
	@$(CXX) $(CFLAGS) -o $@ $(OBJS)

DOTS=$(shell ls *.dot 2> /dev/null)
PS=$(DOTS:%.dot=%.ps)
dot_to_ps: $(PS)

%.ps: %.dot
	@echo DOT $<
	@dot -Tps $< -o $@

clean:
	rm -rf build $(BIN) $(PS)

doc: $(SRC) $(HDR)
	doxygen doxygen.cfg

abgabe: $(BIN)
	rm -rf abgabe/$(ABGABE)
	mkdir -p abgabe/$(ABGABE)
	cp -r $(ABGEBEN) abgabe/$(ABGABE)
	rm -rf `find abgabe/$(ABGABE) -name ".svn"`
	tar cfz abgabe/$(ABGABE).tgz abgabe/$(ABGABE)
