#include "pobj.h"

#include <iostream>
#include <string.h>

using namespace std;

namespace pobj {

	packed_obj_constructor::packed_obj_constructor(const std::string &filename) {
		current_block_t = none;
		outstream = stdout;
		curr_face_node_len = 0;
		Load(filename);
		ship_current_block();
	}

	void packed_obj_constructor::AddVertex(float x, float y, float z) {
		if (current_block_t != vertex) {
			ship_current_block();
			current_block_t = vertex;
		}
		accum.push_back(vec3f(x,y,z));
	}

	void packed_obj_constructor::AddTexCoord(float u, float v, float w) {
		if (current_block_t != texcoord) {
			ship_current_block();
			current_block_t = texcoord;
		}
		accum.push_back(vec3f(u,v,w));
	}

	void packed_obj_constructor::AddNormal(float x, float y, float z) {
		if (current_block_t != normal) {
			ship_current_block();
			current_block_t = normal;
		}
		accum.push_back(vec3f(x,y,z));
	}

	void packed_obj_constructor::write_out(const void *data, int size, int elems) {
		if (fwrite(data, size, elems, outstream) != elems)
			cerr << "error writing data!" << endl;
	}
	void packed_obj_constructor::AddFaceNode(int vertex, int texcoord, int normal) {
		AddFaceNode(ObjLoader::VTN, vertex, texcoord, normal);
	}

	void packed_obj_constructor::AddFaceNode(enum VTN, int vertex, int texcoord, int normal) {
		if (current_block_t != face_vtn) {
			ship_current_block();
			current_block_t = face_vtn;
		}
		curr_face_node3.push_back(vec3i(vertex, texcoord, normal));
	}

	void packed_obj_constructor::AddFaceNode(enum VT,  int vertex, int texcoord) {
		if (current_block_t != face_vt) {
			ship_current_block();
			current_block_t = face_vt;
		}
		curr_face_node2.push_back(vec2i(vertex, texcoord));
	}

	void packed_obj_constructor::AddFaceNode(enum VN,  int vertex, int normal) {
		if (current_block_t != face_vn) {
			ship_current_block();
			current_block_t = face_vn;
		}
		curr_face_node2.push_back(vec2i(vertex, normal));
	}

	void packed_obj_constructor::AddFaceNode(enum V,   int vertex) {
		if (current_block_t != face_v) {
			ship_current_block();
			current_block_t = face_v;
		}
		curr_face_node1.push_back(vertex);
	}

	void packed_obj_constructor::FaceDone() {
		int nodes;
		int size1 = curr_face_node1.size();
		int size2 = curr_face_node2.size();
		int size3 = curr_face_node3.size();
		if (size1) nodes = size1;
		if (size2) nodes = size2;
		if (size3) nodes = size3;
		if (curr_face_node_len > 0 && curr_face_node_len != nodes) {
			block_t tmp = current_block_t;
			ship_current_block();
			current_block_t = tmp;
		}
		curr_face_node_len = nodes;
		if (nodes == size1)
			face_node_accum1.insert(face_node_accum1.end(), curr_face_node1.begin(), curr_face_node1.end());
		if (nodes == size2)
			face_node_accum2.insert(face_node_accum2.end(), curr_face_node2.begin(), curr_face_node2.end());
		if (nodes == size3)
			face_node_accum3.insert(face_node_accum3.end(), curr_face_node3.begin(), curr_face_node3.end());
		curr_face_node1.clear();
		curr_face_node2.clear();
		curr_face_node3.clear();
	}

	bool packed_obj_constructor::CurrentMaterial(const std::string &name) {
		ship_current_block();
		current_block_t = usemtl;
		current_name = name;
	}

	void packed_obj_constructor::StartGroup(const std::string &name) {
		ship_current_block();
		current_block_t = group;
		current_name = name;
	}

	bool packed_obj_constructor::LoadMaterialFile(const std::string &name) {
		ship_current_block();
		current_block_t = mtllib;
		current_name = name;
	}

