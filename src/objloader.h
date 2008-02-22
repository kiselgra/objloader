/* $Id$ */
#ifndef __OBJFILE_H__ 
#define __OBJFILE_H__ 

#include <string>


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
public:
	enum VTN { VTN };
	enum VN  { VN };
	enum VT  { VT };
	enum V   { V };

	ObjLoader();
	~ObjLoader();

	virtual bool Load(const std::string name);
	
	virtual bool LoadMaterialFile(const std::string &name) {}
	virtual bool CurrentMaterial(const std::string &name) {}

	virtual void StartGroup(const std::string &name) {}

	virtual void AddVertex(float x, float y, float z) 		= 0;
	virtual void AddTexCoord(float u, float v, float w = 0) = 0;
	virtual void AddNormal(float x, float y, float z)		= 0;

	virtual void AddFaceNode(enum VTN, int vertex, int texcoord, int normal) = 0;
	virtual void AddFaceNode(enum VT,  int vertex, int texcoord)			 = 0;
	virtual void AddFaceNode(enum VN,  int vertex, int normal)				 = 0;
	virtual void AddFaceNode(enum V,   int vertex)							 = 0;
	virtual void FaceDone()													 = 0;

	int CurrentLine();
};




#endif

