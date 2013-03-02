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

#include "zhAnimationNode.h"
#include "zhLogger.h"
#include "zhSkeleton.h"
#include "zhAnimationTree.h"

namespace zh
{

AnimationNode::AnimationNode()
: mName(""), mOwner(NULL), mParent(NULL), mMainChild(NULL),
mPlaying(true), mPaused(false), mPlayRate(1), mAnnotsEnabled(true)
{
	mTransAnnots = new TransitionAnnotationContainer();
	mParamTransAnnots = new ParamTransitionAnnotationContainer();
	mPlantConstrAnnots = new PlantConstraintAnnotationContainer();
	mSimEventAnnots = new SimEventAnnotationContainer();
}

AnimationNode::~AnimationNode()
{
	delete mTransAnnots;
	delete mParamTransAnnots;
	delete mPlantConstrAnnots;
	delete mSimEventAnnots;
}

const std::string& AnimationNode::getName() const
{
	return mName;
}

void AnimationNode::setName( const std::string& name )
{
	mOwner->_renameNode( this, name );
}

AnimationTree* AnimationNode::getAnimationTree() const
{
	return mOwner;
}

AnimationNode* AnimationNode::getParent() const
{
	return mParent;
}

void AnimationNode::addChild( AnimationNode* node )
{
	zhAssert( node != NULL );
	zhAssert( mOwner == node->mOwner );
	zhAssert( !isLeaf() );

	zhLog( "AnimationNode", "addChild",
		"Adding node %s %u, %s as a child of node %s %u, %s.",
		node->getClassName().c_str(), node->getId(), node->getName().c_str(),
		getClassName().c_str(), mId, mName.c_str() );
	
	if( mChildrenById.count( node->getId() ) != 0 )
		return;
	node->mParent = this;
	mChildrenById.insert( make_pair( node->getId(), node ) );
	mChildrenByName.insert( make_pair( node->getName(), node ) );

	if( mOwner->getRoot() == node )
		mOwner->setRoot((AnimationNode*)NULL);

	if( mMainChild == NULL )
		mMainChild = node;
}

void AnimationNode::removeChild( unsigned short id )
{
	std::map<unsigned short, AnimationNode*>::iterator ci = mChildrenById.find(id);
	
	if( ci != mChildrenById.end() )
	{
		AnimationNode* node = ci->second;

		zhLog( "AnimationNode", "removeChild",
			"Removing node %s %u, %s from its parent node %s %u, %s.",
			node->getClassName().c_str(), node->getId(), node->getName().c_str(),
			getClassName().c_str(), mId, mName.c_str() );

		mChildrenById.erase(ci);
		mChildrenByName.erase( node->getName() );
		node->mParent = NULL;

		if( mMainChild == node )
			mMainChild = NULL;
	}
}

void AnimationNode::removeChild( const std::string& name )
{
	std::map<std::string, AnimationNode*>::iterator ci = mChildrenByName.find(name);
	
	if( ci != mChildrenByName.end() )
	{
		AnimationNode* node = ci->second;

		zhLog( "AnimationNode", "removeChild",
			"Removing node %s %u, %s from its parent node %s %u, %s.",
			node->getClassName().c_str(), node->getId(), node->getName().c_str(),
			getClassName().c_str(), mId, mName.c_str() );

		mChildrenById.erase( node->getId() );
		mChildrenByName.erase(ci);
		node->mParent = NULL;

		if( mMainChild == node )
			mMainChild = NULL;
	}
}

void AnimationNode::removeAllChildren()
{
	zhLog( "AnimationNode", "removeChild",
			"Removing all children of node %s %u, %s.",
			getClassName().c_str(), mId, mName.c_str() );

	for( std::map<unsigned short, AnimationNode*>::iterator ci = mChildrenById.begin();
		ci != mChildrenById.end(); ++ci )
	{
		ci->second->mParent = NULL;
	}
	
	mChildrenById.clear();
	mChildrenByName.clear();
	mMainChild = NULL;
}

void AnimationNode::moveChild( unsigned short id, AnimationNode* node )
{
	zhAssert( hasChild(id) );
	zhAssert( node != NULL );

	std::map<unsigned short, AnimationNode*>::iterator ci = mChildrenById.find(id);
	AnimationNode* cn = ci->second;

	zhLog( "AnimationNode", "moveChild",
		"Moving node %s %u, %s from its parent node %s %u, %s to its new parent node %s %u, %s.",
		getClassName().c_str(), mId, mName.c_str(),
		node->getClassName().c_str(), node->getId(), node->getName().c_str(),
		cn->getClassName().c_str(), cn->getId(), cn->getName().c_str() );

	mChildrenByName.erase( cn->getName() );
	mChildrenById.erase(ci);
	node->addChild(cn);

	if( mMainChild == cn )
		mMainChild = NULL;
}

void AnimationNode::moveChild( const std::string& name, AnimationNode* node )
{
	zhAssert( hasChild(name) );
	zhAssert( node != NULL );

	std::map<std::string, AnimationNode*>::iterator ci = mChildrenByName.find(name);
	AnimationNode* cn = ci->second;

	zhLog( "AnimationNode", "moveChild",
		"Moving node %s %u, %s from its parent node %s %u, %s to its new parent node %s %u, %s.",
		getClassName().c_str(), mId, mName.c_str(),
		node->getClassName().c_str(), node->getId(), node->getName().c_str(),
		cn->getClassName().c_str(), cn->getId(), cn->getName().c_str() );

	mChildrenById.erase( cn->getId() );
	mChildrenByName.erase(ci);
	node->addChild(cn);

	if( mMainChild == cn )
		mMainChild = NULL;
}

bool AnimationNode::hasChild( unsigned short id ) const
{
	return mChildrenById.count(id) > 0;
}

bool AnimationNode::hasChild( const std::string& name ) const
{
	return mChildrenByName.count(name) > 0;
}

AnimationNode* AnimationNode::getChild( unsigned short id ) const
{
	std::map<unsigned short, AnimationNode*>::const_iterator ci = mChildrenById.find(id);

	if( ci != mChildrenById.end() )
		return ci->second;

	return NULL;
}

AnimationNode* AnimationNode::getChild( const std::string& name ) const
{
	std::map<std::string, AnimationNode*>::const_iterator ci = mChildrenByName.find(name);

	if( ci != mChildrenByName.end() )
		return ci->second;

	return NULL;
}

unsigned int AnimationNode::getNumChildren() const
{
	return mChildrenById.size();
}

AnimationNode::ChildIterator AnimationNode::getChildIterator()
{
	return ChildIterator( mChildrenById );
}

AnimationNode::ChildConstIterator AnimationNode::getChildConstIterator() const
{
	return ChildConstIterator( mChildrenById );
}

AnimationNode* AnimationNode::getMainChild() const
{
	if( mMainChild == NULL && getNumChildren() > 0 )
		mMainChild = mChildrenById.begin()->second;

	return mMainChild;
}

void AnimationNode::setMainChild( unsigned short id )
{
	mMainChild = getChild(id);
}

void AnimationNode::setMainChild( const std::string& name )
{
	mMainChild = getChild(name);
}

void AnimationNode::setMainChild( AnimationNode* node )
{
	zhAssert( node == NULL ||
		hasChild( node->getId() ) );

	mMainChild = node;
}

bool AnimationNode::getPlaying() const
{
	return mPlaying;
}

void AnimationNode::setPlaying( bool playing )
{
	mPlaying = playing;

	/*ChildIterator child_i = getChildIterator();
	while( !child_i.end() )
	{
		AnimationNode* child = child_i.next();
		child->setPlaying(mPlaying);
	}*/

	setPlayTime(0);
}

bool AnimationNode::getPaused() const
{
	return mPaused;
}

void AnimationNode::setPaused( bool paused )
{
	mPaused = paused;

	/*ChildIterator child_i = getChildIterator();
	while( !child_i.end() )
	{
		AnimationNode* child = child_i.next();
		child->setPaused(mPaused);
	}*/
}

float AnimationNode::getPlayTime() const
{
	AnimationNode* cn = getMainChild();
	if( cn == NULL )
		return 0;

	return cn->getPlayTime();
}

void AnimationNode::setPlayTime( float time )
{
	AnimationNode* cn = getMainChild();
	if( cn == NULL )
		return;

	float ntime = time/getPlayLength();

	ChildIterator child_i = getChildIterator();
	while( !child_i.end() )
	{
		AnimationNode* child = child_i.next();
		child->setNormalizedPlayTime(ntime);
	}
}

float AnimationNode::getNormalizedPlayTime() const
{
	float length = getPlayLength();
	if( zhEqualf( length, 0 ) )
		return 0;
	
	return getPlayTime()/length;
}

void AnimationNode::setNormalizedPlayTime( float time )
{
	setPlayTime( time * getPlayLength() );
}

float AnimationNode::getPlayRate() const
{
	return mPlayRate;
}

void AnimationNode::setPlayRate( float rate )
{
	mPlayRate = rate;
}

float AnimationNode::getPlayLength() const
{
	AnimationNode* cn = getMainChild();
	if( cn == NULL )
		return 0;

	return cn->getPlayLength();
}

bool AnimationNode::getAnnotationsEnabled() const
{
	return mAnnotsEnabled;
}

void AnimationNode::setAnnotationsEnabled( bool enabled )
{
	mAnnotsEnabled = enabled;
}

TransitionAnnotationContainer* AnimationNode::getTransitionAnnotations() const
{
	return mTransAnnots;
}

ParamTransitionAnnotationContainer* AnimationNode::getParamTransitionAnnotations() const
{
	return mParamTransAnnots;
}

PlantConstraintAnnotationContainer* AnimationNode::getPlantConstraintAnnotations() const
{
	return mPlantConstrAnnots;
}

SimEventAnnotationContainer* AnimationNode::getSimEventAnnotations() const
{
	return mSimEventAnnots;
}

const Skeleton::Situation& AnimationNode::getOrigin() const
{
	AnimationNode* cn = getMainChild();
	if( cn == NULL )
		return Skeleton::Situation::Identity;

	return cn->getOrigin();
}

void AnimationNode::setOrigin( const Skeleton::Situation& origin )
{
	AnimationNode* cn = getMainChild();
	if( cn == NULL )
		return;

	cn->setOrigin(origin);
}

void AnimationNode::maskBone( unsigned short boneId )
{
	mBoneMask.insert(boneId);
}

void AnimationNode::unmaskBone( unsigned short boneId )
{
	mBoneMask.erase(boneId);
}

void AnimationNode::unmaskAllBones()
{
	mBoneMask.clear();
}

bool AnimationNode::isBoneMasked( unsigned short boneId )
{
	return mBoneMask.count(boneId) > 0;
}

const std::set<unsigned short>& AnimationNode::getBoneMask() const
{
	return mBoneMask;
}

void AnimationNode::update( float dt )
{
	if( !mPlaying || zhEqualf( getPlayLength(), 0 ) )
		return;

	if( mPaused )
		dt = 0;

	_updateNode( dt * getPlayRate() );

	// store elapsed time
	mDt = dt;
}

void AnimationNode::apply( float weight, const std::set<unsigned short>& boneMask ) const
{
	if( !mPlaying )
		return;

	// compute merged bone mask
	std::set<unsigned short> jmts;
	std::set_union( boneMask.begin(), boneMask.end(),
		mBoneMask.begin(), mBoneMask.end(),
		std::inserter( jmts, jmts.begin() )
		);

	_applyNode( weight, jmts );
	if( mAnnotsEnabled )
		_applyAnnotations();

	if( isLeaf() )
		// update cumulative blend weight
		mOwner->_setTotalWeight( mOwner->_getTotalWeight() + weight );
}

size_t AnimationNode::_calcMemoryUsage() const
{
	return 0;
}

void AnimationNode::_unload()
{
}

void AnimationNode::_clone( AnimationNode* clonePtr, bool shareData ) const
{
	zhAssert( clonePtr != NULL );
	zhAssert( getClassId() == clonePtr->getClassId() );

	zhLog( "AnimationNode", "_clone",
		"Cloning node %s %u, %s.",
		getClassName().c_str(), mId, mName.c_str() );

	// clone children refs
	ChildConstIterator ci = getChildConstIterator();
	while( !ci.end() )
	{
		AnimationNode* cn = ci.next();
		clonePtr->addChild( clonePtr->getAnimationTree()->getNode( cn->getId() ) );
	}

	// clone main child ref.
	if( getMainChild() != NULL )
		clonePtr->setMainChild( getMainChild()->getId() );

	// clone playback state
	clonePtr->mPlaying = mPlaying;
	clonePtr->mPaused = mPaused;
	clonePtr->mPlayRate = mPlayRate;

	// clone bone mask
	clonePtr->mBoneMask = mBoneMask;

	// copy local annotations
	mTransAnnots->_clone( clonePtr->mTransAnnots );
	mParamTransAnnots->_clone( clonePtr->mParamTransAnnots );
	mPlantConstrAnnots->_clone( clonePtr->mPlantConstrAnnots );
	mSimEventAnnots->_clone( clonePtr->mSimEventAnnots );
}

Skeleton::Situation AnimationNode::_sampleMover() const
{
	AnimationNode* cn = getMainChild();
	if( cn == NULL )
		return Skeleton::Situation::Identity;

	return cn->_sampleMover();
}

Skeleton::Situation AnimationNode::_getRealignedOrigin( const Skeleton::Situation& sit ) const
{
	Skeleton::Situation mv = _sampleMover(); // get current mover

	// compute realigning transf.
	Quat rot = mv.getOrientation().getInverse() * sit.getOrientation();
	Vector3 trans = -mv.getPosition() + sit.getPosition();

	// apply realigning transf.
	Skeleton::Situation orig = getOrigin();
	Vector3 r_origpos = orig.getPosition() + ( mv.getPosition() - orig.getPosition() ).rotate(rot);
	orig.setOrientation( orig.getOrientation() * rot );
	orig.setPosition( orig.getPosition() + trans - ( r_origpos - mv.getPosition() ) );
	
	return orig;
}

void AnimationNode::_updateNode( float dt )
{
	// update child nodes
	ChildIterator ci = getChildIterator();
	while( !ci.end() )
		ci.next()->update(dt);
}

void AnimationNode::_applyNode( float weight, const std::set<unsigned short>& boneMask ) const
{
	// apply child nodes
	ChildConstIterator ci = getChildConstIterator();
	while( !ci.end() )
		ci.next()->apply( weight, boneMask );
}

float AnimationNode::_getPrevTime() const
{
	return getPlayTime() - mDt;
}

void AnimationNode::_applyAnnotations() const
{
	float time = getPlayTime(),
		prev_time = _getPrevTime();
	std::vector<AnimationAnnotation*> annots;

	// Find active annotations
	getTransitionAnnotations()->getActiveAnnotations( prev_time, time, annots );
	for( unsigned int annot_i = 0; annot_i < annots.size(); ++annot_i )
	{
		TransitionAnnotation* annot = static_cast<TransitionAnnotation*>( annots[annot_i] );

		// emit event
		TransitionAnnotEvent annot_evt( const_cast<AnimationNode*>(this), annot );
		annot_evt.emit();
	}

	// Find finished annotations
	annots.clear();
	getTransitionAnnotations()->getFinishedAnnotations( prev_time, time, annots );
	for( unsigned int annot_i = 0; annot_i < annots.size(); ++annot_i )
	{
		TransitionAnnotation* annot = static_cast<TransitionAnnotation*>( annots[annot_i] );

		// emit event
		TransitionAnnotEvent annot_evt( const_cast<AnimationNode*>(this), annot, true );
		annot_evt.emit();
	}

	// Find active annotations
	getParamTransitionAnnotations()->getActiveAnnotations( prev_time, time, annots );
	for( unsigned int annot_i = 0; annot_i < annots.size(); ++annot_i )
	{
		ParamTransitionAnnotation* annot = static_cast<ParamTransitionAnnotation*>( annots[annot_i] );

		// emit event
		ParamTransitionAnnotEvent annot_evt( const_cast<AnimationNode*>(this), annot );
		annot_evt.emit();
	}

	// Find finished annotations
	annots.clear();
	getParamTransitionAnnotations()->getFinishedAnnotations( prev_time, time, annots );
	for( unsigned int annot_i = 0; annot_i < annots.size(); ++annot_i )
	{
		ParamTransitionAnnotation* annot = static_cast<ParamTransitionAnnotation*>( annots[annot_i] );

		// emit event
		ParamTransitionAnnotEvent annot_evt( const_cast<AnimationNode*>(this), annot, true );
		annot_evt.emit();
	}

	// Find active annotations
	getPlantConstraintAnnotations()->getActiveAnnotations( prev_time, time, annots );
	for( unsigned int annot_i = 0; annot_i < annots.size(); ++annot_i )
	{
		PlantConstraintAnnotation* annot = static_cast<PlantConstraintAnnotation*>( annots[annot_i] );

		// emit event
		PlantConstraintAnnotEvent annot_evt( const_cast<AnimationNode*>(this), annot );
		annot_evt.emit();
	}

	// Find finished annotations
	annots.clear();
	getPlantConstraintAnnotations()->getFinishedAnnotations( prev_time, time, annots );
	for( unsigned int annot_i = 0; annot_i < annots.size(); ++annot_i )
	{
		PlantConstraintAnnotation* annot = static_cast<PlantConstraintAnnotation*>( annots[annot_i] );

		// emit event
		PlantConstraintAnnotEvent annot_evt( const_cast<AnimationNode*>(this), annot, true );
		annot_evt.emit();
	}

	// Find active annotations
	getSimEventAnnotations()->getActiveAnnotations( prev_time, time, annots );
	for( unsigned int annot_i = 0; annot_i < annots.size(); ++annot_i )
	{
		SimEventAnnotation* annot = static_cast<SimEventAnnotation*>( annots[annot_i] );

		// emit event
		SimEventAnnotEvent annot_evt( const_cast<AnimationNode*>(this), annot );
		annot_evt.emit();
	}

	// Find finished annotations
	annots.clear();
	getSimEventAnnotations()->getFinishedAnnotations( prev_time, time, annots );
	for( unsigned int annot_i = 0; annot_i < annots.size(); ++annot_i )
	{
		SimEventAnnotation* annot = static_cast<SimEventAnnotation*>( annots[annot_i] );

		// emit event
		SimEventAnnotEvent annot_evt( const_cast<AnimationNode*>(this), annot, true );
		annot_evt.emit();
	}
}

}
