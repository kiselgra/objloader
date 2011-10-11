/* $Id$ */
#include "default.h"

#include <iostream>

using namespace std;

namespace obj_default 
{
	ObjFileLoader::ObjFileLoader(const std::string &filename, const std::string &trafo)
	: curr_face_node(0), curr_mtl(0)
	{
		if (trafo != "")
			this->trafo = lib3dmath::strtomat4f(trafo);
		groups.push_back(Group());
		Load(filename);
	}

	ObjFileLoader::ObjFileLoader(const std::string &filename, const lib3dmath::mat4f &trafo)
	: curr_face_node(0), curr_mtl(0)
	{
		this->trafo = trafo;
		groups.push_back(Group());
		Load(filename);
	}	

	ObjFileLoader::ObjFileLoader(FakeMode fake, const std::string &trafo)
	: curr_face_node(0), curr_mtl(0)
	{
		if (trafo != "")
			this->trafo = lib3dmath::strtomat4f(trafo);
		groups.push_back(Group());
	}

	ObjFileLoader::~ObjFileLoader()
	{
		
	}

	void ObjFileLoader::AddVertex(float x, float y, float z)
	{
		load_verts.push_back(lib3dmath::vec3f(x,y,z));
	}

	void ObjFileLoader::AddTexCoord(float u, float v, float w)
	{
		load_texs.push_back(lib3dmath::vec3f(u,v,w));
	}

	void ObjFileLoader::AddNormal(float x, float y, float z)
	{
		load_norms.push_back(lib3dmath::vec3f(x,y,z));
	}
		
	void ObjFileLoader::AddFaceNode(int vertex, int texcoord, int normal)
	{
		if (curr_face_node == 4)
		{
			cerr << "Only triangle and quad meshes supported!" << endl;
			return;
		}

		if (texcoord < -1) cout << obj_filename << ": face with texcoord = " << texcoord << endl;

		face_nodes[curr_face_node].x = vertex;
		face_nodes[curr_face_node].y = texcoord;
		face_nodes[curr_face_node].z = normal;
		curr_face_node++;
	}

	void ObjFileLoader::AddFaceNode(enum VTN, int vertex, int texcoord, int normal)
	{
		AddFaceNode(vertex, texcoord, normal);
	}

	void ObjFileLoader::AddFaceNode(enum VT, int vertex, int texcoord)
	{
		AddFaceNode(vertex, texcoord, -1);
	}

	void ObjFileLoader::AddFaceNode(enum VN, int vertex, int normal)
	{
		AddFaceNode(vertex, -1, normal);
	}

	void ObjFileLoader::AddFaceNode(enum V, int vertex)
	{
		AddFaceNode(vertex, -1, -1);
	}

	void ObjFileLoader::FaceDone()
	{
		groups.back().load_idxs_v.push_back(lib3dmath::vec3i(face_nodes[0].x-1, face_nodes[1].x-1, face_nodes[2].x-1));
		groups.back().load_idxs_t.push_back(lib3dmath::vec3i(face_nodes[0].y-1, face_nodes[1].y-1, face_nodes[2].y-1));
		groups.back().load_idxs_n.push_back(lib3dmath::vec3i(face_nodes[0].z-1, face_nodes[1].z-1, face_nodes[2].z-1));
		if (curr_face_node == 4) { // add second triangle, for quads
			groups.back().load_idxs_v.push_back(lib3dmath::vec3i(face_nodes[2].x-1, face_nodes[3].x-1, face_nodes[0].x-1));
			groups.back().load_idxs_t.push_back(lib3dmath::vec3i(face_nodes[2].y-1, face_nodes[3].y-1, face_nodes[0].y-1));
			groups.back().load_idxs_n.push_back(lib3dmath::vec3i(face_nodes[2].z-1, face_nodes[3].z-1, face_nodes[0].z-1));
		}
		curr_face_node = 0;

	}
		
	bool ObjFileLoader::CurrentMaterial(const std::string &name)
	{
		map<string, Mtl*>::iterator it = materials.find(name);
		if (it == materials.end())
			return false;


		if (groups.back().load_idxs_v.size() || groups.back().load_idxs_t.size() || groups.back().load_idxs_n.size())
		{
			cerr << "cannot set material of a group after the first face is read." << endl;
			return false;
		}

		curr_mtl = it->second;
		groups.back().mat = curr_mtl;

		return true;
	}

	void ObjFileLoader::StartGroup(const std::string &name)
	{
		if (groups.back().load_idxs_v.size() != 0)
			groups.push_back(Group());
		groups.back().name = name;
		groups.back().mat = curr_mtl;
	}

}

/* vim: set foldmethod=marker: */

