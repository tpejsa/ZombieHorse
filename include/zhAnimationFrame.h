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

#ifndef __zhAnimationFrame_h__
#define __zhAnimationFrame_h__

#include "zhPrereq.h"
#include "zhIterators.h"
#include "zhMath.h"

namespace zh
{

class Animation;
class Skeleton;

/**
* @brief Skeleton pose at a specific frame in an animation,
* consisting of root position and orientation, joint orientations,
* and joint target positions (relative to root position).
*/
struct zhDeclSpec AnimationFrame
{
	Vector3 rootPosition;
	Quat rootOrientation;
	std::vector<Quat> orientations;
	std::vector<Vector3> targetPositions;

	AnimationFrame() {}
	AnimationFrame( Animation* anim, unsigned int frameIndex );
	void extractTargetPositions(Skeleton* skel);
};

/**
* @brief Set of animation frames.
*/
class zhDeclSpec AnimationFrameSet
{

public:

	typedef VectorIterator<std::vector<AnimationFrame>> FrameIterator;
	typedef VectorConstIterator<std::vector<AnimationFrame>> FrameConstIterator;

	/**
	* Constructor.
	*/
	AnimationFrameSet();

	/**
	* Destructor.
	*/
	~AnimationFrameSet();

	/**
	* Add an animation frame to the frame set.
	*
	* @param frame Animation frame.
	*/
	void addFrame( const AnimationFrame& frame );

	/**
	* Remove an animation frame from the frame set.
	*
	* @param frameIndex Animation frame index.
	*/
	void removeFrame( unsigned int frameIndex );

	/**
	* Remove all animation frames from the frame set.
	*/
	void removeAllFrames();

	/**
	* Get the animation frame at the specified index.
	*
	* @param frameIndex Animation frame index.
	* @return Animation frame.
	*/
	const AnimationFrame& getFrame( unsigned int frameIndex ) const;

	/**
	* Set the animation frame at the specified index.
	*
	* @param frameIndex Animation frame index.
	* @param frame Animation frame.
	*/
	void setFrame( unsigned int frameIndex, const AnimationFrame& frame );

	/**
	* Get the number of animation frames in the set.
	*
	* @return Number of animation frames in the set.
	*/
	unsigned int getNumFrames() const;

	/**
	* Get an iterator over the vector of animation frames.
	*/
	FrameIterator getFrameIterator();

	/**
	* Get an iterator over the vector of animation frames.
	*/
	FrameConstIterator getFrameConstIterator() const;

	/**
	* Get the number of animation tracks in the data
	*
	* @return Number of tracks.
	* @remark Each component of position or orientation counts as one track.
	* This is basically the dimensionality of animation data.
	*/
	unsigned int getNumTracks() const;

	/**
	* Extract the world positions of joints for every frame in the set.
	*
	* @param Pointer to the target skeleton.
	*/
	void extractTargetPositions(Skeleton* skel);

private:

	std::vector<AnimationFrame> mFrames;
};

}

#endif // __zhAnimationFrame_h__
