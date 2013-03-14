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

#ifndef __zhAnimationBlendNode_h__
#define __zhAnimationBlendNode_h__

#include "zhPrereq.h"
#include "zhIterators.h"
#include "zhAnimationNode.h"
#include "zhAnimationSpace.h"

namespace zh
{

class Skeleton;
class BoneIKController;

/**
* @brief Class representing an animation blending node.
*/
class zhDeclSpec AnimationBlendNode : public AnimationNode
{

public:

	zhDeclare_AnimationNode( AnimationBlendNode, zhAnimationBlendNode_ClassId, zhAnimationBlendNode_ClassName )
	
	/**
	* Constructor.
	*/
	AnimationBlendNode();

	/**
	* Destructor.
	*/
	~AnimationBlendNode();

	/**
	* Returns true if the current animation node is a leaf
	* (and therefore accepts no child nodes) or false otherwise.
	*/
	bool isLeaf() const;

	// TODO: main child should be an AnimationSampleNode somewhere below this AnimationBlendNode in the tree
	// maybe even the one with the highest blend weight?

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
	* Gets the blend weight of the specified child node.
	*
	* @param nodeId Child node ID.
	* @return Blend weight.
	*/
	virtual float getWeight( unsigned short nodeId ) const;

	/**
	* Gets the blend weight of the specified child node.
	*
	* @param nodeName Child node name.
	* @return Blend weight.
	*/
	virtual float getWeight( const std::string& nodeName ) const;

	/**
	* Sets the blend weight of the specified child node.
	*
	* @param nodeId Child node ID.
	* @param weight Blend weight.
	*/
	virtual void setWeight( unsigned short nodeId, float weight = 1.f );

	/**
	* Sets the blend weight of the specified child node.
	*
	* @param nodeName Child node name.
	* @param weight Blend weight.
	*/
	virtual void setWeight( const std::string& nodeName, float weight = 1.f );

	/**
	* Sets all blend weights to the specified value
	* (e.g. useful for mass-reseting all blend weights to 0).
	*/
	virtual void setAllWeights( float weight = 0.f );

	/**
	* Gets the entire blend weight vector.
	*/
	virtual const Vector& getWeights() const;

	/**
	* Sets the entire blend weight vector.
	*/
	virtual void setWeights( const Vector& weights );

	/**
	* Gets the value of the specified control parameter.
	*
	* @param paramName Parameter name.
	* @return Parameter value.
	*/
	virtual float getParam( const std::string& paramName ) const;

	/**
	* Sets the value of the specified control parameter.
	*
	* @param paramName Parameter name.
	* @param param Parameter value.
	*/
	virtual void setParam( const std::string& paramName, float param );

	/**
	* Gets the entire control parameter value vector.
	*/
	virtual const Vector& getParams() const;

	/**
	* Sets the entire control parameter value vector.
	*/
	virtual void setParams( const Vector& paramValues );

	/**
	* Returns true if parametric control is enabled on this blend node
	* or false otherwise.
	*/
	virtual bool getParamEnabled() const;

	/**
	* Enables or disables parametric control on this blend node.
	*/
	virtual void setParamEnabled( bool enabled = true );

	/**
	* Gets the animation set containing
	* the animation space used by this animation blender.
	*/
	virtual AnimationSetPtr getAnimationSet() const;

	/**
	* Gets the ID of the animation space
	* used by this animation blender.
	*/
	virtual unsigned short getAnimationSpaceId() const;

	/**
	* Gets the animation space
	* used by this animation blender.
	*
	* @return Animation space or NULL if none is set.
	*/
	virtual AnimationSpace* getAnimationSpace() const;

	/**
	* Sets the animation space used by this animation blender.
	*/
	virtual void setAnimationSpace( AnimationSetPtr animSet, unsigned short animSpaceId );

	/**
	* Checks if blend curves are used for blending.
	*
	* @return true if blend curves are used for blending,
	* false otherwise.
	*/
	virtual bool getUseBlendCurves() const;

	/**
	* Sets if blend curves are used for blending.
	*
	* @param useBlendCurves If true, blend curves will be
	* used for blending if available.
	*/
	virtual void setUseBlendCurves( bool useBlendCurves = true );

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

	/**
	* Initializes the interpolated annotations.
	*
	* @param animSpace Pointer to the animation space.
	*/
	virtual void _initAnnotations();

	/**
	* Computes the interpolated annotations.
	*
	* @param weights Blend weights.
	*/
	virtual void _blendAnnotations();

	void _getTWCurvePosition( float u, unsigned int& cpi, float& t ) const; ///< Compute position on the timewarp curve.

