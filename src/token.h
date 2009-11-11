/*  Copyright (C) 2009 Kai Selgrad <stuff * projects . selgrad . org>
 *  * is the symbol @.
 *   
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
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

