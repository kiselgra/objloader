
%{

/*
   anmerkungen
   . matched kein \n

*/

//#include "y.tab.h"
#include "grammar.h"

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

%s COMMENT


%%

<<EOF>>										{	yyterminate();	}
<INITIAL>{WHITE_SPACE}						{ /*ignore*/ }

<INITIAL>"#"								{	BEGIN(COMMENT);	}
<COMMENT>\n									{	BEGIN(INITIAL); }
<COMMENT>.*									{	OUT("comment: " << yytext); }

<INITIAL>"-"?{DIGIT}+("."{DIGIT}*("e""-"?{DIGIT}+)?)?			{	OUT("number: " << yytext);		return new Token(TKNUMBER, yytext, yylineno);	}
<INITIAL>vt									{ 	OUT("texcoord");				return new Token(TKTEX, yytext, yylineno);	}
<INITIAL>vn									{ 	OUT("normal");					return new Token(TKNORMAL, yytext, yylineno);	}
<INITIAL>v									{ 	OUT("vertex");					return new Token(TKVERTEX, yytext, yylineno);	}
<INITIAL>f									{ 	OUT("face");					return new Token(TKFACE, yytext, yylineno);	}
<INITIAL>"/"								{	OUT("/");						return new Token('/', yytext, yylineno);		}
<INITIAL>"usemtl"							{	OUT("usemtl");					return new Token(TKUMTL, yytext, yylineno);	}
<INITIAL>"mtllib"							{	OUT("mtllib");					return new Token(TKMTLL, yytext, yylineno);	}
<INITIAL>g									{	OUT("g");						return new Token(TKGRP, yytext, yylineno); }
<INITIAL>o									{	OUT("g");						return new Token(TKONAME, yytext, yylineno); }

<INITIAL>{ALPHA}{ALNUM}*					{	OUT("identifier");				return new Token(TKIDENTIFIER, yytext, yylineno); }

<INITIAL>.									{ 	OUT("char");					return new Token(yytext[0], yytext, yylineno);}





%%

static std::list<Token*> tokens;
::Token *currtok;

int yylex()
{
//	cerr << "MY yylex called" << endl;
	currtok = yylex_int_libobj();
	if (currtok)
	{
//		cerr << "token: " << currtok->Code() << "       '" << currtok->Text() << "'" << endl;
		tokens.push_back(currtok);
		return currtok->Code();
	}
	else
		return 0;
}



