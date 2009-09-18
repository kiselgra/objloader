/* $Id$ */
#ifndef __OBJFILE_H__ 
#define __OBJFILE_H__ 

#include <string>
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
		std::string tex_a, tex_d, tex_s;
		int illum_model;
	};
	std::map<std::string, Mtl*> materials;

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
};




#endif

