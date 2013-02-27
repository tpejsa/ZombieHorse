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

#include "zhAnimationSpace.h"
#include "zhAnimation.h"
#include "zhMath.h"

namespace zh
{

AnimationSpace::AnimationSpace( unsigned short id, const std::string& name, AnimationSetPtr animSet )
: mId(id), mName(name), mAnimSet(animSet), mParam(NULL)
{
	// TODO: must do something about the problem with this pointer being uninitialized
	zhAssert( animSet != NULL );
}

AnimationSpace::~AnimationSpace()
{
}

unsigned short AnimationSpace::getId() const
{
	return mId;
}

const std::string& AnimationSpace::getName() const
{
	return mName;
}

AnimationSetPtr AnimationSpace::getAnimationSet() const
{
	return mAnimSet;
}

void AnimationSpace::addBaseAnimation( Animation* anim )
{
	zhAssert( anim != NULL );
	zhAssert( mAnimSet->hasAnimation( anim->getId() ) );

	mBaseAnims.push_back(anim);
}

void AnimationSpace::removeBaseAnimation( unsigned int animIndex )
{
	zhAssert( animIndex < mBaseAnims.size() );

	mBaseAnims.erase( mBaseAnims.begin() + animIndex );
}

Animation* AnimationSpace::getBaseAnimation( unsigned int animIndex ) const
{
	zhAssert( animIndex < mBaseAnims.size() );

	return mBaseAnims[animIndex];
}

unsigned int AnimationSpace::getNumBaseAnimations() const
{
	return mBaseAnims.size();
}

AnimationSpace::AnimationIterator AnimationSpace::getBaseAnimationIterator()
{
	return AnimationIterator(mBaseAnims);
}

AnimationSpace::AnimationConstIterator AnimationSpace::getBaseAnimationConstIterator() const
{
	return AnimationConstIterator(mBaseAnims);
}

Animation* AnimationSpace::findBaseAnimation( const std::string& animName ) const
{
	for( unsigned int banim_i = 0; banim_i < mBaseAnims.size(); ++banim_i )
		if( mBaseAnims[banim_i]->getName() == animName )
			return mBaseAnims[banim_i];

	return NULL;
}

bool AnimationSpace::hasTimewarpCurve() const
{
	return mTWCurve.getNumControlPoints() > 0;
}

const CatmullRomSpline<Vector>& AnimationSpace::getTimewarpCurve() const
{
	return mTWCurve;
}

void AnimationSpace::setTimewarpCurve( const CatmullRomSpline<Vector>& curve )
{
	mTWCurve = curve;
	mTWCurve.calcTangents();
}

bool AnimationSpace::hasAlignmentCurve() const
{
	return mAlignCurve.getNumControlPoints() > 0;
}

const CatmullRomSpline<Vector>& AnimationSpace::getAlignmentCurve() const
{
	return mAlignCurve;
}

void AnimationSpace::setAlignmentCurve( const CatmullRomSpline<Vector>& curve )
{
	mAlignCurve = curve;
	mAlignCurve.calcTangents();
}

AnimationParametrization* AnimationSpace::createParametrization(
	AnimationParamClass classId, unsigned int numParams, unsigned int numBaseSamples )
{
	if( classId == AnimationParam_DenseSampling )
		mParam = new DenseSamplingParametrization( numParams, numBaseSamples );

	return mParam;
}

void AnimationSpace::deleteParametrization()
{
	if( mParam != NULL )
	{
		delete mParam;
		mParam = NULL;
	}
}

bool AnimationSpace::hasParametrization() const
{
	return mParam != NULL;
}

AnimationParametrization* AnimationSpace::getParametrization() const
{
	return mParam;
}

size_t AnimationSpace::_calcMemoryUsage() const
{
	size_t mem_usage = 0;

	// Compute memory usage from blend curves:

	if( hasTimewarpCurve() )
		mem_usage += mTWCurve.getNumControlPoints() * getNumBaseAnimations() * sizeof(float);
	if( hasAlignmentCurve() )
		mem_usage += mAlignCurve.getNumControlPoints() * getNumBaseAnimations() * 3 * sizeof(float);

	// Compute memory usage from parametrization:

	if( hasParametrization() )
	{
		mem_usage += mParam->getNumBaseSamples() * mParam->getNumParams() * sizeof(float);
		// if( mParam->getClassId() == DenseSamplingParametrization::ClassId() )
		mem_usage += static_cast<DenseSamplingParametrization*>(mParam)->getNumSamples() *
			( mParam->getNumParams() + mParam->getNumBaseSamples() ) * sizeof(float);
	}

	return mem_usage;
}

void AnimationSpace::_clone( AnimationSpace* clonePtr ) const
{
	zhAssert( clonePtr != NULL );

	for( unsigned int banim_i = 0; banim_i < getNumBaseAnimations(); ++banim_i )
		clonePtr->addBaseAnimation( clonePtr->getAnimationSet()->getAnimation( getBaseAnimation(banim_i)->getId() ) );

	// Copy blend curves:

	clonePtr->mTWCurve = mTWCurve;
	clonePtr->mAlignCurve = mAlignCurve;

	// Copy parametrization:

	AnimationParametrization* cl_param = clonePtr->createParametrization( mParam->getClassId(),
		mParam->getNumParams(), mParam->getNumBaseSamples() );

	for( unsigned int bsi = 0; bsi < cl_param->getNumBaseSamples(); ++bsi )
		cl_param->setBaseSample( bsi, mParam->getBaseSample(bsi) );
	
	// if( mParam->getClassId() == DenseSamplingParametrization::ClassId() )
	DenseSamplingParametrization* dsparam = static_cast<DenseSamplingParametrization*>(mParam);
	for( unsigned int sample_i = 0; sample_i < dsparam->getNumSamples(); ++sample_i )
	{
		Vector params, weights;
		dsparam->getSample( sample_i, params, weights );
		static_cast<DenseSamplingParametrization*>(cl_param)->addSample( params, weights );
	}
	dsparam->buildKDTree();
}

}
