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

#ifndef __zhAnimationSet_h__
#define __zhAnimationSet_h__

#include "zhPrereq.h"
#include "zhIterators.h"
#include "zhSmartPtr.h"
#include "zhResourceManager.h"
#include "zhSkeleton.h"
#include "zhAnimationAnnotation.h"

namespace zh
{

class AnimationSet;
class Animation;
class AnimationSpace;

typedef SmartPtr<AnimationSet> AnimationSetPtr;

/**
* @brief Class representing a set of animation resources.
*/
class zhDeclSpec AnimationSet : public Resource
{

public:

	typedef MapIterator< std::map<unsigned short, Animation*> > AnimationIterator;
	typedef MapConstIterator< std::map<unsigned short, Animation*> > AnimationConstIterator;

	typedef MapIterator< std::map<unsigned short, AnimationSpace*> > AnimationSpaceIterator;
	typedef MapConstIterator< std::map<unsigned short, AnimationSpace*> > AnimationSpaceConstIterator;

	/**
	* Constructor.
	*
	* @param id AnimationSet ID.
	* @param name AnimationSet name.
	* @param mgr Owning ResourceManager.
	*/
	AnimationSet( unsigned long id, const std::string& name, ResourceManager* mgr );

	/**
	* Destructor.
	*/
	~AnimationSet();

	/**
	* Creates a new key-frame animation in this AnimationSet.
	*
	* @param id Animation ID.
	* @param name Animation name.
	* @return Pointer to the Animation.
	*/
	Animation* createAnimation( unsigned short id, const std::string& name );
	
	/**
	* Deletes a key-frame animation from this AnimationSet.
	*
	* @param id Animation ID.
	*/
	void deleteAnimation( unsigned short id );
	
	/**
	* Deletes a key-frame animation from this AnimationSet.
	*
	* @param name Animation name.
	*/
	void deleteAnimation( const std::string& name );
	
	/**
	* Deletes all key-frame animations from this AnimationSet.
	*/
	void deleteAllAnimations();
	
	/**
	* Returns true if the specified key-frame animation exists
	* in this AnimationSet, otherwise false.
	*
	* @param id Animation ID.
	* @return true if the specified key-frame animation exists
	* in this AnimationSet, otherwise false.
	*/
	bool hasAnimation( unsigned short id ) const;

	/**
	* Returns true if the specified key-frame animation exists
	* in this AnimationSet, otherwise false.
	*
	* @param name Animation name.
	* @return true if the specified key-frame animation exists
	* in this AnimationSet, otherwise false.
	*/
	bool hasAnimation( const std::string& name ) const;

	/**
	* Gets the specified key-frame animation.
	*
	* @param id Animation ID.
	* @return Pointer to the Animation or NULL if the specified
	* animation does not exist in the AnimationSet.
	*/
	Animation* getAnimation( unsigned short id ) const;

	/**
	* Gets the specified key-frame animation.
	*
	* @param name Animation name.
	* @return Pointer to the Animation or NULL if the specified
	* animation does not exist in the AnimationSet.
	*/
	Animation* getAnimation( const std::string& name ) const;

	/**
	* Gets an iterator over the map of Animations.
	*/
	AnimationIterator getAnimationIterator();

	/**
	* Gets a const iterator over the map of Animations.
	*/
	AnimationConstIterator getAnimationConstIterator() const;

	/**
	* Gets the number of key-frame animations in the AnimationSet.
	*/
	unsigned int getNumAnimations() const;

	/**
	* Creates a new animation clip from a segment of an existing animation.
	*
	* @param newAnimName New animation name.
	* @param origAnimName Original animation name.
	* @param startTime Animation segment start time.
	* @param length Animation segment length.
	*/
	void createAnimationFromSegment( const std::string& newAnimName, const std::string& origAnimName,
		float startTime, float length );

	/**
	* Creates a new key-frame animation in this AnimationSet.
	*
	* @param id AnimationSpace ID.
	* @param name AnimationSpace name.
	* @return Pointer to the AnimationSpace.
	*/
	AnimationSpace* createAnimationSpace( unsigned short id, const std::string& name );
	
	/**
	* Deletes a key-frame animation from this AnimationSet.
	*
	* @param id AnimationSpace ID.
	*/
	void deleteAnimationSpace( unsigned short id );
	
	/**
	* Deletes a key-frame animation from this AnimationSet.
	*
	* @param name AnimationSpace name.
	*/
	void deleteAnimationSpace( const std::string& name );
	
	/**
	* Deletes all key-frame animations from this AnimationSet.
	*/
	void deleteAllAnimationSpaces();
	
	/**
	* Returns true if the specified key-frame animation exists
	* in this AnimationSet, otherwise false.
	*
	* @param id AnimationSpace ID.
	* @return true if the specified key-frame animation exists
	* in this AnimationSet, otherwise false.
	*/
	bool hasAnimationSpace( unsigned short id ) const;

	/**
	* Returns true if the specified key-frame animation exists
	* in this AnimationSet, otherwise false.
	*
	* @param name AnimationSpace name.
	* @return true if the specified key-frame animation exists
	* in this AnimationSet, otherwise false.
	*/
	bool hasAnimationSpace( const std::string& name ) const;

	/**
	* Gets the specified key-frame animation.
	*
	* @param id AnimationSpace ID.
	* @return Pointer to the AnimationSpace or NULL if the specified
	* animation does not exist in the AnimationSet.
	*/
	AnimationSpace* getAnimationSpace( unsigned short id ) const;

	/**
	* Gets the specified key-frame animation.
	*
	* @param name AnimationSpace name.
	* @return Pointer to the AnimationSpace or NULL if the specified
	* animation does not exist in the AnimationSet.
	*/
	AnimationSpace* getAnimationSpace( const std::string& name ) const;

	/**
	* Gets an iterator over the map of AnimationSpaces.
	*/
	AnimationSpaceIterator getAnimationSpaceIterator();

	/**
	* Gets a const iterator over the map of AnimationSpaces.
	*/
	AnimationSpaceConstIterator getAnimationSpaceConstIterator() const;

	/**
	* Gets the number of key-frame animations in the AnimationSet.
	*/
	unsigned int getNumAnimationSpaces() const;

	/**
	* Calculates the resource memory usage.
	*/
	size_t _calcMemoryUsage() const;

	/**
	* Unloads the resource, freeing up the memory it occupies.
	*/
	void _unload();

	/**
	* Creates a deep copy of the resource.
	*
	* @param clonePtr Pointer to the copy.
	*/
	void _clone( Resource* clonePtr ) const;

private:

	std::map<unsigned short, Animation*> mAnimsById;
	std::map<std::string, Animation*> mAnimsByName;

	std::map<unsigned short, AnimationSpace*> mAnimSpacesById;
	std::map<std::string, AnimationSpace*> mAnimSpacesByName;

};

}

#endif // __zhAnimationSet_h__
