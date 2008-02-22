#include "objloader.h"

#include "parser.h"
#include "lexer.h"

#include "token.h"

extern Token *currtok;
extern ObjLoader *yyobj;

ObjLoader::ObjLoader()
{
}

ObjLoader::~ObjLoader()
{
}

bool ObjLoader::Load(const std::string name)
{
	if (name != "")
		yyin = fopen(name.c_str(), "r");

	yyobj = this;

	yyparse();
}
	
int ObjLoader::CurrentLine() 
{ 
	return currtok ? currtok->Line() : -1; 
}

ObjLoader *yyobj;

