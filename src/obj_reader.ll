
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

#define YY_DECL Token* yylex_int()

// #define LEX_DEBUG_OUT
#ifdef LEX_DEBUG_OUT
#define OUT(X) cout << X << endl
#else
#define OUT(X)
#endif

%}

%option noyywrap
%option yylineno

WHITE_SPACE [\n\ \t\b\012]
DIGIT [0-9]
ALPHA [a-zA-Z_]
ALNUM ({DIGIT}|{ALPHA})

%s COMMENT


%%

<<EOF>>										{	yyterminate();	}
<INITIAL>{WHITE_SPACE}						{ /*ignore*/ }

<INITIAL>"#"								{	BEGIN(COMMENT);	}
<COMMENT>\n									{	BEGIN(INITIAL); }
<COMMENT>.*									{	OUT("comment: " << yytext); }

<INITIAL>"-"?{DIGIT}+("."{DIGIT}*)?			{	OUT("number: " << yytext);		return new Token(TKNUMBER, yytext);	}
<INITIAL>vt									{ 	OUT("texcoord");				return new Token(TKTEX, yytext);	}
<INITIAL>vn									{ 	OUT("normal");					return new Token(TKNORMAL, yytext);	}
<INITIAL>v									{ 	OUT("vertex");					return new Token(TKVERTEX, yytext);	}
<INITIAL>f									{ 	OUT("face");					return new Token(TKFACE, yytext);	}
<INITIAL>"/"								{	OUT("/");						return new Token('/', yytext);		}
<INITIAL>"usemtl"							{	OUT("usemtl");					return new Token(TKUMTL, yytext);	}
<INITIAL>"mtllib"							{	OUT("mtllib");					return new Token(TKMTLL, yytext);	}

<INITIAL>{ALPHA}{ALNUM}*					{	OUT("identifier");				return new Token(TKIDENTIFIER, yytext); }

<INITIAL>.									{ 	OUT("char");					return new Token(yytext[0], yytext);}





%%

static std::list<Token*> tokens;
::Token *currtok;

int yylex()
{
	currtok = yylex_int();
	if (currtok)
	{
		tokens.push_back(currtok);
		return currtok->Code();
	}
	else
		return 0;
}



