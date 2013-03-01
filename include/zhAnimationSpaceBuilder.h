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

#ifndef __zhAnimationSpaceBuilder_h__
#define __zhAnimationSpaceBuilder_h__

#include "zhPrereq.h"
#include "zhAnimationSegment.h"
#include "zhAnimationSet.h"
#include "zhAnimation.h"
#include "zhAnimationSpace.h"

namespace zh
{

class Skeleton;
class MatchGraph;

/**
* @brief Class that provides an implementation of the algorithm
* for constructing blend curves which are used for
* real-time animation blending.
*/
class zhDeclSpec AnimationSpaceBuilder
{

public:

	/**
	* Constructor.
	*
	* @param skel Pointer to the character skeleton.
	* @param animSpace Pointer to the animation space.
	*/
	AnimationSpaceBuilder( Skeleton* skel, AnimationSpace* animSpace );

	/**
	* Destructor.
	*/
	virtual ~AnimationSpaceBuilder();

	/**
	* Gets the character skeleton.
	*
	* @return Pointer to the skeleton.
	*/
	Skeleton* getSkeleton() const { return mSkel; }

	/**
	* Gets the animation space.
	*
	* @return Pointer to the animation space.
	*/
	AnimationSpace* getAnimationSpace() const { return mAnimSpace; }

	/**
	* If true, timewarp and alignment curves will be built
	* for the animation space.
	*/
	bool getBuildBlendCurves() const { return mBuildBlendCurves; }

	/**
	* If set to true, timewarp and alignment curves will be built
	* for the animation space.
	*/
	void setBuildBlendCurves( bool buildBlendCurves = true ) { mBuildBlendCurves = buildBlendCurves; }

	/**
	* Gets the length of the animation window
	* used in animation comparison.
	*/
	float getWindowLength() const { return mWndLength; }

	/**
	* Sets the length of the animation window
	* used in animation comparison (should be long enough to incorporate
	* first- and second-order derivatives af animation parameters and
	* no longer than animation transition length).
	*/
	void setWindowLength( float wndLength = 0.35f ) { mWndLength = wndLength; }

	/**
	* Gets the maximum distance value for local minima, normalized
	* (lower value yields fewer matches, but they are of higher quality).
	*/
	float getMinDistance() const { return mMinDist; }

	/**
	* Sets the maximum distance value for local minima, normalized
	* (lower value yields fewer matches, but they are of higher quality).
	*/
	void setMinDistance( float minDist = 0.05f ) { mMinDist = minDist; }

	/**
	* Gets the knot spacing in timewarp and alignment curves.
	* If 1, a knot will be generated for every frame correspondence
	* in the base animations. Greater knot spacing will reduce
	* computational and memory requirements of blending, at the expense of
	* visual quality.
	*/
	unsigned int getKnotSpacing() const { return mKnotSpacing; }

	/**
	* Sets the knot spacing in timewarp and alignment curves.
	* If 1, a knot will be generated for every frame correspondence
	* in the base animations. Greater knot spacing will reduce
	* computational and memory requirements of blending, at the expense of
	* visual quality.
	*/
	void setKnotSpacing( unsigned int knotSpacing = 3 ) { mKnotSpacing = knotSpacing; }

	/**
	* If true, the animation space build algorithm
	* will identify matching annotations in base animations.
	*/
	bool getMatchAnnotations() const { return mBuildBlendCurves; }

	/**
	* If set to true, the animation space build algorithm
	* will identify matching annotations in base animations.
	*/
	void setMatchAnnotations( bool matchAnnots = true ) { mMatchAnnots = matchAnnots; }

	/**
	* Builds an animation space from a set of matching animation segments.
	*
	* @param animSegs Vector of matching animation segments that will be used
	* as base animations in the animation space.
	* @remark This version of the build function creates base animations
	* from a set of manually specified animation segments.
	* It compares the base animations and builds animation distance grids
	* from scratch, upon which it computes the temporal alignments
	* in those grids. This method is slower and should be used
	* when the animator manually defines the base animations for blending.
	*/
	virtual void build( const std::vector<AnimationSegment>& animSegs );

	/**
	* Builds an animation space from a set of matching animation segments.
	*
	* @param matches Match graph that was used in construction of the
	* animation space. Its edges specify temporal alignments that will
	* be used in construction of the blend curves.
	* @param refNodeHandle Handle of the match graph node corresponding
	* to the base animation that will be used as reference animation
	* in construction of the animation space.
	* @remark This version of the build function creates base animations
	* from nodes (i.e. animation segments) in
	* the match graph and adds them to the animation space.
	* Then it constructs blend curves for the animation space by using
	* precomputed temporal alignments stored in match graph edges.
	* This method is faster and employed only in conjunction with
	* the automated animation search system which identifies blendable
	* animation segments.
	*/
	virtual void build( MatchGraph* matches, unsigned short refNodeHandle = 0 );

protected:

	virtual void _copyAnnotsToBaseAnim( const AnimationSegment& animSeg, AnimationAnnotationContainer* srcAnnots,
		AnimationAnnotationContainer* trgAnnots );

	Skeleton* mSkel;
	AnimationSpace* mAnimSpace;

	bool mBuildBlendCurves;
	float mWndLength;
	float mMinDist;
	unsigned int mKnotSpacing;
	bool mMatchAnnots;

};

}

#endif // __zhAnimationSpaceBuilder_h__
