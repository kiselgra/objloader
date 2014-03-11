#include "pobj.h"

#include <iostream>


using namespace std;
using namespace pobj;

int main(int argc, char **argv) {

	if (argc != 2) {
		cerr << "we requrie exaclty one argument: the pobj file to be converted to obj format." << endl;
		return -1;
	}
	packed_obj_loader loader(argv[1]);

	return 0;
}



/* vim: set foldmethod=marker: */

