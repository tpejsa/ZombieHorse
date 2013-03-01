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

#ifndef __zhAnimAnnot_h__
#define __zhAnimAnnot_h__

#include "zhPrereq.h"
#include "zhIterators.h"
#include "zhAllocObj.h"
#include "zhMemoryPool.h"
#include "zhSkeleton.h"
#include "zhEvent.h"

namespace zh
{

enum AnimationAnnotationClass
{
	AnimAnnot_Transition,
	AnimAnnot_ParamTransition,
	AnimAnnot_PlantConstraint,
	AnimAnnot_SimEvent
};

/**
* @brief Base class for animation annotations.
*/
class zhDeclSpec AnimationAnnotation : public AllocObj<MemoryPool>
{

public:

	/**
	* Constructor.
	*
	* @param startTime Animation annotation start time.
	* @param endTime Animation annotation end time.
	*/
	AnimationAnnotation( float startTime, float endTime );

	/**
	* Destructor.
	*/
	virtual ~AnimationAnnotation();

	/**
	* Gets the animation annotation class ID.
	*
	* This pure virtual function should be implemented
	* in concrete animation annotation classes.
	*/
	virtual AnimationAnnotationClass getClassId() const = 0;

	/**
	* Gets the start time of this animation annotation.
	*/
	float getStartTime() const;

	/**
	* Sets the start time of this animation annotation.
	*/
	void setStartTime( float time );

	/**
	* Gets the end time of this animation annotation.
	*/
	float getEndTime() const;

	/**
	* Sets the end time of this animation annotation.
	*/
	void setEndTime( float time );

	/**
	* Copies annotation data to another annotation.
	*
	* @param clonePtr Pointer to the copy.
	*/
	virtual void _clone( AnimationAnnotation* clonePtr ) const;

protected:

	float mStartTime;
	float mEndTime;

};

/**
* @brief Class representing an animation annotation
* denoting a transition interval to another animation.
*/
class zhDeclSpec TransitionAnnotation : public AnimationAnnotation
{

public:

	/**
	* Constructor.
	*
	* @param startTime Animation annotation start time.
	* @param endTime Animation annotation end time.
	*/
	TransitionAnnotation( float startTime, float endTime );

	/**
	* Destructor.
	*/
	~TransitionAnnotation();

	/**
	* Gets the animation annotation class ID.
	*/
	AnimationAnnotationClass getClassId() const { return AnimAnnot_Transition; }

	/**
	* Gets the target animation set *or* node class ID.
	*/
	unsigned long getTargetSetId() const;

	/**
	* Sets the target animation set *or* node class ID.
	*/
	void setTargetSetId( unsigned long targetSetId );

	/**
	* Gets the target animation ID.
	*/
	unsigned short getTargetId() const;

	/**
	* Sets the target animation ID.
	*/
	void setTargetId( unsigned short targetId );

	/**
	* Gets the transition start time in the target animation.
	*/
	float getTargetTime() const;

	/**
	* Sets the transition start time in the target animation.
	*/
	void setTargetTime( float targetTime );

	/**
	* Gets the 2D transformation that aligns the target animation
	* to the source animation in the ground plane.
	*/
	const Skeleton::Situation& getAlignTransf() const;

	/**
	* Sets the 2D transformation that aligns the target animation
	* to the source animation in the ground plane.
	*/
	void setAlignTransf( const Skeleton::Situation& transf );

	/**
	* Copies annotation data to another annotation.
	*
	* @param clonePtr Pointer to the copy.
	*/
	void _clone( AnimationAnnotation* clonePtr ) const;

private:

	unsigned long mTargetSetId;
	unsigned short mTargetId;
	float mTargetTime;
	Skeleton::Situation mAlignTransf;

};

/**
* @brief Class representing an animation annotation
* denoting a transition interval to a parametrized animation.
*/
class zhDeclSpec ParamTransitionAnnotation : public AnimationAnnotation
{

public:

	/**
	* Constructor.
	*
	* @param startTime Animation annotation start time.
	* @param endTime Animation annotation end time.
	*/
	ParamTransitionAnnotation( float startTime, float endTime );

	/**
	* Destructor.
	*/
	~ParamTransitionAnnotation();

	/**
	* Gets the animation annotation class ID.
	*/
	AnimationAnnotationClass getClassId() const { return AnimAnnot_ParamTransition; }

	/**
	* Gets the target animation set ID.
	*/
	unsigned long getTargetSetId() const;

	/**
	* Sets the target animation set ID.
	*/
	void setTargetSetId( unsigned long targetSetId );

	/**
	* Gets the target animation space ID.
	*/
	unsigned short getTargetId() const;

