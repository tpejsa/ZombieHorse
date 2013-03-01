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

#ifndef __zhAnimationNodeEvents_h__
#define __zhAnimationNodeEvents_h__

#include "zhPrereq.h"
#include "zhEvent.h"

namespace zh
{

/**
* @brief Generic event triggered when an annotation
* is encountered during animation playback.
*/
template <class AT>
class AnnotationEvent : public Event< AnnotationEvent<AT> >
{

public:

	/**
	* Constructor.
	*
	* @param animNode Animation node that emitted the event.
	* @param annot Animation annotation that triggered the event.
	* @param finished If set to false, event was triggered by annotation end,
	* otherwise it was triggered by annotation start.
	*/
	AnnotationEvent( AnimationNode* animNode, AT* annot, bool finished = false ) : Event< AnnotationEvent<AT> >(animNode)
	{
		mAnimNode = animNode;
		mAnnot = annot;
		mFinished = finished;
	}

	/**
	* Gets the animation node that emitted the event.
	*/
	virtual AnimationNode* getAnimationNode() const { return mAnimNode; }

	/**
	* Gets the animation annotation that triggered the event.
	*/
	virtual AT* getAnnotation() const { return mAnnot; }

	/**
	* If true, event was triggered by annotation start.
	*/
	virtual bool getActive() const { return !mFinished; }

	/**
	* If true, event was triggered by annotation end.
	*/
	virtual bool getFinished() const { return mFinished; }

protected:

	AnimationNode* mAnimNode;
	AT* mAnnot;
	bool mFinished;

};

typedef AnnotationEvent<TransitionAnnotation> TransitionAnnotEvent;
typedef AnnotationEvent<ParamTransitionAnnotation> ParamTransitionAnnotEvent;
typedef AnnotationEvent<PlantConstraintAnnotation> PlantConstraintAnnotEvent;
typedef AnnotationEvent<SimEventAnnotation> SimEventAnnotEvent;

typedef EventEmitter<TransitionAnnotEvent> TransitionAnnotEmitter;
typedef EventEmitter<ParamTransitionAnnotEvent> ParamTransitionAnnotEmitter;
typedef EventEmitter<PlantConstraintAnnotEvent> PlantConstraintAnnotEmitter;
typedef EventEmitter<SimEventAnnotEvent> SimEventAnnotEmitter;

typedef EventListener<TransitionAnnotEvent> TransitionAnnotListener;
typedef EventListener<ParamTransitionAnnotEvent> ParamTransitionAnnotListener;
typedef EventListener<PlantConstraintAnnotEvent> PlantConstraintAnnotListener;
typedef EventListener<SimEventAnnotEvent> SimEventAnnotListener;

}

#endif // __zhAnimationNodeEvents_h__
