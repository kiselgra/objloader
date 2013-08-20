/* $Id$ */
#include "default.h"

#include <iostream>
#include <map>
#include <list>
#include <unordered_map>

#include <cstring>
#include <float.h>
#include <libgen.h>	// basename

using namespace std;
		
namespace obj_default {
	struct triplet {
		triplet(uint32_t v, uint32_t t, uint32_t n) : v(v), t(t), n(n) {}
		bool operator==(const triplet &o) const {
			return v == o.v && t == o.t && n == o.n;
		}
		uint32_t v, t, n;
	};
}

namespace std {
	// http://stackoverflow.com/questions/7222143/unordered-map-hash-function-c, apparently from boost
	template<typename T> inline void hash_combine(std::size_t &seed, const T &v)
	{
		std::hash<T> hasher;
		seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}
	template<> struct hash<obj_default::triplet>
	{
		inline size_t operator()(const obj_default::triplet &v) const
		{
			size_t seed = 0;
			::hash_combine(seed, v.v);
			::hash_combine(seed, v.t);
			::hash_combine(seed, v.n);
			return seed;
		}
	};
}

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
			cout << "Warning: change of material inside of group." << endl;
			if (groups.back().mat_set_explicitly) {
				cout << "Making new group." << endl;
				StartGroup(groups.back().name + ":" + name + "*");
				CurrentMaterial(name);
				return true;
			}
			else
				cout << "Changing material of current group (because it was not set explicitly, yet." << endl;
		}

		curr_mtl = it->second;
		groups.back().mat = curr_mtl;
		groups.back().mat_set_explicitly = true;

		return true;
	}

	void fix_filename(std::string &str) 
	{
		if (str != "" && (str[0] == '/' || strstr(str.c_str(), "\\")))
		{
			char *tmp = strdup(str.c_str());
			char *base = basename(tmp);
			string old = str;
			str = base;
			free(tmp);
			if (old != str)
				cerr << "had to convert strange filename '" << old << "' to simple filename '" << str << "'" << endl;
		}
	}

	void ObjFileLoader::PushMaterial(const Mtl &m)
	{
		Mtl newm = m;
		fix_filename(newm.tex_a);
		fix_filename(newm.tex_d);
		fix_filename(newm.tex_s);
		fix_filename(newm.tex_bump);
		fix_filename(newm.tex_alpha);
		ObjLoader::PushMaterial(newm);
	}

	void ObjFileLoader::StartGroup(const std::string &name)
	{
		if (groups.back().load_idxs_v.size() != 0)
			groups.push_back(Group());
		groups.back().name = name;
		groups.back().mat = curr_mtl;
		groups.back().mat_set_explicitly = false;
	}

	uint32_t add_vtn(unordered_map<triplet, uint32_t> &umap, const triplet &t, uint32_t &counter, 
	                 vector<lib3dmath::vec3f> &nv, vector<lib3dmath::vec3f> &nt, vector<lib3dmath::vec3f> &nn,
	                 vector<lib3dmath::vec3f> &ov, vector<lib3dmath::vec3f> &ot, vector<lib3dmath::vec3f> &on) {
		if (umap.find(t) == umap.end()) {
			nv.push_back(ov[t.v]);
			if (ot.size() > t.t) nt.push_back(ot[t.t]);
			else                 nt.push_back(lib3dmath::vec3f(0,0,0));
			if (on.size() > t.n) nn.push_back(on[t.n]);
			else                 nn.push_back(lib3dmath::vec3f(0,0,0));
			umap[t] = counter;
			return counter++;
		}
		else
			return umap[t];
	}

	void ObjFileLoader::Inflate()
	{
		typedef lib3dmath::vec3f vec3f;
		typedef lib3dmath::vec3i vec3i;

		// sort groups by material
		std::map<Mtl*, std::list<Group*> > groups_by_material;
		for (auto &g : groups)
			groups_by_material[g.mat].push_back(&g);

		// generate clear vtn buffers
		vector<vec3f> new_v, new_t, new_n;
		unordered_map<triplet, uint32_t> vtn_map;
		uint32_t counter = 0;
		vec3i null(0,0,0);

		// go over all groups
		for (auto &it : groups_by_material)
			for (auto *g : it.second) {
				// storage for new indices (beause the source data my not be uniformly allocated)
				vector<vec3i> new_idx_v, new_idx_t, new_idx_n;
				new_idx_v.reserve(g->load_idxs_v.size());
				new_idx_t.reserve(g->load_idxs_v.size()); // v is right here, all arrays will be of equal length
				new_idx_n.reserve(g->load_idxs_v.size()); // v is right here, all arrays will be of equal length
				// go over all referenced indices
				auto load_idxs_v = g->load_idxs_v.begin();
				auto load_idxs_t = g->load_idxs_t.begin();
				auto load_idxs_n = g->load_idxs_n.begin();
				while (load_idxs_v != g->load_idxs_v.end()) {
					vec3i v = *load_idxs_v;
					vec3i t = (load_idxs_t != g->load_idxs_t.end()) ? *load_idxs_t : null;
					vec3i n = (load_idxs_n != g->load_idxs_n.end()) ? *load_idxs_n : null;
					triplet a(v.x, t.x, n.x);
					triplet b(v.y, t.y, n.y);
					triplet c(v.z, t.z, n.z);
					// check if vtn-index combination has an index already
					// use it, or generate a new vtn entry
					uint32_t new_index_a = add_vtn(vtn_map, a, counter, new_v, new_t, new_n, load_verts, load_texs, load_norms);
					uint32_t new_index_b = add_vtn(vtn_map, b, counter, new_v, new_t, new_n, load_verts, load_texs, load_norms);
					uint32_t new_index_c = add_vtn(vtn_map, c, counter, new_v, new_t, new_n, load_verts, load_texs, load_norms);
					new_idx_v.push_back(vec3i(new_index_a, new_index_b, new_index_c));
					new_idx_t.push_back(vec3i(new_index_a, new_index_b, new_index_c));
					new_idx_n.push_back(vec3i(new_index_a, new_index_b, new_index_c));
					// create new index data
					load_idxs_v++;
					if (load_idxs_t != g->load_idxs_t.end()) load_idxs_t++;
					if (load_idxs_n != g->load_idxs_n.end()) load_idxs_n++;
				}
				// swap index data
				g->load_idxs_v.swap(new_idx_v);
				g->load_idxs_t.swap(new_idx_t);
				g->load_idxs_n.swap(new_idx_n);
			}

		// swap vertex array data
		load_verts.swap(new_v);
		load_texs.swap(new_t);
		load_norms.swap(new_n);
	}

	typedef pair<lib3dmath::vec3f, lib3dmath::vec3f> bb_t;

	bb_t compute_bb(const vector<lib3dmath::vec3f> &load_verts, const vector<lib3dmath::vec3i> load_idxs_v) {
		lib3dmath::vec3f min(FLT_MAX, FLT_MAX, FLT_MAX), max(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		for (int i = 0; i < load_idxs_v.size(); ++i) {
			const lib3dmath::vec3f *v = &load_verts[load_idxs_v[i].x];
			if (v->x < min.x) min.x = v->x;  if (v->x > max.x) max.x = v->x;
			if (v->y < min.y) min.y = v->y;  if (v->y > max.y) max.y = v->y;
			if (v->z < min.z) min.z = v->z;  if (v->z > max.z) max.z = v->z;
			v = &load_verts[load_idxs_v[i].y];                            
			if (v->x < min.x) min.x = v->x;  if (v->x > max.x) max.x = v->x;
			if (v->y < min.y) min.y = v->y;  if (v->y > max.y) max.y = v->y;
			if (v->z < min.z) min.z = v->z;  if (v->z > max.z) max.z = v->z;
			v = &load_verts[load_idxs_v[i].z];                            
			if (v->x < min.x) min.x = v->x;  if (v->x > max.x) max.x = v->x;
			if (v->y < min.y) min.y = v->y;  if (v->y > max.y) max.y = v->y;
			if (v->z < min.z) min.z = v->z;  if (v->z > max.z) max.z = v->z;
		}
		return make_pair(min, max);
	}

	bb_t merge_bb(const bb_t &a, const bb_t &b) {
		bb_t m;
		m.first.x = min(a.first.x, b.first.x);
		m.first.y = min(a.first.y, b.first.y);
		m.first.z = min(a.first.z, b.first.z);
		m.second.x = max(a.second.x, b.second.x);
		m.second.y = max(a.second.y, b.second.y);
		m.second.z = max(a.second.z, b.second.z);
		return m;
	}

	lib3dmath::vec3f bb_diam(const bb_t &bb) {
		return lib3dmath::vec3f(bb.second.x - bb.first.x,
		                        bb.second.y - bb.first.y,
		                        bb.second.z - bb.first.z);
	}

	void ObjFileLoader::CollapseMaterials(float f)
	{
		std::map<Mtl*, std::list<Group*> > groups_by_material;
		for (auto &g : groups)
			groups_by_material[g.mat].push_back(&g);

// 		bool just_paste = true;
		bool just_paste = false;
		if (just_paste) {
			for (auto &it : groups_by_material)
				if (it.second.size() > 1) {
					// only for collapsable lists
					Group *base = it.second.front();
					list<Group*>::iterator git = it.second.begin()++;
					while (git != it.second.end()) {
						Group *g = *git;
						base->load_idxs_v.insert(base->load_idxs_v.end(), g->load_idxs_v.begin(), g->load_idxs_v.end());
						base->load_idxs_n.insert(base->load_idxs_n.end(), g->load_idxs_n.begin(), g->load_idxs_n.end());
						base->load_idxs_t.insert(base->load_idxs_t.end(), g->load_idxs_t.begin(), g->load_idxs_t.end());
						git++;
					}
					base->name += "**";
					while (it.second.size() > 1) {
						Group *g = it.second.back();
						for (list<Group>::iterator iit = groups.begin(); iit != groups.end(); ++iit)
							if (&*iit == g) {
								groups.erase(iit);
								break;
							}
						// delete?
						it.second.pop_back();
					}
				}
		}
		else {
			typedef lib3dmath::vec3f vec3f;
			float dir_expansion = f;
			// look at all clusters
			for (auto &by_material : groups_by_material) {
				cout << "reducing groups of material " << by_material.first->name << "..." << endl;
				bool merged = true;
				while (merged && by_material.second.size() > 1) {
					merged = false;
					// find a group of the cluster to merge to the first group
					Group *base = by_material.second.front();
					cout << "    the base group is " << base->name << endl;
					cout << "        (";
					for (auto c : by_material.second) cout << c->name << " ";
					cout << ")" << endl;
					// find bb
					bb_t base_bb = compute_bb(load_verts, base->load_idxs_v);
					vec3f base_diam = bb_diam(base_bb);
					list<Group*>::iterator git = ++by_material.second.begin();
					while (git != by_material.second.end()) {
						Group *g = *git;
						cout << "    candidate: " << g->name << endl;
						bb_t this_bb = compute_bb(load_verts, g->load_idxs_v);
						vec3f diam = bb_diam(merge_bb(base_bb, this_bb));
						if (diam.x < dir_expansion * base_diam.x &&
						    diam.y < dir_expansion * base_diam.y &&
						    diam.z < dir_expansion * base_diam.z) {
							cout << "    merge!" << endl;
							merged = true;
							// if the bb's expansion is not too bad we integrate the boxes.
							base->load_idxs_v.insert(base->load_idxs_v.end(), g->load_idxs_v.begin(), g->load_idxs_v.end());
							base->load_idxs_n.insert(base->load_idxs_n.end(), g->load_idxs_n.begin(), g->load_idxs_n.end());
							base->load_idxs_t.insert(base->load_idxs_t.end(), g->load_idxs_t.begin(), g->load_idxs_t.end());
							// find merged-in entry in global group table and remove it.
							for (list<Group>::iterator iit = groups.begin(); iit != groups.end(); ++iit)
								if (&*iit == g) {
									groups.erase(iit);
									break;
								}
							// remove merged-in entry from working set.
							git = by_material.second.erase(git);
						}
						else
							git++;
					}
				}
			}
		}
	}
}

/* vim: set foldmethod=marker: */

