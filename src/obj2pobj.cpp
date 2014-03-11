#include "pobj.h"


#include <iostream>


using namespace std;
using namespace pobj;

int main(int argc, char **argv) {

	if (argc != 2) {
		cerr << "we requrie exaclty one argument: the obj file to be converted to pobj format." << endl;
		return -1;
	}
	packed_obj_constructor loader(argv[1]);

	return 0;
}



/* vim: set foldmethod=marker: */

