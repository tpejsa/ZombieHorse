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

#include "zhBone.h"
#include "zhSkeleton.h"

namespace zh
{

Bone::Bone( unsigned short id, const std::string& name, Skeleton* skel ) :
mId(id), mName(name), mSkel(skel), mParent(NULL)
{
	zhAssert( skel != NULL );

	mInitPos = Vector3( 0, 0, 0 );
	mInitOrient = Quat( 0, 0, 0 );
	mInitScal = Vector3( 1, 1, 1 );
	mPos = mInitPos;
	mOrient = mInitOrient;
	mScal = mInitScal;
}

Bone::~Bone()
{
}

Skeleton* Bone::getSkeleton() const
{
	return mSkel;
}

const Vector3& Bone::getPosition() const
{
	return mPos;
}

void Bone::setPosition( const Vector3& pos )
{
	mPos = pos;
}

Vector3 Bone::getWorldPosition() const
{
	Vector3 wpos;
	
	if( mParent == NULL )
	{
		wpos = mPos;
	}
	else
	{
		wpos = mParent->getWorldPosition() +
			( mParent->getWorldScale() * mPos ).rotate( mParent->getWorldOrientation() );
	}

	return wpos;
}

const Vector3& Bone::getInitialPosition() const
{
	return mInitPos;
}

void Bone::setInitialPosition( const Vector3& pos )
{
	mInitPos = pos;
}

const Quat& Bone::getOrientation() const
{
	return mOrient;
}

void Bone::setOrientation( const Quat& orient )
{
	mOrient = orient;
}

Quat Bone::getWorldOrientation() const
{
	Quat worient;

	if( mParent == NULL )
	{
		worient = mOrient;
	}
	else
	{
		worient = mParent->getWorldOrientation() * mOrient;
	}

	return worient;
}

const Quat& Bone::getInitialOrientation() const
{
	return mInitOrient;
}

void Bone::setInitialOrientation( const Quat& orient )
{
	mInitOrient = orient;
}

const Vector3& Bone::getScale() const
{
	return mScal;
}

void Bone::setScale( const Vector3& scal )
{
	mScal = scal;
}

Vector3 Bone::getWorldScale() const
{
	Vector3 wscal;

	if( mParent == NULL )
	{
		wscal = mScal;
	}
	else
	{
		wscal = mParent->getWorldScale() * mScal;
	}

	return wscal;
}

const Vector3& Bone::getInitialScale() const
{
	return mInitScal;
}

void Bone::setInitialScale( const Vector3& scal )
{
	mInitScal = scal;
}

void Bone::resetToInitialPose()
{
	mPos = mInitPos;
	mOrient = mInitOrient;
	mScal = mInitScal;
}

void Bone::translate( const Vector3& trans )
{
	mPos += trans;
}

void Bone::rotate( const Quat& rot )
{
	mOrient *= rot.getNormalized();
}

void Bone::rotate( float yaw, float pitch, float roll )
{
	rotate( Quat( yaw, pitch, roll ) );
}

void Bone::scale( const Vector3& scal )
{
	mScal *= scal;
}

Bone* Bone::getParent() const
{
	return mParent;
}

void Bone::addChild( Bone* bone )
{
	zhAssert( bone != NULL );

	if( hasChild( bone->getId() ) )
		return;

	bone->mParent = this;
	mChildrenById.insert( std::make_pair( bone->getId(), bone ) );
	mChildrenByName.insert( std::make_pair( bone->getName(), bone ) );
}

void Bone::removeChild( unsigned short childId )
{
	std::map<unsigned short, Bone*>::iterator ci = mChildrenById.find(childId);
	
	if( ci != mChildrenById.end() )
	{
		Bone* child = ci->second;

		mChildrenById.erase(ci);
		mChildrenByName.erase( child->getName() );
		child->mParent = NULL;
	}
}

void Bone::removeChild( const std::string& childName )
{
	std::map<std::string, Bone*>::iterator ci = mChildrenByName.find(childName);
	
	if( ci != mChildrenByName.end() )
	{
		Bone* child = ci->second;

		mChildrenByName.erase(ci);
		mChildrenById.erase( child->getId() );
		child->mParent = NULL;
	}
}

void Bone::removeAllChildren()
{
	for( std::map<unsigned short, Bone*>::iterator ci = mChildrenById.begin();
		ci != mChildrenById.end(); ++ci )
	{
		ci->second->mParent = NULL;
	}
	
	mChildrenById.clear();
	mChildrenByName.clear();
}

void Bone::moveChild( unsigned short childId, Bone* bone )
{
	zhAssert( hasChild(childId) );
	zhAssert( bone != NULL );

	std::map<unsigned short, Bone*>::iterator ci = mChildrenById.find(childId);
	Bone* child = ci->second;

	mChildrenById.erase(ci);
	mChildrenByName.erase( child->getName() );
	bone->addChild(child);
}

void Bone::moveChild( const std::string& childName, Bone* bone )
{
	zhAssert( hasChild(childName) );
	zhAssert( bone != NULL );

	std::map<std::string, Bone*>::iterator ci = mChildrenByName.find(childName);
	Bone* child = ci->second;

	mChildrenByName.erase(ci);
	mChildrenById.erase( child->getId() );
	bone->addChild(child);
}

bool Bone::hasChild( unsigned short childId ) const
{
	return mChildrenById.count(childId) != 0;
}

bool Bone::hasChild( const std::string& childName ) const
{
	return mChildrenByName.count(childName) != 0;
}

Bone* Bone::getChild( unsigned short childId ) const
{
	std::map<unsigned short, Bone*>::const_iterator ci = mChildrenById.find(childId);

	if( ci != mChildrenById.end() )
		return ci->second;

	return NULL;
}

Bone* Bone::getChild( const std::string& childName ) const
{
	std::map<std::string, Bone*>::const_iterator ci = mChildrenByName.find(childName);

	if( ci != mChildrenByName.end() )
		return ci->second;

	return NULL;
}

unsigned int Bone::getNumChildren() const
{
	return mChildrenById.size();
}

Bone::ChildIterator Bone::getChildIterator()
{
	return ChildIterator(mChildrenById);
}

Bone::ChildConstIterator Bone::getChildConstIterator() const
{
	return ChildConstIterator(mChildrenById);
}

bool Bone::findChain( Bone* endBone, std::vector<Bone*>& chain )
{
	zhAssert( endBone != NULL );

	if( this == endBone )
	{
		chain.insert( chain.begin(), this );
		return true;
	}

	Bone::ChildConstIterator child_i = getChildConstIterator();
	while( child_i.hasMore() )
	{
		Bone* child = child_i.next();

		if( child->findChain( endBone, chain ) )
		{
			chain.insert( chain.begin(), this );
			return true;
		}
	}

	return false;
}

void Bone::tag( BoneTag tag )
{
	mSkel->_addBoneTag( tag, mId );
}

void Bone::untag()
{
	mSkel->_removeBoneTagsFromBone(mId);
}

}
