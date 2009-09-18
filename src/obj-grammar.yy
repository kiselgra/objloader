%token TKNUMBER
%token TKVERTEX
%token TKTEX
%token TKNORMAL
%token TKFACE
%token TKUMTL
%token TKMTLL
%token TKGRP
%token TKONAME
%token TKSNAME

%token TKIDENTIFIER

%start obj_file
	
%{

#include "objloader.h"
#include "token.h"

#include "lexer.h"

#include <iostream>
#include <queue>

extern Token *obj_currtok;
inline int yyerror(const char *err)
{
	std::cerr << "Objloader: Error on line " << libobjlineno << ": " << err << ".    current token = ";
	if (obj_currtok)
		std::cerr << obj_currtok->Code() << "    '" << obj_currtok->Text() << "'" << std::endl;
	else
		std::cerr << "<empty>" << std::endl;
}

extern ObjLoader *yyobj;

static std::queue<float> floats;
static std::queue<int> ints;

static float pop_float()
{
	float f = floats.front();
	floats.pop();
	return f;
}

static int pop_int()
{
	int i = ints.front();
	ints.pop();
	return i;
}

static std::string last_name;

// #define YACC_DEBUG_OUT

#ifdef YACC_DEBUG_OUT
#define dbg(x) std::cout << x << std::endl
#else
#define dbg(x)
#endif
%}

%%

/*
 *
 *	IMPORTANT:
 *		if you reduce an 'index' or a 'floating' but don't want to use it - pop it nevertheless!
 *		otherwise the rest of the system is going to be confused about this.
 *
 */

obj_file:
	obj_file entry						{ dbg("obj -> obj_file entry"); }
	| entry								{ dbg("obj -> entry"); }
	;

entry:
	vertex								{ dbg("entry"); }
	| texcoord							{ dbg("entry"); }
	| normal							{ dbg("entry"); }
	| face								{ dbg("entry"); }
	| usemtl							{ dbg("entry"); }
	| mtllib							{ dbg("entry"); }
	| group								{ dbg("entry"); }
	| sgroup							{ dbg("entry"); }
	| object_name						{ dbg("entry"); }
	;

face:
	TKFACE face_nodes					{ dbg("face"); 	yyobj->FaceDone(); }
	;

face_nodes:
	face_node face_nodes				{ dbg("face_nodes -> face_node face_nodes"); }
	| face_node							{ dbg("face_nodes -> face_node"); }
	;

face_node:
	index '/' index '/' index 			{ int v=pop_int(), t=pop_int(), n=pop_int();	dbg("face_node  x/y/z");	yyobj->AddFaceNode(ObjLoader::VTN, v,t,n);	}
	| index '/' '/' index		 		{ int v=pop_int(), n=pop_int();					dbg("face_node  x//z");		yyobj->AddFaceNode(ObjLoader::VN, v,n);	}
	| index '/' index 					{ int v=pop_int(), t=pop_int();					dbg("face_node  x/y");		yyobj->AddFaceNode(ObjLoader::VT, v,t);	}
	| index								{ int v=pop_int();								dbg("face_node  x");		yyobj->AddFaceNode(ObjLoader::V, v);	}
	;

normal:
	TKNORMAL floating floating floating { float x=pop_float(), y=pop_float(), z=pop_float();	dbg("got normal"); 	yyobj->AddNormal(x,y,z);	}
	;

texcoord:
	TKTEX floating floating				{ float u=pop_float(), v=pop_float();					dbg("got texcoord 2"); 	yyobj->AddTexCoord(u,v);	}
	| TKTEX floating floating floating	{ float u=pop_float(), v=pop_float(), w = pop_float();	dbg("got texcoord 3"); 	yyobj->AddTexCoord(u,v,w);		}
	;

vertex:
	TKVERTEX floating floating floating	{ float x=pop_float(), y=pop_float(), z=pop_float(); 	dbg("got vertex"); yyobj->AddVertex(x, y, z); }
	;

usemtl:
	TKUMTL name							{ dbg("usemtl"); 	if (!yyobj->CurrentMaterial(last_name))	yyerror(((std::string)"Material '" + last_name + "' not defined.").c_str()); }
	;

mtllib:
	TKMTLL name							{ dbg("mtllib");  	if (!yyobj->LoadMaterialFile(last_name)) yyerror(((std::string)"Cannot read material lib '" + last_name + "'.").c_str());  }
	;

group:
	TKGRP name							{ dbg("group");		yyobj->StartGroup(last_name); }
	;

sgroup:
	TKSNAME index						{ int grp_idx = pop_int();	dbg("sgroup");	}
	| TKSNAME name						{ dbg("sgroup");	}
	;

object_name:
	TKONAME name						{ dbg("obj-name");		yyobj->ObjectName(last_name); }
	;

floating:
	TKNUMBER							{ floats.push(atof(obj_currtok->Text().c_str())); $$ = 0; }
	;
	
index:
	TKNUMBER							{ ints.push(atoi(obj_currtok->Text().c_str())); $$ = 0; }
	;

name:
	TKIDENTIFIER						{ last_name = obj_currtok->Text(); }
	;


%%


