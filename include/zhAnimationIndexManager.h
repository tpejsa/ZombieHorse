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

#ifndef __zhAnimationIndexManager_h__
#define __zhAnimationIndexManager_h__

#include "zhPrereq.h"
#include "zhSingleton.h"
#include "zhResourceManager.h"
#include "zhAnimationIndex.h"

#define zhZHILoader_ClassId 1
#define zhZHILoader_ClassName "ZHILoader"

#define zhZHISerializer_ClassId 1
#define zhZHISerializer_ClassName "ZHISerializer"

namespace zh
{

/**
* @brief Animation index resource manager.
*/
class zhDeclSpec AnimationIndexManager : public ResourceManager,
	public Singleton<AnimationIndexManager>
{

	friend class Singleton<AnimationIndexManager>;

private:

	/**
	* Constructor.
	*/
	AnimationIndexManager() : ResourceManager(), Singleton<AnimationIndexManager>()
	{
	}

	/**
	* Destructor.
	*/
	~AnimationIndexManager()
	{
	}

public:

	/**
	* Gets the Resource class ID.
	*/
	ResourceClass getClassId() const { return Resource_AnimationIndex; }

private:

	Resource* _createResource( unsigned long id, const std::string& name );

};

}

#endif // __zhAnimationIndexManager_h__
