/******************************************************************************
Copyright (C) 2013 Tomislav Pejsa

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

/**
* @file zhFileSystem.h
* @brief Helper macros and functions for working with the file system.
*/

#ifndef __zhFileSystem_h__
#define __zhFileSystem_h__

#include "zhPrereq.h"
#include "zhString.h"

#include <cctype>

namespace zh
{

/**
* Parses path string, extracting directory, filename, filename prefix and file extension.
*
* @param path Path string.
* @param dir Directory string.
* @param filename Filename string.
* @param prefix Filename prefix string.
* @param ext File extension string.
*/
inline void parsePathStr( const std::string& path,
						 std::string& dir,
						 std::string& filename,
						 std::string& prefix,
						 std::string& ext
						 )
{
	std::string::size_type si = path.find_last_of( "\\/" );
	if( si == std::string::npos )
	{
		dir = "";
		filename = path;
	}
	else
	{
		dir = path.substr( 0, si ) + "\\";
		filename = path.substr( si + 1 );

		for( unsigned int ci = 0; ci < dir.length(); ++ci )
			if( dir[ci] == '/' )
				dir[ci] = '\\';
	}

	std::string::size_type ei = filename.find_last_of( "." );
	if( ei == std::string::npos )
	{
		ext = "";
		prefix = filename;
	}
	else
	{
		ext = filename.substr( ei + 1 );
		prefix = filename.substr( 0, filename.length() - ext.length() - 1 );
	}

	// convert ext. to lower case
	std::transform( ext.begin(), ext.end(), ext.begin(),
		( int(*)(int) )std::tolower );
}

/**
* Processes the directory string, replacing backslashes with slashes
* and appending slash to the end if missing.
*/
inline std::string processDirStr( const std::string& dir )
{
	std::string pdir = dir;

	if( pdir == "" )
		return "./";

	for( size_t ci = 0; ci < pdir.length(); ++ci )
		if( pdir[ci] == '\\' )
			pdir[ci] = '/';

	if( pdir[ pdir.length() - 1 ] != '/' )
		pdir += '/';

	return pdir;
}

}

#endif // __zhFileSystem_h__
