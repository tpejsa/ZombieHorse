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

#include "zhAnimationBlender.h"
#include "zhString.h"
#include "zhMath.h"
#include "zhSkeleton.h"
#include "zhAnimation.h"
#include "zhAnimationNode.h"
#include "zhAnimationSampler.h"

namespace zh
{

AnimationBlender::AnimationBlender()
: mParamEnabled(true), mAnimSet(NULL), mAnimSpaceId(0), mUseBlendCurves(true), mTWCurveTime(0)
{
}

AnimationBlender::~AnimationBlender()
{
}

bool AnimationBlender::isLeaf() const
{
	return false;
}

void AnimationBlender::setPlaying( bool playing )
{
	AnimationNode::setPlaying(playing);

	if( !mPlaying )
	{
		mTWCurveTime = 0;

		// Disable all constraint enforcement
		// TODO
	}
}

float AnimationBlender::getPlayTime() const
{
	return getNormalizedPlayTime() * getPlayLength();
}

void AnimationBlender::setPlayTime( float time )
{
	setNormalizedPlayTime( time/getPlayLength() );
}

float AnimationBlender::getNormalizedPlayTime() const
{
	AnimationSpace* anim_space = getAnimationSpace();
	AnimationNode* cn = getMainChild();

	if( cn == NULL )
		return 0;

	if( !mUseBlendCurves || ( anim_space != NULL && !anim_space->hasTimewarpCurve() ) )
	{
		return cn->getNormalizedPlayTime();
	}
	else
	{
		return mTWCurveTime / ( anim_space->getTimewarpCurve().getNumControlPoints() - 1 );
	}
}

void AnimationBlender::setNormalizedPlayTime( float time )
{
	AnimationSpace* anim_space = getAnimationSpace();

	if( !mUseBlendCurves || ( anim_space != NULL && !anim_space->hasTimewarpCurve() ) )
	{
		ChildIterator ci = getChildIterator();
		while( !ci.end() )
			ci.next()->setNormalizedPlayTime(time);
	}
	else
	{
		mTWCurveTime = 0;
		ChildIterator ci = getChildIterator();
		while( !ci.end() )
			ci.next()->setPlayTime(0);

		_update( time * getPlayLength() );
	}
}

float AnimationBlender::getPlayLength() const
{
	float play_length = 0;

	ChildConstIterator ci = getChildConstIterator();
	while( !ci.end() )
	{
		AnimationNode* cn = ci.next();
		play_length += cn->getPlayLength() * getWeight( cn->getId() );
	}

	return play_length;
}

const Skeleton::Situation& AnimationBlender::getOrigin() const
{
	return mOrigin;
}

void AnimationBlender::setOrigin( const Skeleton::Situation& origin )
{
	AnimationSpace* anim_space = getAnimationSpace();

	// set local origin
	mOrigin = origin;

	// also set origins of child nodes
	if( !mUseBlendCurves || ( anim_space != NULL && !anim_space->hasAlignmentCurve() ) )
	{
		// not using per-frame alignment, so origin is simply inherited
		
		ChildConstIterator child_i = getChildConstIterator();
		while( !child_i.end() )
		{
			AnimationNode* child = child_i.next();

			child->setOrigin(mOrigin);
		}
	}
	else
	{
		// using per-frame alignment, realign origin for each child anim.

		// get align. transf.
		unsigned int cpi;
		float t;
		_getTWCurvePosition( mTWCurveTime, cpi, t );
		const CatmullRomSpline<Vector>& align_curve = anim_space->getAlignmentCurve();
		Vector child_align = align_curve.getPoint( cpi, t );

		ChildConstIterator child_i = getChildConstIterator();
		while( !child_i.end() )
		{
			AnimationNode* child = child_i.next();

			// get base anim. index
			unsigned short child_id = child->getId();
			unsigned int banim_i = mChildrenToBaseAnims[child_id];

			// compute origin
			Skeleton::Situation child_orig = mOrigin;
			/*Skeleton::Situation child_orig( child_align[3*banim_i], child_align[3*banim_i+1], child_align[3*banim_i+2] );
			child_orig.transform(origin);*/

			child->setOrigin(child_orig);
		}
	}
}

float AnimationBlender::getWeight( unsigned short nodeId ) const
{
	zhAssert( hasChild(nodeId) );

	unsigned int banim_i = mChildrenToBaseAnims.find(nodeId)->second;
	return mWeights[banim_i];
}

float AnimationBlender::getWeight( const std::string& nodeName ) const
{
	zhAssert( hasChild(nodeName) );

	unsigned int banim_i = mChildrenToBaseAnims.find( getChild(nodeName)->getId() )->second;
	return mWeights[banim_i];
}

void AnimationBlender::setWeight( unsigned short nodeId, float weight )
{
	zhAssert( hasChild(nodeId) );

	unsigned int banim_i = mChildrenToBaseAnims.find(nodeId)->second;
	mWeights[banim_i] = weight;

	_blendAnnotations();
}

void AnimationBlender::setWeight( const std::string& nodeName, float weight )
{
	zhAssert( hasChild(nodeName) );
	
	unsigned int banim_i = mChildrenToBaseAnims.find( getChild(nodeName)->getId() )->second;
	mWeights[banim_i] = weight;

	_blendAnnotations();
}

void AnimationBlender::setAllWeights( float weight )
{
	for( unsigned int i = 0; i < mWeights.size(); ++i )
		mWeights[i] = weight;

	_blendAnnotations();
}

const Vector& AnimationBlender::getWeights() const
{
	return mWeights;
}

void AnimationBlender::setWeights( const Vector& weights )
{
	zhAssert( weights.size() == getNumChildren() );

	mWeights = weights;

	_blendAnnotations();

	// TODO: remove this
	zhLog( "", "", "Blended plant constraints for weights %s:", toString<Vector>(mWeights).c_str() );
	AnimationAnnotationContainer::AnnotationConstIterator annot_i = getPlantConstraintAnnotations()->getAnnotationConstIterator();
	while( annot_i.hasMore() )
	{
		PlantConstraintAnnotation* pc_annot = static_cast<PlantConstraintAnnotation*>( annot_i.next() );

		zhLog( "", "", "%f - %f, %u", pc_annot->getStartTime(), pc_annot->getEndTime(), pc_annot->getBoneId() );
	}
	//
}

float AnimationBlender::getParam( const std::string& paramName ) const
{
	AnimationSpace* anim_space = getAnimationSpace();
	if( anim_space == NULL || !anim_space->hasParametrization() )
		return 0;

	AnimationParametrization* animparam = anim_space->getParametrization();
	unsigned int param_i = animparam->findParam(paramName);
	if( param_i >= mParams.size() )
		return 0;

	return mParams[param_i];
}

void AnimationBlender::setParam( const std::string& paramName, float param )
{
	AnimationSpace* anim_space = getAnimationSpace();
	if( !mParamEnabled || anim_space == NULL || !anim_space->hasParametrization() )
		return;

	AnimationParametrization* animparam = anim_space->getParametrization();
	if( mParams.size() < animparam->getNumParams() )
		// param. value vector still uninitialized
		mParams = Vector( animparam->getNumParams() );

	unsigned int param_i = animparam->findParam(paramName);
	if( param_i >= mParams.size() )
		return;
	
	mParams[param_i] = param;
	setWeights( animparam->sample(mParams) );
}

const Vector& AnimationBlender::getParams() const
{
	return mParams;
}

void AnimationBlender::setParams( const Vector& paramValues )
{
	AnimationSpace* anim_space = getAnimationSpace();
	if( !mParamEnabled || anim_space == NULL || !anim_space->hasParametrization() )
		return;

	AnimationParametrization* animparam = anim_space->getParametrization();
	if( mParams.size() < animparam->getNumParams() )
		// param. value vector still uninitialized
		mParams = Vector( animparam->getNumParams() );

	for( unsigned int param_i = 0; param_i < mParams.size() && param_i < paramValues.size(); ++param_i )
		mParams[param_i] = paramValues[param_i];

	
	setWeights( animparam->sample(mParams) );
}

bool AnimationBlender::getParamEnabled() const
{
	return mParamEnabled;
}

void AnimationBlender::setParamEnabled( bool enabled )
{
	mParamEnabled = enabled;
}

AnimationSetPtr AnimationBlender::getAnimationSet() const
{
	return mAnimSet;
}

unsigned short AnimationBlender::getAnimationSpaceId() const
{
	return mAnimSpaceId;
}

AnimationSpace* AnimationBlender::getAnimationSpace() const
{
	if( mAnimSet == NULL || !mAnimSet->hasAnimationSpace(mAnimSpaceId) )
		return NULL;

	return mAnimSet->getAnimationSpace(mAnimSpaceId);
}

void AnimationBlender::setAnimationSpace( AnimationSetPtr animSet, unsigned short animSpaceId )
{
	zhAssert( animSet != NULL );

	mAnimSet = animSet;
	mAnimSpaceId = animSpaceId;

	AnimationSpace* anim_space = getAnimationSpace();
	zhAssert( anim_space != NULL );
	zhAssert( anim_space->getNumBaseAnimations() == getNumChildren() );

	// iterate through all child nodes and determine
	// which base anim. in the animation space maps to which child
	mChildrenToBaseAnims.clear();
	for( unsigned int banim_i = 0; banim_i < anim_space->getNumBaseAnimations(); ++banim_i )
	{
		Animation* banim = anim_space->getBaseAnimation(banim_i);

		AnimationNode::ChildConstIterator child_i = getChildConstIterator();
		while( child_i.hasMore() )
		{
			AnimationNode* child = child_i.next();

			if( child->isClass( AnimationSampler::ClassId() ) )
			{
				AnimationSampler* snode = static_cast<AnimationSampler*>(child);
				if( snode->getAnimationId() == banim->getId() )
				{
					mChildrenToBaseAnims[ child->getId() ] = banim_i;
					break;
				}
			}
			else if( child->isClass( AnimationBlender::ClassId() ) )
			{
				AnimationBlender* bnode = static_cast<AnimationBlender*>(child);
				if( bnode->getAnimationSpaceId() == banim->getId() )
				{
					mChildrenToBaseAnims[ child->getId() ] = banim_i;
					break;
				}
			}
		}
	}

	// init. blend weights and param. values
	mWeights = Vector( getNumChildren() );
	if( anim_space->hasParametrization() )
		mParams = Vector( anim_space->getParametrization()->getNumParams() );

	// init. annotations on this node
	_initAnnotations();
}

bool AnimationBlender::getUseBlendCurves() const
{
	return mUseBlendCurves;
}

void AnimationBlender::setUseBlendCurves( bool useBlendCurves )
{
	mUseBlendCurves = useBlendCurves;
}

Skeleton::Situation AnimationBlender::_sampleMover() const
{
	// TODO: this is reasonably accurate only when not using blend curves, but whatever

	Skeleton::Situation mv;

	float total_weight = 0;

	// blend child movers
	ChildConstIterator child_i = getChildConstIterator();
	while( !child_i.end() )
	{
		AnimationNode* child = child_i.next();
		unsigned short child_id = child->getId();
		unsigned int banim_i = mChildrenToBaseAnims.find(child_id)->second;
		float weight = mWeights[banim_i];

		if( zhEqualf( mWeights[banim_i], 0 ) )
			continue;

		Skeleton::Situation child_mv = child->_sampleMover();
		mv = Skeleton::Situation(
			mv.getPosition() + child_mv.getPosition() * weight,
			mv.getOrientation().slerp( child_mv.getOrientation(), weight/( weight + total_weight ) )
			);
	}

	return mv;
}

size_t AnimationBlender::_calcMemoryUsage() const
{
	return 0;
}

void AnimationBlender::_unload()
{
}

void AnimationBlender::_clone( AnimationNode* clonePtr, bool shareData ) const
{
	zhAssert( clonePtr != NULL );
	zhAssert( getClassId() == clonePtr->getClassId() );

	AnimationNode::_clone( clonePtr, shareData );

	zhLog( "AnimationBlender", "_clone",
		"Cloning AnimationBlender %s %u, %s.",
		getClassName().c_str(), mId, mName.c_str() );

	AnimationBlender* clone = static_cast<AnimationBlender*>( clonePtr );

	// clone data members

	clone->mTWCurveTime = mTWCurveTime;
	clone->mOrigin = mOrigin;

	clone->setAnimationSpace( mAnimSet, mAnimSpaceId );
	clone->mChildrenToBaseAnims = mChildrenToBaseAnims; // TODO: this may not initialize correctly because children of this blender may not be cloned yet
	if( getAnimationSpace()->hasParametrization() )
		clone->setParams(mParams);
	else
		clone->setWeights(mWeights);

	clone->mUseBlendCurves = mUseBlendCurves;
}

void AnimationBlender::_update( float dt )
{
	AnimationSpace* anim_space = getAnimationSpace();
	float play_length = getPlayLength();

	if( mUseBlendCurves && anim_space != NULL && anim_space->hasTimewarpCurve() )
	{
		const CatmullRomSpline<Vector>& tw_curve = anim_space->getTimewarpCurve();
		float twcurve_length = (float)( tw_curve.getNumControlPoints() - 1 );

		// compute blend curve time
		if( mTWCurveTime < 0 ) mTWCurveTime = 0;
		float du = dt/play_length * twcurve_length;
		float u0 = mTWCurveTime;
		mTWCurveTime += du;
		if( mTWCurveTime > twcurve_length )
			mTWCurveTime -= floor(mTWCurveTime/twcurve_length) * twcurve_length;

		// compute position on the blend curve
		unsigned int cpi0, cpi;
		float t0, t;
		_getTWCurvePosition( u0, cpi0, t0 );
		_getTWCurvePosition( mTWCurveTime, cpi, t );
		// sample timewarp curve
		Vector ctimes0 = tw_curve.getPoint( cpi0, t0 ),
			ctimes = tw_curve.getPoint( cpi, t );

		// update child nodes		
		ChildIterator child_i = getChildIterator();
		while( !child_i.end() )
		{
			AnimationNode* child = child_i.next();
			unsigned short child_id = child->getId();
			unsigned int banim_i = mChildrenToBaseAnims[child_id];

			if( zhEqualf( mWeights[banim_i], 0 ) )
				continue;

			float t0 = ctimes0[banim_i], t = ctimes[banim_i];
			if( t >= t0 )
				child->update( t - t0 );
			else
				child->update( child->getPlayLength() - t0 + t );
		}

		if( anim_space->hasAlignmentCurve() )
		{
			// realign child anim. origin
			setOrigin(mOrigin);
		}
	}
	else
	{
		// update child nodes
		ChildIterator child_i = getChildIterator();
		while( !child_i.end() )
		{
			AnimationNode* child = child_i.next();
			unsigned short child_id = child->getId();
			unsigned int banim_i = mChildrenToBaseAnims[child_id];

			if( zhEqualf( mWeights[banim_i], 0 ) )
				continue;

			child->update( dt * child->getPlayLength()/play_length );
		}
	}
}

void AnimationBlender::_applyNode( float weight, const std::set<unsigned short>& boneMask ) const
{

	// Apply child nodes		
	ChildConstIterator child_i = getChildConstIterator();
	while( !child_i.end() )
	{
		AnimationNode* child_node = child_i.next();
		unsigned short child_id = child_node->getId();
		unsigned int banim_i = mChildrenToBaseAnims.find(child_id)->second;
		float child_weight = mWeights[banim_i];

		if( zhEqualf( child_weight, 0 ) )
			continue;

		child_node->apply( weight*child_weight, boneMask );
	}

	// Find active plant constraints
	/*std::vector<AnimationAnnotation*> annots;
	float t = getPlayTime();
	getPlantConstraintAnnotations()->getActiveAnnotations( t, annots );

	// Get planted bones
	Skeleton* skel = mOwner->getSkeleton();
	std::set<Bone*> pbones;
	for( unsigned int annot_i = 0; annot_i < annots.size(); ++annot_i )
	{
		PlantConstraintAnnotation* pcannot = static_cast<PlantConstraintAnnotation*>( annots[annot_i] );
		Bone* pbone = skel->getBone( pcannot->getBoneId() );

		if( pbone == NULL )
			// Anim. annotated with non-existent bone constr., ignore
			continue;

		pbones.insert(pbone);
	}

	// Enforce constraints on planted bones
	// TODO*/
}

void AnimationBlender::_initAnnotations()
{
	AnimationSpace* anim_space = getAnimationSpace();
	zhAssert( anim_space != NULL );
	Animation* banim0 = anim_space->getBaseAnimation(0);

	TransitionAnnotationContainer* btannots = getTransitionAnnotations();
	ParamTransitionAnnotationContainer* bptannots = getParamTransitionAnnotations();
	PlantConstraintAnnotationContainer* bpcannots = getPlantConstraintAnnotations();
	SimEventAnnotationContainer* bseannots = getSimEventAnnotations();

	// clear existing blended annotations
	btannots->deleteAllAnnotations();
	bptannots->deleteAllAnnotations();
	bpcannots->deleteAllAnnotations();
	bseannots->deleteAllAnnotations();

	// create blended transition annots
	TransitionAnnotationContainer* tannots0 = banim0->getTransitionAnnotations();
	for( unsigned int tannot_i = 0; tannot_i < tannots0->getNumAnnotations(); ++tannot_i )
	{
		TransitionAnnotation* tannot0 = static_cast<TransitionAnnotation*>( tannots0->getAnnotation(tannot_i) );
		bool has_matches = true;

		// make sure there are matches in other base anims
		for( unsigned int banim_i = 1; banim_i < anim_space->getNumBaseAnimations(); ++banim_i )
		{
			TransitionAnnotationContainer* tannots = anim_space->getBaseAnimation(banim_i)->getTransitionAnnotations();
			TransitionAnnotation* tannot = static_cast<TransitionAnnotation*>( tannots->getAnnotation(tannot_i) );

			if( tannot_i >= tannots->getNumAnnotations() || !matchAnnotations<TransitionAnnotation>( tannot0, tannot ) )
			{
				// we've encountered one or more annots without a match in other base animations
				has_matches = false;
				break;
			}
		}

		if(!has_matches)
		{
			// no matches means we can't blend annotations
			btannots->deleteAllAnnotations();
			break;
		}
		
		// create a blended annotation
		TransitionAnnotation* btannot = static_cast<TransitionAnnotation*>(
			btannots->createAnnotation( tannot0->getStartTime(), tannot0->getEndTime() ) );
		*btannot = *tannot0;
	}

	// create blended param. transition annots
	ParamTransitionAnnotationContainer* ptannots0 = banim0->getParamTransitionAnnotations();
	for( unsigned int ptannot_i = 0; ptannot_i < ptannots0->getNumAnnotations(); ++ptannot_i )
	{
		ParamTransitionAnnotation* ptannot0 = static_cast<ParamTransitionAnnotation*>( ptannots0->getAnnotation(ptannot_i) );
		bool has_matches = true;

		// make sure there are matches in other base anims
		for( unsigned int banim_i = 1; banim_i < anim_space->getNumBaseAnimations(); ++banim_i )
		{
			ParamTransitionAnnotationContainer* ptannots = anim_space->getBaseAnimation(banim_i)->getParamTransitionAnnotations();
			ParamTransitionAnnotation* ptannot = static_cast<ParamTransitionAnnotation*>( ptannots->getAnnotation(ptannot_i) );

			if( ptannot_i >= ptannots->getNumAnnotations() || !matchAnnotations<ParamTransitionAnnotation>( ptannot0, ptannot ) )
			{
				// we've encountered one or more annots without a match in other base animations
				has_matches = false;
				break;
			}
		}

		if(!has_matches)
		{
			// no matches means we can't blend annotations
			bptannots->deleteAllAnnotations();
			break;
		}
		
		// create a blended annotation
		ParamTransitionAnnotation* bptannot = static_cast<ParamTransitionAnnotation*>(
			bptannots->createAnnotation( ptannot0->getStartTime(), ptannot0->getEndTime() ) );
		*bptannot = *ptannot0;
	}

	// create blended plant constr. annots
	PlantConstraintAnnotationContainer* pcannots0 = banim0->getPlantConstraintAnnotations();
	for( unsigned int pcannot_i = 0; pcannot_i < pcannots0->getNumAnnotations(); ++pcannot_i )
	{
		PlantConstraintAnnotation* pcannot0 = static_cast<PlantConstraintAnnotation*>( pcannots0->getAnnotation(pcannot_i) );
		bool has_matches = true;

		// make sure there are matches in other base anims
		for( unsigned int banim_i = 1; banim_i < anim_space->getNumBaseAnimations(); ++banim_i )
		{
			PlantConstraintAnnotationContainer* pcannots = anim_space->getBaseAnimation(banim_i)->getPlantConstraintAnnotations();
			PlantConstraintAnnotation* pcannot = static_cast<PlantConstraintAnnotation*>( pcannots->getAnnotation(pcannot_i) );

			if( pcannot_i >= pcannots->getNumAnnotations() || !matchAnnotations<PlantConstraintAnnotation>( pcannot0, pcannot ) )
			{
				// we've encountered one or more annots without a match in other base animations
				has_matches = false;
				break;
			}
		}

		if(!has_matches)
		{
			// no matches means we can't blend annotations
			bpcannots->deleteAllAnnotations();
			break;
		}
		
		// create a blended annotation
		PlantConstraintAnnotation* bpcannot = static_cast<PlantConstraintAnnotation*>(
			bpcannots->createAnnotation( pcannot0->getStartTime(), pcannot0->getEndTime() ) );
		*bpcannot = *pcannot0;
	}

	// create blended sim. event annots
	SimEventAnnotationContainer* seannots0 = banim0->getSimEventAnnotations();
	for( unsigned int seannot_i = 0; seannot_i < seannots0->getNumAnnotations(); ++seannot_i )
	{
		SimEventAnnotation* seannot0 = static_cast<SimEventAnnotation*>( seannots0->getAnnotation(seannot_i) );
		bool has_matches = true;

		// make sure there are matches in other base anims
		for( unsigned int banim_i = 1; banim_i < anim_space->getNumBaseAnimations(); ++banim_i )
		{
			SimEventAnnotationContainer* seannots = anim_space->getBaseAnimation(banim_i)->getSimEventAnnotations();
			SimEventAnnotation* seannot = static_cast<SimEventAnnotation*>( seannots->getAnnotation(seannot_i) );

			if( seannot_i >= seannots->getNumAnnotations() || !matchAnnotations<SimEventAnnotation>( seannot0, seannot ) )
			{
				// we've encountered one or more annots without a match in other base animations
				has_matches = false;
				break;
			}
		}

		if(!has_matches)
		{
			// no matches means we can't blend annotations
			bseannots->deleteAllAnnotations();
			break;
		}
		
		// create a blended annotation
		SimEventAnnotation* bseannot = static_cast<SimEventAnnotation*>(
			bseannots->createAnnotation( seannot0->getStartTime(), seannot0->getEndTime() ) );
		*bseannot = *seannot0;
	}

	// blend annotations with current weights
	_blendAnnotations();
}

void AnimationBlender::_blendAnnotations()
{
	AnimationSpace* anim_space = getAnimationSpace();
	zhAssert( anim_space != NULL );
	Animation* banim0 = anim_space->getBaseAnimation(0);

	TransitionAnnotationContainer* btannots = getTransitionAnnotations();
	ParamTransitionAnnotationContainer* bptannots = getParamTransitionAnnotations();
	PlantConstraintAnnotationContainer* bpcannots = getPlantConstraintAnnotations();
	SimEventAnnotationContainer* bseannots = getSimEventAnnotations();

	// compute blended transition annotations
	for( unsigned int tannot_i = 0; tannot_i < btannots->getNumAnnotations(); ++tannot_i )
	{
		TransitionAnnotation* btannot = static_cast<TransitionAnnotation*>( btannots->getAnnotation(tannot_i) );

		// reset blend
		resetAnnotation<TransitionAnnotation>(btannot);

		// compute new blend
		for( unsigned int banim_i = 0; banim_i < anim_space->getNumBaseAnimations(); ++banim_i )
		{
			TransitionAnnotationContainer* tannots = anim_space->getBaseAnimation(banim_i)->getTransitionAnnotations();

			TransitionAnnotation* tannot = static_cast<TransitionAnnotation*>( tannots->getAnnotation(tannot_i) );
			blendAnnotations<TransitionAnnotation>( btannot, tannot, mWeights[banim_i] );
		}
	}

	// compute blended param. transition annotations
	for( unsigned int ptannot_i = 0; ptannot_i < bptannots->getNumAnnotations(); ++ptannot_i )
	{
		ParamTransitionAnnotation* bptannot = static_cast<ParamTransitionAnnotation*>( bptannots->getAnnotation(ptannot_i) );

		// reset blend
		resetAnnotation<ParamTransitionAnnotation>(bptannot);

		// compute new blend
		for( unsigned int banim_i = 0; banim_i < anim_space->getNumBaseAnimations(); ++banim_i )
		{
			ParamTransitionAnnotationContainer* ptannots = anim_space->getBaseAnimation(banim_i)->getParamTransitionAnnotations();

			ParamTransitionAnnotation* ptannot = static_cast<ParamTransitionAnnotation*>( ptannots->getAnnotation(ptannot_i) );
			blendAnnotations<ParamTransitionAnnotation>( bptannot, ptannot, mWeights[banim_i] );
		}
	}

	// compute blended plant constr. annotations
	for( unsigned int pcannot_i = 0; pcannot_i < bpcannots->getNumAnnotations(); ++pcannot_i )
	{
		PlantConstraintAnnotation* bpcannot = static_cast<PlantConstraintAnnotation*>( bpcannots->getAnnotation(pcannot_i) );

		// reset blend
		resetAnnotation<PlantConstraintAnnotation>(bpcannot);

		// compute new blend
		for( unsigned int banim_i = 0; banim_i < anim_space->getNumBaseAnimations(); ++banim_i )
		{
			PlantConstraintAnnotationContainer* pcannots = anim_space->getBaseAnimation(banim_i)->getPlantConstraintAnnotations();

			PlantConstraintAnnotation* pcannot = static_cast<PlantConstraintAnnotation*>( pcannots->getAnnotation(pcannot_i) );
			blendAnnotations<PlantConstraintAnnotation>( bpcannot, pcannot, mWeights[banim_i] );
		}
	}

	// compute blended sim. event annotations
	for( unsigned int seannot_i = 0; seannot_i < bseannots->getNumAnnotations(); ++seannot_i )
	{
		SimEventAnnotation* bseannot = static_cast<SimEventAnnotation*>( bseannots->getAnnotation(seannot_i) );

		// reset blend
		resetAnnotation<SimEventAnnotation>(bseannot);

		// compute new blend
		for( unsigned int banim_i = 0; banim_i < anim_space->getNumBaseAnimations(); ++banim_i )
		{
			SimEventAnnotationContainer* seannots = anim_space->getBaseAnimation(banim_i)->getSimEventAnnotations();

			SimEventAnnotation* seannot = static_cast<SimEventAnnotation*>( seannots->getAnnotation(seannot_i) );
			blendAnnotations<SimEventAnnotation>( bseannot, seannot, mWeights[banim_i] );
		}
	}
}

void AnimationBlender::_getTWCurvePosition( float u, unsigned int& cpi, float& t ) const
{
	cpi = (unsigned int)u;
	t = u - cpi;
}

}
