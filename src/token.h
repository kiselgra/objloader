/* $Id$ */
#ifndef __TOKEN_H__ 
#define __TOKEN_H__ 

#include <string>

class Token
{
	std::string text;
	int code;

public:
	Token(int tokencode, const char *tokenstring) : text(tokenstring), code(tokencode) {}

	const std::string& Text() const { return text; }
	int Code() const { return code; }
};

#endif

