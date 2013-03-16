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
#include "zhLogger.h"
#include "zhAnimationSystem.h"

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

	Bone* bone = new Bone( id, name, this );

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

	// Detach bone from the hierarchy
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

	// Detach bone from the hierarchy
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

Bone* Skeleton::getBoneByTag( BoneTag tag ) const
{
	std::map<BoneTag, Bone*>::const_iterator bone_i = mBonesByTag.find(tag);
	
	if( bone_i != mBonesByTag.end() )
		return bone_i->second;

	return NULL;
}

bool Skeleton::hasBoneWithTag( BoneTag tag ) const
{
	return mBonesByTag.count(tag) > 0;
}

void Skeleton::findTagsOnBone( Bone* bone, std::vector<BoneTag>& tags ) const
{
	zhAssert( bone != NULL && hasBone(bone->getId()) );

	for( std::map<BoneTag, Bone*>::const_iterator bone_i = mBonesByTag.begin();
		bone_i != mBonesByTag.end(); ++bone_i )
	{
		if( bone_i->second->getId() == bone->getId() )
			tags.push_back(bone_i->first);
	}
}

IKSolver* Skeleton::createIKSolver( unsigned long classId, unsigned short id,
	const std::string& name, BoneTag startBone, BoneTag endBone )
{
	zhAssert( !hasIKSolver(id) && !hasIKSolver(name) &&
		hasBoneWithTag(startBone) && hasBoneWithTag(endBone) );

	// Construct IK chain
	Bone* sbone = getBoneByTag(startBone);
	Bone* ebone = getBoneByTag(endBone);
	std::vector<Bone*> chain;
	sbone->findChain( ebone, chain );
	
	zhAssert( !chain.empty() );

	// Create and initialize solver
	IKSolver* solver = zhAnimationSystem->_getIKSolverFactory().createObject( classId, id );
	solver->_init( name, chain, this );

	zhLog( "Skeleton", "createIKSolver",
		"Creating solver %s %u, %s on skeleton %s.",
		solver->getClassName().c_str(), solver->getId(), solver->getName().c_str(),
		mName.c_str() );

	mIKSolversById.insert( make_pair( id, solver ) );
	mIKSolversByName.insert( make_pair( name, solver ) );

	return solver;
}

void Skeleton::deleteIKSolver( unsigned short id )
{
	std::map<unsigned short, IKSolver*>::iterator iksi = mIKSolversById.find(id);

	if( iksi != mIKSolversById.end() )
	{
		IKSolver* solver = iksi->second;

		zhLog( "Skeleton", "deleteIKSolver",
			"Deleting solver %s %u, %s from skeleton %s.",
			solver->getClassName().c_str(), solver->getId(),
			mName.c_str() );

		mIKSolversById.erase(iksi);
		mIKSolversByName.erase( solver->getName() );
		delete solver;
	}
}

void Skeleton::deleteIKSolver( const std::string& name )
{
	std::map<std::string, IKSolver*>::iterator iksi = mIKSolversByName.find(name);

	if( iksi != mIKSolversByName.end() )
	{
		IKSolver* solver = iksi->second;

		zhLog( "Skeleton", "deleteIKSolver",
			"Deleting solver %s %u, %s from skeleton %s.",
			solver->getClassName().c_str(), solver->getId(), solver->getName().c_str(),
			mName.c_str() );

		mIKSolversById.erase( solver->getId() );
		mIKSolversByName.erase(name);
		delete solver;
	}
}

void Skeleton::deleteAllIKSolvers()
{
	zhLog( "Skeleton", "deleteAllIKSolvers",
		"Deleting all solvers from skeleton %s.",
		mName.c_str() );

	for( std::map<unsigned short, IKSolver*>::iterator iksi = mIKSolversById.begin();
		iksi != mIKSolversById.end(); ++iksi )
		delete iksi->second;

	mIKSolversById.clear();
	mIKSolversByName.clear();
}

