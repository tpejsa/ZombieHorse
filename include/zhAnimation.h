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

#ifndef __zhAnimation_h__
#define __zhAnimation_h__

#include "zhPrereq.h"
#include "zhIterators.h"
#include "zhAnimationSet.h"
#include "zhAnimationAnnotation.h"
#include "zhBoneAnimationTrack.h"

namespace zh
{

class Skeleton;

enum KFInterpolationMethod
{
	KFInterp_Linear,
	KFInterp_Spline
};

/**
* @brief Class representing a key-frame animation.
*/
class zhDeclSpec Animation
{

public:

	typedef MapIterator< std::map<unsigned short, BoneAnimationTrack*> > BoneTrackIterator;
	typedef MapConstIterator< std::map<unsigned short, BoneAnimationTrack*> > BoneTrackConstIterator;

	static const std::set<unsigned short> EmptyBoneMask;

	/**
	* Constructor.
	*
	* @param id Animation ID.
	* @param name Animation name.
	* @param animSet Pointer to the AnimationSet which owns this animation.
	*/
	Animation( unsigned short id, const std::string& name, AnimationSetPtr animSet );

	/**
	* Destructor.
	*/
	~Animation();

	/**
	* Gets the animation ID.
	*/
	unsigned short getId() const;

	/**
	* Gets the animation name.
	*/
	const std::string& getName() const;

	/**
	* Gets the fully qualified animation name.
	*/
	std::string getFullName() const;

	/**
	* Gets a pointer to the owning AnimationSet.
	*/
	AnimationSetPtr getAnimationSet() const;

	/**
	* Creates an animation track for the bone with the specified ID.
	*
	* @param boneId Bone ID.
	* @return Pointer to the animation track.
	*/
	BoneAnimationTrack* createBoneTrack( unsigned short boneId );

	/**
	* Deletes the specified bone animation track.
	*
	* @param boneId Bone ID.
	*/
	void deleteBoneTrack( unsigned short boneId );

	/**
	* Deletes all bone animation tracks in this animation.
	*/
	void deleteAllBoneTracks();

	/**
	* Returns true if the specified bone animation track exists,
	* otherwise false.
	*/
	bool hasBoneTrack( unsigned short boneId ) const;

	/**
	* Gets the specified bone animation track.
	*
	* @param boneId Bone ID.
	* @return Pointer to the animation track or
	* NULL if the track doesn't exist.
	*/
	BoneAnimationTrack* getBoneTrack( unsigned short boneId ) const;

	/**
	* Gets the number of bone animation tracks in this animation.
	*/
	unsigned int getNumBoneTracks() const;

	/**
	* Gets an iterator over the map of bone animation tracks.
	*/
	BoneTrackIterator getBoneTrackIterator();

	/**
	* Gets a const iterator over the map of bone animation tracks.
	*/
	BoneTrackConstIterator getBoneTrackConstIterator() const;

	/**
	* Gets the interpolation method used for key-frame values.
	*/
	KFInterpolationMethod getKFInterpolationMethod() const;

	/**
	* Sets the interpolation method used for key-frame values.
	*/
	void setKFInterpolationMethod( KFInterpolationMethod interpMethod );

	/**
	* Gets the length of this animation.
	*/
	float getLength() const;

	/**
	* Get the animation frame rate (valid only for uniform sampling
	* animations, like motion capture data).
	*/
	int getFrameRate() const;

	/**
	* Set the animation frame rate (valid only for uniform sampling
	* animations, like motion capture data).
	*/
	void setFrameRate( int frameRate );

	/**
	* Applies the animation to the specified skeleton.
	* 
	* @param skel Pointer to the Skeleton which should be updated
	* by this animation.
	* @param time Time at which this animation should be applied.
	* @param weight Blend weight with which this animation should be applied.
	* @param scale Scaling factor applied to this animation.
	* @param boneMask Bone mask. Animation is not applied to masked bones.
	*/
	void apply( Skeleton* skel, float time, float weight = 1.f, float scale = 1.f,
		const std::set<unsigned short> boneMask = EmptyBoneMask ) const;

	/**
	* Computes the axis-aligned bounding box of the root motion path in this
	* animation clip.
	*/
	void computeAnimationBounds( float& minX, float& maxX, float& minY, float& maxY,
		float& minZ, float& maxZ ) const;

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
	* Calculates the resource memory usage.
	*/
	size_t _calcMemoryUsage() const;

	/**
	* Creates a deep copy of the resource.
	*
	* @param clonePtr Pointer to the copy.
	*/
	void _clone( Animation* clonePtr ) const;

private:

	unsigned short mId;
	std::string mName;
	AnimationSetPtr mAnimSet;
	std::map<unsigned short, BoneAnimationTrack*> mBoneTracks;
	KFInterpolationMethod mInterpMethod;
	int mFrameRate;

	TransitionAnnotationContainer* mTransAnnots;
	ParamTransitionAnnotationContainer* mParamTransAnnots;
	PlantConstraintAnnotationContainer* mPlantConstrAnnots;
	SimEventAnnotationContainer* mSimEventAnnots;

};

}

#endif // __zhAnimation_h__
