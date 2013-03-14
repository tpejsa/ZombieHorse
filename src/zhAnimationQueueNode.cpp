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

#include "zhAnimationQueueNode.h"
#include "zhAnimationSampleNode.h"
#include "zhAnimationBlendNode.h"

namespace zh
{

AnimationQueueNode::AnimationQueueNode()
: mCurrentNode(NULL), mNextNode(NULL), mWeight(1),
mDefaultTransitionLength(0), mDefaultNode(NULL),
mTransStarted(false), mTransFinished(false)
{
}

AnimationQueueNode::~AnimationQueueNode()
{
}

bool AnimationQueueNode::isLeaf() const
{
	return false;
}

float AnimationQueueNode::getPlayTime() const
{
	AnimationNode* cn = getCurrentNode();
	if( cn == NULL )
		return 0;

	return cn->getPlayTime();
}

void AnimationQueueNode::setPlayTime( float time )
{
	AnimationNode* cn = getCurrentNode();
	if( cn == NULL )
		return;

	return cn->setPlayTime(time);
}

float AnimationQueueNode::getPlayLength() const
{
	AnimationNode* cn = getCurrentNode();
	if( cn == NULL )
		return mDefaultNode != NULL ? mDefaultNode->getPlayLength() : 0;

	return cn->getPlayLength();
}

const Skeleton::Situation& AnimationQueueNode::getOrigin() const
{
	AnimationNode* cn = getCurrentNode();
	if( cn == NULL )
		return Skeleton::Situation::Identity;

	return cn->getOrigin();
}

void AnimationQueueNode::setOrigin( const Skeleton::Situation& origin )
{
	AnimationNode* cn = getCurrentNode();
	if( cn == NULL )
		return;

	cn->setOrigin(origin);
}

AnimationNode* AnimationQueueNode::getCurrentNode() const
{
	return mCurrentNode;
}

float AnimationQueueNode::getCurrentWeight() const
{
	return mWeight;
}

AnimationNode* AnimationQueueNode::getNextNode() const
{
	return mTransitionQueue.size() > 0 ?
		mTransitionQueue.front().getTargetNode() : NULL;
}

float AnimationQueueNode::getNextWeight() const
{
	return 1.f - mWeight;
}

void AnimationQueueNode::addTransition( unsigned short id )
{
	zhAssert( hasChild(id) );

	AnimationNode* cn = getChild(id);
	addTransition(cn);
}

void AnimationQueueNode::addTransition( const std::string& name )
{
	zhAssert( hasChild(name) );

	AnimationNode* cn = getChild(name);
	addTransition(cn);
}

void AnimationQueueNode::addTransition( AnimationNode* node )
{
	zhAssert( node != NULL && hasChild( node->getId() ) );

	// source and target nodes
	AnimationNode *tnode = node, *snode = NULL;
	AnimationBlendNode *ptnode = NULL;

	// transition spec.
	float stime, etime, ttime;
	Vector tparams;

	// is the target node a blend?
	if( tnode->isClass( AnimationBlendNode::ClassId() ) )
	{
		ptnode = static_cast<AnimationBlendNode*>(tnode);
		tparams = ptnode->getParams();
	}

	if( mCurrentNode == NULL )
	{
		// transition queue is empty, add the first animation
		addTransition( Transition( 0, 0, 0, tnode, tparams ) );
		return;
	}

	// determine transition source
	if( mTransitionQueue.size() > 0 )
		snode = mTransitionQueue.back().getTargetNode();
	else
		snode = mCurrentNode;

	Vector sparams0;
	if( snode->isClass( AnimationBlendNode::ClassId() ) )
	{
		// source node is a blend,
		// make its state match what it will be just prior to transition

		AnimationBlendNode* psnode = static_cast<AnimationBlendNode*>(snode);

		sparams0 = psnode->getParams();
		if( psnode != mCurrentNode )
		{
			psnode->setParams( mTransitionQueue.back().getTargetParams() );
			// TODO: I'm pretty sure this is a bug...
		}
	}

	bool found_trans = false;
	
	if( mAnnotsEnabled )
	{
		// transition annotations enabled,
		// schedule a transition
		if( tnode->isClass( AnimationBlendNode::ClassId() ) )
		{
			// schedule parametric transition

			AnimationBlendNode* ptnode = static_cast<AnimationBlendNode*>(tnode);

			ParamTransitionAnnotationContainer::AnnotationConstIterator ptannot_i =
				snode->getParamTransitionAnnotations()->getAnnotationConstIterator();
			while( ptannot_i.hasMore() )
			{
				ParamTransitionAnnotation* ptannot = static_cast<ParamTransitionAnnotation*>( ptannot_i.next() );

				if( ptannot->getTargetSetId() == ptnode->getAnimationSet()->getId() &&
					ptannot->getTargetId() == ptnode->getAnimationSpaceId() )
				{
					// found a suitable transition annotation, get trans. data
					found_trans = true;

					stime = ptannot->getStartTime();
					etime = ptannot->getEndTime();
					ttime = ptannot->getTargetTime() * ptnode->getPlayLength();

					break;
				}
			}
		}
		else
		{
			// schedule regular transition

			AnimationSampleNode* stnode = NULL;
			if( tnode->isClass( AnimationSampleNode::ClassId() ) )
				stnode = static_cast<AnimationSampleNode*>(tnode);

			TransitionAnnotationContainer::AnnotationConstIterator tannot_i =
				snode->getTransitionAnnotations()->getAnnotationConstIterator();
			while( tannot_i.hasMore() )
			{
				TransitionAnnotation* tannot = static_cast<TransitionAnnotation*>( tannot_i.next() );

				if( stnode != NULL && tannot->getTargetSetId() == stnode->getAnimationSet()->getId() &&
					tannot->getTargetId() == stnode->getAnimationId() || // target is an animation sampler, check its anim. resource
					tnode->getClassId() == tannot->getTargetSetId() ) // target is a procedural anim. node, is it the right node class?
				{
					// found a suitable transition annotation, get trans. data

					found_trans = true;

					stime = tannot->getStartTime();
					etime = tannot->getEndTime();
					ttime = tannot->getTargetTime();

					break;
				}
			}
		}
	}

	if( !found_trans )
	{
		// no transition annot. found, set up default transition
		etime = snode->getPlayLength();
		stime = etime - mDefaultTransitionLength;
		ttime = 0;
	}

	if( snode->isClass( AnimationBlendNode::ClassId() ) )
	{
		// source node is a blend,
		// restore its current state

		AnimationBlendNode* psnode = static_cast<AnimationBlendNode*>(snode);
		psnode->setParams(sparams0);
	}

	//
	// TODO: there is a bug somewhere where stime and etime become NaNs
	zhAssert( stime == stime );
	//

	addTransition( Transition( stime, etime, ttime, tnode, tparams ) );
}

void AnimationQueueNode::addTransition( AnimationBlendNode* node, const Vector& params )
{
	// save current blend state
	Vector params0 = node->getParams();

	// add node to queue
	node->setParams(params);
	addTransition(node);

	// restore current blend state
	node->setParams(params0);
}

void AnimationQueueNode::addTransition( const Transition& transSpec )
{
	zhAssert( transSpec.getTargetNode() != NULL &&
		hasChild( transSpec.getTargetNode()->getId() )
		);

	if( mCurrentNode == NULL )
	{
		// no animations scheduled, add the first one

		zhLog( "AnimationQueueNode", "addTransition", "Scheduling node %s in transition blender %s.",
			transSpec.getTargetNode()->getName().c_str(), mName.c_str() );

		mCurrentNode = transSpec.getTargetNode();
		mCurrentNode->setPlaying();
		mCurrentNode->setPlayTime( transSpec.getTargetTime() + transSpec.getEndTime() - transSpec.getStartTime() );

		if( mCurrentNode->isClass( AnimationBlendNode::ClassId() ) )
			static_cast<AnimationBlendNode*>(mCurrentNode)->setParams( transSpec.getTargetParams() );
	}
	else
	{
		// add the new animation to the transition queue

		zhLog( "AnimationQueueNode", "addTransition",
			"Scheduling transition in transition blender %s from node %s to node %s, with start time %f, end time %f, target time %f.",
			mName.c_str(),
			mTransitionQueue.empty() ? mCurrentNode->getName().c_str() : mTransitionQueue.front().getTargetNode()->getName().c_str(),
			transSpec.getTargetNode()->getName().c_str(), transSpec.getStartTime(), transSpec.getEndTime(), transSpec.getTargetTime() );

		mTransitionQueue.push(transSpec);
	}
}

void AnimationQueueNode::removeLastTransition()
{
	if( !mTransitionQueue.empty() )
		mTransitionQueue.pop();
	else
		mCurrentNode = NULL;
}

void AnimationQueueNode::removeAllTransitions()
{
	while( !mTransitionQueue.empty() )
		mTransitionQueue.pop();
	mCurrentNode = NULL;
}

const std::queue<AnimationQueueNode::Transition>& AnimationQueueNode::getTransitionQueue() const
{
	return mTransitionQueue;
}

float AnimationQueueNode::getDefaultTransitionLength() const
{
	return mDefaultTransitionLength;
}

void AnimationQueueNode::setDefaultTransitionLength( float length )
{
	if( length < 0 ) length = 0;

	mDefaultTransitionLength = length;
}

AnimationNode* AnimationQueueNode::getDefaultNode() const
{
	return mDefaultNode;
}

void AnimationQueueNode::setDefaultNode( unsigned short id )
{
	mDefaultNode = getChild(id);
}

void AnimationQueueNode::setDefaultNode( const std::string& name )
{
	mDefaultNode = getChild(name);
}

void AnimationQueueNode::setDefaultNode( AnimationNode* node )
{
	zhAssert( node == NULL ||
		hasChild( node->getId() ) );

	mDefaultNode = node;
}

float AnimationQueueNode::_getNextTime() const
{
	return mNextTime;
}

const Skeleton::Situation& AnimationQueueNode::_getNextOrigin() const
{
	return mNextOrigin;
}

bool AnimationQueueNode::_getTransitionStarted() const
{
	return mTransStarted;
}

bool AnimationQueueNode::_getTransitionFinished() const
{
	return mTransFinished;
}

Skeleton::Situation AnimationQueueNode::_sampleMover() const
{
	AnimationNode* cn = getCurrentNode();
	if( cn == NULL )
		return Skeleton::Situation::Identity;

	return cn->_sampleMover();
}

void AnimationQueueNode::_clone( AnimationNode* clonePtr, bool shareData ) const
{
	zhAssert( clonePtr != NULL );
	zhAssert( getClassId() == clonePtr->getClassId() );

	AnimationNode::_clone( clonePtr, shareData );

	zhLog( "AnimationQueueNode", "_clone",
		"Cloning AnimationQueueNode %s %u, %s.",
		getClassName().c_str(), mId, mName.c_str() );

	AnimationQueueNode* clone = static_cast<AnimationQueueNode*>( clonePtr );

	// clone data members

	if( mCurrentNode != NULL )
		clone->mCurrentNode = clone->getAnimationTree()->getNode( mCurrentNode->getId() );
	if( mNextNode != NULL )
		clone->mNextNode = clone->getAnimationTree()->getNode( mNextNode->getId() );
	clone->mWeight = mWeight;

	// TODO: will the cloned queue have the same ordering?
	for( std::queue<Transition>::container_type::const_iterator tri = mTransitionQueue._Get_container().begin();
		tri != mTransitionQueue._Get_container().end(); ++tri )
	{
		Transition tr( tri->getStartTime(), tri->getEndTime(), tri->getTargetTime(),
			clone->getAnimationTree()->getNode( tri->getTargetNode()->getId() ),
			tri->getTargetParams() );
		clone->addTransition(tr);
	}

	clone->mDefaultTransitionLength = mDefaultTransitionLength;
	if( mDefaultNode != NULL )
		clone->mDefaultNode = clone->getAnimationTree()->getNode( mDefaultNode->getId() );
}

void AnimationQueueNode::_updateNode( float dt )
{
	/*

	              mNextNode
	                  |
	                  V
	mCurrentNode, Transition1, Transition2, ...

	*/
	
	if( mTransitionQueue.empty() )
	{
		if( mDefaultNode != NULL )
			// play default animation
			addTransition(mDefaultNode);
	}

	if( mCurrentNode == NULL )
	{
		return;
	}

	// update the currently playing animation
	float time0 = mCurrentNode->getPlayTime();
	float length = mCurrentNode->getPlayLength();
	mCurrentNode->update(dt);
	float time = time0 + dt;

	if( mTransitionQueue.empty() )
	{
		// no more animations are scheduled,
		// we're done here

		if( time >= length - 0.00001f )
			mCurrentNode = NULL;

		return;
	}

	AnimationBlendNode* next_pnode = NULL; // if target is blend, cast down to this pointer
	if( mNextNode != NULL && mNextNode->isClass( AnimationBlendNode::ClassId() ) )
		next_pnode = static_cast<AnimationBlendNode*>(mNextNode);
	
	// get next transition times
	float stime = mTransitionQueue.front().getStartTime();
	if( stime < 0 ) stime = 0;
	float etime = mTransitionQueue.front().getEndTime();
	if( etime > length ) etime = length;
	float ttime = mTransitionQueue.front().getTargetTime();
	if( ttime < 0 ) ttime = 0;
	float mNextTime = ttime + time - stime;
	if( next_pnode != NULL )
		mNextParams = mTransitionQueue.front().getTargetParams();

	// assume we haven't started or finished a transition
	mTransStarted = false;
	mTransFinished = false;

	if( time < stime )
	{
		// haven't reached the transition yet
		
		mNextNode = NULL;
	}
	else if( time >= stime && time <= etime )
	{
		// we're in the transition

		if( mNextNode == NULL )
		{
			// we've just entered the transition, get target anim. and compute its origin

			mTransStarted = true;

			mNextNode = mTransitionQueue.front().getTargetNode();
			mNextOrigin = mNextNode->_getRealignedOrigin( mOwner->_getPrevSituation() );
			
			if( mCurrentNode != mNextNode )
			{
				// next anim. is not same as current anim., so update it right now
				// otherwise, postpone update to apply phase

				mNextNode->setPlayTime(mNextTime);
				mNextNode->setOrigin(mNextOrigin);
				if( next_pnode != NULL )
					next_pnode->setParams(mNextParams);
			}
		}
		else
		{
			// already entered the transition on a previous update
		
			if( mCurrentNode != mNextNode )
			{
				// next anim. is not same as current anim., so update it right now
				// otherwise, postpone update to apply phase

				mNextNode->update(dt);
			}
		}

		// compute blend weight
		float t = ( time - stime ) / ( etime - stime );
		float t2 = t * t;
		mWeight = 2 * t2 * t - 3 * t2 + 1.f;
	}
	else // if( time > etime )
	{
		// transition finished

		mTransFinished = true;

		if( mNextNode == NULL )
		{
			// we've skipped over the transition, get target anim. and compute its origin

			mNextNode = mTransitionQueue.front().getTargetNode();
			mNextOrigin = mNextNode->_getRealignedOrigin( mOwner->_getPrevSituation() );
			
			mNextNode->setPlayTime(mNextTime);
			mNextNode->setOrigin(mNextOrigin);

			if( mNextNode->isClass( AnimationBlendNode::ClassId() ) )
			{
				next_pnode = static_cast<AnimationBlendNode*>(mNextNode);

				// apply target param. values
				next_pnode->setParams(mNextParams);
			}
		}
		else
		{
			mNextNode->update(dt);
		}

		// update transition queue
		mCurrentNode = mNextNode;
		mNextNode = NULL;
		//mTransitionQueue.push( mTransitionQueue.front() );
		// TODO: need a way to enable/disable transition queue looping
		mTransitionQueue.pop();

		// update again in case we've already entered another transition
		_updateNode(0);
	}
}

void AnimationQueueNode::_applyNode( float weight, const std::set<unsigned short>& boneMask ) const
{
	if( mCurrentNode == NULL )
		// no animations are scheduled, nothing to do here
		return;

	if( mNextNode == NULL )
	{
		// we're not in a transition, apply the current animation
		mCurrentNode->apply( weight, boneMask );
	}
	else
	{
		// we're in a transition, apply both animations with
		// appropriate weights

		AnimationBlendNode* next_pnode = NULL; // if target is blend, cast down to this pointer
		if( mNextNode->isClass( AnimationBlendNode::ClassId() ) )
			next_pnode = static_cast<AnimationBlendNode*>(mNextNode);

		// apply current anim.
		mCurrentNode->apply( weight * mWeight, boneMask );

		float ctime = 0;
		Skeleton::Situation corig;
		Vector cparams;
		if( mCurrentNode == mNextNode )
		{
			// next anim. same as current anim.,
			// save next anim. playback state
			ctime = mNextNode->getPlayTime();
			corig = mNextNode->getOrigin();
			if( next_pnode != NULL )
				cparams = next_pnode->getParams();
			
			// update next anim.
			mNextNode->setPlayTime(mNextTime);
			mNextNode->setOrigin(mNextOrigin);
			if( next_pnode != NULL )
				next_pnode->setParams(mNextParams);
		}

		// apply next anim.
		mNextNode->apply( weight * ( 1.f - mWeight ), boneMask );

		if( mCurrentNode == mNextNode )
		{
			// next anim. same as current anim.,
			// restore next anim. playback state
			mNextNode->setPlayTime(ctime);
			mNextNode->setOrigin(corig);
			if( next_pnode != NULL )
				next_pnode->setParams(cparams);
		}

		// notify listeners of transition start
		if(mTransStarted)
		{
			AnimationQueueNodeEvent evt( const_cast<AnimationQueueNode*>(this), false );
			evt.emit();
			
			mTransStarted = false;
		}

		// notify listeners of transition end
		if(mTransFinished)
		{
			AnimationQueueNodeEvent evt( const_cast<AnimationQueueNode*>(this), true );
			evt.emit();
			
			mTransFinished = false;
		}
	}
}

}
