/* $Id$ */
#ifndef __LEXER_H__ 
#define __LEXER_H__ 

#include <cstdio>
#include "token.h"

extern int libobjlex();
extern int libobjlineno;
extern Token *currtok;
extern std::FILE *libobjin;


#endif

