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

#include "zhAnimationAnnotation.h"

namespace zh
{

AnimationAnnotation::AnimationAnnotation( float startTime, float endTime )
: mStartTime(startTime), mEndTime(endTime)
{
}

AnimationAnnotation::~AnimationAnnotation()
{
}

float AnimationAnnotation::getStartTime() const
{
	return mStartTime;
}

void AnimationAnnotation::setStartTime( float time )
{
	mStartTime = time;
	if( mEndTime < mStartTime ) mEndTime = mStartTime;
}

float AnimationAnnotation::getEndTime() const
{
	return mEndTime;
}

void AnimationAnnotation::setEndTime( float time )
{
	mEndTime = time;
	if( mEndTime < mStartTime ) mStartTime = mEndTime;
}

void AnimationAnnotation::_clone( AnimationAnnotation* clonePtr ) const
{
	zhAssert( getClassId() == clonePtr->getClassId() );

	/*clonePtr->mStartTime = mStartTime;
	clonePtr->mEndTime = mEndTime;*/
}

TransitionAnnotation::TransitionAnnotation( float startTime, float endTime )
: AnimationAnnotation( startTime, endTime ), mTargetSetId(0), mTargetId(0), mTargetTime(0)
{
}

TransitionAnnotation::~TransitionAnnotation()
{
}

unsigned long TransitionAnnotation::getTargetSetId() const
{
	return mTargetSetId;
}

void TransitionAnnotation::setTargetSetId( unsigned long targetSetId )
{
	mTargetSetId = targetSetId;
}

unsigned short TransitionAnnotation::getTargetId() const
{
	return mTargetId;
}

void TransitionAnnotation::setTargetId( unsigned short targetId )
{
	mTargetId = targetId;
}

float TransitionAnnotation::getTargetTime() const
{
	return mTargetTime;
}

void TransitionAnnotation::setTargetTime( float targetTime )
{
	mTargetTime = targetTime;
}

const Model::Situation& TransitionAnnotation::getAlignTransf() const
{
	return mAlignTransf;
}

void TransitionAnnotation::setAlignTransf( const Model::Situation& transf )
{
	mAlignTransf = transf;
}

void TransitionAnnotation::_clone( AnimationAnnotation* clonePtr ) const
{
	AnimationAnnotation::_clone(clonePtr);

	TransitionAnnotation* cl = static_cast<TransitionAnnotation*>(clonePtr);

	cl->mTargetSetId = mTargetSetId;
	cl->mTargetId = mTargetId;
	cl->mTargetTime = mTargetTime;
	cl->mAlignTransf = mAlignTransf;
}

ParamTransitionAnnotation::ParamTransitionAnnotation( float startTime, float endTime )
: AnimationAnnotation( startTime, endTime ), mTargetId(0), mTargetTime(0)
{
}

ParamTransitionAnnotation::~ParamTransitionAnnotation()
{
}

unsigned long ParamTransitionAnnotation::getTargetSetId() const
{
	return mTargetSetId;
}

void ParamTransitionAnnotation::setTargetSetId( unsigned long targetSetId )
{
	mTargetSetId = targetSetId;
}

unsigned short ParamTransitionAnnotation::getTargetId() const
{
	return mTargetId;
}

void ParamTransitionAnnotation::setTargetId( unsigned short targetId )
{
	mTargetId = targetId;
}

const Vector& ParamTransitionAnnotation::getLowerBound() const
{
	return mLBound;
}

void ParamTransitionAnnotation::setLowerBound( const Vector& lBound )
{
	mLBound = lBound;
}

const Vector& ParamTransitionAnnotation::getUpperBound() const
{
	return mUBound;
}

void ParamTransitionAnnotation::setUpperBound( const Vector& uBound )
{
	mUBound = uBound;
}

float ParamTransitionAnnotation::getTargetTime() const
{
	return mTargetTime;
}

void ParamTransitionAnnotation::setTargetTime( float time )
{
	mTargetTime = time;
}

const Model::Situation& ParamTransitionAnnotation::getAlignTransf() const
{
	return mAlignTransf;
}

void ParamTransitionAnnotation::setAlignTransf( const Model::Situation& transf )
{
	mAlignTransf = transf;
}

void ParamTransitionAnnotation::_clone( AnimationAnnotation* clonePtr ) const
{
	AnimationAnnotation::_clone(clonePtr);

	ParamTransitionAnnotation* cl = static_cast<ParamTransitionAnnotation*>(clonePtr);

	cl->mTargetSetId = mTargetSetId;
	cl->mTargetId = mTargetId;
	cl->mLBound = mLBound;
	cl->mUBound = mUBound;
	cl->mTargetTime = mTargetTime;
	cl->mAlignTransf = mAlignTransf;
}

PlantConstraintAnnotation::PlantConstraintAnnotation( float startTime, float endTime )
: AnimationAnnotation( startTime, endTime ), mBoneId(0)
{
}

PlantConstraintAnnotation::~PlantConstraintAnnotation()
{
}

unsigned short PlantConstraintAnnotation::getBoneId() const
{
	return mBoneId;
}
	
void PlantConstraintAnnotation::setBoneId( unsigned short boneId )
{
	mBoneId = boneId;
}

void PlantConstraintAnnotation::_clone( AnimationAnnotation* clonePtr ) const
{
	AnimationAnnotation::_clone(clonePtr);

	PlantConstraintAnnotation* cl = static_cast<PlantConstraintAnnotation*>(clonePtr);

	cl->mBoneId = mBoneId;
}

SimEventAnnotation::SimEventAnnotation( float startTime, float endTime )
: AnimationAnnotation( startTime, endTime ), mEvtCId(USHRT_MAX), mEvtId(USHRT_MAX)
{
}

SimEventAnnotation::~SimEventAnnotation()
{
}

unsigned short SimEventAnnotation::getEventClassId() const
{
	return mEvtCId;
}
	
void SimEventAnnotation::setEventClassId( unsigned short evtCId )
{
	mEvtCId = evtCId;
}

unsigned short SimEventAnnotation::getEventId() const
{
	return mEvtId;
}
	
void SimEventAnnotation::setEventId( unsigned short evtId )
{
	mEvtId = evtId;
}

void SimEventAnnotation::_clone( AnimationAnnotation* clonePtr ) const
{
	AnimationAnnotation::_clone(clonePtr);

	SimEventAnnotation* cl = static_cast<SimEventAnnotation*>(clonePtr);

	cl->mEvtCId = mEvtCId;
	cl->mEvtId = mEvtId;
}

GesturePhaseAnnotation::GesturePhaseAnnotation( float startTime, float endTime )
: AnimationAnnotation( startTime, endTime ), mGesturePhase(GesturePhase_Start)
{
}

GesturePhaseAnnotation::~GesturePhaseAnnotation()
{
}

AnimationGesturePhase GesturePhaseAnnotation::getGesturePhase() const
{
	return mGesturePhase;
}

void GesturePhaseAnnotation::setGesturePhase( AnimationGesturePhase gesturePhase )
{
	mGesturePhase = gesturePhase;
}

void GesturePhaseAnnotation::_clone( AnimationAnnotation* clonePtr ) const
{
	AnimationAnnotation::_clone(clonePtr);

	GesturePhaseAnnotation* cl = static_cast<GesturePhaseAnnotation*>(clonePtr);

	cl->mGesturePhase = mGesturePhase;
}

AnimationAnnotationContainer::AnimationAnnotationContainer()
{
}

AnimationAnnotationContainer::~AnimationAnnotationContainer()
{
	deleteAllAnnotations();
}

AnimationAnnotation* AnimationAnnotationContainer::createAnnotation( float startTime, float endTime )
{
	if( endTime < startTime ) endTime = startTime;

	AnimationAnnotation* annot = _createAnnotation( startTime, endTime );

	// add new annotation
	// (annots must be ordered by start time)
	if( mAnnots.empty() ||
		mAnnots[ mAnnots.size() - 1 ]->getStartTime() < startTime )
	{
		mAnnots.push_back(annot);
	}
	else
	{
		for( std::vector<AnimationAnnotation*>::iterator annoti = mAnnots.begin();
			annoti != mAnnots.end(); ++annoti )
		{
			if( startTime <= (*annoti)->getStartTime() )
			{
				mAnnots.insert( annoti, annot );
				break;
			}
		}
	}

	return annot;
}

void AnimationAnnotationContainer::deleteAnnotation( unsigned int index )
{
	zhAssert( index < getNumAnnotations() );

	delete mAnnots[index];
	mAnnots.erase( mAnnots.begin() + index );
}

void AnimationAnnotationContainer::deleteAllAnnotations()
{
	for( unsigned int ani = 0; ani < mAnnots.size(); ++ani )
		delete mAnnots[ani];

	mAnnots.clear();
}

AnimationAnnotation* AnimationAnnotationContainer::getAnnotation( unsigned int index ) const
{
	zhAssert( index < getNumAnnotations() );

	return mAnnots[index];
}

unsigned int AnimationAnnotationContainer::getNumAnnotations() const
{
	return mAnnots.size();
}

AnimationAnnotationContainer::AnnotationIterator AnimationAnnotationContainer::getAnnotationIterator()
{
	return AnnotationIterator( mAnnots );
}

AnimationAnnotationContainer::AnnotationConstIterator AnimationAnnotationContainer::getAnnotationConstIterator() const
{
	return AnnotationConstIterator( mAnnots );
}

unsigned int AnimationAnnotationContainer::getActiveAnnotations( float time, std::vector<AnimationAnnotation*>& annots ) const
{
	// TODO: this should be optimized

	annots.clear();

	for( unsigned int ani = 0; ani < mAnnots.size(); ++ani )
	{
		AnimationAnnotation* an = mAnnots[ani];
		float st = an->getStartTime(),
			et = an->getEndTime();

		if( time >= st && time <= et )
			annots.push_back(an);

		if( st > time )
			break;
	}

	return annots.size();
}

unsigned int AnimationAnnotationContainer::getActiveAnnotations( float prevTime, float time,
																std::vector<AnimationAnnotation*>& annots, bool countPassed ) const
{
	// TODO: this should be optimized

	annots.clear();

	for( unsigned int ani = 0; ani < mAnnots.size(); ++ani )
	{
		AnimationAnnotation* an = mAnnots[ani];
		float st = an->getStartTime(),
			et = an->getEndTime();

		if( prevTime < st && time >= st && time <= et ||
			countPassed && prevTime < st && time > et )
			annots.push_back(an);

		if( prevTime > et )
			break;
	}

	return annots.size();
}

unsigned int AnimationAnnotationContainer::getFinishedAnnotations( float prevTime, float time,
																  std::vector<AnimationAnnotation*>& annots, bool countPassed ) const
{
	// TODO: this should be optimized

	annots.clear();

	for( unsigned int ani = 0; ani < mAnnots.size(); ++ani )
	{
		AnimationAnnotation* an = mAnnots[ani];
		float st = an->getStartTime(),
			et = an->getEndTime();

		if( prevTime >= st && prevTime <= et && time > et ||
			countPassed && prevTime < st && time > et )
			annots.push_back(an);

		if( prevTime > et )
			break;
	}

	return annots.size();
}

void AnimationAnnotationContainer::_clone( AnimationAnnotationContainer* clonePtr ) const
{
	AnnotationConstIterator annot_i = getAnnotationConstIterator();
	while( annot_i.hasMore() )
	{
		AnimationAnnotation* annot = annot_i.next();

		AnimationAnnotation* cannot = clonePtr->createAnnotation( annot->getStartTime(), annot->getEndTime() );
		annot->_clone(cannot);
	}
}

AnimationAnnotation* TransitionAnnotationContainer::_createAnnotation( float startTime, float endTime )
{
	return new TransitionAnnotation( startTime, endTime );
}

AnimationAnnotation* ParamTransitionAnnotationContainer::_createAnnotation( float startTime, float endTime )
{
	return new ParamTransitionAnnotation( startTime, endTime );
}

AnimationAnnotation* PlantConstraintAnnotationContainer::_createAnnotation( float startTime, float endTime )
{
	return new PlantConstraintAnnotation( startTime, endTime );
}

AnimationAnnotation* SimEventAnnotationContainer::_createAnnotation( float startTime, float endTime )
{
	return new SimEventAnnotation( startTime, endTime );
}

AnimationAnnotation* GesturePhaseAnnotationContainer::_createAnnotation( float startTime, float endTime )
{
	return new GesturePhaseAnnotation( startTime, endTime );
}

}
