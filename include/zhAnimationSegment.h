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

#ifndef __zhAnimationSegment_h__
#define __zhAnimationSegment_h__

#include "zhPrereq.h"

namespace zh
{

class Animation;

/**
* @brief Specifies a segment of an animation clip.
*/
struct zhDeclSpec AnimationSegment
{

public:

	/**
	* Constructor.
	*
	* @param anim Pointer to the animation.
	* @startTime Segment start time.
	* @endTime Segment end time.
	*/
	AnimationSegment( Animation* anim, float startTime, float endTime );

	Animation* getAnimation() const { return mAnim; }
	float getStartTime() const { return mStartTime; }
	float getEndTime() const { return mEndTime; }
	float getLength() const { return mEndTime - mStartTime; }
	
	/**
	* Computes the relative overlap factor of this animation segment
	* with another one (0 - no overlap; 1 - the segments are identical).
	*/
	float overlap( const AnimationSegment& seg ) const;

	/**
	* Merges this animation segment with another one.
	* The two segments must overlap.
	*/
	AnimationSegment merge( const AnimationSegment& seg ) const;

private:

	Animation* mAnim;
	float mStartTime;
	float mEndTime;

};

}

#endif // __zhAnimationSegment_h__
