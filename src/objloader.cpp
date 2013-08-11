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
#include <fstream>

using namespace std;

extern Token *obj_currtok;
extern ObjLoader *yyobj;

ObjLoader::ObjLoader()
: verts_read(0), norms_read(0), texs_read(0), parse_obj_without_bision(true)
{
}

ObjLoader::~ObjLoader()
{
}

bool ObjLoader::Load(const std::string name)
{
	obj_filename = name;
	yyobj = this;

	if (!parse_obj_without_bision) {
		if (name != "")
			libobjin = fopen(name.c_str(), "r");

		libobjparse();
	}
	else {
		if (name != "")
			stream = new std::ifstream(name.c_str());
		else
			stream = &cin;
		ugly_parse(*stream);
	}

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


#define eat_white while (isspace(in.peek()) && in.peek() != '\n') in.get();

inline void handle_vidx(int &idx) {
	if (idx < 0)
		idx = yyobj->verts_read + idx + 1;
}
inline void handle_nidx(int &idx) {
	if (idx < 0)
		idx = yyobj->norms_read + idx + 1;
}
inline void handle_tidx(int &idx) {
	if (idx < 0)
		idx = yyobj->texs_read + idx + 1;
}


std::string trim(const std::string& str, const std::string& whitespace = " \t")
{
	const auto str_begin = str.find_first_not_of(whitespace);
	if (str_begin == std::string::npos)
		return ""; // no content

	const auto str_end = str.find_last_not_of(whitespace);
	const auto str_range = str_end - str_begin + 1;

	return str.substr(str_begin, str_range);
}

bool ObjLoader::ugly_add_face(std::istream &in) {
	int v = 0, t = 0, n = 0;
	bool use_t = false, use_n = false;
	in >> v;
	eat_white;
	if (in.peek() == '/') {
		in.get();
		eat_white;
		if (in.peek() != '/') {
			in >> t;
			eat_white;
			use_t = true;
		}
		if (in.peek() == '/') {
			in.get();
			in >> n;
			use_n = true;
		}
	}
	
	handle_vidx(v);
	handle_nidx(n);
	handle_tidx(t);

	if (use_n && use_t)
		yyobj->AddFaceNode(ObjLoader::VTN, v, t, n);
	else if (use_n)
		yyobj->AddFaceNode(ObjLoader::VN, v, n);
	else if (use_t)
		yyobj->AddFaceNode(ObjLoader::VN, v, t);
	else
		yyobj->AddFaceNode(ObjLoader::V, v);

	eat_white;
	if (in.peek() == '\n')
		return false;
	return true;	// keep going
}

void ObjLoader::ugly_parse(std::istream &in)
{
	while (true) {
		string command;
		in >> command;
// 		if (in.eof() || command == "")
		if (in.eof())
			break;
// 		cout << "command: '" << command << "'" << endl;
		if (command == "v") {
			float x, y, z;
			in >> x >> y >> z;
			verts_read++;
			yyobj->AddVertex(x, y, z);
		}
		else if (command == "vn") {
			float x, y, z;
			in >> x >> y >> z;
			norms_read++;
			yyobj->AddNormal(x, y, z);
		}
		else if (command == "vt") {
			float x, y, z;
			in >> x >> y;
			eat_white;
			texs_read++;
			if (in.peek() != '\n') {
				in >> z;
				yyobj->AddTexCoord(x, y, z);
			}
			else
				yyobj->AddTexCoord(x, y);
		}
		else if (command == "f") {
			while (ugly_add_face(in))
				;
			yyobj->FaceDone();
		}
		else if (command == "mtllib") {
			eat_white;
			string rest; getline(in, rest);
			rest = trim(rest);
			yyobj->LoadMaterialFile(rest);
		}
		else if (command == "usemtl") {
			eat_white;
			string rest; getline(in, rest);
			rest = trim(rest);
			cout << "UseMtl '" << rest << "'" << endl;
			if (!yyobj->CurrentMaterial(rest))
				cerr << "Material '" << rest << "' is not defined." << endl;
		}
		else if (command == "s") {
			string line; getline(in, line);
		}
		else if (command == "g") {
			eat_white;
			string rest; getline(in, rest);
			rest = trim(rest);
			cout << "Group '" << rest << "'" << endl;
			yyobj->StartGroup(rest);
		}
		else if (command == "o") {
			eat_white;
			string rest; getline(in, rest);
			rest = trim(rest);
			cout << "Object '" << rest << "'" << endl;
			yyobj->ObjectName(rest);
		}
		else if (command == "#") {
			string line; getline(in, line);
		}
		else {
			cout << "UNKNOWn '" << command << "'" << endl;
			exit(0);
		}
	}
}

