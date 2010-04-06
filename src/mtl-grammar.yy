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
%token TK_NUMBER
%token TK_NEW_MAT
%token TK_KA
%token TK_KD
%token TK_KS
%token TK_KE
%token TK_TF
%token TK_ALPHA
%token TK_NS
%token TK_REF_IDX
%token TK_ILLUM
%token TK_MAP_KA
%token TK_MAP_KD
%token TK_MAP_KS
%token TK_MAP_D
%token TK_MAP_BUMP
%token TK_IDENTIFIER
%token TK_FILENAME

%start mtl_file 
	
%{

#include "objloader.h"
#include "token.h"

#include "lexer.h"

#include <iostream>
#include <queue>
#include <string>

extern Token *mtl_currtok;
inline int yyerror(const char *err)
{
	std::cerr << "Objloader-MTL: Error on line " << mtllineno << ": " << err << ".    current token = ";
	if (mtl_currtok)
		std::cerr << mtl_currtok->Code() << "    '" << mtl_currtok->Text() << "'" << std::endl;
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

static ObjLoader::Mtl temp_mat;

static void rest_mat()
{
	temp_mat.amb_r = 0;	 	temp_mat.amb_g = 0;		temp_mat.amb_b = 0;	
	temp_mat.dif_r = 0;	 	temp_mat.dif_g = 0;		temp_mat.dif_b = 0;	
	temp_mat.spe_r = 0;	 	temp_mat.spe_g = 0;		temp_mat.spe_b = 0;	
	temp_mat.emi_r = 0;	 	temp_mat.emi_g = 0;		temp_mat.emi_b = 0;	
	temp_mat.trans_r = 0;	temp_mat.trans_g = 0;	temp_mat.trans_b = 0;	
	temp_mat.alpha = 0;	 	temp_mat.shininess = 0;	temp_mat.ref_idx = 0;
	temp_mat.name = "";
	temp_mat.tex_a = ""; 	temp_mat.tex_d = ""; 	temp_mat.tex_s = "";
	temp_mat.illum_model = 0;
}

%}

%%

/*
 *
 *	IMPORTANT:
 *		if you reduce an 'index' or a 'floating' but don't want to use it - pop it nevertheless!
 *		otherwise the rest of the system is going to be confused about this.
 *
 */ 
mtl_file:
	mtl_file entry						{ dbg("mtl -> mtl_file entry"); }
	| entry								{ dbg("mtl -> entry"); }
	;

entry:
	matname decls						{ dbg("entry"); yyobj->PushMaterial(temp_mat); }
	;

matname:
	TK_NEW_MAT name						{ rest_mat();	temp_mat.name = last_name;	}
	;

decls:
	decls decl							{ dbg("decls -> decls decl"); }
	| decl								{ dbg("decls -> decl"); }
	;

decl:
	TK_KA floating floating floating	{ float r=pop_float(), g=pop_float(), b=pop_float(); temp_mat.amb_r = r; temp_mat.amb_g = g; temp_mat.amb_b = b; }
	| TK_KD floating floating floating	{ float r=pop_float(), g=pop_float(), b=pop_float(); temp_mat.dif_r = r; temp_mat.dif_g = g; temp_mat.dif_b = b; }
	| TK_KS floating floating floating	{ float r=pop_float(), g=pop_float(), b=pop_float(); temp_mat.spe_r = r; temp_mat.spe_g = g; temp_mat.spe_b = b; }
	| TK_KE floating floating floating	{ float r=pop_float(), g=pop_float(), b=pop_float(); temp_mat.emi_r = r; temp_mat.emi_g = g; temp_mat.emi_b = b; }
	| TK_TF floating floating floating	{ float r=pop_float(), g=pop_float(), b=pop_float(); temp_mat.trans_r = r; temp_mat.trans_g = g; temp_mat.trans_b = b; }
	| TK_ALPHA floating					{ float a=pop_float(); temp_mat.alpha = a;	}
	| TK_NS floating					{ float s=pop_float(); temp_mat.shininess = s;	}
	| TK_REF_IDX floating				{ float s=pop_float(); temp_mat.ref_idx = s;	}
	| TK_ILLUM index					{ int i=pop_int(); temp_mat.illum_model = i;	}
	| TK_MAP_KA	filename				{ std::string s = last_name; temp_mat.tex_a = s;	}
	| TK_MAP_KD	filename				{ std::string s = last_name; temp_mat.tex_d = s;	}
	| TK_MAP_KS	filename				{ std::string s = last_name; temp_mat.tex_s = s;	}
	| TK_MAP_BUMP filename				{ std::string s = last_name; temp_mat.tex_s = s;	}
	| TK_MAP_D filename					{ std::string s = last_name; temp_mat.tex_s = s;	}
	;


floating:
	TK_NUMBER							{ floats.push(atof(mtl_currtok->Text().c_str())); $$ = 0; }
	;
	
index:
	TK_NUMBER							{ ints.push(atoi(mtl_currtok->Text().c_str())); $$ = 0; }
	;

name:
	TK_IDENTIFIER						{ last_name = mtl_currtok->Text(); }
	;

filename:
	/*TK_FILENAME							{ last_name = mtl_currtok->Text(); }*/
	TK_IDENTIFIER							{ last_name = mtl_currtok->Text(); }
	;


%%



