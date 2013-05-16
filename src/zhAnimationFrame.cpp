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

#include "zhAnimationFrame.h"
#include "zhAnimation.h"

namespace zh
{

AnimationFrame::AnimationFrame( Animation* anim, unsigned int frameIndex )
{
	zhAssert( anim != NULL );
	zhAssert( anim->getNumBoneTracks() > 0 );
	zhAssert( anim->getBoneTrack(0)->getNumKeyFrames() > frameIndex );

	Animation::BoneTrackConstIterator btr_i = anim->getBoneTrackConstIterator();
	while( btr_i.hasMore() )
	{
		BoneAnimationTrack* btr = btr_i.next();
		zhAssert( frameIndex < btr->getNumKeyFrames() );
		TransformKeyFrame* tkf = static_cast<TransformKeyFrame*>( btr->getKeyFrame(frameIndex) );

		if( btr->getBoneId() == 0 )
		{
			// Root bone
			this->rootPosition = Vector3(0, tkf->getTranslation().y, 0);
			Quat q = tkf->getRotation().log();
			this->rootOrientation = Quat(q.x, 0, q.z);
		}
		else
			// Some joint
			this->orientations.push_back( tkf->getRotation().log() );
	}
}

void AnimationFrame::extractTargetPositions(Skeleton* skel)
{
	zhAssert( skel != NULL && skel->getNumBones() == (1+orientations.size()) );
	// Apply animation frame to the skeleton
	skel->resetToInitialPose();
	Bone* root = skel->getRoot();
	root->setPosition(rootPosition);
	root->setOrientation(rootOrientation.exp());
	Skeleton::BoneIterator bone_i = skel->getBoneIterator();
	while( bone_i.hasMore() )
	{
		Bone* bone = bone_i.next();
		if( bone->getId() == 0 )
			continue;
		bone->rotate( orientations[bone->getId()-1].exp() );
	}
	Vector3 root_pos = skel->getRoot()->getWorldPosition();

	// Extract joint world positions
	std::vector<BoneTag> tags;
	tags.push_back(BT_LWrist);
	tags.push_back(BT_RWrist);
	tags.push_back(BT_LAnkle);
	tags.push_back(BT_RAnkle);
	for( unsigned int tag_i = 0; tag_i < (unsigned int)tags.size(); ++tag_i )
	{
		if( !skel->hasBoneWithTag(tags[tag_i]) )
			continue;
		
		Bone* bone = skel->getBoneByTag(tags[tag_i]);
		Vector3 wpos = bone->getWorldPosition();
		targetPositions.push_back(wpos - root_pos);
	}

	// Set skeleton pose back to initial
	skel->resetToInitialPose();
}

AnimationFrameSet::AnimationFrameSet()
{
}

AnimationFrameSet::~AnimationFrameSet()
{
}

void AnimationFrameSet::addFrame( const AnimationFrame& frame )
{
	mFrames.push_back(frame);
}

void AnimationFrameSet::removeFrame( unsigned int frameIndex )
{
	zhAssert( frameIndex < getNumFrames() );

	mFrames.erase( mFrames.begin() + frameIndex );
}

void AnimationFrameSet::removeAllFrames()
{
	mFrames.clear();
}

const AnimationFrame& AnimationFrameSet::getFrame( unsigned int frameIndex ) const
{
	zhAssert( frameIndex < getNumFrames() );

	return mFrames[frameIndex];
}

void AnimationFrameSet::setFrame( unsigned int frameIndex, const AnimationFrame& frame )
{
	zhAssert( frameIndex < getNumFrames() );

	mFrames[frameIndex] = frame;
}

unsigned int AnimationFrameSet::getNumFrames() const
{
	return (unsigned int)mFrames.size();
}

AnimationFrameSet::FrameIterator AnimationFrameSet::getFrameIterator()
{
	return FrameIterator(mFrames);
}

AnimationFrameSet::FrameConstIterator AnimationFrameSet::getFrameConstIterator() const
{
	return FrameConstIterator(mFrames);
}

unsigned int AnimationFrameSet::getNumTracks() const
{
	if( getNumFrames() <= 0 )
		return 0;

	return 6+3*(unsigned int)(mFrames[0].orientations.size());
}

void AnimationFrameSet::extractTargetPositions(Skeleton* skel)
{
	zhAssert( skel != NULL );

	for( unsigned int fri = 0; fri < getNumFrames(); ++fri )
		mFrames[fri].extractTargetPositions(skel);
}

}
