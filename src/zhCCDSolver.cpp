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

#include "zhCCDSolver.h"
#include "zhLogger.h"

namespace zh
{

CCDSolver::CCDSolver()
: mChainLength(2), mMaxAngleChange(zhPI2), mNumIters(zhIK_CCDNumIters)
{
}

CCDSolver::~CCDSolver()
{
}

unsigned int CCDSolver::getChainLength() const
{
	return mChainLength;
}

void CCDSolver::setChainLength( unsigned int length )
{
	mChainLength = length;
}

float CCDSolver::getMaxAngleChange() const
{
	return mMaxAngleChange;
}

void CCDSolver::setMaxAngleChange( float change )
{
	mMaxAngleChange = change;
}

unsigned int CCDSolver::getNumIterations() const
{
	return mNumIters;
}

void CCDSolver::setNumIterations( unsigned int numIters )
{
	mNumIters = numIters;
}

void CCDSolver::solve( Model* mdl, float weight )
{
	zhAssert( mdl != NULL );
	Skeleton* skel = mdl->getSkeleton();
	zhAssert( skel->hasBone(mBoneId) );

	Bone* ebone = skel->getBone(mBoneId);

	for( unsigned int i = 0; i < mNumIters; ++i )
	{
		unsigned int link_ct = 0;
		Bone* bone = ebone;

		while( link_ct != mChainLength )
		{
			++link_ct;
			bone = bone->getParent();
			if( bone == NULL )
				break;

			Vector3 pj = bone->getWorldPosition(); // bone position
			Vector3 pe = ebone->getWorldPosition(); // end-effector position

			if( pe == mTargetPos )
				// target already reached, bail
				return;

			if( pj == pe || pj == mTargetPos )
				// bone has zero length, skip it
				continue;

			Vector3 be = pe - pj; // bone to end-effector vector
			be.normalize();
			Vector3 bt = mTargetPos - pj; // bone to target vector
			bt.normalize();

			// compute rotation in world space
			Vector3 axis( 1, 0, 0 );
			float angle = 0;
			float dot_bebt = be.dot(bt);
			if( zhEqualf( dot_bebt, -1.f ) )
			{
				axis = Vector3::XAxis.cross(be);
				if( zhEqualf( axis.lengthSq(), 0 ) )
					axis = Vector3::YAxis.cross(be);
				axis.normalize();
				angle = zhPI;
			}
			else if( dot_bebt < 1.f )
			{
				axis = be.cross(bt);
				angle = be.angle(bt);
			}
			
			// constrain angle
			while( fabs(angle) > zhPI ) angle -= ( angle > 0 ? zh2PI : -zh2PI );
			if( fabs(angle) > mMaxAngleChange )
				angle = angle > 0 ? mMaxAngleChange : -mMaxAngleChange;

			// compute and apply final (local) rotation
			if( !zhEqualf( angle, 0 ) )
			{
				axis.rotate( bone->getWorldOrientation() );
				Quat rot( axis, angle );

				bone->rotate( Quat().slerp( rot, weight ) );
			}
		}
	}
}

}
