/* $Id$ */
#ifndef __TOKEN_H__ 
#define __TOKEN_H__ 

#include <string>

class Token
{
	std::string text;
	int code;
	int line;

public:
	Token(int tokencode, const char *tokenstring, int line) : text(tokenstring), code(tokencode), line(line) {}

	const std::string& Text() const { return text; }
	int Code() const { return code; }
	int Line() const { return line; }
};

#endif

