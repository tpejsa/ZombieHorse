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

#ifndef __zhAnimationTransitionBuilder_h__
#define __zhAnimationTransitionBuilder_h__

#include "zhPrereq.h"

namespace zh
{

class Skeleton;
class Animation;
class AnimationSpace;
class MatchGraph;

/**
* @brief Class that provides implementations of algorithms
* for constructing high-quality transitions between animations.
*/
class zhDeclSpec AnimationTransitionBuilder
{

public:

	/**
	* Constructor.
	*
	* @param skel Character skeleton.
	*/
	AnimationTransitionBuilder( Skeleton* skel );

	/**
	* Destructor.
	*/
	virtual ~AnimationTransitionBuilder();

	/**
	* Gets the character skeleton.
	*
	* @return Pointer to the skeleton.
	*/
	virtual Skeleton* getSkeleton() const { return mSkel; }

	/**
	* Builds a transition between two animations.
	*
	* @param srcAnim Source parametrized animation.
	* @param trgAnim Target parametrized animation.
	* @param minDist Maximum distance value for local minima, normalized
	* (lower value yields fewer transitions, but they are of higher quality).
	* @return Number of transitions that have been built.
	*/
	virtual unsigned int buildTransitions( AnimationSpace* srcAnim, AnimationSpace* trgAnim,
		float transLength = 0.35f, float minDist = 0.05f );

	/**
	* Builds a transition between two animations.
	*
	* @param srcAnim Source parametrized animation.
	* @param trgAnim Target animation.
	* @param minDist Maximum distance value for local minima, normalized
	* (lower value yields fewer transitions, but they are of higher quality).
	* @return Number of transitions that have been built.
	*/
	virtual unsigned int buildTransitions( AnimationSpace* srcAnim, Animation* trgAnim,
		float transLength = 0.35f, float minDist = 0.05f );

	/**
	* Builds a transition between two animations.
	*
	* @param srcAnim Source animation.
	* @param trgAnim Target parametrized animation.
	* @param minDist Maximum distance value for local minima, normalized
	* (lower value yields fewer transitions, but they are of higher quality).
	* @return Number of transitions that have been built.
	*/
	virtual unsigned int buildTransitions( Animation* srcAnim, AnimationSpace* trgAnim,
		float transLength = 0.35f, float minDist = 0.05f );

	/**
	* Builds a transition between two animations.
	*
	* @param srcAnim Source animation.
	* @param trgAnim Target animation.
	* @param minDist Maximum distance value for local minima, normalized
	* (lower value yields fewer transitions, but they are of higher quality).
	* @param twoWay If true, transitions will also be built
	* from the target animation to the source animation.
	* @return Number of transitions that have been built.
	*/
	virtual unsigned int buildTransitions( Animation* srcAnim, Animation* trgAnim,
		float transLength = 0.35f, float minDist = 0.05f, bool twoWay = false );

protected:

	Skeleton* mSkel;

};

}

#endif // __zhAnimationTransitionBuilder_h__

