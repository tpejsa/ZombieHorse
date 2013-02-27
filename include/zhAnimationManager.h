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

#ifndef __zhAnimationManager_h__
#define __zhAnimationManager_h__

#include "zhPrereq.h"
#include "zhSingleton.h"
#include "zhResourceManager.h"
#include "zhAnimationSet.h"

#define zhZHALoader_ClassId 1
#define zhZHALoader_ClassName "ZHALoader"
#define zhZHASerializer_ClassId 1
#define zhZHASerializer_ClassName "ZHASerializer"

namespace zh
{

/**
* @brief Animation resource manager.
*/
class zhDeclSpec AnimationManager : public ResourceManager,
	public Singleton<AnimationManager>
{

	friend class Singleton<AnimationManager>;

private:

	/**
	* Constructor.
	*/
	AnimationManager() : ResourceManager(), Singleton<AnimationManager>()
	{
	}

	/**
	* Destructor.
	*/
	~AnimationManager()
	{
	}

public:

	/**
	* Gets the Resource class ID.
	*/
	ResourceClass getClassId() const { return Resource_AnimationSet; }

private:

	Resource* _createResource( unsigned long id, const std::string& name );

};

}

#endif // __zhAnimationManager_h__
