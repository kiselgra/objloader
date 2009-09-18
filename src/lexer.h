/* $Id$ */
#ifndef __LEXER_H__ 
#define __LEXER_H__ 

#include <cstdio>
#include "token.h"

extern int libobjlex();
extern int libobjlineno;
extern Token *obj_currtok;
extern std::FILE *libobjin;

extern int mtllex();
extern int mtllineno;
extern Token *mtl_currtok;
extern std::FILE *mtlin;


#endif

