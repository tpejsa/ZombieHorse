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

#include "zhBoneIKController.h"
#include "zhMath.h"

namespace zh
{

BoneIKController::BoneIKController()
{
	// create default solver
	mSolver = new CCDSolver();
	mSolver->setBoneId(mBoneId);
}

BoneIKController::~BoneIKController()
{
	if( mSolver != NULL )
		delete mSolver;
}

const Vector3& BoneIKController::getTargetPosition() const
{
	return mSolver->getTargetPosition();
}

void BoneIKController::setTargetPosition( const Vector3& pos ) const
{
	mSolver->setTargetPosition(pos);
}

const Quat& BoneIKController::getOffsetRotation() const
{
	return mOffsetRot;
}

void BoneIKController::setOffsetRotation( const Quat& rot )
{
	mOffsetRot = rot;
}

IKSolver* BoneIKController::getIKSolver() const
{
	return mSolver;
}

void BoneIKController::setIKSolver( IKSolverClass solver )
{
	Vector3 pos = mSolver->getTargetPosition();

	if( mSolver != NULL )
		delete mSolver;

	// if( solver == IKSolver_CCD )
	mSolver = new CCDSolver();
	
	mSolver->setBoneId(mBoneId);
	mSolver->setTargetPosition(pos);
}

void BoneIKController::_update( float dt )
{
}

void BoneIKController::_applyController( float weight ) const
{
	mSolver->solve( mOwner->getModel(), weight );
	// TODO: apply offset rotation
}

size_t BoneIKController::_calcMemoryUsage() const
{
	return 0;
}

void BoneIKController::_unload()
{
}

void BoneIKController::_clone( BoneController* clonePtr, bool shareData ) const
{
	zhAssert( clonePtr != NULL );
	zhAssert( getClassId() == clonePtr->getClassId() );

	BoneController::_clone( clonePtr, shareData );

	zhLog( "BoneIKController", "_clone",
		"Cloning BoneIKController %s %u, %s.",
		getClassName().c_str(), mId, mName.c_str() );

	BoneIKController* clone = static_cast<BoneIKController*>( clonePtr );

	clone->setBoneId( mBoneId );
	clone->setTargetPosition( mSolver->getTargetPosition() );
	clone->setIKSolver( mSolver->getClassId() );
}

}
