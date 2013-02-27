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

#ifndef __zhAnimationController_h__
#define __zhAnimationController_h__

#include "zhPrereq.h"
#include "zhIterators.h"
#include "zhObjectFactory.h"
#include "zhCharacterController.h"
#include "zhAnimationTree.h"
#include "zhAnimationTreeManager.h"

namespace zh
{

class AnimationTreeInstance;

/**
* @brief Character animation controller. Handles animation playback
* and blending.
*/
class zhDeclSpec AnimationController : public CharacterController
{

public:

	typedef MapIterator< std::map<unsigned long, AnimationTreeInstance*> > AnimationTreeIterator;
	typedef MapConstIterator< std::map<unsigned long, AnimationTreeInstance*> > AnimationTreeConstIterator;

	zhDeclare_CharacterController( AnimationController, zhAnimationController_ClassId, zhAnimationController_ClassName )

	/**
	* Constructor.
	*/
	AnimationController();

	/**
	* Destructor.
	*/
	~AnimationController();

	/**
	* Adds a new animation tree instance to animation controller.
	*
	* @param animTree Pointer to the animation tree resource.
	* @return Pointer to the new animation tree instance.
	*/
	virtual AnimationTreeInstance* addAnimationTree( AnimationTreePtr animTree );

	/**
	* Removes the specified animation tree instance.
	*
	* @param id Animation tree ID.
	*/
	virtual void removeAnimationTree( unsigned long id );

	/**
	* Removes the specified animation tree instance.
	*
	* @param name Animation tree name.
	*/
	virtual void removeAnimationTree( const std::string& name );

	/**
	* Removes all animation tree instances from this AnimationController.
	*/
	virtual void removeAllAnimationTrees();

	/**
	* Returns true if an instance of the specified animation tree
	* exists in this animation controller, otherwise false.
	*
	* @param id Animation tree ID.
	*/
	virtual bool hasAnimationTree( unsigned long id ) const;
	
	/**
	* Returns true if an instance of the specified animation tree
	* exists in this animation controller, otherwise false.
	*
	* @param name Animation tree name.
	*/
	virtual bool hasAnimationTree( const std::string& name ) const;

	/**
	* Gets a pointer to the specified animation tree instance.
	*
	* @param id Animation tree ID.
	* @return Pointer to the animation tree instance or NULL
	* if an instance does not exist on this animation controller.
	*/
	virtual AnimationTreeInstance* getAnimationTree( unsigned long id ) const;

	/**
	* Gets a pointer to the specified animation tree instance.
	*
	* @param name Animation tree name.
	* @return Pointer to the animation tree instance or NULL
	* if an instance does not exist in this animation controller.
	*/
	virtual AnimationTreeInstance* getAnimationTree( const std::string& name ) const;

	/**
	* Gets the number of animation tree instances in this
	* animation controller.
	*/
	virtual unsigned int getNumAnimationTrees() const;

	/**
	* Gets an iterator over the map of animation tree instances.
	*/
	virtual AnimationTreeIterator getAnimationTreeIterator();

	/**
	* Gets a const iterator over the map of animation tree instances.
	*/
	virtual AnimationTreeConstIterator getAnimationTreeConstIterator() const;

	/**
	* Designates an animation tree as active.
	*
	* @param id Animation tree ID.
	*/
	virtual AnimationTreeInstance* selectAnimationTree( unsigned long id );

	/**
	* Designates an animation tree as active.
	*
	* @param name Animation tree name.
	*/
	virtual AnimationTreeInstance* selectAnimationTree( const std::string& name );

	/**
	* Updates the controller with elapsed time.
	*/
	void update( float dt );

protected:

	/**
	* Creates an instance of the specified animation tree for use
	* by the animation controller.
	*/
	virtual AnimationTreeInstance* _createAnimTreeInst( AnimationTreePtr animTree );

	std::map<unsigned long, AnimationTreeInstance*> mAnimTreesById;
	std::map<std::string, AnimationTreeInstance*> mAnimTreesByName;

	AnimationTreeInstance* mCurAnimTree;

};

}

#endif // __zhAnimationController_h__
