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

#include "zhAnimationTrack.h"

#include <algorithm>

namespace zh
{

KeyFrame::KeyFrame( float time, unsigned int index ) : mTime(time), mIndex(index)
{
}

KeyFrame::~KeyFrame()
{
}

float KeyFrame::getTime() const
{
	return mTime;
}

unsigned int KeyFrame::getIndex() const
{
	return mIndex;
}

void KeyFrame::_setIndex( unsigned int index )
{
	mIndex = index;
}

bool KeyFrameCompare::operator()( const KeyFrame* kf1, const KeyFrame* kf2 )
{
	return kf1->getTime() < kf2->getTime();
}

bool KeyFrameCompare::operator()( const KeyFrame* kf1, float t2 )
{
	return kf1->getTime() < t2;
}

bool KeyFrameCompare::operator()( float t1, const KeyFrame* kf2 )
{
	return t1 < kf2->getTime();
}

AnimationTrack::AnimationTrack( Animation* anim )
{
	zhAssert( anim != NULL );

	mAnim = anim;
}

AnimationTrack::~AnimationTrack()
{
	deleteAllKeyFrames();
}

Animation* AnimationTrack::getAnimation() const
{
	return mAnim;
}

KeyFrame* AnimationTrack::createKeyFrame( float time )
{
	KeyFrame* kf = NULL;

	for( std::vector<KeyFrame*>::iterator kfi = mKeyFrames.begin();
		kfi != mKeyFrames.end(); ++kfi )
	{
		if( zhEqualf( (*kfi)->getTime(), time ) )
		{
			kf = *kfi;
			break;
		}
		else if( (*kfi)->getTime() > time )
		{
			kf = _createKeyFrame(time);
			mKeyFrames.insert( kfi, kf );
			break;
		}
	}

	if( kf == NULL )
	{
		kf = _createKeyFrame(time);
		mKeyFrames.push_back(kf);
	}

	_updateKeyFrameIndices();

	return kf;
}

void AnimationTrack::deleteKeyFrame( unsigned int index )
{
	zhAssert( index < getNumKeyFrames() );

	delete mKeyFrames[index];
	mKeyFrames.erase( mKeyFrames.begin() + index );

	_updateKeyFrameIndices();
}

void AnimationTrack::deleteAllKeyFrames()
{
	for( unsigned int kfi = 0; kfi < mKeyFrames.size(); ++kfi )
		delete mKeyFrames[kfi];

	mKeyFrames.clear();
}

KeyFrame* AnimationTrack::getKeyFrame( unsigned int index ) const
{
	zhAssert( index < getNumKeyFrames() );

	return mKeyFrames[index];
}

unsigned int AnimationTrack::getNumKeyFrames() const
{
	return mKeyFrames.size();
}

AnimationTrack::KeyFrameIterator AnimationTrack::getKeyFrameIterator()
{
	return KeyFrameIterator( mKeyFrames );
}

AnimationTrack::KeyFrameConstIterator AnimationTrack::getKeyFrameConstIterator() const
{
	return KeyFrameConstIterator( mKeyFrames );
}

float AnimationTrack::getKeyFramesAtTime( float time, KeyFrame** kf1, KeyFrame** kf2 ) const
{
	float t = 0;
	*kf1 = *kf2 = NULL;

	if( mKeyFrames.size() == 0 )
		return 0;

	KeyFrameCompare kfc;
	std::vector<KeyFrame*>::const_iterator kfi =
		upper_bound( mKeyFrames.begin(), mKeyFrames.end(), time, kfc );

	if( kfi == mKeyFrames.begin() )
	{
		*kf1 = *kf2 = *kfi;
	}
	else if( kfi == mKeyFrames.end() )
	{
		*kf1 = *kf2 = *( kfi - 1 );
	}
	else
	{
		*kf1 = *( kfi - 1 );
		*kf2 = *kfi;
		t = ( time - (*kf1)->getTime() ) / ( (*kf2)->getTime() - (*kf1)->getTime() );
	}

	return t;
}

float AnimationTrack::getLength() const
{
	if( getNumKeyFrames() <= 0 )
		return 0;

	return mKeyFrames[ getNumKeyFrames() - 1 ]->getTime();
}

void AnimationTrack::_updateKeyFrameIndices()
{
	for( unsigned int kfi = 0; kfi < mKeyFrames.size(); ++kfi )
		mKeyFrames[kfi]->_setIndex(kfi);
}

}