	void packed_obj_constructor::ship_current_block() {
		char c;
		unsigned int n;
		void *p;
		int elems;
		switch (current_block_t) {
			case none: 
				break;

			case vertex:
			case texcoord:
			case normal:
				c = (char)current_block_t;
				write_out(&c, 1, 1);
				n = accum.size();
				write_out(&n, sizeof(unsigned int), 1);
				write_out(&accum[0], sizeof(float), 3*accum.size());
				accum.clear();
				break;

			case mtllib:
			case usemtl:
			case group:
			case object:
				c = (char)current_block_t;
				write_out(&c, 1, 1);
				n = current_name.length();
				write_out(&n, sizeof(unsigned int), 1);
				write_out(current_name.c_str(), 1, n+1);	// we include the terminating 0.
				current_name = "";
				break;

			case face_v:
			case face_vn:
			case face_vt:
			case face_vtn:
				c = (char)current_block_t;
				write_out(&c, 1, 1);
				if (current_block_t == face_v)        n = face_node_accum1.size(), p = &face_node_accum1[0], elems = 1;
				else if (current_block_t == face_vtn) n = face_node_accum3.size(), p = &face_node_accum3[0], elems = 3;
				else                                  n = face_node_accum2.size(), p = &face_node_accum2[0], elems = 2;
				write_out(&n, sizeof(unsigned int), 1);
				write_out(&curr_face_node_len, sizeof(unsigned int), 1);
				write_out(p, sizeof(int), n * elems);
				curr_face_node_len = 0;
				face_node_accum1.clear();
				face_node_accum2.clear();
				face_node_accum3.clear();
				break;
		}
		current_block_t = none;
	}



	class debug_pobj_loader : public ObjLoader {
	public:
		virtual void AddVertex(float x, float y, float z) {
			cout << "v " << x << " " << y << " " << z << endl;
		}
		virtual void AddTexCoord(float u, float v, float w) {
			cout << "vt " << u << " " << v << " " << w << endl;
		}
		virtual void AddNormal(float x, float y, float z) {
			cout << "vn " << x << " " << y << " " << z << endl;
		}
		virtual void AddFaceNode(int vertex, int texcoord, int normal) {
			cout << " " << vertex << "/" << texcoord << "/" << normal;
		}
		virtual void AddFaceNode(enum VTN, int vertex, int texcoord, int normal) {
			cout << " " << vertex << "/" << texcoord << "/" << normal;
		}
		virtual void AddFaceNode(enum VT,  int vertex, int texcoord) {
			cout << " " << vertex << "/" << texcoord;
		}
		virtual void AddFaceNode(enum VN,  int vertex, int normal) {
			cout << " " << vertex << "//" << normal;
		}
		virtual void AddFaceNode(enum V,   int vertex) {
			cout << " " << vertex;
		}
		virtual void FaceDone() {
			cout << endl;
		}
		virtual bool CurrentMaterial(const std::string &name) {
			cout << "usemtl " << name << endl;
		} 
		virtual void StartGroup(const std::string &name) {
			cout << "g " << name << endl;
		}
		virtual void ObjectName(const std::string &name) {
			cout << "o " << name << endl;
		}
		virtual bool LoadMaterialFile(const std::string &name) {
			cout << "mtllib " << name << endl;
		}
	};

	debug_pobj_loader debug;

	packed_obj_loader::packed_obj_loader(const std::string &filename) : fp(0) {
		fp = fopen(filename.c_str(), "rb");
		if (!fp)
			fprintf(stderr, "Cannot open file '%s'\n", filename.c_str());
		loop();
	}
	
	void packed_obj_loader::read(void *to, int size, int elems) {
		if (fread(to, size, elems, fp) != elems)
			if (!(size * elems == 1 && feof(fp)))
				fprintf(stderr, "Error readign from file\n");
	}

