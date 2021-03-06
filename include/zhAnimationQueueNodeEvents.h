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

#ifndef __zhAnimationQueueNodeEvents_h__
#define __zhAnimationQueueNodeEvents_h__

#include "zhPrereq.h"
#include "zhEvent.h"

namespace zh
{

class AnimationQueueNode;

/**
* @brief Class representing an event which is emitted by
* AnimationQueueNode when a transition starts.
*/
class AnimationQueueNodeEvent :
	public Event<AnimationQueueNodeEvent>
{

public:

	/**
	* Constructor.
	*
	* @param node Transition blending node that triggered the event.
	* @param finished If true, event was triggered by transition end,
	* otherwise it was triggered by transition start.
	*/
	AnimationQueueNodeEvent( AnimationQueueNode* node, bool finished = false );

	/**
	* Gets the transition blend node which emitted this event.
	*/
	virtual AnimationQueueNode* getTransitionBlendNode() const;

	/**
	* If true, event was triggered by transition start.
	*/
	virtual bool getStarted() const;

	/**
	* If true, event was triggered by transition end.
	*/
	virtual bool getFinished() const;

protected:

	AnimationQueueNode* mTransBlendNode;
	bool mFinished;

};

typedef EventEmitter<AnimationQueueNodeEvent> TransitionBlendEmitter;
typedef EventListener<AnimationQueueNodeEvent> TransitionBlendListener;

}

#endif // __zhAnimationQueueNodeEvents_h__
