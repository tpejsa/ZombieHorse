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

#ifndef __zhAnimationSpace_h__
#define __zhAnimationSpace_h__

#include "zhPrereq.h"
#include "zhLogger.h"
#include "zhAnimation.h"
#include "zhAnimationSet.h"
#include "zhAnimationParametrization.h"

namespace zh
{

class AnimationSpace;

/**
* @brief Animation space resource.
*/
class zhDeclSpec AnimationSpace
{

public:

	typedef VectorIterator< std::vector<Animation*> > AnimationIterator;
	typedef VectorConstIterator< std::vector<Animation*> > AnimationConstIterator;

	/**
	* Constructor.
	*
	* @param id AnimationSpace ID.
	* @param name AnimationSpace name.
	* @param mgr Owning ResourceManager.
	*/
	AnimationSpace( unsigned short id, const std::string& name, AnimationSetPtr animSet );

	/**
	* Destructor.
	*/
	~AnimationSpace();

	/**
	* Gets the animation space ID.
	*/
	unsigned short getId() const;

	/**
	* Gets the animation space name.
	*/
	const std::string& getName() const;

	/**
	* Gets a pointer to the owning AnimationSet.
	*/
	AnimationSetPtr getAnimationSet() const;

	/**
	* Adds a base animation to the AnimationSpace.
	*
	* @param anim Pointer to the Animation.
	*/
	void addBaseAnimation( Animation* anim );

	/**
	* Removes a base animation from the AnimationSpace.
	*
	* @param animIndex Base animation index.
	*/
	void removeBaseAnimation( unsigned int animIndex );

	/**
	* Gets a base animation from the animation space.
	*
	* @param animIndex Base animation index.
	* @return Pointer to the the base animation.
	*/
	Animation* getBaseAnimation( unsigned int animIndex ) const;

	/**
	* Gets the number of base animations in the animation space.
	*/
	unsigned int getNumBaseAnimations() const;

	/**
	* Gets an iterator over the vector of base animations.
	*/
	AnimationIterator getBaseAnimationIterator();

	/**
	* Gets a const iterator over the vector of base animations.
	*/
	AnimationConstIterator getBaseAnimationConstIterator() const;

	/**
	* Finds the specified base animation in the animation space.
	*
	* @param animName Base animation name.
	* @return Base animation or NULL if the animation could not be found.
	*/
	Animation* findBaseAnimation( const std::string& animName ) const;

	/**
	* Checks if this animation space has a timewarp curve defined.
	*
	* @return true if there is a timewarp curve, false otherwise.
	*/
	bool hasTimewarpCurve() const;

	/**
	* Gets the timewarp curve used for temporal alignment during blending.
	*/
	const CatmullRomSpline<Vector>& getTimewarpCurve() const;

	/**
	* Sets the timewarp curve used for temporal alignment during blending.
	*/
	void setTimewarpCurve( const CatmullRomSpline<Vector>& curve );

	/**
	* Checks if this animation space has an alignment curve defined.
	*
	* @return true if there is an alignment curve, false otherwise.
	*/
	bool hasAlignmentCurve() const;

	/**
	* Gets the alignment curve used for spatial alignment during blending.
	*/
	const CatmullRomSpline<Vector>& getAlignmentCurve() const;

	/**
	* Sets the alignment curve used for spatial alignment during blending.
	*/
	void setAlignmentCurve( const CatmullRomSpline<Vector>& curve );

	/**
	* Creates a new animation parametrization.
	*
	* @param classId Parametrization class ID.
	* @return Pointer to the parametrization.
	*/
	AnimationParametrization* createParametrization(
		AnimationParamClass classId, unsigned int numParams, unsigned int numBaseSamples );

	/**
	* Deletes the current animation parametrization.
	*/
	void deleteParametrization();

	/**
	* Checks if this animation space has a parametrization defined.
	*
	* @return true if there is a parametrization, false otherwise.
	*/
	bool hasParametrization() const;

	/**
	* Gets a pointer to the current animation parametrization.
	*/
	AnimationParametrization* getParametrization() const;
	
	/**
	* Calculates the resource memory usage.
	*/
	size_t _calcMemoryUsage() const;

	/**
	* Creates a deep copy of the resource.
	*
	* @param clonePtr Pointer to the copy.
	*/
	void _clone( AnimationSpace* clonePtr ) const;

private:

	unsigned short mId;
	std::string mName;

	AnimationSetPtr mAnimSet;
	std::vector<Animation*> mBaseAnims;

	CatmullRomSpline<Vector> mTWCurve;
	CatmullRomSpline<Vector> mAlignCurve;

	AnimationParametrization* mParam;

};

}

#endif // __zhAnimationSpace_h__
