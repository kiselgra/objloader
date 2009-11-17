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
#include "objloader.h"

#include "parser.h"
#include "lexer.h"

#include "token.h"

#include <iostream>

using namespace std;

extern Token *obj_currtok;
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
	return obj_currtok ? obj_currtok->Line() : -1; 
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

