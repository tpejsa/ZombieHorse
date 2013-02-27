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

#ifndef __zhAnnotationMatchMaker_h__
#define __zhAnnotationMatchMaker_h__

#include "zhPrereq.h"
#include "zhAnimationSegment.h"
#include "zhAnimationSpace.h"
#include "zhAnimationBlender.h"

namespace zh
{

/**
* @brief Class that provides an implementation of the algorithm
* for detecting matching semantic annotations on animation clips
* that comprise an animation space.
*/
class zhDeclSpec AnnotationMatchMaker
{

public:

	/**
	* Constructor.
	*
	* @param animSpace Animation space.
	*/
	AnnotationMatchMaker( AnimationSpace* animSpace );

	/**
	* Destructor.
	*/
	virtual ~AnnotationMatchMaker();

	/**
	* Gets the animation space.
	*
	* @return Animation space.
	*/
	virtual AnimationSpace* getAnimationSpace() const;

	/**
	* Detects and creates annotation matches in the animation space
	* for all annotations.
	*/
	virtual void makeMatches();

	/**
	* Detects and creates annotation matches in the animation space
	* for the specified class of annotations.
	*/
	virtual void makeMatches( AnimationAnnotationClass annotClass );

protected:

	virtual void _makeMatches( const std::vector<AnimationAnnotationContainer*>& annotSet );
	virtual bool _areMatchable( AnimationAnnotation* annot1, AnimationAnnotation* annot2 );
	virtual float _toGlobalTime( Animation* anim, float time ) const;
	virtual float _toLocalTime( Animation* anim, float time ) const;
	virtual bool _areConnected( const std::vector<AnimationAnnotation*>& annots ) const;
	virtual bool _subsumes( AnimationAnnotationContainer* annots1, unsigned int annotIndex1,
		AnimationAnnotationContainer* annots2, unsigned int annotIndex2 ) const;
	virtual float _overlap( AnimationAnnotation* annot1, AnimationAnnotation* annot2 ) const;
	virtual void _getSplitTimes( AnimationAnnotationContainer* annots1, unsigned int annotIndex1,
		AnimationAnnotationContainer* annots2, unsigned int annotIndex2, float& lEndTime, float& rStartTime );

	AnimationSpace* mAnimSpace;

};

}

#endif // __zhAnnotationMatchMaker_h__
