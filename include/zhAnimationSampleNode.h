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

#ifndef __zhAnimationSampleNode_h__
#define __zhAnimationSampleNode_h__

#include "zhPrereq.h"
#include "zhIterators.h"
#include "zhObjectFactory.h"
#include "zhAnimationTree.h"
#include "zhAnimationNode.h"
#include "zhAnimationManager.h"
#include "zhAnimation.h"
#include "zhAnimationAnnotation.h"

namespace zh
{

class AnimationSampleNode;

/**
* @brief Class representing an animation sampling node.
*/
class zhDeclSpec AnimationSampleNode : public AnimationNode
{

public:

	zhDeclare_AnimationNode( AnimationSampleNode, zhAnimationSampleNode_ClassId, zhAnimationSampleNode_ClassName )

	/**
	* Constructor.
	*/
	AnimationSampleNode();

	/**
	* Destructor.
	*/
	~AnimationSampleNode();

	/**
	* Returns true if the current animation node is a leaf
	* (and therefore accepts no child nodes) or false otherwise.
	*/
	bool isLeaf() const;

	/**
	* Sets the play status of this animation node and its children.
	*
	* @param play If true, this animation node will be playing.
	*/
	void setPlaying( bool playing = true );

	/**
	* Gets the current play time of this animation node.
	*/
	float getPlayTime() const;

	/**
	* Sets the current play time of this animation node.
	*/
	void setPlayTime( float time );

	/**
	* Gets the current play time of this animation node,
	* normalized to range 0 to 1.
	*/
	float getNormalizedPlayTime() const;

	/**
	* Sets the current play time of this animation node,
	* normalized to range 0 to 1.
	*/
	void setNormalizedPlayTime( float time );

	/**
	* Gets the total playback length of this animation node.
	*/
	float getPlayLength() const;

	/**
	* Gets the container of transition annotations.
	*/
	TransitionAnnotationContainer* getTransitionAnnotations() const;

	/**
	* Gets the container of param. transition annotations.
	*/
	ParamTransitionAnnotationContainer* getParamTransitionAnnotations() const;

	/**
	* Gets the container of plant constraint annotations.
	*/
	PlantConstraintAnnotationContainer* getPlantConstraintAnnotations() const;

	/**
	* Gets the container of simulation event annotations.
	*/
	SimEventAnnotationContainer* getSimEventAnnotations() const;

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
	* Gets the animation set containing
	* the animation sampled by this animation sampler.
	*/
	virtual AnimationSetPtr getAnimationSet() const;

	/**
	* Gets the ID of the animation
	* sampled by this animation sampler.
	*/
	virtual unsigned short getAnimationId() const;

	/**
	* Gets the animation
	* sampled by this animation sampler.
	*
	* @return Animation or NULL if none is set.
	*/
	virtual Animation* getAnimation() const;

	/**
	* Sets the animation sampled by this animation sampler.
	*/
	virtual void setAnimation( AnimationSetPtr animSet, unsigned short animId );

	/**
	* Samples this animation's mover channel at the current time.
	*
	* @return Mover value (with origin transformation applied)
	* @remark This is a helper method used for alignment of consecutive animations.
	* Any node that represents an animation with a mover channel
	* should override this method, otherwise alignment won't work correctly.
	*/
	Skeleton::Situation _sampleMover() const;

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
	void _updateNode( float dt );

	/**
	* Applies this animation node to the current model.
	* Any subclass that wishes to implement its own apply method 
	* should do so by overriding this method.
	*
	* @param weight Blend weight.
	* @param boneMask Bone mask. Animation is not applied to masked bones.
	*/
	void _applyNode( float weight = 1.f, const std::set<unsigned short>& boneMask = Animation::EmptyBoneMask ) const;

	float mPlayTime;
	Skeleton::Situation mOrigin;

	AnimationSetPtr mAnimSet;
	unsigned short mAnimId;

};

}

#endif // __zhAnimationSampleNode_h__
