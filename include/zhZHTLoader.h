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

#ifndef __zhZHTLoader_h__
#define __zhZHTLoader_h__

#include "zhPrereq.h"
#include "zhResourceManager.h"
#include "zhAnimationTreeManager.h"

namespace zh
{

/**
* @brief ZHT animation tree file loader class.
*/
class zhDeclSpec ZHTLoader : public ResourceLoader
{

public:

	zhDeclare_ResourceLoader( ZHTLoader, zhZHTLoader_ClassId, zhZHTLoader_ClassName )

	/**
	* Checks if the loader can load the specified resource
	* (i.e. if it is supported file type).
	* This pure virtual function needs to be implemented in
	* a derived resource IO class.
	*
	* @param resource Pointer to the resource that should be initialized
	* from the file.
	* @param path Resource file path.
	* @return true if resource can be loaded, otherwise false.
	*/
	bool tryLoad( ResourcePtr resource, const std::string& path );

	/**
	* Loads the resource from a file.
	*
	* @param res Pointer to the resource that should be initialized
	* from the file.
	* @param path Resource file path.
	* @return true if resource has been successfully loaded, otherwise false.
	*/
	bool load( ResourcePtr res, const std::string& path );

};

}

#endif // __zhZHTLoader_h__
