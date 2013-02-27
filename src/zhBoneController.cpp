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

#include "zhBoneController.h"

namespace zh
{

BoneController::BoneController()
: mName(""), mOwner(NULL), mPrev(NULL), mNext(NULL),
mBoneId(0), mEnabled(false), mRespTime(0.3f), mWeight(0)
{
}

BoneController::~BoneController()
{
}

const std::string& BoneController::getName() const
{
	return mName;
}

void BoneController::setName( const std::string& name )
{
	mOwner->_renameBoneController( this, name );
}

AnimationTree* BoneController::getAnimationTree() const
{
	return mOwner;
}

BoneController* BoneController::getPrevious() const
{
	return mPrev;
}

BoneController* BoneController::getNext() const
{
	return mNext;
}

void BoneController::setNext( BoneController* next )
{
	if( mNext != NULL )
		mNext->mPrev = NULL;

	mNext = next;

	if( next != NULL )
	{
		if( next->getPrevious() != NULL )
			next->getPrevious()->setNext(NULL);
		
		next->mPrev = this;
	}

	if( mOwner->getFirst() == next )
		mOwner->setFirst((BoneController*)NULL);
}

unsigned short BoneController::getBoneId() const
{
	return mBoneId;
}

void BoneController::setBoneId( unsigned short id )
{
	mBoneId = id;
}

bool BoneController::getEnabled() const
{
	return mEnabled;
}

void BoneController::setEnabled( bool enabled )
{
	mEnabled = enabled;
	mWeight = 0;
}

float BoneController::getResponseTime() const
{
	return mRespTime;
}

void BoneController::setResponseTime( float time )
{
	mRespTime = time > 0 ? time : 0;
}

void BoneController::update( float dt )
{
	if( mEnabled )
	{
		if( mRespTime > 0 )
		{
			mWeight += dt/mRespTime;
			if( mWeight > 1 ) mWeight = 1;
		}
		else
		{
			mWeight = 1;
		}

		_update(dt);
	}
	else
	{
		if( mRespTime > 0 )
		{
			mWeight -= dt/mRespTime;
			if( mWeight < 0 ) mWeight = 0;
		}
		else
		{
			mWeight = 0;
		}
	}

	if( mNext != NULL )
		mNext->update(dt);
}

void BoneController::_update( float dt )
{
}

void BoneController::apply() const
{
	if( mWeight > 0 )
		_applyController(mWeight);

	if( mNext != NULL )
		mNext->apply();
}

void BoneController::_applyController( float weight ) const
{
}

size_t BoneController::_calcMemoryUsage() const
{
	return 0;
}

void BoneController::_unload()
{
}

void BoneController::_clone( BoneController* clonePtr, bool shareData ) const
{
	clonePtr->mBoneId = mBoneId;
	clonePtr->mEnabled = mEnabled;

	if( mNext != NULL )
		clonePtr->setNext( clonePtr->getAnimationTree()->getBoneController( mNext->getId() ) );
}

void BoneController::_setAnimationTree( AnimationTree* treePtr )
{
	zhAssert( treePtr != NULL );

	mOwner = treePtr;
}

}
