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

#include "zhAnimationController.h"
#include "zhLogger.h"
#include "zhModelController.h"

namespace zh
{

AnimationController::AnimationController()
: mCurAnimTree(NULL)
{
}

AnimationController::~AnimationController()
{
}

AnimationTreeInstance* AnimationController::addAnimationTree( AnimationTreePtr animTree )
{
	zhAssert( animTree != NULL );
	zhAssert( !hasAnimationTree( animTree->getId() ) );

	zhLog( "AnimationController", "addAnimationTree",
		"Creating new AnimationTreeInstance %u, %s on AnimationController %s on Character %s.",
		animTree->getId(), animTree->getName().c_str(), mId.c_str(), mChar->getId().c_str() );
		
	AnimationTreeInstance* ati = _createAnimTreeInst(animTree);
	mAnimTreesById.insert( make_pair( ati->getId(), ati ) );
	mAnimTreesByName.insert( make_pair( ati->getName(), ati ) );

	if( mCurAnimTree == NULL )
		mCurAnimTree = ati;

	return ati;
}

void AnimationController::removeAnimationTree( unsigned long id )
{
	AnimationTreeInstance* ati = getAnimationTree(id);

	if( ati != NULL )
	{
		zhLog( "AnimationController", "removeAnimationTree",
			"Removing AnimationTreeInstance %u, %s from AnimationController %s on Character %s.",
			ati->getId(), ati->getName().c_str(), mId.c_str(), mChar->getId().c_str() );

		if( ati == mCurAnimTree )
			mCurAnimTree = NULL;

		mAnimTreesById.erase(id);
		mAnimTreesByName.erase( ati->getName() );
		delete ati;
	}
}

void AnimationController::removeAnimationTree( const std::string& name )
{
	AnimationTreeInstance* ati = getAnimationTree(name);

	if( ati != NULL )
	{
		zhLog( "AnimationController", "removeAnimationTree",
			"Removing AnimationTreeInstance %u, %s from AnimationController %s on Character %s.",
			ati->getId(), ati->getName().c_str(), mId.c_str(), mChar->getId().c_str() );

		if( ati == mCurAnimTree )
			mCurAnimTree = NULL;

		mAnimTreesById.erase( ati->getId() );
		mAnimTreesByName.erase(name);
		delete ati;
	}
}

void AnimationController::removeAllAnimationTrees()
{
	zhLog( "AnimationController", "removeAllAnimationTrees",
			"Removing all AnimationTreeInstances from AnimationController %s on Character %s.",
			mId.c_str(), mChar->getId().c_str() );

	mCurAnimTree = NULL;

	for( std::map<unsigned long, AnimationTreeInstance*>::iterator atii = mAnimTreesById.begin();
		atii != mAnimTreesById.end(); ++atii )
		delete atii->second;

	mAnimTreesById.clear();
	mAnimTreesByName.clear();
}

bool AnimationController::hasAnimationTree( unsigned long id ) const
{
	return mAnimTreesById.count(id) > 0;
}

bool AnimationController::hasAnimationTree( const std::string& name ) const
{
	return mAnimTreesByName.count(name) > 0;
}

AnimationTreeInstance* AnimationController::getAnimationTree( unsigned long id ) const
{
	std::map<unsigned long, AnimationTreeInstance*>::const_iterator atii = mAnimTreesById.find(id);

	if( atii != mAnimTreesById.end() )
		return atii->second;

	return NULL;
}

AnimationTreeInstance* AnimationController::getAnimationTree( const std::string& name ) const
{
	std::map<std::string, AnimationTreeInstance*>::const_iterator atii = mAnimTreesByName.find(name);

	if( atii != mAnimTreesByName.end() )
		return atii->second;

	return NULL;
}

unsigned int AnimationController::getNumAnimationTrees() const
{
	return mAnimTreesById.size();
}

AnimationController::AnimationTreeIterator AnimationController::getAnimationTreeIterator()
{
	return AnimationTreeIterator( mAnimTreesById );
}

AnimationController::AnimationTreeConstIterator AnimationController::getAnimationTreeConstIterator() const
{
	return AnimationTreeConstIterator( mAnimTreesById );
}

AnimationTreeInstance* AnimationController::selectAnimationTree( unsigned long id )
{
	zhAssert( hasAnimationTree(id) );

	return mCurAnimTree = getAnimationTree(id);
}

AnimationTreeInstance* AnimationController::selectAnimationTree( const std::string& name )
{
	zhAssert( hasAnimationTree(name) );

	return mCurAnimTree = getAnimationTree(name);
}

void AnimationController::update( float dt )
{
	// apply active animation tree
	if( mCurAnimTree != NULL )
	{
		mCurAnimTree->update(dt);
		mCurAnimTree->apply();
	}
}

AnimationTreeInstance* AnimationController::_createAnimTreeInst( AnimationTreePtr animTree )
{
	zhAssert( animTree != NULL );

	zhLog( "AnimationTree", "_createAnimTreeInst",
		"Creating new instance of animation tree %u, %s.",
		animTree->getId(), animTree->getName().c_str() );

	AnimationTreeInstance* atree_inst = new AnimationTreeInstance( animTree, this );
	if( mChar->getModelController() != NULL )
		atree_inst->setModel( mChar->getModelController()->getModel() );

	return atree_inst;
}

}
