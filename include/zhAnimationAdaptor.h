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

#ifndef __zhAnimationAdaptor_h__
#define __zhAnimationAdaptor_h__

#include "zhPrereq.h"
#include "zhIterators.h"
#include "zhSkeleton.h"

namespace zh
{

class AnimationNode;

/**
* @brief Class representing an animation retargetting system.
*
* Any AnimationNode can be associate with an AnimationAdaptor, which takes
* its output animation data and adapts it to a different character figure.
*/
class zhDeclSpec AnimationAdaptor
{

public:

	/**
	* Constructor.
	*
	* @param origSkel Pointer to the skeleton with the original motion.
	* @param animNode Pointer to the owning animation node.
	*/
	AnimationAdaptor( Skeleton* origSkel, AnimationNode* animNode );

	/**
	* Destructor.
	*/
	~AnimationAdaptor();

	/**
	* Get the skeleton with the original motion.
	*
	* @return Pointer to the skeleton.
	*/
	Skeleton* getOriginalSkeleton() const;

	/**
	* Get the owning animation node.
	*
	* @return Pointer to the owning naimation node.
	*/
	AnimationNode* getAnimationNode() const;

	/**
	* Get prediction factor, which measures how much predicted motion
	* influences retargetting results.
	*/
	float getPredictionFactor() const;

	/**
	* Set prediction factor, which measures how much predicted motion
	* influences retargetting results.
	*/
	void setPredictionFactor( float predFact );

	/**
	* Adapt the animation that's playing on the original skeleton
	* to the target skeleton.
	*
	* @param Pointer to the target skeleton.
	*/
	void adapt( Skeleton* targetSkel );

protected:

	void _computeIKGoals( Skeleton* targetSkel ) const;
	float _computeIKGoalWeight(Bone* endEff) const;
	float _computeEnvObjDistance( Bone* endEff, Bone* envObj ) const;
	float _computeGroundDistance(Bone* endEff) const;

	Skeleton* mOrigSkel;
	AnimationNode* mAnimNode;
	std::vector<BoneTag> mEndEffectors;
	float mPredFact;
	mutable std::map<std::pair<unsigned short, unsigned short>, float> mPrevEnvObjDist;
	mutable std::map<unsigned short, float> mPrevGroundDist;
};

}

#endif // __zhAnimationAdaptor_h__
