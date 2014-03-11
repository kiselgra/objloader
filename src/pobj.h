#ifndef __POBJ_H__ 
#define __POBJ_H__ 

#include <string>
#include <vector>
#include <map>
#include <list>

#include <libmcm/vectors.h>

#include "objloader.h"

namespace pobj {
	enum block_t {
		none,
		mtllib,
		usemtl,
		group,
		object,
		smooth, /* not used */
		vertex,
		normal,
		texcoord,
		face_v,
		face_vn,
		face_vt,
		face_vtn,
	};
	struct name_block {
		int len;
		char *name;
	};
	struct vertex_block {
		int verts;
		float *data;
	};
	union data_block {
		struct name_block name_block;
		struct vertex_block vertex_block;
	};
	struct block {
		block_t type;
		data_block data;
	};

	class packed_obj_constructor : public ObjLoader {
		block_t current_block_t;

		void ship_current_block();
		std::vector<vec3f> accum;
		std::string current_name;

		std::vector<vec3i> curr_face_node3;
		std::vector<vec2i> curr_face_node2;
		std::vector<int> curr_face_node1;
		int curr_face_node_len;
		std::vector<vec3i> face_node_accum3;
		std::vector<vec2i> face_node_accum2;
		std::vector<int> face_node_accum1;

		FILE *outstream;
		void write_out(const void *data, int size, int elems);

	public:
		packed_obj_constructor(const std::string &filename);
		virtual void AddVertex(float x, float y, float z);
		virtual void AddTexCoord(float u, float v, float w);
		virtual void AddNormal(float x, float y, float z);
		
		virtual void AddFaceNode(int vertex, int texcoord, int normal);
		virtual void AddFaceNode(enum VTN, int vertex, int texcoord, int normal);
		virtual void AddFaceNode(enum VT,  int vertex, int texcoord);
		virtual void AddFaceNode(enum VN,  int vertex, int normal);
		virtual void AddFaceNode(enum V,   int vertex);
		virtual void FaceDone();
			
		virtual bool CurrentMaterial(const std::string &name);
		virtual void StartGroup(const std::string &name);
		
		virtual bool LoadMaterialFile(const std::string &name);
	};

	class packed_obj_loader {
		FILE *fp;
		int read_name(char *to);
		void read(void *to, int size, int elems);
		int read_vertices(std::vector<vec3f> &verts);
		template<typename E> int read_faces(std::vector<E> &vect, int elems, int &verts_per_face);
		void loop();
	public:
		packed_obj_loader(const std::string &filename);
	};
}

#endif

