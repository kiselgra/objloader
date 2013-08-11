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
#ifndef __OBJFILE_H__ 
#define __OBJFILE_H__ 

#include <string>
#include <istream>
#include <map>


/*!	Interface für Alias/Wavefront Obj Datei Loader.
 *
 * 	Nach den einlesen eines Vertex, einer Texturkoordinate oder einer Normalen wird direkt die entsprechende
 * 	Funktion aufgerufen. Ebenso wird für jeden Teilausdruck innerhalb einer face-definition AddFaceNode
 * 	aufgerufen. Nachdem alle Knotenpunkte der Oberfläche eingelesen wurden wird FaceDone aufgerufen.
 * 	Das Material funktioniert analog.
 *
 * 	Die einzelnen Funktionen müssen vom Anwender überschrieben werden. So gibts das Parsen umsonst, aber
 * 	ohne gleichzeitig Datenstrukturen aufzuzwängen.
 */
class ObjLoader
{
protected:
	std::string obj_filename;
	bool ugly_add_face(std::istream &in);
	void ugly_parse(std::istream &in);

public:
	enum VTN { VTN };
	enum VN  { VN };
	enum VT  { VT };
	enum V   { V };

	struct Mtl
	{
		float amb_r, amb_g, amb_b,
			  dif_r, dif_g, dif_b,
			  spe_r, spe_g, spe_b,
			  emi_r, emi_g, emi_b,
			  trans_r, trans_g, trans_b,
			  alpha, shininess, ref_idx;
		std::string name;
		std::string tex_a, tex_d, tex_s, tex_bump, tex_alpha;
		int illum_model;
	};
	std::map<std::string, Mtl*> materials;
	bool parse_obj_without_bision;	//!< does not affect mtl!
	std::istream *stream;

	ObjLoader();
	~ObjLoader();

	virtual bool Load(const std::string name);
	
	virtual bool LoadMaterialFile(const std::string &name);
	virtual void PushMaterial(const Mtl &m);
	virtual bool CurrentMaterial(const std::string &name) { return true; }

	virtual void StartGroup(const std::string &name) {}
	virtual void ObjectName(const std::string &name) {}

	virtual void AddVertex(float x, float y, float z) 		= 0;
	virtual void AddTexCoord(float u, float v, float w = 0) = 0;
	virtual void AddNormal(float x, float y, float z)		= 0;

	virtual void AddFaceNode(enum VTN, int vertex, int texcoord, int normal) = 0;
	virtual void AddFaceNode(enum VT,  int vertex, int texcoord)			 = 0;
	virtual void AddFaceNode(enum VN,  int vertex, int normal)				 = 0;
	virtual void AddFaceNode(enum V,   int vertex)							 = 0;
	//! Achtung, bei Obj sind die Vertices ab 1 indiziert (belegen zumindest meine Beispielmodelle)
	virtual void FaceDone()													 = 0;

	int CurrentLine();

	int verts_read, norms_read, texs_read;
};




#endif

