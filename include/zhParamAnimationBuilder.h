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

#ifndef __zhParamAnimationBuilder_h__
#define __zhParamAnimationBuilder_h__

#include "zhPrereq.h"
#include "zhAnimationParametrization.h"

namespace zh
{

class Model;
class Animation;
class AnimationSpace;
class MatchGraph;

/**
* @brief Animation parametrization specification
* for a single parameter.
*
* The parameter is derived from the transformation
* of a skeletal bone relative to one of its superiors.
*/
struct AnimationParamSpec
{

	enum TransfType
	{
		TransfType_Translation,
		TransfType_Rotation
	};

	enum Axis
	{
		Axis_x,
		Axis_y,
		Axis_z
	};

	/**
	* Constructor.
	*
	* @param paramName Parameter name.
	* @param baseAnim Pointer to a base animation.
	* @param time Time in the base animation
	* where bone transformation should be sampled.
	* @param boneId ID of a bone in the character skeleton.
	* @param superBoneId ID of a superior bone in the skeletal hierarchy
	* (not necessarily the immediate parent).
	* @param transf Bone transformation (rotation or translation;
	* relative to the parent bone) that should be parametrized.
	* @param axis Transformation axis (x, y or z).
	*/
	AnimationParamSpec( const std::string& paramName, Animation* baseAnim, float time, unsigned short boneId,
		unsigned short superBoneId, TransfType transf, Axis axis )
		: mParamName(paramName), mBaseAnim(baseAnim), mTime(time), mBoneId(boneId),
		mSuperBoneId(superBoneId), mTransf(transf), mAxis(axis)
	{
		zhAssert( baseAnim != NULL );
	}

	/**
	* Gets the parameter name.
	*/
	const std::string& getParamName() const { return mParamName; }

	/**
	* Sets the parameter name.
	*/
	void setParamName( const std::string& paramName ) { mParamName = paramName; }

	/**
	* Gets the base animation.
	*
	* @return Base animation.
	*/
	Animation* getBaseAnimation() const { return mBaseAnim; }

	/**
	* Sets the base animation.
	*
	* @param baseAnim Base animation.
	*/
	void setBaseAnimation( Animation* baseAnim ) { zhAssert( baseAnim != NULL ); mBaseAnim = baseAnim; }

	/**
	* Gets the time in the base animation
	* where bone transformation should be sampled.
	*/
	float getTime() const { return mTime; }

	/**
	* Sets the time in the base animation
	* where bone transformation should be sampled.
	*/
	void setTime( float time ) { mTime = time; }

	/**
	* Gets the bone ID.
	*/
	unsigned short getBoneId() const { return mBoneId; }

	/**
	* Sets the bone ID.
	*/
	void setBoneId( unsigned short boneId ) { mBoneId = boneId; }

	/**
	* Gets the superior bone ID.
	*/
	unsigned short getSuperBoneId() const { return mSuperBoneId; }

	/**
	* Sets the superior bone ID.
	*/
	void setSuperiorBoneId( unsigned short superBoneId ) { mSuperBoneId = superBoneId; }

	/**
	* Gets the type of the bone transformation (translation or rotation).
	*/
	TransfType getTransfType() const { return mTransf; }

	/**
	* Sets the type of the bone transformation (translation or rotation).
	*/
	void setTransfType( TransfType transf ) { mTransf = transf; }

	/**
	* Gets the transformation axis.
	*/
	Axis getAxis() const { return mAxis; }

	/**
	* Sets the transformation axis.
	*/
	void setAxis( Axis axis ) { mAxis = axis; }

private:

	std::string mParamName;
	Animation* mBaseAnim;
	float mTime;
	unsigned short mBoneId;
	unsigned short mSuperBoneId;
	TransfType mTransf;
	Axis mAxis;

};

class zhDeclSpec ParamAnimationBuilder
{

public:

	/**
	* Constructor.
	*
	* @param mdl Pointer to the character model.
	* @param animSpace Pointer to the animation space.
	*/
	ParamAnimationBuilder( Model* mdl, AnimationSpace* animSpace );

	/**
	* Destructor.
	*/
	virtual ~ParamAnimationBuilder();

	/**
	* Gets the parametrization class ID.
	*/
	virtual AnimationParamClass getClassId() const = 0;

	/**
	* Gets the character model.
	*
	* @return Pointer to the model.
	*/
	Model* getModel() const { return mMdl; }

	/**
	* Gets the animation space.
	*
	* @return Pointer to the animation space.
	*/
	AnimationSpace* getAnimationSpace() const { return mAnimSpace; }

	/**
	* Builds a parametrization over the animation space.
	*
	* @param paramSpecs Vector containing specifications of all parameters.
	*/
	virtual void parametrize( const std::vector<AnimationParamSpec>& paramSpecs );

protected:

	virtual void _buildParametrization() = 0;

	Model* mMdl;
	AnimationSpace* mAnimSpace;

};

}

#endif // __zhParamAnimationBuilder_h__