	int packed_obj_loader::read_name(char *name) {
		int n;
		read(&n, sizeof(unsigned int), 1);
		if (n > 4096) {
			fprintf(stderr, "Too long names in pobj file.\n");
			throw "Too long names in pobj file.";
		}
		read(name, 1, n+1);
		return n;
	}
	
	int packed_obj_loader::read_vertices(vector<vec3f> &verts) {
		int n;
		read(&n, sizeof(unsigned int), 1);
		if (verts.capacity() < n)
			verts.resize(n);
		read(&verts[0], sizeof(float), 3*n);
		return n;
	}

	template<typename E> int packed_obj_loader::read_faces(vector<E> &vect, int elems, int &verts_per_face) {
		int n;
		read(&n, sizeof(unsigned int), 1);
		read(&verts_per_face, sizeof(unsigned int), 1);
		if (vect.capacity() < n)
			vect.resize(n);
		read(&vect[0], sizeof(int), n * elems);
		return n;
	}

	void packed_obj_loader::loop() {
		char code;
		char name[4096];
		int n;
		int m;
		vector<vec3f> vertices;
		vector<int> faces1;
		vector<vec2i> faces2;
		vector<vec3i> faces3;

		cout.precision(4);
		cout.setf(std::ios::fixed, std::ios::floatfield);

		while (true) {
			read(&code, 1, 1);
// 			cout << "code = " << (int)code << endl;
			if (feof(fp))
				break;
			switch (code) {
				case none: break;
				case mtllib:
					n = read_name(name);
					debug.LoadMaterialFile(name);
					break;
				case usemtl:
					n = read_name(name);
					debug.CurrentMaterial(name);
					break;
				case group:
					n = read_name(name);
					debug.StartGroup(name);
					break;
				case object:
					n = read_name(name);
					debug.ObjectName(name);
					break;
				case vertex:
					n = read_vertices(vertices);
					for (int i = 0; i < n; ++i)
						debug.AddVertex(vertices[i].x, vertices[i].y, vertices[i].z);
					break;
				case normal:
					n = read_vertices(vertices);
					for (int i = 0; i < n; ++i)
						debug.AddNormal(vertices[i].x, vertices[i].y, vertices[i].z);
					break;
				case texcoord:
					n = read_vertices(vertices);
					for (int i = 0; i < n; ++i)
						debug.AddTexCoord(vertices[i].x, vertices[i].y, vertices[i].z);
					break;
				case face_v:
					cout << "f";
					n = read_faces(faces1, 1, m);
					for (int i = 0; i < n; ++i) {
						debug.AddFaceNode(ObjLoader::V, faces1[i]);
						if (i != 0 && i % m == 0)
							debug.FaceDone();
					}
					break;
				case face_vn:
					cout << "f";
					n = read_faces(faces2, 2, m);
					for (int i = 0; i < n; ++i) {
						debug.AddFaceNode(ObjLoader::VN, faces2[i].x, faces2[i].y);
						if ((i+1) % m == 0) {
							debug.FaceDone();
							if (i+1 < n)
								cout << "f";
						}
					}
					break;
				case face_vt:
					cout << "f";
					n = read_faces(faces2, 2, m);
					for (int i = 0; i < n; ++i) {
						debug.AddFaceNode(ObjLoader::VT, faces2[i].x, faces2[i].y);
						if ((i+1) % m == 0) {
							debug.FaceDone();
							if (i+1 < n)
								cout << "f";
						}
					}
					break;
				case face_vtn:
					cout << "f";
					n = read_faces(faces3, 3, m);
					for (int i = 0; i < n; ++i) {
// 						cout << "(" << i << ") ";
						debug.AddFaceNode(ObjLoader::VTN, faces3[i].x, faces3[i].y, faces3[i].z);
						if ((i+1) % m == 0) {
							debug.FaceDone();
							if (i+1 < n)
								cout << "f";
						}
					}
					break;
			}
		}
	}
}


/* vim: set foldmethod=marker: */

