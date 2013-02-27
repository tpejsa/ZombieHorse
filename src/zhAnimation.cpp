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
#include "zhModel.h"

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
	mGestPhaseAnnots = new GesturePhaseAnnotationContainer();
}

Animation::~Animation()
{
	deleteAllBoneTracks();
	deleteAllMeshTracks();

	delete mTransAnnots;
	delete mParamTransAnnots;
	delete mPlantConstrAnnots;
	delete mSimEventAnnots;
	delete mGestPhaseAnnots;
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

MeshAnimationTrack* Animation::createMeshTrack( unsigned short meshId )
{
	zhAssert( !hasMeshTrack(meshId) );

	MeshAnimationTrack* mat = new MeshAnimationTrack( meshId, this );
	mMeshTracks.insert( make_pair( mat->getMeshId(), mat ) );

	return mat;
}

void Animation::deleteMeshTrack( unsigned short meshId )
{
	std::map<unsigned short, MeshAnimationTrack*>::iterator mti = mMeshTracks.find(meshId);

	if( mti != mMeshTracks.end() )
	{
		delete mti->second;
		mMeshTracks.erase(mti);
	}
}

void Animation::deleteAllMeshTracks()
{
	for( std::map<unsigned short, MeshAnimationTrack*>::iterator mti = mMeshTracks.begin();
		mti != mMeshTracks.end(); ++mti )
		delete mti->second;

	mMeshTracks.clear();
}

bool Animation::hasMeshTrack( unsigned short meshId ) const
{
	return mMeshTracks.count(meshId) > 0;
}

MeshAnimationTrack* Animation::getMeshTrack( unsigned short meshId ) const
{
	std::map<unsigned short, MeshAnimationTrack*>::const_iterator mti = mMeshTracks.find(meshId);

	if( mti != mMeshTracks.end() )
		return mti->second;

	return NULL;
}

unsigned int Animation::getNumMeshTracks() const
{
	return mMeshTracks.size();
}

Animation::MeshTrackIterator Animation::getMeshTrackIterator()
{
	return MeshTrackIterator(mMeshTracks);
}

Animation::MeshTrackConstIterator Animation::getMeshTrackConstIterator() const
{
	return MeshTrackConstIterator(mMeshTracks);
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

	MeshTrackConstIterator mti = getMeshTrackConstIterator();
	while( !mti.end() )
	{
		if( ( cur_length = mti.next()->getLength() ) > length )
			length = cur_length;
	}

	return length;
}

void Animation::apply( Model* model, float time, float weight, float scale,
					  const std::set<unsigned short> boneMask ) const
{
	zhAssert( model != NULL );

	Skeleton* skel = model->getSkeleton();

	// apply bone tracks
	BoneTrackConstIterator bti = getBoneTrackConstIterator();
	while( !bti.end() )
	{
		BoneAnimationTrack* bat = bti.next();
		if( boneMask.count( bat->getBoneId() ) <= 0 )
			bat->apply( model, time, weight, scale );
	}

	// apply mesh tracks
	MeshTrackConstIterator mti = getMeshTrackConstIterator();
	while( !mti.end() )
	{
		mti.next()->apply( model, time, weight, scale );
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

GesturePhaseAnnotationContainer* Animation::getGesturePhaseAnnotations() const
{
	return mGestPhaseAnnots;
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

	MeshTrackConstIterator mti = getMeshTrackConstIterator();
	while( !mti.end() )
	{
		MeshAnimationTrack* mat = mti.next();
		
		if( mat->getNumKeyFrames() <= 0 )
			continue;

		mem_usage += ( mti.next()->getNumKeyFrames() * sizeof(float) *
			((MorphKeyFrame*)mat->getKeyFrame(0))->getMorphTargetWeights().size() );
	}

	// Compute memory usage from annotations:

	mem_usage += mTransAnnots->getNumAnnotations() * sizeof(TransitionAnnotation);
	mem_usage += mParamTransAnnots->getNumAnnotations() * sizeof(ParamTransitionAnnotation);
	mem_usage += mPlantConstrAnnots->getNumAnnotations() * sizeof(PlantConstraintAnnotation);
	mem_usage += mSimEventAnnots->getNumAnnotations() * sizeof(SimEventAnnotation);
	mem_usage += mGestPhaseAnnots->getNumAnnotations() * sizeof(GesturePhaseAnnotation);

	return mem_usage;
}

void Animation::_clone( Animation* clonePtr ) const
{
	zhAssert( clonePtr != NULL );

	// Copy animation tracks:

	// create empty bone tracks
	BoneTrackConstIterator bti = getBoneTrackConstIterator();
	while( !bti.end() )
	{
		BoneAnimationTrack* bat = bti.next();
		clonePtr->createBoneTrack( bat->getBoneId() );
	}

	// create empty mesh tracks
	MeshTrackConstIterator mti = getMeshTrackConstIterator();
	while( !mti.end() )
	{
		MeshAnimationTrack* mat = mti.next();
		clonePtr->createMeshTrack( mat->getMeshId() );
	}

	// copy bone tracks
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

	// copy mesh tracks
	mti = getMeshTrackConstIterator();
	while( !mti.end() )
	{
		MeshAnimationTrack* mat = mti.next();
		MeshAnimationTrack* cmat = clonePtr->getMeshTrack( mat->getMeshId() );

		for( unsigned int kfi = 0; kfi < mat->getNumKeyFrames(); ++kfi )
		{
			MorphKeyFrame* kf = ( MorphKeyFrame* )mat->getKeyFrame(kfi);
			MorphKeyFrame* ckf = ( MorphKeyFrame* )cmat->createKeyFrame( kf->getTime() );
			ckf->setMorphTargetWeights( kf->getMorphTargetWeights() );
		}
	}

	clonePtr->setKFInterpolationMethod( mInterpMethod );

	// copy local annotations
	mTransAnnots->_clone( clonePtr->mTransAnnots );
	mParamTransAnnots->_clone( clonePtr->mParamTransAnnots );
	mPlantConstrAnnots->_clone( clonePtr->mPlantConstrAnnots );
	mSimEventAnnots->_clone( clonePtr->mSimEventAnnots );
	mGestPhaseAnnots->_clone( clonePtr->mGestPhaseAnnots );
}

}
