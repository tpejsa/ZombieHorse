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

#include "zhSkeleton.h"

namespace zh
{

const Skeleton::Situation Skeleton::Situation::Identity = Skeleton::Situation();

Skeleton::Situation::Situation()
{
}

Skeleton::Situation::Situation( float posX, float posZ, float orientY )
{
	mPos = Vector3( posX, 0, posZ );
	mOrient = Quat( Vector3::YAxis, orientY );
}

Skeleton::Situation::Situation( const Vector3& pos, const Quat& orient )
: mPos(pos), mOrient(orient)
{
}

Vector3 Skeleton::Situation::getPosition() const
{
	return mPos;
}

void Skeleton::Situation::setPosition( const Vector3& pos )
{
	mPos = pos;
}

Quat Skeleton::Situation::getOrientation() const
{
	return mOrient;
}

void Skeleton::Situation::setOrientation( const Quat& orient )
{
	mOrient = orient;
}

float Skeleton::Situation::getPosX() const
{
	return mPos.x;
}

float Skeleton::Situation::getPosZ() const
{
	return mPos.z;
}

float Skeleton::Situation::getOrientY() const
{
	float ax, ay, az;
	mOrient.getEuler( ax, ay, az );
	
	return ay;
}

Skeleton::Situation& Skeleton::Situation::invert()
{
	mPos = -mPos;
	mOrient.invert();

	return *this;
}

Skeleton::Situation Skeleton::Situation::getInverse() const
{
	Situation sit = *this;
	sit.invert();

	return sit;
}

Skeleton::Situation& Skeleton::Situation::transform( const Situation& transf )
{
	mPos += transf.mPos.getRotated(mOrient);
	mOrient *= transf.mOrient;

	return *this;
}

Skeleton::Situation Skeleton::Situation::getTransformed( const Situation& transf ) const
{
	Situation sit = *this;
	sit.transform(transf);

	return sit;
}

Skeleton::Situation Skeleton::Situation::getTransformTo( const Situation& sit ) const
{
	Quat inv_orient = mOrient.getInverse();
	Situation transf;
	transf.mOrient = inv_orient * sit.mOrient;
	transf.mPos = sit.mPos - mPos;
	transf.mPos.rotate(inv_orient);

	return transf;
}

const Skeleton::Situation& Skeleton::Situation::projectToGround()
{
	mPos.y = 0;
	float ax, ay, az;
	mOrient.getEuler( ax, ay, az );
	mOrient = Quat( 0, ay, 0 );

	return *this;
}

Skeleton::Situation Skeleton::Situation::getProjToGround() const
{
	Situation sit = *this;
	sit.projectToGround();

	return sit;
}

Skeleton::Skeleton( const std::string& name ) : mName(name), mRoot(NULL)
{
}

Skeleton::~Skeleton()
{
	deleteAllBones();
}

const std::string& Skeleton::getName() const
{
	return mName;
}

Skeleton::Situation Skeleton::getSituation() const
{
	if( mRoot == NULL )
		return Skeleton::Situation();

	return Skeleton::Situation( mRoot->getPosition(), mRoot->getOrientation() )/*.getProjToGround()*/;
}

void Skeleton::setSituation( const Situation& sit )
{
	if( mRoot != NULL )
	{
		mRoot->setPosition( sit.getPosition() );
		mRoot->setOrientation( sit.getOrientation() );
	}
}

Bone* Skeleton::getRoot() const
{
	if( mRoot == NULL )
	{
		BoneConstIterator bone_i = getBoneConstIterator();
		while( !bone_i.end() )
		{
			if( bone_i.value()->getParent() == NULL )
			{
				mRoot = bone_i.value();
				break;
			}

			bone_i.next();
		}
	}

	return mRoot;
}

Bone* Skeleton::createBone( unsigned short id, const std::string& name )
{
	zhAssert( !hasBone(id) && !hasBone(name) );

	Bone* bone = new Bone( id, name );

	mBonesById[id] = bone;
	mBonesByName[name] = bone;

	mRoot = NULL;

	return bone;
}

void Skeleton::deleteBone( unsigned short id )
{
	Bone* bone = getBone(id);

	if( bone == NULL )
		return;

	// detach bone from the hierarchy
	if( bone->getParent() != NULL )
		bone->getParent()->removeChild(id);
	bone->removeAllChildren();
	if( bone == mRoot )
		mRoot = NULL;

	mBonesById.erase(id);
	mBonesByName.erase( bone->getName() );
	delete bone;
}

void Skeleton::deleteBone( const std::string& name )
{
	Bone* bone = getBone(name);

	if( bone == NULL )
		return;

	// detach bone from the hierarchy
	if( bone->getParent() != NULL )
		bone->getParent()->removeChild( bone->getId() );
	bone->removeAllChildren();
	if( bone == mRoot )
		mRoot = NULL;

	mBonesById.erase( bone->getId() );
	mBonesByName.erase(name);
	delete bone;
}

void Skeleton::deleteAllBones()
{
	for( std::map<unsigned short, Bone*>::iterator mi = mBonesById.begin();
		mi != mBonesById.end(); ++mi )
		delete mi->second;

	mBonesById.clear();
	mBonesByName.clear();
	mRoot = NULL;
}

bool Skeleton::hasBone( unsigned short id ) const
{
	return mBonesById.count(id) > 0;
}

bool Skeleton::hasBone( const std::string& name ) const
{
	return mBonesByName.count(name) > 0;
}

Bone* Skeleton::getBone( unsigned short id ) const
{
	std::map<unsigned short, Bone*>::const_iterator mi = mBonesById.find(id);

	if( mi != mBonesById.end() )
		return mi->second;

	return NULL;
}

Bone* Skeleton::getBone( const std::string& name ) const
{
	std::map<std::string, Bone*>::const_iterator mi = mBonesByName.find(name);

	if( mi != mBonesByName.end() )
		return mi->second;

	return NULL;
}

unsigned int Skeleton::getNumBones() const
{
	return mBonesById.size();
}

Skeleton::BoneIterator Skeleton::getBoneIterator()
{
	return BoneIterator( mBonesById );
}

Skeleton::BoneConstIterator Skeleton::getBoneConstIterator() const
{
	return BoneConstIterator( mBonesById );
}

void Skeleton::resetToInitialPose()
{
	BoneIterator bone_i = getBoneIterator();
	while( !bone_i.end() )
		bone_i.next()->resetToInitialPose();
}

}
