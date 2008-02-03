#include <iostream>
#include <argp.h>

using namespace std;

const char *argp_program_version =  VERSION;

char doc[] = "LIB XYZ compiler and linker flags";								// TODO

struct argp_option options[] = 
{
	{	"cflags",		'c',	0,	0,		"print pre-processor and compiler flags"	},
	{	"libs",			'l',	0,	0,		"print library linking information"	},
	{ 0 }
};


error_t parse_options(int key, char *arg, argp_state *state);

argp parser = { options, parse_options, 0, doc };

error_t parse_options(int key, char *arg, argp_state *state)
{
	switch (key)
	{
	case 'c':
		cout << "-I" << INC_DIR << endl;
		break;
	
	case 'l':
		cout << "-L" << LIB_DIR << " -l" << (PACKAGE+3) << endl;
		break;
	
	default:
		return ARGP_ERR_UNKNOWN;
	}
	return 0;
}



int main(int argc, char **argv)
{
	if (argc == 1)
	{
 		argp_help(&parser, stdout, ARGP_HELP_USAGE, argv[0]);
		return 0;
	}
	return argp_parse(&parser, argc, argv, 0, 0, 0);
	
}

