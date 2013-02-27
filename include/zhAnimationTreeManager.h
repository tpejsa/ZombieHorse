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

#ifndef __zhAnimationTreeManager_h__
#define __zhAnimationTreeManager_h__

#include "zhPrereq.h"
#include "zhSmartPtr.h"
#include "zhObjectFactory.h"
#include "zhSingleton.h"
#include "zhResourceManager.h"
#include "zhAnimationTree.h"

#define zhDeclare_AnimationNode( AN, classId, className ) \
	zhDeclare_Class( AnimationNode, AN, classId, className, unsigned short )
#define zhRegister_AnimationNode( AN ) \
	AnimationSystem::Instance()->getAnimationTreeManager()->_getNodeFactory()->registerClass( AN::ClassId(), AN::ClassName(), &AN::Create )
#define zhUnregister_AnimationNode( AN ) \
	AnimationSystem::Instance()->getAnimationTreeManager()->_getNodeFactory()->registerClass( AN::ClassId() )

#define zhDeclare_BoneController( JC, classId, className ) \
	zhDeclare_Class( BoneController, JC, classId, className, unsigned short )
#define zhRegister_BoneController( JC ) \
	AnimationSystem::Instance()->getAnimationTreeManager()->_getBoneControllerFactory()->registerClass( JC::ClassId(), JC::ClassName(), &JC::Create )
#define zhUnregister_BoneController( JC ) \
	AnimationSystem::Instance()->getAnimationTreeManager()->_getBoneControllerFactory()->registerClass( JC::ClassId() )

#define zhZHTLoader_ClassId 1
#define zhZHTLoader_ClassName "ZHTLoader"

#define zhZHTSerializer_ClassId 1
#define zhZHTSerializer_ClassName "ZHTSerializer"

namespace zh
{

class AnimationNode;
class BoneController;

/**
* @brief Animation tree resource manager.
*/
class zhDeclSpec AnimationTreeManager : public ResourceManager,
	public Singleton<AnimationTreeManager>
{

	friend class Singleton<AnimationTreeManager>;

public:

	typedef ObjectFactory< AnimationNode, unsigned short > NodeFactory;
	typedef ObjectFactory< BoneController, unsigned short > BoneControllerFactory;

private:

	/**
	* Constructor.
	*/
	AnimationTreeManager();

	/**
	* Destructor.
	*/
	~AnimationTreeManager();

public:

	/**
	* Gets the Resource class ID.
	*/
	ResourceClass getClassId() const { return Resource_AnimationTree; }

	/**
	* Gets the factory that produces nodes for the animation tree.
	* This function is meant to be called only
	* by animation tree instance clases.
	*/
	NodeFactory* _getNodeFactory() const;

	/**
	* Gets the factory that produces bone controllers
	* for the animation tree.
	* This function is meant to be called only
	* by animation tree instance clases.
	*/
	BoneControllerFactory* _getBoneControllerFactory() const;

protected:

	Resource* _createResource( unsigned long id, const std::string& name );

private:

	NodeFactory* mNodeFact;
	BoneControllerFactory* mBoneCtrlFact;

};

}

#endif // __zhAnimationTreeManager_h__
