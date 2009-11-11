/*  Copyright (C) 2009 Kai Selgrad <stuff * projects . selgrad . org>
 *  * is the symbol @.
 *   
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
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

