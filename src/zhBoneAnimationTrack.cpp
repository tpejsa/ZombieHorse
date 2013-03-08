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

#include "zhBoneAnimationTrack.h"
#include "zhSkeleton.h"
#include "zhAnimation.h"

namespace zh
{

TransformKeyFrame::TransformKeyFrame( float time, unsigned int index )
: KeyFrame( time, index )
{
	mScale = Vector3(1,1,1);
}

TransformKeyFrame::~TransformKeyFrame()
{
}

const Vector3& TransformKeyFrame::getTranslation() const
{
	return mTranslation;
}

void TransformKeyFrame::setTranslation( const Vector3& trans )
{
	mTranslation = trans;
}

const Quat& TransformKeyFrame::getRotation() const
{
	return mRotation;
}

void TransformKeyFrame::setRotation( const Quat& rot )
{
	mRotation = rot;
}

const Vector3& TransformKeyFrame::getScale() const
{
	return mScale;
}

void TransformKeyFrame::setScale( const Vector3& scal )
{
	mScale = scal;
}

BoneAnimationTrack::BoneAnimationTrack( unsigned short boneId, Animation* anim )
: AnimationTrack(anim), mBoneId(boneId)
{
}

BoneAnimationTrack::~BoneAnimationTrack()
{
}

unsigned short BoneAnimationTrack::getBoneId() const
{
	return mBoneId;
}

void BoneAnimationTrack::getInterpolatedKeyFrame( float time, KeyFrame* kf ) const
{
	zhAssert( kf != NULL );

	TransformKeyFrame* tkf = static_cast<TransformKeyFrame*>(kf);
	KeyFrame *kf1, *kf2;
	TransformKeyFrame *tkf1, *tkf2;

	// get nearest 2 key-frames
	float t = getKeyFramesAtTime( time, &kf1, &kf2 );
	tkf1 = static_cast<TransformKeyFrame*>(kf1);
	tkf2 = static_cast<TransformKeyFrame*>(kf2);

	// interpolate between them
	if( zhEqualf( t, 0 ) )
	{
		tkf->setTranslation( tkf1->getTranslation() );
		tkf->setRotation( tkf1->getRotation() );
		tkf->setScale( tkf1->getScale() );
	}
	else if( zhEqualf( t, 1 ) )
	{
		tkf->setTranslation( tkf2->getTranslation() );
		tkf->setRotation( tkf2->getRotation() );
		tkf->setScale( tkf2->getScale() );
	}
	else
	{
		Vector3 v1, v2;
		Quat q1, q2;

		// interpolate transformations
		if( mAnim->getKFInterpolationMethod() == KFInterp_Linear )
		{
			v1 = tkf1->getTranslation();
			v2 = tkf2->getTranslation();
			tkf->setTranslation( v1 + ( v2 - v1 ) * t );

			q1 = tkf1->getRotation();
			q2 = tkf2->getRotation();
			tkf->setRotation( q1.nlerp( q2, t ) );

			v1 = tkf1->getScale();
			v2 = tkf2->getScale();
			tkf->setScale( v1 + ( v2 - v1 ) * t );
		}
		else // if( mAnim->getKFInterpolationMethod() == KFInterp_Spline )
		{
			if( mTransSpline.getNumControlPoints() <= 0 )
				// interpolation splines not built yet, build them now
				_buildInterpSplines();

			tkf->setTranslation( mTransSpline.getPoint( tkf1->getIndex(), t ) );
			tkf->setRotation( mRotSpline.getPoint( tkf1->getIndex(), t ) );
			tkf->setScale( mScalSpline.getPoint( tkf1->getIndex(), t ) );
		}
	}
}

void BoneAnimationTrack::apply( Skeleton* skel, float time, float weight, float scale ) const
{
	TransformKeyFrame tkf( time, 0 );
	Bone* bone = skel->getBone(mBoneId);
	if( bone == NULL )
		// Skeleton doesn't contain a bone for this track
		return;

	getInterpolatedKeyFrame( time, &tkf );

	bone->translate( tkf.getTranslation() * weight * scale );
	bone->rotate( Quat().slerp( tkf.getRotation(), weight ) );
	bone->scale( Vector3(1,1,1) + ( Vector3(1,1,1) - tkf.getScale() ) * weight * scale );
}

KeyFrame* BoneAnimationTrack::_createKeyFrame( float time )
{
	return new TransformKeyFrame( time, 0 );
}

void BoneAnimationTrack::_buildInterpSplines() const
{
	TransformKeyFrame* tkf;

	for( unsigned int kfi = 0; kfi < mKeyFrames.size(); ++kfi )
	{
		tkf = static_cast<TransformKeyFrame*>( mKeyFrames[kfi] );
		mTransSpline.addControlPoint( tkf->getTranslation() );
		mRotSpline.addControlPoint( tkf->getRotation() );
		mScalSpline.addControlPoint( tkf->getScale() );
	}

	mTransSpline.calcTangents();
	mRotSpline.calcTangents();
	mScalSpline.calcTangents();
}

}