	/**
	* Sets the target animation ID.
	*/
	void setTargetId( unsigned short targetId );

	/**
	* Gets the lower boundary of the transition target region.
	*/
	const Vector& getLowerBound() const;

	/**
	* Sets the lower boundary of the transition target region.
	*/
	void setLowerBound( const Vector& lBound );
	
	/**
	* Gets the upper boundary of the transition target region.
	*/
	const Vector& getUpperBound() const;

	/**
	* Sets the upper boundary of the transition target region.
	*/
	void setUpperBound( const Vector& uBound );

	/**
	* Gets the normalized transition start time in the target animation.
	*/
	float getTargetTime() const;

	/**
	* Gets the normalized transition start time in the target animation.
	*/
	void setTargetTime( float time );

	/**
	* Gets the 2D transformation that aligns the target animation
	* to the source animation in the ground plane.
	*/
	const Skeleton::Situation& getAlignTransf() const;

	/**
	* Sets the 2D transformation that aligns the target animation
	* to the source animation in the ground plane.
	*/
	void setAlignTransf( const Skeleton::Situation& transf );

	/**
	* Copies annotation data to another annotation.
	*
	* @param clonePtr Pointer to the copy.
	*/
	void _clone( AnimationAnnotation* clonePtr ) const;

private:

	unsigned long mTargetSetId;
	unsigned short mTargetId;
	Vector mLBound, mUBound;
	float mTargetTime;
	Skeleton::Situation mAlignTransf;

};

/**
* @brief Class representing an animation annotation
* denoting an end-effector plant constraint (typically footplant).
*/
class zhDeclSpec PlantConstraintAnnotation : public AnimationAnnotation
{

public:

	/**
	* Constructor.
	*
	* @param startTime Animation annotation start time.
	* @param endTime Animation annotation end time.
	* @param boneId End-effector bone ID.
	*/
	PlantConstraintAnnotation( float startTime, float endTime );

	/**
	* Destructor.
	*/
	~PlantConstraintAnnotation();

	/**
	* Gets the animation annotation class ID.
	*/
	AnimationAnnotationClass getClassId() const { return AnimAnnot_PlantConstraint; }

	/**
	* Gets the end-effector bone ID.
	*/
	unsigned short getBoneId() const;
	
	/**
	* Sets the end-effector bone ID.
	*/
	void setBoneId( unsigned short boneId );

	/**
	* Copies annotation data to another annotation.
	*
	* @param clonePtr Pointer to the copy.
	*/
	void _clone( AnimationAnnotation* clonePtr ) const;

private:

	unsigned short mBoneId;

};

/**
* @brief Class representing an animation annotation
* denoting a simulation event triggered by the animation.
*/
class zhDeclSpec SimEventAnnotation : public AnimationAnnotation
{

public:

	/**
	* Constructor.
	*
	* @param startTime Animation annotation start time.
	* @param endTime Animation annotation end time.
	*/
	SimEventAnnotation( float startTime, float endTime );

	/**
	* Destructor.
	*/
	~SimEventAnnotation();

	/**
	* Gets the animation annotation class ID.
	*/
	AnimationAnnotationClass getClassId() const { return AnimAnnot_SimEvent; }

	/**
	* Gets the simulation event class ID.
	*/
	unsigned short getEventClassId() const;
	
	/**
	* Sets the simulation event class ID.
	*/
	void setEventClassId( unsigned short evtCId );

	/**
	* Gets the simulation event ID.
	*/
	unsigned short getEventId() const;
	
	/**
	* Sets the simulation event ID.
	*/
	void setEventId( unsigned short evtId );

	/**
	* Copies annotation data to another annotation.
	*
	* @param clonePtr Pointer to the copy.
	*/
	void _clone( AnimationAnnotation* clonePtr ) const;

private:

	unsigned short mEvtCId;
	unsigned short mEvtId;

};

/**
* @brief Base class for animation annotation containers.
*/
class zhDeclSpec AnimationAnnotationContainer
{

public:

	typedef VectorIterator< std::vector<AnimationAnnotation*> > AnnotationIterator;
	typedef VectorConstIterator< std::vector<AnimationAnnotation*> > AnnotationConstIterator;

	/**
	* Constructor.
	*/
	AnimationAnnotationContainer();

	/**
	* Destructor.
	*/
	virtual ~AnimationAnnotationContainer();

	/**
	* Gets the animation annotation class ID.
	*
	* This pure virtual function should be implemented
	* in concrete animation annotation container classes.
	*/
	virtual AnimationAnnotationClass getClassId() const = 0;

