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

#include "zhParamAnimationBuilder.h"
#include "zhString.h"
#include "zhModel.h"
#include "zhAnimationSpace.h"

namespace zh
{

ParamAnimationBuilder::ParamAnimationBuilder( Model* mdl, AnimationSpace* animSpace )
: mMdl(mdl), mAnimSpace(animSpace)
{
	zhAssert( mdl != NULL && animSpace != NULL );
}

ParamAnimationBuilder::~ParamAnimationBuilder()
{
}

void ParamAnimationBuilder::parametrize( const std::vector<AnimationParamSpec>& paramSpecs )

{
	zhAssert( mAnimSpace->getNumBaseAnimations() > paramSpecs.size() );

	if( paramSpecs.empty() )
		return;

	Skeleton* skel = mMdl->getSkeleton();

	// create empty parametrization
	AnimationParametrization* animparam = mAnimSpace->createParametrization(
		getClassId(), paramSpecs.size(), mAnimSpace->getNumBaseAnimations() );

	// extract param. values for each base anim.
	for( unsigned int param_i = 0; param_i < paramSpecs.size(); ++param_i )
	{
		zhAssert( skel->hasBone( paramSpecs[param_i].getBoneId() ) &&
			skel->hasBone( paramSpecs[param_i].getSuperBoneId() )
			);

		Bone *bone = skel->getBone( paramSpecs[param_i].getBoneId() ),
			*superbone = skel->getBone( paramSpecs[param_i].getSuperBoneId() );

		animparam->setParam( param_i, paramSpecs[param_i].getParamName() );

		// determine query anim. index
		unsigned int qanim_i = 0;
		for( unsigned int anim_i = 0; anim_i < mAnimSpace->getNumBaseAnimations(); ++anim_i )
		{
			if( mAnimSpace->getBaseAnimation(anim_i) == paramSpecs[param_i].getBaseAnimation() )
			{
				qanim_i = anim_i;
				break;
			}
		}

		// get param. sample times in query anim.
		float time0 = paramSpecs[param_i].getTime();
		if( time0 < 0 )
			time0 = 0;
		else if( time0 > paramSpecs[param_i].getBaseAnimation()->getLength() )
			time0 = paramSpecs[param_i].getBaseAnimation()->getLength();

		// compute param. sample times in all base anims
		Vector param_times( mAnimSpace->getNumBaseAnimations() );
		if( mAnimSpace->hasTimewarpCurve() )
		{
			const CatmullRomSpline<Vector>& tw_curve = mAnimSpace->getTimewarpCurve();
			float u = time0 / mAnimSpace->getBaseAnimation(qanim_i)->getLength() * ( tw_curve.getNumControlPoints() - 1 ); // TW curve param. value
			unsigned int cpi = (unsigned int)u;
			float t = u - cpi;
			
			param_times = tw_curve.getPoint( cpi, t );
		}
		else
		{
			for( unsigned int banim_i = 0; banim_i < mAnimSpace->getNumBaseAnimations(); ++banim_i )
			{
				Animation* banim = mAnimSpace->getBaseAnimation(banim_i);

				param_times[banim_i] = time0 / paramSpecs[param_i].getBaseAnimation()->getLength() *
					banim->getLength();
			}
		}

		// apply query base anim. to model at 0 time
		skel->resetToInitialPose();
		mAnimSpace->getBaseAnimation(qanim_i)->apply( mMdl, 0, 1, 1, Animation::EmptyBoneMask );

		// get transf.'s
		Vector3 bpos = superbone->getWorldPosition();
		Quat borient = superbone->getWorldOrientation();
		Vector3 bscal = superbone->getWorldScale();

		// compute param. values for all base anims
		for( unsigned int banim_i = 0; banim_i < mAnimSpace->getNumBaseAnimations(); ++banim_i )
		{
			Animation* banim = mAnimSpace->getBaseAnimation(banim_i);

			// apply base anim. to model at the param. sample time
			skel->resetToInitialPose();
			banim->apply( mMdl, param_times[banim_i], 1, 1, Animation::EmptyBoneMask );

			// extract param. values
			float param = 0;
			if( paramSpecs[param_i].getTransfType() == AnimationParamSpec::TransfType_Translation )
			{
				Vector3 trans = bone->getWorldPosition() - bpos;
				trans = trans.rotate( borient.getInverse() );
				trans = trans * Vector3( bscal.x, bscal.y, bscal.z );

				if( paramSpecs[param_i].getAxis() == AnimationParamSpec::Axis_x )
					param = trans.x;
				else if( paramSpecs[param_i].getAxis() == AnimationParamSpec::Axis_y )
					param = trans.y;
				else // if( paramSpecs[param_i].getAxis() == AnimationParamSpec::Axis_z )
					param = trans.z;
			}
			else if( paramSpecs[param_i].getTransfType() == AnimationParamSpec::TransfType_Rotation )
			{
				Quat rot = borient.getInverse() * bone->getWorldOrientation();
				float ax, ay, az;
				rot.getEuler( ax, ay, az );

				if( paramSpecs[param_i].getAxis() == AnimationParamSpec::Axis_x )
					param = ax;
				else if( paramSpecs[param_i].getAxis() == AnimationParamSpec::Axis_y )
					param = ay;
				else // if( paramSpecs[param_i].getAxis() == AnimationParamSpec::Axis_z )
					param = az;
			}

			// update base sample param. value in the parametrization
			Vector params = animparam->getBaseSample(banim_i);
			params[param_i] = param;
			animparam->setBaseSample( banim_i, params );
		}
	}
	
	skel->resetToInitialPose();

	// generate the parametrization
	_buildParametrization();
}

}
