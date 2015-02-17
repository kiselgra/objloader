#include "default.h"

#include <iostream>
#include <stdexcept>
#include <map>
#include <list>
#include <unordered_map>
#include <fstream>

#include <cstring>
#include <float.h>
#include <libgen.h>	// basename
#include <unistd.h> // access

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

using namespace std;
		
extern ObjLoader *yyobj;

namespace obj_default {
	struct triplet {
		triplet(uint32_t v, uint32_t t, uint32_t n) : v(v), t(t), n(n) {}
		bool operator==(const triplet &o) const {
			return v == o.v && t == o.t && n == o.n;
		}
		uint32_t v, t, n;
	};
}

matrix4x4f strtomat4f(const std::string &s)
{
	matrix4x4f m;
	if (!s.size()) {
		make_unit_matrix4x4f(&m);
		return m;
	}

	const char *p = s.c_str();
	char *n;
	matrix4x4f mat;

	mat.col_major[0+4*0] = strtod(p, &n);
	mat.col_major[0+4*1] = strtod(n, &n);
	mat.col_major[0+4*2] = strtod(n, &n);
	mat.col_major[0+4*3] = strtod(n, &n);

	for (int y = 1; y < 4; ++y)
		for (int x = 0; x < 4; ++x)
			mat.col_major[y+4*x] = strtod(n, &n);

	return mat;
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
	: curr_face_node(0), curr_mtl(0), inflated(false), collapsed(false)
	{
		if (trafo != "")
			this->trafo = strtomat4f(trafo);
		else
			make_unit_matrix4x4f(&this->trafo);
		matrix4x4f tmp;
		invert_matrix4x4f(&tmp, &this->trafo);
		transpose_matrix4x4f(&normal_trafo, &tmp);
		groups.push_back(Group());
		Load(filename);
	}

	ObjFileLoader::ObjFileLoader(const std::string &filename, const matrix4x4f &trafo)
	: curr_face_node(0), curr_mtl(0), inflated(false), collapsed(false)
	{
		this->trafo = trafo;
		matrix4x4f tmp;
		invert_matrix4x4f(&tmp, &this->trafo);
		transpose_matrix4x4f(&normal_trafo, &tmp);
		groups.push_back(Group());
		Load(filename);
	}	

	ObjFileLoader::ObjFileLoader(FakeMode fake, const std::string &trafo)
	: curr_face_node(0), curr_mtl(0), inflated(false), collapsed(false)
	{
		if (trafo != "")
			this->trafo = strtomat4f(trafo);
		else
			make_unit_matrix4x4f(&this->trafo);
		matrix4x4f tmp;
		invert_matrix4x4f(&tmp, &this->trafo);
		transpose_matrix4x4f(&normal_trafo, &tmp);
		groups.push_back(Group());
	}

	ObjFileLoader::~ObjFileLoader()
	{
		
	}
		
	bool ObjFileLoader::Load(const std::string name) {
		if (access(name.c_str(), F_OK) != 0) {
			cerr << "cannot open " << name << endl;
			return false;
		}
		ifstream test(name.c_str());
		string word;
		test >> word;
		test.close();
// 		cout << word << endl;
		if (word == "pobj/libobjloader") {
			// some initialization that would have been done in the ObjLoader c'tor.
			obj_filename = name;	
			yyobj = this;
			return LoadBinaryObj(name);
		}
		else {
			return ObjLoader::Load(name);
		}
	}

	void ObjFileLoader::AddVertex(float x, float y, float z)
	{
		vec4f v(x, y, z, 1);
		vec4f out;
		multiply_matrix4x4f_vec4f(&out, &trafo, &v);
		load_verts.push_back(vec3f(out.x,out.y,out.z));
	}

	void ObjFileLoader::AddTexCoord(float u, float v, float w)
	{
		load_texs.push_back(vec3f(u,v,w));
	}

	void ObjFileLoader::AddNormal(float x, float y, float z)
	{
		vec4f v(x, y, z, 0);
		vec4f out;
		multiply_matrix4x4f_vec4f(&out, &normal_trafo, &v);
		load_norms.push_back(vec3f(out.x,out.y,out.z));
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
		groups.back().load_idxs_v.push_back(vec3i(face_nodes[0].x-1, face_nodes[1].x-1, face_nodes[2].x-1));
		groups.back().load_idxs_t.push_back(vec3i(face_nodes[0].y-1, face_nodes[1].y-1, face_nodes[2].y-1));
		groups.back().load_idxs_n.push_back(vec3i(face_nodes[0].z-1, face_nodes[1].z-1, face_nodes[2].z-1));
		if (curr_face_node == 4) { // add second triangle, for quads
			groups.back().load_idxs_v.push_back(vec3i(face_nodes[2].x-1, face_nodes[3].x-1, face_nodes[0].x-1));
			groups.back().load_idxs_t.push_back(vec3i(face_nodes[2].y-1, face_nodes[3].y-1, face_nodes[0].y-1));
			groups.back().load_idxs_n.push_back(vec3i(face_nodes[2].z-1, face_nodes[3].z-1, face_nodes[0].z-1));
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
	                 vector<vec3f> &nv, vector<vec3f> &nt, vector<vec3f> &nn,
	                 vector<vec3f> &ov, vector<vec3f> &ot, vector<vec3f> &on) {
		if (umap.find(t) == umap.end()) {
			nv.push_back(ov[t.v]);
			if (ot.size() > t.t) nt.push_back(ot[t.t]);
			else                 nt.push_back(vec3f(0,0,0));
			if (on.size() > t.n) nn.push_back(on[t.n]);
			else                 nn.push_back(vec3f(0,0,0));
			umap[t] = counter;
			return counter++;
		}
		else
			return umap[t];
	}

	void ObjFileLoader::Inflate()
	{
		typedef vec3f vec3f;
		if (inflated) return;
		inflated = true;

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

	typedef pair<vec3f, vec3f> bb_t;

	bb_t compute_bb(const vector<vec3f> &load_verts, const vector<vec3i> load_idxs_v) {
		vec3f min(FLT_MAX, FLT_MAX, FLT_MAX), max(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		for (int i = 0; i < load_idxs_v.size(); ++i) {
			const vec3f *v = &load_verts[load_idxs_v[i].x];
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

	vec3f bb_diam(const bb_t &bb) {
		return vec3f(bb.second.x - bb.first.x,
		                        bb.second.y - bb.first.y,
		                        bb.second.z - bb.first.z);
	}

	void ObjFileLoader::CollapseMaterials(float f)
	{
		if (collapsed) return;
		collapsed = true;
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
			typedef vec3f vec3f;
			float dir_expansion = f;
			// look at all clusters
			for (auto &by_material : groups_by_material) {
// 				cout << "reducing groups of material " << (by_material.first ? by_material.first->name : string("(null)")) << "..." << endl;
				bool merged = true;
				while (merged && by_material.second.size() > 1) {
					merged = false;
					// find a group of the cluster to merge to the first group
					Group *base = by_material.second.front();
// 					cout << "    the base group is " << base->name << endl;
// 					cout << "        (";
// 					for (auto c : by_material.second) cout << c->name << " ";
// 					cout << ")" << endl;
					// find bb
					bb_t base_bb = compute_bb(load_verts, base->load_idxs_v);
					vec3f base_diam = bb_diam(base_bb);
					list<Group*>::iterator git = ++by_material.second.begin();
					while (git != by_material.second.end()) {
						Group *g = *git;
// 						cout << "    candidate: " << g->name << endl;
						bb_t this_bb = compute_bb(load_verts, g->load_idxs_v);
						vec3f diam = bb_diam(merge_bb(base_bb, this_bb));
						if (diam.x < dir_expansion * base_diam.x &&
						    diam.y < dir_expansion * base_diam.y &&
						    diam.z < dir_expansion * base_diam.z) {
// 							cout << "    merge!" << endl;
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

	bool ObjFileLoader::LoadMaterialFile(const std::string &name) {
		mtlfile = name;
		return ObjLoader::LoadMaterialFile(name);
	}
	
	void w(FILE *out, const void *p, int size, int elems) {
		if (fwrite(p, size, elems, out) != elems) {
			cerr << "error in fwrite" << endl;
			throw std::runtime_error("error in fwrite");
		}
	}
	void w(FILE *out, uint32_t u) {
		w(out, &u, sizeof(uint32_t), 1);
	}
	void w(FILE *out, const char *str) {
		w(out, str, sizeof(char), strlen(str)+1);
	}
	void ObjFileLoader::SaveBinaryObj(const std::string &filename)
	{
		const char *header = (char*)"pobj/libobjloader\n";
		const char *mtllib = mtlfile.c_str();
		FILE *out = fopen(filename.c_str(), "wb");
		// write header
		w(out, header);
		w(out, mtllib);
		w(out, (uint32_t)inflated);
		w(out, (uint32_t)collapsed);
		// write base vertex data
		w(out, (uint32_t)load_verts.size());
		w(out, (uint32_t)load_norms.size());
		w(out, (uint32_t)load_texs.size());
		w(out, (uint32_t)groups.size());
		w(out, &load_verts[0], sizeof(vec3f), load_verts.size());
		w(out, &load_norms[0], sizeof(vec3f), load_norms.size());
		w(out, &load_texs[0],  sizeof(vec3f), load_texs.size());
		// write groups
		for (list<Group>::iterator it = groups.begin(); it != groups.end(); ++it) {
			w(out, it->name.c_str());
			w(out, it->mat->name.c_str());
			w(out, (uint32_t)it->mat_set_explicitly);
			w(out, (uint32_t)it->load_idxs_v.size());
			w(out, (uint32_t)it->load_idxs_n.size());
			w(out, (uint32_t)it->load_idxs_t.size());
			w(out, &it->load_idxs_v[0], sizeof(vec3i), it->load_idxs_v.size());
			w(out, &it->load_idxs_n[0], sizeof(vec3i), it->load_idxs_n.size());
			w(out, &it->load_idxs_t[0], sizeof(vec3i), it->load_idxs_t.size());
		}
		// done
		fclose(out);
	}
	
	bool ObjFileLoader::LoadBinaryObj(const std::string &filename)
	{
		cout << "hello [" << filename << "]" << endl;
		const char *header = (char*)"pobj/libobjloader\n";
		// {{{
		int fd = open(filename.c_str(), O_RDONLY);
		if (fd < 0) {
			cerr << "cannot open " << filename << endl;
			return false;
		}
		struct stat statbuf;
		if (fstat(fd, &statbuf) < 0) {
			cerr << "cannot stat " << filename << endl;
			return false;
		}
		char *data;
		if ((data = (char*)mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fd, 0)) == (caddr_t) -1) {
			cerr << "mmap error for " << filename << endl;
			return false;
		}

		if (strcmp(header, data) != 0) {
			cerr << "pobj file " << filename << " does not identify as binary obj." << endl;
			return false;
		}

		// read header
		unsigned int offset = strlen(data)+1;
		mtlfile = string(data+offset);
		offset += strlen(mtlfile.c_str())+1;

		// load material file
		if (!LoadMaterialFile(mtlfile)) {
			cerr << "cannot open material file '" << mtlfile << "'!" << endl;
			return false;
		}
		// }}}

		uint32_t *u = (uint32_t*)(data+offset);
		inflated = (bool)u[0];
		collapsed = (bool)u[1];
		offset += 2*sizeof(uint32_t);

		// read base vertex data
		u = (uint32_t*)(data+offset);
		uint32_t n_verts  = u[0], 
				 n_norms  = u[1], 
				 n_texs   = u[2], 
				 n_groups = u[3];
		cout << n_verts << " " << n_norms << " " << n_texs << " " << n_groups << endl;
		offset += 4*sizeof(uint32_t);
		load_verts.resize(n_verts);
		load_norms.resize(n_norms);
		load_texs.resize(n_texs);
		memcpy(&load_verts[0], data+offset, n_verts*sizeof(vec3f)); offset += n_verts*sizeof(vec3f);
		memcpy(&load_norms[0], data+offset, n_norms*sizeof(vec3f)); offset += n_norms*sizeof(vec3f);
		memcpy(&load_texs[0],  data+offset, n_texs*sizeof(vec3f));  offset += n_texs*sizeof(vec3f);

		// read groups
		for (int i = 0; i < n_groups; ++i) {
			string name = string(data+offset);
			string materialname = string(data+offset+name.length()+1);
			StartGroup(name);
			CurrentMaterial(materialname);

			groups.back().mat = curr_mtl;
			offset += groups.back().name.length()+1 + materialname.length()+1;
			u = (uint32_t*)(data + offset);
			uint32_t mat_set = u[0],
					 n_v = u[1],
					 n_n = u[2],
					 n_t = u[3];
			groups.back().mat_set_explicitly = (bool)mat_set;
			offset += 4*sizeof(uint32_t);
			groups.back().load_idxs_v.resize(n_v);
			groups.back().load_idxs_n.resize(n_n);
			groups.back().load_idxs_t.resize(n_t);
			memcpy(&groups.back().load_idxs_v[0], data+offset, n_v*sizeof(vec3i)); offset += n_v*sizeof(vec3i);
			memcpy(&groups.back().load_idxs_n[0], data+offset, n_n*sizeof(vec3i)); offset += n_n*sizeof(vec3i);
			memcpy(&groups.back().load_idxs_t[0], data+offset, n_t*sizeof(vec3i)); offset += n_t*sizeof(vec3i);
		}

		munmap(data, statbuf.st_size);
		return true;
	}
}

/* vim: set foldmethod=marker: */