bool Skeleton::hasIKSolver( unsigned short id ) const
{
	return mIKSolversById.count(id) > 0;
}

bool Skeleton::hasIKSolver( const std::string& name ) const
{
	return mIKSolversByName.count(name) > 0;
}

IKSolver* Skeleton::getIKSolver( unsigned short id ) const
{
	std::map<unsigned short, IKSolver*>::const_iterator iksi = mIKSolversById.find(id);

	if( iksi != mIKSolversById.end() )
		return iksi->second;

	return NULL;
}

IKSolver* Skeleton::getIKSolver( const std::string& name ) const
{
	std::map<std::string, IKSolver*>::const_iterator iksi = mIKSolversByName.find(name);

	if( iksi != mIKSolversByName.end() )
		return iksi->second;

	return NULL;
}

unsigned int Skeleton::getNumIKSolvers() const
{
	return mIKSolversById.size();
}

Skeleton::IKSolverIterator Skeleton::getIKSolverIterator()
{
	return IKSolverIterator(mIKSolversById);
}

Skeleton::IKSolverConstIterator Skeleton::getIKSolverConstIterator() const
{
	return IKSolverConstIterator(mIKSolversById);
}

void Skeleton::solveIK()
{
	// Get all solvers sorted by their priority
	std::multimap<unsigned short, IKSolver*> solvers;
	IKSolverIterator solver_i = getIKSolverIterator();
	while( solver_i.hasMore() )
	{
		IKSolver* solver = solver_i.next();
		solvers.insert( std::make_pair( solver->getPriority(), solver ) );
	}

	// Execute the solvers in correct order
	for( std::multimap<unsigned short, IKSolver*>::iterator psolver_i = solvers.begin();
		psolver_i != solvers.end(); ++psolver_i )
		psolver_i->second->solve();
}

void Skeleton::_clone( Skeleton* clonePtr ) const
{
	zhAssert( clonePtr != NULL );

	// Copy bones
	BoneConstIterator bone_i = getBoneConstIterator();
	while( bone_i.hasMore() )
	{
		Bone* bone = bone_i.next();
		Bone* cbone = clonePtr->createBone( bone->getId(), bone->getName() );
		cbone->setInitialPosition( bone->getInitialPosition() );
		cbone->setInitialOrientation( bone->getInitialOrientation() );
		cbone->setInitialScale( bone->getInitialScale() );
	}

	// Reconstruct bone hierarchy
	bone_i = getBoneConstIterator();
	while( bone_i.hasMore() )
	{
		Bone* bone = bone_i.next();
		Bone* cbone = clonePtr->getBone( bone->getId() );

		Bone* parent = bone->getParent();
		if( parent != NULL )
			clonePtr->getBone( parent->getId() )->addChild(cbone);
	}

	// Copy bone tags
	for( std::map<BoneTag, Bone*>::const_iterator bti = mBonesByTag.begin();
		bti != mBonesByTag.end(); ++bti )
		clonePtr->getBone( bti->second->getId() )->tag(bti->first);
}

void Skeleton::_addBoneTag( BoneTag tag, unsigned short boneId )
{
	zhAssert( hasBone(boneId) );

	mBonesByTag[tag] = getBone(boneId);
}

void Skeleton::_removeBoneTag( BoneTag tag )
{
	mBonesByTag.erase(tag);
}

void Skeleton::_removeBoneTagsFromBone( unsigned short boneId )
{
	std::vector<BoneTag> tags;
	for( std::map<BoneTag, Bone*>::iterator tag_i = mBonesByTag.begin();
		tag_i != mBonesByTag.end(); ++tag_i )
	{
		if( tag_i->second->getId() == boneId )
			tags.push_back(tag_i->first);
	}

	for( std::vector<BoneTag>::iterator tag_j = tags.begin(); tag_j != tags.end(); ++tag_j )
		_removeBoneTag(*tag_j);
}

void Skeleton::_removeAllBoneTags()
{
	mBonesByTag.clear();
}

}
