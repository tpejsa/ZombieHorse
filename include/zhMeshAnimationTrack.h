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

#ifndef __zhMeshAnimationTrack_h__
#define __zhMeshAnimationTrack_h__

#include "zhPrereq.h"
#include "zhIterators.h"
#include "zhAnimationTrack.h"

namespace zh
{

/**
* @brief Class representing a morphing key-frame.
*/
class zhDeclSpec MorphKeyFrame : public KeyFrame
{

public:

	/**
	* Constructor.
	*/
	MorphKeyFrame( float time, unsigned int index );

	/**
	* Destructor.
	*/
	~MorphKeyFrame();

	/**
	* Gets the morph target weights on this key-frame.
	*/
	const Vector& getMorphTargetWeights() const;

	/**
	* Sets the morph target weights on this key-frame.
	*/
	void setMorphTargetWeights( const Vector& weights );

private:

	Vector mMTWeights;

};

/**
* @brief Class representing a mesh animation track.
*/
class zhDeclSpec MeshAnimationTrack : public AnimationTrack
{

public:

	/**
	* Constructor.
	*
	* @param meshId Mesh ID.
	* @param anim Pointer to the owning animation.
	*/
	MeshAnimationTrack( unsigned short meshId, Animation* anim );

	/**
	* Destructor.
	*/
	~MeshAnimationTrack();

	/**
	* Gets the animation track class ID.
	*/
	static AnimationTrackClass ClassId() { return AnimationTrack_Mesh; }

	/**
	* Gets the animation track class ID.
	*/
	AnimationTrackClass getClassId() const { return MeshAnimationTrack::ClassId(); }

	/**
	* Gets the ID of the mesh affected by this track.
	*/
	unsigned short getMeshId() const;

	/**
	* Gets the key-frame interpolated from the neighboring key-frames
	* at the specified time.
	*
	* @param time Track sampling time.
	* @param kf Pointer to the interpolated key-frame.
	*/
	void getInterpolatedKeyFrame( float time, KeyFrame* kf ) const;

	/**
	* Applies the animation track to the specified model.
	*
	* @param model Pointer to the Model
	* which should be updated by this animation track.
	* @param time Time at which this animation track should be applied.
	* @param weight Blend weight with which this animation track
	* should be applied.
	* @param scale Scaling factor applied to this animation track.
	*/
	 void apply( Model* model, float time, float weight = 1.f, float scale = 1.f ) const;

protected:

	KeyFrame* _createKeyFrame( float time );

private:

	unsigned short mMeshId;

};

}

#endif // __zhMeshAnimationTrack_h__
