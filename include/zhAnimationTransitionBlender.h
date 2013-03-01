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

#ifndef __zhAnimationTransitionBlender_h__
#define __zhAnimationTransitionBlender_h__

#include "zhPrereq.h"
#include "zhIterators.h"
#include "zhObjectFactory.h"
#include "zhEvent.h"
#include "zhMath.h"
#include "zhAnimationTree.h"
#include "zhAnimationNode.h"
#include "zhAnimation.h"
#include "zhAnimationTransitionBlenderEvents.h"

#include <queue>

namespace zh
{

class AnimationBlender;

/**
* @brief Class representing an animation transition
* blending and scheduling node.
*/
class zhDeclSpec AnimationTransitionBlender : public AnimationNode,
	public TransitionBlendEmitter
{

public:

	/**
	* @brief Specifies a scheduled transition.
	*/
	struct Transition
	{

	public:

		/**
		* Constructor.
		*
		* @param startTime Transition start time.
		* @param endTime Transition end time.
		* @param targetTime Transition start time in the target animation.
		* @param targetNode Pointer to the target animation node.
		* @param targetParams Parameter values in the target animation node
		* (ignored if targetNode is not parametrized).
		*/
		Transition( float startTime, float endTime,
			float targetTime, AnimationNode* targetNode, const Vector& targetParams )
			: mStartTime(startTime), mEndTime(endTime),
			mTargetTime(targetTime), mTargetNode(targetNode), mTargetParams(targetParams)

		{
		}

		/**
		* Destructor.
		*/
		~Transition()
		{
		}

		/**
		* Gets the transition start time.
		*/
		float getStartTime() const
		{
			return mStartTime;
		}

		/**
		* Gets the transition end time.
		*/
		float getEndTime() const
		{
			return mEndTime;
		}

		/**
		* Gets the transition start time in the target animation.
		*/
		float getTargetTime() const
		{
			return mTargetTime;
		}

		/**
		* Gets a pointer to the target animation node.
		*/
		AnimationNode* getTargetNode() const
		{
			return mTargetNode;
		}

		/**
		* Gets the parameter values in the target animation node.
		*/
		const Vector& getTargetParams() const
		{
			return mTargetParams;
		}

	private:

		float mStartTime;
		float mEndTime;
		float mTargetTime;
		AnimationNode* mTargetNode;
		Vector mTargetParams;

	};

	typedef std::queue<Transition> TransitionQueue;

	zhDeclare_AnimationNode( AnimationTransitionBlender, zhAnimationTransitionBlender_ClassId, zhAnimationTransitionBlender_ClassName )

	/**
	* Constructor.
	*/
	AnimationTransitionBlender();

	/**
	* Destructor.
	*/
	~AnimationTransitionBlender();

	/**
	* Returns true if the current animation node is a leaf
	* (and therefore accepts no child nodes) or false otherwise.
	*/
	bool isLeaf() const;

	/**
	* Gets the current play time of this animation node.
	*/
	float getPlayTime() const;

	/**
	* Sets the current play time of this animation node.
	*/
	void setPlayTime( float time );

	/**
	* Gets the total playback length of this animation node.
	*/
	float getPlayLength() const;
	
	/**
	* Gets this animation's origin in space.
	*
	* @remark This property enables the animation to be
	* translated and rotated in space.
	*/
	const Skeleton::Situation& getOrigin() const;

	/**
	* Sets this animation's origin in space.
	*/
	void setOrigin( const Skeleton::Situation& origin );

	/**
	* Gets a pointer to the currently active animation node.
	*/
	virtual AnimationNode* getCurrentNode() const;

	/**
	* Gets the transition blend weight of the currently active
	* animation node.
	*/
	virtual float getCurrentWeight() const;

	/**
	* Gets a pointer to the next scheduled animation node.
	*/
	virtual AnimationNode* getNextNode() const;

	/**
	* Gets the transition blend weight of the next scheduled
	* animation node.
	*/
	virtual float getNextWeight() const;

	/**
	* Schedules a new transition.
	*
	* @param node Target animation node ID.
	*/
	virtual void addTransition( unsigned short id );

	/**
	* Schedules a new transition.
	*
	* @param node Target animation node name.
	*/
	virtual void addTransition( const std::string& name );

	/**
	* Schedules a new transition.
	*
	* @param node Target animation node.
	*/
	virtual void addTransition( AnimationNode* node );

	/**
	* Schedules a new transition.
	*
	* @param node Target animation blend node.
	* @param params Parameter values for the target blend.
	*/
	virtual void addTransition( AnimationBlender* node, const Vector& params );

	/**
	* Schedules a new transition.
	*
	* @param transSpec Transition specification.
	*/
	virtual void addTransition( const Transition& transSpec );

	/**
	* Removes the last scheduled transition.
	*/
	virtual void removeLastTransition();

	/**
	* Removes all scheduled transitions.
	*/
	virtual void removeAllTransitions();

	/**
	* Gets the queue of currently scheduled transitions.
	*/
	virtual const TransitionQueue& getTransitionQueue() const;

	/**
	* Gets the default transition length
	* (used when transitioning from an unnanotated animation).
	*/
	virtual float getDefaultTransitionLength() const;

	/**
	* Sets the default transition length
	* (used when transitioning from an unnanotated animation).
	*/
	virtual void setDefaultTransitionLength( float length );

	/**
	* Gets a pointer to the animation node that is active by default
	* (i.e. when no animations are scheduled). If NULL,
	* transition blender is inactive while the transition
	* schedule queue is empty.
	*/
	virtual AnimationNode* getDefaultNode() const;

	/**
	* Sets the child node that is active by default
	* (i.e. when no animations are scheduled).
	*
	* @param id Child node ID.
	*/
	virtual void setDefaultNode( unsigned short id );

	/**
	* Sets the child node that is active by default
	* (i.e. when no animations are scheduled).
	*
	* @param name Child node name.
	*/
	virtual void setDefaultNode( const std::string& name );

	/**
	* Sets the child node that is active by default
	* (i.e. when no animations are scheduled).
	*
	* @param node Child node or NULL
	* to set no default node.
	*/
	virtual void setDefaultNode( AnimationNode* node );

	/**
	* Gets the current time in the target animation.
	*/
	virtual float _getNextTime() const;

	/**
	* Gets the current origin for the target animation.
	*/
	virtual const Skeleton::Situation& _getNextOrigin() const;

	/**
	* If true, a transition has just started.
	*/
	virtual bool _getTransitionStarted() const;

	/**
	* If true, a transition has just finished.
	*/
	virtual bool _getTransitionFinished() const;

	/**
	* Samples this animation's mover at the current time.
	*
	* @return Mover value (with origin transformation applied)
	* @remark This is a helper method used for alignment of consecutive animations.
	* Any node that represents an animation with a mover channel
	* should override this method, otherwise alignment won't work correctly.
	*/
	Skeleton::Situation _sampleMover() const;

	/**
	* Calculates the AnimationNode memory usage.
	*/
	size_t _calcMemoryUsage() const;

	/**
	* Unloads the AnimationNode, freeing up the memory it occupies.
	*/
	void _unload();

	/**
	* Creates a deep copy of the AnimationNode.
	*
	* @param clonePtr Pointer to the copy.
	* @param shareData Specifies if the clone
	* should share data with the original.
	*/
	void _clone( AnimationNode* clonePtr, bool shareData = false ) const;

protected:

	/**
	* Updates this animation node's playback state. Any subclass
	* that wishes to implement its own update method should do so
	* by overriding this method.
	*
	* @param dt Elapsed time.
	*/
	void _update( float dt );

	/**
	* Applies this animation node to the current model.
	* Any subclass that wishes to implement its own apply method 
	* should do so by overriding this method.
	*
	* @param weight Blend weight.
	* @param boneMask Bone mask. Animation is not applied to masked bones.
	*/
	void _applyNode( float weight = 1.f, const std::set<unsigned short>& boneMask = Animation::EmptyBoneMask ) const;

	AnimationNode* mCurrentNode;
	AnimationNode* mNextNode;
	float mWeight;
	TransitionQueue mTransitionQueue;
	float mDefaultTransitionLength;
	AnimationNode* mDefaultNode;

	float mNextTime;
	Skeleton::Situation mNextOrigin;
	Vector mNextParams;
	mutable bool mTransStarted;
	mutable bool mTransFinished;

};

}

#endif // __zhAnimationTransitionBlender_h__
