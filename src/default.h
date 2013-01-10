#ifndef __DEFAULT_H__ 
#define __DEFAULT_H__ 

#include <string>
#include <vector>
#include <map>
#include <list>

#include <lib3dmath/matrix.h>
#include "objloader.h"

namespace obj_default 
{
	class ObjFileLoader : public ObjLoader
	{
	public:
		struct Group
		{
			std::string name;
			std::vector<lib3dmath::vec3i> load_idxs_v;
			std::vector<lib3dmath::vec3i> load_idxs_n;
			std::vector<lib3dmath::vec3i> load_idxs_t;
			Mtl *mat;
			bool mat_set_explicitly;
		};
		enum FakeMode { FAKE };

		//! beim einladen dynamisch
		std::vector<lib3dmath::vec3f> 	load_verts;
		std::vector<lib3dmath::vec3f> 	load_norms;
		std::vector<lib3dmath::vec3f> 	load_texs;
		std::list<Group>	groups;
		int curr_face_node;
		lib3dmath::vec3i face_nodes[4];

		Mtl *curr_mtl;

		bool create_mats;
		bool create_des;
		std::string mat_name;
		lib3dmath::mat4f trafo;

		ObjFileLoader(const std::string &filename, const std::string &trafo);
		ObjFileLoader(const std::string &filename, const lib3dmath::mat4f &trafo);
		ObjFileLoader(FakeMode fake, const std::string &trafo);
		~ObjFileLoader();
		
		void AddVertex(float x, float y, float z);
		void AddTexCoord(float u, float v, float w);
		void AddNormal(float x, float y, float z);
		
		void AddFaceNode(int vertex, int texcoord, int normal);
		void AddFaceNode(enum VTN, int vertex, int texcoord, int normal);
		void AddFaceNode(enum VT,  int vertex, int texcoord);
		void AddFaceNode(enum VN,  int vertex, int normal);
		void AddFaceNode(enum V,   int vertex);
		void FaceDone();
		
		bool CurrentMaterial(const std::string &name);
		void PushMaterial(const Mtl &m);
		void StartGroup(const std::string &name);

		/*! the v/t/n specification found in some obj files is not generally in gl-compatible n/n/n but the most compact n/m/o form.
		 *  this function is called `infalte' because it does restore the gl-compatible layout.
		 */
		void Inflate();
		std::map<Mtl*, std::list<Group*> > groups_by_material;
		void CollapseMaterials();

		static const int NOT_PRESENT = -2;
	};

}

#endif
