#include "objloader.h"

#include "parser.h"
#include "lexer.h"

#include "token.h"

#include <iostream>

using namespace std;

extern Token *currtok;
extern ObjLoader *yyobj;

ObjLoader::ObjLoader()
: verts_read(0), norms_read(0), texs_read(0)
{
}

ObjLoader::~ObjLoader()
{
}

bool ObjLoader::Load(const std::string name)
{
	obj_filename = name;

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

	for (map<string,Mtl*>::iterator it = materials.begin(); it != materials.end(); ++it)
		cout << it->second->name << endl;
}
	
int ObjLoader::CurrentLine() 
{ 
	return currtok ? currtok->Line() : -1; 
}
	
bool ObjLoader::LoadMaterialFile(const std::string &name)
{
	string filename = name;
	int pos = obj_filename.find_last_of("/");
	if (pos != string::npos)
	{
		string base = obj_filename.substr(0, pos+1);
		filename = base + name;
	}

	mtlin = fopen(filename.c_str(), "r");
	if (mtlin == 0)
	{
		cerr << "could not open '" << filename << "'" << endl;
		return false;
	}
	
	mtlparse();
	return true;
}

void ObjLoader::PushMaterial(const Mtl &m)
{
	materials[m.name] = new Mtl(m);
}


ObjLoader *yyobj;

