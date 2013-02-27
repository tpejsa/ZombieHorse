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

#include "zhAnimationSegment.h"
#include "zhAnimation.h"

namespace zh
{

AnimationSegment::AnimationSegment( Animation* anim, float startTime, float endTime )
: mAnim(anim), mStartTime(startTime), mEndTime(endTime)
{
	zhAssert( anim != NULL );

	if( startTime > anim->getLength() )
		startTime = anim->getLength();

	if( endTime > anim->getLength() )
		endTime = anim->getLength();

	if( startTime > endTime )
		endTime = startTime;
}

float AnimationSegment::overlap( const AnimationSegment& seg ) const
{
	if( mAnim != seg.getAnimation() )
		return 0;

	AnimationSegment seg1 = *this,
		seg2 = seg;
	
	if( seg2.mStartTime < seg1.mStartTime )
		std::swap( seg1, seg2 );

	float l_int = 0; // length of the intersection

	if( seg2.mStartTime < seg1.mEndTime )
	{
		if( seg2.mEndTime <= seg1.mEndTime )
			l_int = seg2.mEndTime - seg2.mStartTime;
		else
			l_int = seg1.mEndTime - seg2.mStartTime;
	}

	float l_seg = std::min<float>( seg1.mEndTime - seg1.mStartTime,
		seg2.mEndTime - seg2.mStartTime ); // length of the shorter segment

	return l_int/l_seg;
}

AnimationSegment AnimationSegment::merge( const AnimationSegment& seg ) const
{
	if( mAnim != seg.getAnimation() )
		return AnimationSegment( mAnim, 0, 0 );

	AnimationSegment seg1 = *this,
		seg2 = seg;
	
	if( seg2.mStartTime < seg1.mStartTime )
		std::swap( seg1, seg2 );

	if( seg2.mStartTime < seg1.mEndTime )
	{
		if( seg2.mEndTime <= seg1.mEndTime )
			return seg1;
		else
			return AnimationSegment( mAnim, seg1.mStartTime, seg2.mEndTime );
	}

	return AnimationSegment( mAnim, 0, 0 );
}

}
