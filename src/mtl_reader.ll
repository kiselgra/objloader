
%{

/*
   anmerkungen
   . matched kein \n

*/

//#include "y.tab.h"
#include "mtl-grammar.hpp"

#include "token.h"

#include <string>
#include <map>
#include <iostream>
#include <list>

using namespace std;

#define YY_DECL Token* yylex_int_mtl()

// #define LEX_DEBUG_OUT
#ifdef LEX_DEBUG_OUT
#define OUT(X) cout << X << endl
#else
#define OUT(X)
#endif

#define newtoken(T) new Token(T, yytext, yylineno)

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

<INITIAL>"-"?{DIGIT}+("."{DIGIT}*("e""-"?{DIGIT}+)?)?			{	OUT("number: " << yytext);		return new Token(TK_NUMBER, yytext, yylineno);	}

<INITIAL>newmtl				{	OUT("newmtl");	return newtoken(TK_NEW_MAT);	}
<INITIAL>Ka					{	OUT("Ka");		return newtoken(TK_KA);	}
<INITIAL>Kd					{	OUT("Kd");		return newtoken(TK_KD);	}
<INITIAL>Ks					{	OUT("Ks");		return newtoken(TK_KS);	}
<INITIAL>Ke					{	OUT("Ke");		return newtoken(TK_KE);	}	
<INITIAL>(d|Tr)				{	OUT("alpha");	return newtoken(TK_ALPHA);	}
<INITIAL>illum				{	OUT("illum");	return newtoken(TK_ILLUM);	}
<INITIAL>map_Ka				{	OUT("map_Ka");	return newtoken(TK_MAP_KA);	}
<INITIAL>map_Kd				{	OUT("map_Kd");	return newtoken(TK_MAP_KD);	}
<INITIAL>map_Ks				{	OUT("map_Ks");	return newtoken(TK_MAP_KS);	}
<INITIAL>Ns					{	OUT("Ns");		return newtoken(TK_NS);	}
<INITIAL>Ni					{	OUT("Ni");		return newtoken(TK_REF_IDX);	}
<INITIAL>Tf					{	OUT("Tf");		return newtoken(TK_TF);	}


<INITIAL>{ALPHA}{ALNUM}*					{	OUT("identifier");				return new Token(TK_IDENTIFIER, yytext, yylineno); }
<INITIAL>[-0-9a-zA-Z_./]*					{	OUT("filename");				return new Token(TK_FILENAME, yytext, yylineno); }

<INITIAL>.									{ 	OUT("char");					return new Token(yytext[0], yytext, yylineno);}





%%

static std::list<Token*> tokens;
::Token *mtl_currtok;

int yylex()
{
//	cerr << "MY yylex called" << endl;
	mtl_currtok = yylex_int_mtl();
	if (mtl_currtok)
	{
//		cerr << "token: " << mtl_currtok->Code() << "       '" << mtl_currtok->Text() << "'" << endl;
		tokens.push_back(mtl_currtok);
		return mtl_currtok->Code();
	}
	else
		return 0;
}



