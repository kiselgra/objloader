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
		libobjin = fopen(name.c_str(), "r");

// 	printf("------\n");
// 	while (!feof(yyin))
// 		putc(fgetc(yyin), stdout);
// 	printf("------\n");
// 
// 	exit (0);

	yyobj = this;

	libobjparse();
}
	
int ObjLoader::CurrentLine() 
{ 
	return currtok ? currtok->Line() : -1; 
}

ObjLoader *yyobj;

