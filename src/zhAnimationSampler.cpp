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

#include "zhAnimationSampler.h"

namespace zh
{

AnimationSampler::AnimationSampler()
: mAnimSet(NULL), mAnimId(0), mPlayTime(0)
{
}

AnimationSampler::~AnimationSampler()
{
}

bool AnimationSampler::isLeaf() const
{
	return true;
}

void AnimationSampler::setPlaying( bool playing )
{
	AnimationNode::setPlaying(playing);

	if( !mPlaying )
		mPlayTime = 0;
}

float AnimationSampler::getPlayTime() const
{
	return mPlayTime;
}

void AnimationSampler::setPlayTime( float time )
{
	float length = getPlayLength();
	
	mPlayTime = time;

	if( mPlayTime >= length )
		mPlayTime -= floor(mPlayTime/length) * length;
}

float AnimationSampler::getNormalizedPlayTime() const
{
	return mPlayTime / getPlayLength();
}

void AnimationSampler::setNormalizedPlayTime( float time )
{
	setPlayTime( time * getPlayLength() );
}

float AnimationSampler::getPlayLength() const
{
	Animation* anim = getAnimation();

	return anim != NULL ? anim->getLength() : 0;
}

TransitionAnnotationContainer* AnimationSampler::getTransitionAnnotations() const
{
	Animation* anim = getAnimation();
	zhAssert( anim != NULL );

	return anim->getTransitionAnnotations();
}

ParamTransitionAnnotationContainer* AnimationSampler::getParamTransitionAnnotations() const
{
	Animation* anim = getAnimation();
	zhAssert( anim != NULL );

	return anim->getParamTransitionAnnotations();
}

PlantConstraintAnnotationContainer* AnimationSampler::getPlantConstraintAnnotations() const
{
	Animation* anim = getAnimation();
	zhAssert( anim != NULL );

	return anim->getPlantConstraintAnnotations();
}

SimEventAnnotationContainer* AnimationSampler::getSimEventAnnotations() const
{
	Animation* anim = getAnimation();
	zhAssert( anim != NULL );

	return anim->getSimEventAnnotations();
}

GesturePhaseAnnotationContainer* AnimationSampler::getGesturePhaseAnnotations() const
{
	Animation* anim = getAnimation();
	zhAssert( anim != NULL );

	return anim->getGesturePhaseAnnotations();
}

const Model::Situation& AnimationSampler::getOrigin() const
{
	return mOrigin;
}

void AnimationSampler::setOrigin( const Model::Situation& origin )
{
	mOrigin = origin;
}

AnimationSetPtr AnimationSampler::getAnimationSet() const
{
	return mAnimSet;
}

unsigned short AnimationSampler::getAnimationId() const
{
	return mAnimId;
}

Animation* AnimationSampler::getAnimation() const
{
	if( mAnimSet == NULL || !mAnimSet->hasAnimation(mAnimId) )
		return NULL;

	return mAnimSet->getAnimation(mAnimId);
}

void AnimationSampler::setAnimation( AnimationSetPtr animSet, unsigned short animId )
{
	zhAssert( animSet != NULL );

	mAnimSet = animSet;
	mAnimId = animId;
}

Model::Situation AnimationSampler::_sampleMover() const
{
	Animation* anim = getAnimation();

	if( anim == NULL )
		return Model::Situation::Identity;

	Model* mdl = mOwner->getModel();
	Skeleton* skel = mdl->getSkeleton();
	Bone* root = skel->getRoot();

	// sample anim. mover
	Vector3 ipos = root->getInitialPosition();
	Quat iorient = root->getInitialOrientation();
	BoneAnimationTrack* rbat = anim->getBoneTrack( root->getId() );
	TransformKeyFrame tkf( 0, 0 );
	rbat->getInterpolatedKeyFrame( mPlayTime, &tkf );
	Vector3 pos = ipos + tkf.getTranslation();
	Quat orient = iorient * tkf.getRotation();
	Model::Situation mv( pos, orient );

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

size_t AnimationSampler::_calcMemoryUsage() const
{
	return 0;
}

void AnimationSampler::_unload()
{
}

void AnimationSampler::_clone( AnimationNode* clonePtr, bool shareData ) const
{
	zhAssert( clonePtr != NULL );
	zhAssert( getClassId() == clonePtr->getClassId() );

	AnimationNode::_clone( clonePtr, shareData );

	zhLog( "AnimationSampler", "_clone",
		"Cloning AnimationSampler %s %u, %s.",
		getClassName().c_str(), mId, mName.c_str() );

	AnimationSampler* clone = static_cast<AnimationSampler*>( clonePtr );

	clone->mPlayTime = mPlayTime;
	clone->mOrigin = mOrigin;

	clone->mAnimSet = mAnimSet;
	clone->mAnimId = mAnimId;
	
	clone->mAnnotsEnabled = mAnnotsEnabled;
}

void AnimationSampler::_update( float dt )
{
	float length = getPlayLength();
	
	mPlayTime += dt;

	if( mPlayTime >= length )
		mPlayTime -= floor(mPlayTime/length) * length;
}

void AnimationSampler::_applyNode( float weight, const std::set<unsigned short>& boneMask ) const
{
	Animation* anim = getAnimation();

	if( anim == NULL )
		return;

	Model* mdl = mOwner->getModel();
	Skeleton* skel = mdl->getSkeleton();
	Bone* root = skel->getRoot();
	float scale = root->getScale().y;

	std::set<unsigned short> bone_mask = boneMask;

	if( mOwner->getApplyMover() )
	{
		// get anim. mover
		Model::Situation mv = _sampleMover();

		// apply realigned mover
		/*mv = Model::Situation( mv.getPosition() - root->getInitialPosition(),
			root->getInitialOrientation().getInverse() * mv.getOrientation() );
		root->translate( mv.getPosition() * weight );
		root->rotate( Quat().slerp( mv.getOrientation(), weight ) );*/
		root->setPosition( root->getPosition() + mv.getPosition() * weight );
		root->setOrientation( root->getOrientation().slerp( mv.getOrientation(),
			weight / ( mOwner->_getTotalWeight() + weight ) ) );

		/*
		float px1 = mv.getPosX(),
			pz1 = mv.getPosZ(),
			oy1 = mv.getOrientY();
		Model::Situation sit = mdl->getSituation();
		float px = sit.getPosX(),
			pz = sit.getPosZ(),
			oy = sit.getOrientY();
		float total_w = mOwner->_getTotalWeight();
		zhLog( "AnimationSampler", "_applyNode", "%s ; t = %f ; w/total_w = %.2f / %.2f ; mover = ( %f, %f, %f ) ; situation = ( %f, %f, %f )",
			mName.c_str(), mPlayTime, weight, total_w, px1, pz1, oy1, px, pz, oy );
		*/

		// mover applied separately, so mask root bone
		bone_mask.insert( root->getId() );
	}

	// apply animation
	anim->apply( mdl, mPlayTime, weight, scale, bone_mask );
}

}