	float mTWCurveTime; ///< Current time on the timewarp curve.
	Skeleton::Situation mOrigin;

	AnimationSetPtr mAnimSet;
	unsigned short mAnimSpaceId;
	std::map<unsigned short, unsigned int> mChildrenToBaseAnims;
	///< For each child node ID this map specifies the index of the corresponding base animation in the animation space.
	Vector mWeights;
	Vector mParams;
	bool mParamEnabled;

	bool mUseBlendCurves;

};

template <class AT>
bool matchAnnotations( AT* annot1, AT* annot2 )
{
	return true;
}

template <>
inline bool matchAnnotations<TransitionAnnotation>(
	TransitionAnnotation* annot1, TransitionAnnotation* annot2 )
{
	return annot1->getTargetSetId() == annot2->getTargetSetId() &&
		annot1->getTargetId() == annot2->getTargetId();
}

template <>
inline bool matchAnnotations<ParamTransitionAnnotation>(
	ParamTransitionAnnotation* annot1, ParamTransitionAnnotation* annot2 )
{
	return annot1->getTargetSetId() == annot2->getTargetSetId() &&
		annot1->getTargetId() == annot2->getTargetId();
}

template <>
inline bool matchAnnotations<PlantConstraintAnnotation>(
	PlantConstraintAnnotation* annot1, PlantConstraintAnnotation* annot2 )
{
	return annot1->getBoneId() == annot2->getBoneId();
}

template <>
inline bool matchAnnotations<SimEventAnnotation>(
	SimEventAnnotation* annot1, SimEventAnnotation* annot2 )
{
	return annot1->getEventClassId() == annot2->getEventClassId() &&
		annot1->getEventId() == annot2->getEventId();
}

template <class AT>
void resetAnnotation( AT* annot )
{
	annot->setStartTime(0);
	annot->setEndTime(0);
}

template <>
inline void resetAnnotation<TransitionAnnotation>( TransitionAnnotation* annot )
{
	annot->setStartTime(0);
	annot->setEndTime(0);

	annot->setTargetTime(0);
	annot->setAlignTransf( Skeleton::Situation() );
}

template <>
inline void resetAnnotation<ParamTransitionAnnotation>( ParamTransitionAnnotation* annot )
{
	annot->setStartTime(0);
	annot->setEndTime(0);

	annot->setLowerBound( Vector( annot->getLowerBound() ).null() );
	annot->setUpperBound( Vector( annot->getUpperBound() ).null() );
	annot->setTargetTime(0);
	annot->setAlignTransf( Skeleton::Situation() );
}

template <class AT>
void blendAnnotations( AT* annot1, AT* annot2, float weight )
{
	annot1->setStartTime( annot1->getStartTime() + annot2->getStartTime() * weight );
	annot1->setEndTime( annot1->getEndTime() + annot2->getEndTime() * weight );
}

template <>
inline void blendAnnotations<TransitionAnnotation>( TransitionAnnotation* annot1, TransitionAnnotation* annot2, float weight )
{
	annot1->setStartTime( annot1->getStartTime() + annot2->getStartTime() * weight );
	annot1->setEndTime( annot1->getEndTime() + annot2->getEndTime() * weight );

	annot1->setTargetTime( annot1->getTargetTime() + annot2->getTargetTime() * weight );
	Vector3 pos = annot1->getAlignTransf().getPosition() + annot2->getAlignTransf().getPosition() * weight;
	Quat orient = annot1->getAlignTransf().getOrientation() * Quat().slerp( annot2->getAlignTransf().getOrientation(), weight );
	annot1->setAlignTransf( Skeleton::Situation( pos, orient ) );
}

template <>
inline void blendAnnotations<ParamTransitionAnnotation>( ParamTransitionAnnotation* annot1, ParamTransitionAnnotation* annot2, float weight )
{
	annot1->setStartTime( annot1->getStartTime() + annot2->getStartTime() * weight );
	annot1->setEndTime( annot1->getEndTime() + annot2->getEndTime() * weight );

	annot1->setLowerBound( annot1->getLowerBound() + annot2->getLowerBound() * weight );
	annot1->setUpperBound( annot1->getUpperBound() + annot2->getUpperBound() * weight );
	annot1->setTargetTime( annot1->getTargetTime() + annot2->getTargetTime() * weight );
	Vector3 pos = annot1->getAlignTransf().getPosition() + annot2->getAlignTransf().getPosition() * weight;
	Quat orient = annot1->getAlignTransf().getOrientation() * Quat().slerp( annot2->getAlignTransf().getOrientation(), weight );
	annot1->setAlignTransf( Skeleton::Situation( pos, orient ) );
}

}

#endif // __zhAnimationBlendNode__
