#include "default.h"

using namespace std;

int main(int argc, char **argv) {
	if (argc != 3) {
		cerr << "we require exactly two arguments: the src object file, and the destination object file, as in:" << endl;
		cerr << argv[0] << " /path/to/file.{obj,bobj}" << endl;
	}
	string pobj = argv[2];

	cout << "loading " << argv[1] << endl;
	obj_default::ObjFileLoader loader(argv[1], "1 0 0 0   0 1 0 0   0 0 1 0   0 0 0 1");
	loader.Inflate();
	cout << "saving to " << pobj << endl;
	loader.SaveBinaryObj(pobj);

	cout << "testwise loading of " << pobj << endl;
	obj_default::ObjFileLoader loader2(pobj, "1 0 0 0   0 1 0 0   0 0 1 0   0 0 0 1");
}

/* vim: set foldmethod=marker: */

