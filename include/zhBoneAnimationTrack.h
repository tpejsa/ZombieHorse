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

#ifndef __zhBoneAnimationTrack_h__
#define __zhBoneAnimationTrack_h__

#include "zhPrereq.h"
#include "zhIterators.h"
#include "zhMath.h"
#include "zhAnimationTrack.h"

namespace zh
{

/**
* @brief Class representing a transform (translation, rotation, scale)
* key-frame.
*/
class zhDeclSpec TransformKeyFrame : public KeyFrame
{

public:

	/**
	* Constructor.
	*/
	TransformKeyFrame( float time, unsigned int index );

	/**
	* Destructor.
	*/
	~TransformKeyFrame();

	/**
	* Gets the translation component.
	*/
	const Vector3& getTranslation() const;

	/**
	* Sets the translation component.
	*/
	void setTranslation( const Vector3& trans );

	/**
	* Gets the rotation component.
	*/
	const Quat& getRotation() const;

	/**
	* Sets the rotation component.
	*/
	void setRotation( const Quat& rot );

	/**
	* Gets the scale component.
	*/
	const Vector3& getScale() const;

	/**
	* Gets the scale component.
	*/
	void setScale( const Vector3& scal );

private:

	Vector3 mTranslation;
	Quat mRotation;
	Vector3 mScale;

};

/**
* @brief Class representing a bone animation track.
*/
class zhDeclSpec BoneAnimationTrack : public AnimationTrack
{

public:

	/**
	* Constructor.
	*
	* @param boneId Bone ID.
	* @param anim Pointer to the owning animation.
	*/
	BoneAnimationTrack( unsigned short boneId, Animation* anim );

	/**
	* Destructor.
	*/
	~BoneAnimationTrack();

	/**
	* Gets the animation track class ID.
	*/
	static AnimationTrackClass ClassId() { return AnimationTrack_Bone; }

	/**
	* Gets the animation track class ID.
	*/
	AnimationTrackClass getClassId() const { return BoneAnimationTrack::ClassId(); }

	/**
	* Gets the ID of the bone affected by this track.
	*/
	unsigned short getBoneId() const;

	/**
	* Gets the key-frame interpolated from the neighboring key-frames
	* at the specified time.
	*
	* @param time Track sampling time.
	* @param kf Pointer to the interpolated key-frame.
	*/
	void getInterpolatedKeyFrame( float time, KeyFrame* kf ) const;

	/**
	* Applies the animation track to the specified skeleton.
	*
	* @param skel Pointer to the Skeleton
	* which should be updated by this animation track.
	* @param time Time at which this animation track should be applied.
	* @param weight Blend weight with which this animation track
	* should be applied.
	* @param scale Scaling factor applied to this animation track.
	*/
	 void apply( Skeleton* skel, float time, float weight = 1.f, float scale = 1.f ) const;

	 /**
	 * Builds the splines used for key-frame interpolation.
	 *
	 * @remark This function is called automatically by Animation.
	 * Do not call it manually without a good reason.
	 */
	 void _buildInterpSplines() const;

protected:

	KeyFrame* _createKeyFrame( float time );

private:

	unsigned short mBoneId;

	mutable CatmullRomSpline<Vector3> mTransSpline;
	mutable CatmullRomSpline<Quat> mRotSpline;
	mutable CatmullRomSpline<Vector3> mScalSpline;

};

}

#endif // __zhBoneAnimationTrack_h__
