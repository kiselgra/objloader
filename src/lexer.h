/* $Id$ */
#ifndef __LEXER_H__ 
#define __LEXER_H__ 

#include <cstdio>
#include "token.h"

extern int yylex();
extern int yylineno;
extern Token *currtok;
extern std::FILE *yyin;


#endif

