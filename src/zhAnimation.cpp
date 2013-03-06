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

#include "zhAnimation.h"
#include "zhSkeleton.h"

namespace zh
{

const std::set<unsigned short> Animation::EmptyBoneMask;

Animation::Animation( unsigned short id, const std::string& name, AnimationSetPtr animSet )
: mId(id), mName(name), mAnimSet(animSet), mInterpMethod(KFInterp_Spline)
{
	zhAssert( animSet != NULL );

	mTransAnnots = new TransitionAnnotationContainer();
	mParamTransAnnots = new ParamTransitionAnnotationContainer();
	mPlantConstrAnnots = new PlantConstraintAnnotationContainer();
	mSimEventAnnots = new SimEventAnnotationContainer();
}

Animation::~Animation()
{
	deleteAllBoneTracks();

	delete mTransAnnots;
	delete mParamTransAnnots;
	delete mPlantConstrAnnots;
	delete mSimEventAnnots;
}

unsigned short Animation::getId() const
{
	return mId;
}

const std::string& Animation::getName() const
{
	return mName;
}

AnimationSetPtr Animation::getAnimationSet() const
{
	return mAnimSet;
}

BoneAnimationTrack* Animation::createBoneTrack( unsigned short boneId )
{
	zhAssert( !hasBoneTrack(boneId) );

	BoneAnimationTrack* bat = new BoneAnimationTrack( boneId, this );
	mBoneTracks.insert( make_pair( bat->getBoneId(), bat ) );

	return bat;
}

void Animation::deleteBoneTrack( unsigned short boneId )
{
	std::map<unsigned short, BoneAnimationTrack*>::iterator bti = mBoneTracks.find(boneId);

	if( bti != mBoneTracks.end() )
	{
		delete bti->second;
		mBoneTracks.erase(bti);
	}
}

void Animation::deleteAllBoneTracks()
{
	for( std::map<unsigned short, BoneAnimationTrack*>::iterator bti = mBoneTracks.begin();
		bti != mBoneTracks.end(); ++bti )
		delete bti->second;

	mBoneTracks.clear();
}

bool Animation::hasBoneTrack( unsigned short boneId ) const
{
	return mBoneTracks.count(boneId) > 0;
}

BoneAnimationTrack* Animation::getBoneTrack( unsigned short boneId ) const
{
	std::map<unsigned short, BoneAnimationTrack*>::const_iterator bti = mBoneTracks.find(boneId);

	if( bti != mBoneTracks.end() )
		return bti->second;

	return NULL;
}

unsigned int Animation::getNumBoneTracks() const
{
	return mBoneTracks.size();
}

Animation::BoneTrackIterator Animation::getBoneTrackIterator()
{
	return BoneTrackIterator(mBoneTracks);
}

Animation::BoneTrackConstIterator Animation::getBoneTrackConstIterator() const
{
	return BoneTrackConstIterator(mBoneTracks);
}

KFInterpolationMethod Animation::getKFInterpolationMethod() const
{
	return mInterpMethod;
}

void Animation::setKFInterpolationMethod( KFInterpolationMethod interpMethod )
{
	mInterpMethod = interpMethod;

	if( mInterpMethod == KFInterp_Spline )
	{
		BoneTrackIterator bti = getBoneTrackIterator();
		while( !bti.end() )
			bti.next()->_buildInterpSplines();
	}
}

float Animation::getLength() const
{
	float cur_length = 0, length = 0;

	BoneTrackConstIterator bti = getBoneTrackConstIterator();
	while( !bti.end() )
	{
		if( ( cur_length = bti.next()->getLength() ) > length )
			length = cur_length;
	}

	return length;
}

int Animation::getFrameRate() const
{
	float cur_length = 0, length = 0;
	int num_fr = 0;

	BoneTrackConstIterator bti = getBoneTrackConstIterator();
	while( !bti.end() )
	{
		BoneAnimationTrack* bat = bti.next();
		if( ( cur_length = bat->getLength() ) > length )
		{
			length = cur_length;
			num_fr = bat->getNumKeyFrames();
		}
	}

	return length > 0 ? zhRoundi(num_fr/length) : 0;
}

void Animation::apply( Skeleton* skel, float time, float weight, float scale,
					  const std::set<unsigned short> boneMask ) const
{
	zhAssert( skel != NULL );

	// apply bone tracks
	BoneTrackConstIterator bti = getBoneTrackConstIterator();
	while( !bti.end() )
	{
		BoneAnimationTrack* bat = bti.next();
		if( boneMask.count( bat->getBoneId() ) <= 0 )
			bat->apply( skel, time, weight, scale );
	}
}

void Animation::computeAnimationBounds( float& minX, float& maxX, float& minY, float& maxY,
	float& minZ, float& maxZ ) const
{
	BoneAnimationTrack* root_tr = getBoneTrack(0);
	if( root_tr == NULL )
		return;

	minX = minY = minZ = FLT_MAX;
	maxX = maxY = maxZ = -FLT_MAX;
	AnimationTrack::KeyFrameConstIterator kfi = root_tr->getKeyFrameConstIterator();
	while( kfi.hasMore() )
	{
		TransformKeyFrame* tkf = static_cast<TransformKeyFrame*>(kfi.next());

		Vector3 trans = tkf->getTranslation();
		if( trans.x < minX )
			minX = trans.x;
		if( trans.x > maxX )
			maxX = trans.x;
		if( trans.y < minY )
			minY = trans.y;
		if( trans.y > maxY )
			maxY = trans.y;
		if( trans.z < minZ )
			minZ = trans.z;
		if( trans.z > maxZ )
			maxZ = trans.z;
	}
}

TransitionAnnotationContainer* Animation::getTransitionAnnotations() const
{
	return mTransAnnots;
}

ParamTransitionAnnotationContainer* Animation::getParamTransitionAnnotations() const
{
	return mParamTransAnnots;
}

PlantConstraintAnnotationContainer* Animation::getPlantConstraintAnnotations() const
{
	return mPlantConstrAnnots;
}

SimEventAnnotationContainer* Animation::getSimEventAnnotations() const
{
	return mSimEventAnnots;
}

size_t Animation::_calcMemoryUsage() const
{
	size_t mem_usage = 0;

	// Compute memory usage from anim. tracks:

	BoneTrackConstIterator bti = getBoneTrackConstIterator();
	while( !bti.end() )
	{
		mem_usage += ( bti.next()->getNumKeyFrames() *
			( sizeof(float) + 2 * sizeof(Vector3) + sizeof(Quat) ) );
	}

	// Compute memory usage from annotations:

	mem_usage += mTransAnnots->getNumAnnotations() * sizeof(TransitionAnnotation);
	mem_usage += mParamTransAnnots->getNumAnnotations() * sizeof(ParamTransitionAnnotation);
	mem_usage += mPlantConstrAnnots->getNumAnnotations() * sizeof(PlantConstraintAnnotation);
	mem_usage += mSimEventAnnots->getNumAnnotations() * sizeof(SimEventAnnotation);

	return mem_usage;
}

void Animation::_clone( Animation* clonePtr ) const
{
	zhAssert( clonePtr != NULL );

	// Copy animation tracks:

	// Create empty bone tracks
	BoneTrackConstIterator bti = getBoneTrackConstIterator();
	while( !bti.end() )
	{
		BoneAnimationTrack* bat = bti.next();
		clonePtr->createBoneTrack( bat->getBoneId() );
	}

	// Copy bone tracks
	bti = getBoneTrackConstIterator();
	while( !bti.end() )
	{
		BoneAnimationTrack* bat = bti.next();
		BoneAnimationTrack* cbat = clonePtr->getBoneTrack( bat->getBoneId() );

		for( unsigned int kfi = 0; kfi < bat->getNumKeyFrames(); ++kfi )
		{
			TransformKeyFrame* kf = ( TransformKeyFrame* )bat->getKeyFrame(kfi);
			TransformKeyFrame* ckf = ( TransformKeyFrame* )cbat->createKeyFrame( kf->getTime() );
			ckf->setTranslation( kf->getTranslation() );
			ckf->setRotation( kf->getRotation() );
			ckf->setScale( kf->getScale() );
		}
	}

	clonePtr->setKFInterpolationMethod( mInterpMethod );

	// Copy local annotations
	mTransAnnots->_clone( clonePtr->mTransAnnots );
	mParamTransAnnots->_clone( clonePtr->mParamTransAnnots );
	mPlantConstrAnnots->_clone( clonePtr->mPlantConstrAnnots );
	mSimEventAnnots->_clone( clonePtr->mSimEventAnnots );
}

}
