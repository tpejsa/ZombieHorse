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

#include "zhPlantConstrDetector.h"
#include "zhSkeleton.h"
#include "zhAnimation.h"
//#include "zhCCDSolver.h"
#include "zhAnimationSegment.h"

namespace zh
{

PlantConstrDetector::PlantConstrDetector( Skeleton* skel, Animation* anim )
: mSkel(skel), mAnim(anim), mMaxPosChange(1.f), mMinConstrLength(0.1f)//, mSolver(NULL)
{
	zhAssert( skel != NULL );
	zhAssert( anim != NULL );

	/*setIKSolver(IKSolver_CCD);
	static_cast<CCDSolver*>(mSolver)->setMaxAngleChange(zhPI4/2);
	static_cast<CCDSolver*>(mSolver)->setNumIterations(50);*/
}

PlantConstrDetector::~PlantConstrDetector()
{
	//if( mSolver != NULL ) delete mSolver;
}

void PlantConstrDetector::detect( unsigned short boneId )
{
	zhAssert( mSkel->hasBone(boneId) );
	Bone* bone = mSkel->getBone(boneId);
	unsigned int num_samples = (unsigned int)( mAnim->getLength() * zhAnimation_SampleRate ) + 1; // number of samples in the anim.
	float dt = 1.f / zhAnimation_SampleRate; // offset between samples

	float stime = 0, etime; // constr. start and end times
	mSkel->resetToInitialPose();
	mAnim->apply( mSkel, 0, 1, 1, Animation::EmptyBoneMask );
	Vector3 last_pos = bone->getWorldPosition(); // bone position in last frame
	bool bone_stat = true; // flags indicating if the bone is stationary, and if it has been such long enough for there to be a constraint
	for( unsigned int si = 0; si < num_samples; ++si )
	{
		float t = si * dt;
		mSkel->resetToInitialPose();
		mAnim->apply( mSkel, t, 1, 1, Animation::EmptyBoneMask );

		// get bone position on current frame
		Vector3 pos = bone->getWorldPosition();
		float dpos = pos.distance(last_pos);

		if( dpos <= mMaxPosChange )
		{
			// position unchanged, bone is stationary

			if( !bone_stat )
				stime = t;

			etime = t;
			bone_stat = true;
		}

		if( dpos > mMaxPosChange || si >= num_samples - 1 )
		{
			// position changed (bone is no longer stationary),
			// or end of anim. reached

			if( bone_stat && etime - stime >= mMinConstrLength )
			{
				// bone was stationary sufficiently long,
				// annotate plant constraint on anim.
				_createPlantConstr( stime, etime, boneId );
			}

			bone_stat = false;
		}

		last_pos = pos;
	}

	mSkel->resetToInitialPose();
}

void PlantConstrDetector::detect( const std::string& boneName )
{
	zhAssert( mSkel->hasBone(boneName) );

	detect( mSkel->getBone(boneName)->getId() );
}

/*IKSolver* PlantConstrDetector::getIKSolver() const
{
	return mSolver;
}

void PlantConstrDetector::setIKSolver( IKSolverClass solver )
{
	if( mSolver != NULL ) delete mSolver;

	mSolver = new CCDSolver();
}

void PlantConstrDetector::cleanup()
{
	float dt = 1.f / zhAnimation_SampleRate; // offset between samples

	PlantConstraintAnnotationContainer* annots = mAnim->getPlantConstraintAnnotations();
	for( unsigned int annot_i = 0; annot_i < annots->getNumAnnotations(); ++annot_i )
	{
		PlantConstraintAnnotation* annot = static_cast<PlantConstraintAnnotation*>( annots->getAnnotation(annot_i) );
		Bone* bone = mSkel->getBone( annot->getBoneId() );

		unsigned int num_samples = (unsigned int)( ( annot->getEndTime() - annot->getStartTime() ) * zhAnimation_SampleRate ); // number of samples in the anim. segment
		std::vector<float> kftimes;
		std::map< unsigned short, std::vector<Vector3> > kftrans;
		std::map< unsigned short, std::vector<Quat> > kfrot;
		std::map< unsigned short, std::vector<Vector3> > kfscal;

		skel->resetToInitialPose();
		mAnim->apply( mSkel, annot->getStartTime(), 1, 1, Animation::EmptyBoneMask );

		// initialize IK solver
		mSolver->setBoneId( bone->getId() );
		Vector3 pos = bone->getWorldPosition();
		mSolver->setTargetPosition(pos);

		// compute modified key-frame data
		for( unsigned int si = 0; si < num_samples; ++si )
		{
			float t = annot->getStartTime() + si * dt;
			kftimes.push_back(t);
			
			skel->resetToInitialPose();
			mAnim->apply( mSkel, t, 1, 1, Animation::EmptyBoneMask );
			mSolver->solve(mSkel);

			// TODO: how to make foot/hand position parallel to surface?

			// extract key-frames from solved pose
			Bone* pbone = bone;
			for( unsigned int bone_i = 0; bone_i <= mSolver->getChainLength(); ++bone_i )
			{
				kftrans[ pbone->getId() ].push_back( pbone->getPosition() - pbone->getInitialPosition() );
				kfrot[ pbone->getId() ].push_back( pbone->getInitialOrientation().getInverse() * pbone->getOrientation() );
				kfscal[ pbone->getId() ].push_back( pbone->getScale() );

				if( bone->getParent() == NULL )
					break;
				pbone = pbone->getParent();
			}
		}

		// create new key-frames on affected bone tracks
		Bone* pbone = bone;
		for( unsigned int bone_i = 0; bone_i <= mSolver->getChainLength(); ++bone_i )
		{
			BoneAnimationTrack* bat = mAnim->getBoneTrack( pbone->getId() );
			if( bat == NULL )
				mAnim->createBoneTrack( pbone->getId() );

			// delete old key-frames
			for( unsigned int kfi = 0; kfi < bat->getNumKeyFrames(); ++kfi )
			{
				KeyFrame* kf = bat->getKeyFrame(kfi);

				if( kf->getTime() > annot->getEndTime() )
					break;
				
				if( kf->getTime() >= annot->getStartTime() && kf->getTime() <= annot->getEndTime() )
				{
					bat->deleteKeyFrame(kfi);
					--kfi;
				}
			}

			// create new key-frames
			for( unsigned int kfi = 0; kfi < kftimes.size(); ++kfi )
			{
				TransformKeyFrame* tkf = static_cast<TransformKeyFrame*>( bat->createKeyFrame( kftimes[kfi] ) );
				tkf->setTranslation( kftrans[ pbone->getId() ][kfi] );
				tkf->setRotation( kfrot[ pbone->getId() ][kfi] );
				tkf->setScale( kfscal[ pbone->getId() ][kfi] );
			}

			if( bone->getParent() == NULL )
				break;
			pbone = pbone->getParent();
		}
	}
}*/

void PlantConstrDetector::_createPlantConstr( float startTime, float endTime, unsigned short boneId )
{
	// create new plant constraint, but make sure overlapping annotations are merged
	AnimationSegment new_annotseg( mAnim, startTime, endTime );
	PlantConstraintAnnotationContainer* annots = mAnim->getPlantConstraintAnnotations();
	for( unsigned int annot_i = 0; annot_i < annots->getNumAnnotations(); ++annot_i )
	{
		PlantConstraintAnnotation* annot = static_cast<PlantConstraintAnnotation*>(
			annots->getAnnotation(annot_i) );

		if( annot->getBoneId() != boneId )
			continue;

		AnimationSegment annotseg( mAnim, annot->getStartTime(), annot->getEndTime() );
		if( annotseg.overlap(new_annotseg) > 0 )
		{
			new_annotseg = new_annotseg.merge(annotseg);
			annots->deleteAnnotation(annot_i);
			--annot_i;
		}
	}

	// create new (merged) annotation
	PlantConstraintAnnotation* annot = static_cast<PlantConstraintAnnotation*>(
		annots->createAnnotation( new_annotseg.getStartTime(), new_annotseg.getEndTime() ) );
	annot->setBoneId(boneId);
}

}
