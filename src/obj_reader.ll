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

%{

/*
   anmerkungen
   . matched kein \n

*/

//#include "y.tab.h"
#include "obj-grammar.hpp"

#include "token.h"

#include <string>
#include <map>
#include <iostream>
#include <list>

using namespace std;

#define YY_DECL Token* yylex_int_libobj()

// #define LEX_DEBUG_OUT
#ifdef LEX_DEBUG_OUT
#define OUT(X) cout << X << endl
#else
#define OUT(X)
#endif

%}

%option noyywrap
%option yylineno

WHITE_SPACE [\n\r\ \t\b\012]
DIGIT [0-9]
ALPHA [a-zA-Z_().]
ALNUM ({DIGIT}|{ALPHA})
ALLTHE3DSMAXSHIT [-a-zA-Z_0-9./]
PM [+-]

%s COMMENT


%%

<<EOF>>										{	yyterminate();	}
<INITIAL>{WHITE_SPACE}						{ /*ignore*/ }

<INITIAL>"#"								{	BEGIN(COMMENT);	}
<COMMENT>\n									{	BEGIN(INITIAL); }
<COMMENT>.*									{	OUT("comment: " << yytext); }

<INITIAL>"-"?{DIGIT}+("."{DIGIT}*("e"{PM}?{DIGIT}+)?)?			{	OUT("number: " << yytext);		return new Token(TKNUMBER, yytext, yylineno);	}
<INITIAL>"-"?{DIGIT}+"e"{PM}?{DIGIT}+							{	OUT("number: " << yytext);		return new Token(TKNUMBER, yytext, yylineno);	}
<INITIAL>vt									{ 	OUT("texcoord");				return new Token(TKTEX, yytext, yylineno);	}
<INITIAL>vn									{ 	OUT("normal");					return new Token(TKNORMAL, yytext, yylineno);	}
<INITIAL>v									{ 	OUT("vertex");					return new Token(TKVERTEX, yytext, yylineno);	}
<INITIAL>f									{ 	OUT("face");					return new Token(TKFACE, yytext, yylineno);	}
<INITIAL>"/"								{	OUT("/");						return new Token('/', yytext, yylineno);		}
<INITIAL>"usemtl"							{	OUT("usemtl");					return new Token(TKUMTL, yytext, yylineno);	}
<INITIAL>"mtllib"							{	OUT("mtllib");					return new Token(TKMTLL, yytext, yylineno);	}
<INITIAL>g									{	OUT("g");						return new Token(TKGRP, yytext, yylineno); }
<INITIAL>o									{	OUT("g");						return new Token(TKONAME, yytext, yylineno); }
<INITIAL>s									{	OUT("s");						return new Token(TKSNAME, yytext, yylineno); }

<INITIAL>{ALLTHE3DSMAXSHIT}*{ALPHA}{ALLTHE3DSMAXSHIT}*	{	OUT("identifier");				return new Token(TKIDENTIFIER, yytext, yylineno); }
 /*<INITIAL>{ALPHA}{ALNUM}*					{	OUT("identifier");				return new Token(TKIDENTIFIER, yytext, yylineno); }*/

<INITIAL>.									{ 	OUT("char");					return new Token(yytext[0], yytext, yylineno);}





%%

static std::list<Token*> tokens;
::Token *obj_currtok;

int yylex()
{
//	cerr << "MY yylex called" << endl;
	/* new */
	static ::Token *old_tok = 0;
	delete old_tok;
	old_tok = obj_currtok;
	/* --- */
	obj_currtok = yylex_int_libobj();
	if (obj_currtok)
	{
//		cerr << "token: " << obj_currtok->Code() << "       '" << obj_currtok->Text() << "'" << endl;
		/* old
		tokens.push_back(obj_currtok);
		return obj_currtok->Code();
		*/
		int code = obj_currtok->Code();
		return code;
	}
	else
		return 0;
}



