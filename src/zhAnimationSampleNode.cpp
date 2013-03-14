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

#include "zhAnimationSampleNode.h"

namespace zh
{

AnimationSampleNode::AnimationSampleNode()
: mAnimSet(NULL), mAnimId(0), mPlayTime(0)
{
}

AnimationSampleNode::~AnimationSampleNode()
{
}

bool AnimationSampleNode::isLeaf() const
{
	return true;
}

void AnimationSampleNode::setPlaying( bool playing )
{
	AnimationNode::setPlaying(playing);

	if( !mPlaying )
		mPlayTime = 0;
}

float AnimationSampleNode::getPlayTime() const
{
	return mPlayTime;
}

void AnimationSampleNode::setPlayTime( float time )
{
	float length = getPlayLength();
	
	mPlayTime = time;

	if( mPlayTime >= length )
		mPlayTime -= floor(mPlayTime/length) * length;
}

float AnimationSampleNode::getNormalizedPlayTime() const
{
	return mPlayTime / getPlayLength();
}

void AnimationSampleNode::setNormalizedPlayTime( float time )
{
	setPlayTime( time * getPlayLength() );
}

float AnimationSampleNode::getPlayLength() const
{
	Animation* anim = getAnimation();

	return anim != NULL ? anim->getLength() : 0;
}

TransitionAnnotationContainer* AnimationSampleNode::getTransitionAnnotations() const
{
	Animation* anim = getAnimation();
	zhAssert( anim != NULL );

	return anim->getTransitionAnnotations();
}

ParamTransitionAnnotationContainer* AnimationSampleNode::getParamTransitionAnnotations() const
{
	Animation* anim = getAnimation();
	zhAssert( anim != NULL );

	return anim->getParamTransitionAnnotations();
}

PlantConstraintAnnotationContainer* AnimationSampleNode::getPlantConstraintAnnotations() const
{
	Animation* anim = getAnimation();
	zhAssert( anim != NULL );

	return anim->getPlantConstraintAnnotations();
}

SimEventAnnotationContainer* AnimationSampleNode::getSimEventAnnotations() const
{
	Animation* anim = getAnimation();
	zhAssert( anim != NULL );

	return anim->getSimEventAnnotations();
}

const Skeleton::Situation& AnimationSampleNode::getOrigin() const
{
	return mOrigin;
}

void AnimationSampleNode::setOrigin( const Skeleton::Situation& origin )
{
	mOrigin = origin;
}

AnimationSetPtr AnimationSampleNode::getAnimationSet() const
{
	return mAnimSet;
}

unsigned short AnimationSampleNode::getAnimationId() const
{
	return mAnimId;
}

Animation* AnimationSampleNode::getAnimation() const
{
	if( mAnimSet == NULL || !mAnimSet->hasAnimation(mAnimId) )
		return NULL;

	return mAnimSet->getAnimation(mAnimId);
}

void AnimationSampleNode::setAnimation( AnimationSetPtr animSet, unsigned short animId )
{
	zhAssert( animSet != NULL );

	mAnimSet = animSet;
	mAnimId = animId;
}

Skeleton::Situation AnimationSampleNode::_sampleMover() const
{
	Animation* anim = getAnimation();

	if( anim == NULL )
		return Skeleton::Situation::Identity;

	Skeleton* skel = mOwner->_getCurrentSkeleton();
	Bone* root = skel->getRoot();

	// sample anim. mover
	Vector3 ipos = root->getInitialPosition();
	Quat iorient = root->getInitialOrientation();
	BoneAnimationTrack* rbat = anim->getBoneTrack( root->getId() );
	TransformKeyFrame tkf( 0, 0 );
	rbat->getInterpolatedKeyFrame( mPlayTime, &tkf );
	Vector3 pos = ipos + tkf.getTranslation();
	Quat orient = iorient * tkf.getRotation();
	Skeleton::Situation mv( pos, orient );

	// realign mover
	mv = mOrigin.getTransformed(mv);
	
	// vertical translation and rotation are applied directly
	pos.x = mv.getPosX();
	pos.z = mv.getPosZ();
	float ax, ay, az;
	orient.getEuler( ax, ay, az );
	orient = Quat( ax, mv.getOrientY(), az );
	mv.setPosition(pos);
	mv.setOrientation(orient);

	return mv;
}

void AnimationSampleNode::_clone( AnimationNode* clonePtr, bool shareData ) const
{
	zhAssert( clonePtr != NULL );
	zhAssert( getClassId() == clonePtr->getClassId() );

	AnimationNode::_clone( clonePtr, shareData );

	zhLog( "AnimationSampleNode", "_clone",
		"Cloning AnimationSampleNode %s %u, %s.",
		getClassName().c_str(), mId, mName.c_str() );

	AnimationSampleNode* clone = static_cast<AnimationSampleNode*>( clonePtr );

	clone->mPlayTime = mPlayTime;
	clone->mOrigin = mOrigin;

	clone->mAnimSet = mAnimSet;
	clone->mAnimId = mAnimId;
	
	clone->mAnnotsEnabled = mAnnotsEnabled;
}

void AnimationSampleNode::_updateNode( float dt )
{
	float length = getPlayLength();
	
	mPlayTime += dt;

	if( mPlayTime >= length )
		mPlayTime -= floor(mPlayTime/length) * length;
}

void AnimationSampleNode::_applyNode( float weight, const std::set<unsigned short>& boneMask ) const
{
	Animation* anim = getAnimation();

	if( anim == NULL )
		return;

	Skeleton* skel = mOwner->_getCurrentSkeleton();
	Bone* root = skel->getRoot();
	float scale = root->getScale().y;

	std::set<unsigned short> bone_mask = boneMask;

	if( mOwner->getApplyMover() )
	{
		// get anim. mover
		Skeleton::Situation mv = _sampleMover();

		// apply realigned mover
		/*mv = Skeleton::Situation( mv.getPosition() - root->getInitialPosition(),
			root->getInitialOrientation().getInverse() * mv.getOrientation() );
		root->translate( mv.getPosition() * weight );
		root->rotate( Quat().slerp( mv.getOrientation(), weight ) );*/
		root->setPosition( root->getPosition() + mv.getPosition() * weight );
		root->setOrientation( root->getOrientation().slerp( mv.getOrientation(),
			weight / ( mOwner->_getTotalWeight() + weight ) ) );

		// mover applied separately, so mask root bone
		bone_mask.insert( root->getId() );
	}

	// apply animation
	anim->apply( skel, mPlayTime, weight, scale, bone_mask );
}

}
