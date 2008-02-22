/* $Id$ */
#ifndef __OBJFILE_LOGGER_H__ 
#define __OBJFILE_LOGGER_H__ 

#include "objloader.h"

#include <iostream>

/*!	Beispiel und Kontroll-Implementierung.
 *
 * 	Gibt die einzelnen Funktionsaufrufe und dazugehörigen Parameter aus, sobald sie aus dem Parser aufgerufen werden.
 */
class ObjLogger : public ObjLoader
{
public:
	ObjLogger() {};
	~ObjLogger() {};

	virtual bool LoadMaterialFile(const std::string &name) 						{	std::cout << "Load material file " << name  << std::endl;	}
	virtual bool CurrentMaterial(const std::string &name) 						{	std::cout << "Set material to " << name  << std::endl;	}
	
	virtual void AddVertex(float x, float y, float z) 							{	std::cout << "AddVertex    " << x << "  " << y << "  " << z   << std::endl;	}
	virtual void AddTexCoord(float u, float v, float w = 0) 					{	std::cout << "AddTexCoord  " << u << "  " << v << "  " << w   << std::endl;	}
	virtual void AddNormal(float x, float y, float z)							{	std::cout << "AddNormal    " << x << "  " << y << "  " << z   << std::endl;	}

	virtual void StartGroup(const std::string &name)							{	std::cout << "Group " << name << std::endl; }
	
	virtual void AddFaceNode(enum VTN, int vertex, int texcoord, int normal)	{	std::cout << "FaceNode:   v=" << vertex << "  t=" << texcoord << "  n=" << normal  << std::endl;	}
	virtual void AddFaceNode(enum VT,  int vertex, int texcoord)				{	std::cout << "FaceNode:   v=" << vertex << "  t=" << texcoord << std::endl;	}
	virtual void AddFaceNode(enum VN,  int vertex, int normal)					{	std::cout << "FaceNode:   v=" << vertex << "  n=" << normal   << std::endl;	}
	virtual void AddFaceNode(enum V,   int vertex)								{	std::cout << "FaceNode:   v=" << vertex << std::endl;	}
	virtual void FaceDone()														{	std::cout << "FaceDone" << std::endl;	}
};


#endif

