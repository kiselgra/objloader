#include "objloader.h"

#include "parser.h"
#include "lexer.h"

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

ObjLoader *yyobj;