	/**
	* Creates an animation annotation
	* on the specified time interval.
	*
	* @param startTime Animation annotation start time.
	* @param endTime Animation annotation end time.
	* @return Pointer to the animation annotation.
	*/
	virtual AnimationAnnotation* createAnnotation( float startTime, float endTime );

	/**
	* Deletes the specified annotation.
	*
	* @param index Annotation index.
	*/
	virtual void deleteAnnotation( unsigned int index );

	/**
	* Deletes all annotations in this animation.
	*/
	virtual void deleteAllAnnotations();

	/**
	* Gets the specified annotation.
	*
	* @param index Annotation index.
	* @return Pointer to the animation annotation.
	*/
	virtual AnimationAnnotation* getAnnotation( unsigned int index ) const;

	/**
	* Gets the number of annotations in this animation.
	*/
	virtual unsigned int getNumAnnotations() const;

	/**
	* Gets an iterator over the map of annotations.
	*/
	virtual AnnotationIterator getAnnotationIterator();

	/**
	* Gets a const iterator over the map of annotations.
	*/
	virtual AnnotationConstIterator getAnnotationConstIterator() const;

	/**
	* Gets the list of animation annotations that are active
	* at the specified time.
	*
	* @param time Current sampling time.
	* @param annots Annotations that are active.
	* @return Number of active annotations.
	*/
	virtual unsigned int getActiveAnnotations( float time, std::vector<AnimationAnnotation*>& annots ) const;

	/**
	* Gets the list of animation annotations that have become active
	* at the specified time.
	*
	* @param prevTime Previous sampling time.
	* @param time Current sampling time.
	* @param annots Annotations that have become active.
	* @param countPassed If true, annotations that have been passed over
	* (i.e. that start and end between prevTime and time)
	* will be counted as well.
	* @return Number of active annotations.
	*/
	virtual unsigned int getActiveAnnotations( float prevTime, float time,
		std::vector<AnimationAnnotation*>& annots, bool countPassed = false ) const;

	/**
	* Gets the list of animation annotations that are no longer active
	* at the specified time.
	*
	* @param prevTime Previous sampling time.
	* @param time Current sampling time.
	* @param annots Annotations that are no longer active.
	* @param countPassed If true, annotations that have been passed over
	* (i.e. that start and end between prevTime and time)
	* will be counted as well.
	* @return Number of finished annotations.
	*/
	virtual unsigned int getFinishedAnnotations( float prevTime, float time,
		std::vector<AnimationAnnotation*>& annots, bool countPassed = true ) const;

	/**
	* Creates a deep copy of the annotation container.
	*
	* @param clonePtr Pointer to the copy.
	*/
	virtual void _clone( AnimationAnnotationContainer* clonePtr ) const;

protected:

	virtual AnimationAnnotation* _createAnnotation( float startTime, float endTime ) = 0;
	///< Actual annotation creation, implemented in concrete AnimationAnnotationContainer subclasses.

	std::vector<AnimationAnnotation*> mAnnots;

};

/**
* @brief Transition annotation container class.
*/
class zhDeclSpec TransitionAnnotationContainer : public AnimationAnnotationContainer
{

public:

	/**
	* Gets the animation annotation class ID.
	*/
	AnimationAnnotationClass getClassId() const { return AnimAnnot_Transition; }

protected:

	AnimationAnnotation* _createAnnotation( float startTime, float endTime );

};

/**
* @brief Param. transition annotation container class.
*/
class zhDeclSpec ParamTransitionAnnotationContainer : public AnimationAnnotationContainer
{

public:

	/**
	* Gets the animation annotation class ID.
	*/
	AnimationAnnotationClass getClassId() const { return AnimAnnot_ParamTransition; }

protected:

	AnimationAnnotation* _createAnnotation( float startTime, float endTime );

};

/**
* @brief Plant constraint annotation container class.
*/
class zhDeclSpec PlantConstraintAnnotationContainer : public AnimationAnnotationContainer
{

public:

	/**
	* Gets the animation annotation class ID.
	*/
	AnimationAnnotationClass getClassId() const { return AnimAnnot_PlantConstraint; }

protected:

	AnimationAnnotation* _createAnnotation( float startTime, float endTime );

};

/**
* @brief Simulation event annotation container class.
*/
class zhDeclSpec SimEventAnnotationContainer : public AnimationAnnotationContainer
{

public:

	/**
	* Gets the animation annotation class ID.
	*/
	AnimationAnnotationClass getClassId() const { return AnimAnnot_SimEvent; }

protected:

	AnimationAnnotation* _createAnnotation( float startTime, float endTime );

};

}

#endif // __zhAnimAnnot_h__
