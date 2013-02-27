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

#include "zhAnimationSet.h"
#include "zhAnimation.h"
#include "zhAnimationSpace.h"

namespace zh
{

AnimationSet::AnimationSet( unsigned long id, const std::string& name, ResourceManager* mgr )
: Resource( id, name, mgr )
{
}

AnimationSet::~AnimationSet()
{
	deleteAllAnimations();
}

Animation* AnimationSet::createAnimation( unsigned short id, const std::string& name )
{
	zhAssert( !hasAnimation(id) && !hasAnimation(name) );

	Animation* anim = new Animation( id, name, AnimationSetPtr::DynamicCast<Resource>( getPtr() ) );
	mAnimsById.insert( make_pair( id, anim ) );
	mAnimsByName.insert( make_pair( name, anim ) );

	zhLog( "AnimationSet", "createAnimation",
		"Creating Animation %u, %s in AnimationSet %u, %s.",
		id, name.c_str(), mId, mName.c_str() );

	return anim;
}

void AnimationSet::deleteAnimation( unsigned short id )
{
	std::map<unsigned short, Animation*>::iterator ai = mAnimsById.find(id);

	if( ai != mAnimsById.end() )
	{
		zhLog( "AnimationSet", "deleteAnimation",
			"Deleting Animation %u, %s from AnimationSet %u, %s.",
			ai->second->getId(), ai->second->getName().c_str(), mId, mName.c_str() );

		mAnimsByName.erase( ai->second->getName() );
		delete ai->second;
		mAnimsById.erase(ai);
	}
}

void AnimationSet::deleteAnimation( const std::string& name )
{
	Animation* anim = getAnimation(name);

	if( anim != NULL )
		deleteAnimation( anim->getId() );
}

void AnimationSet::deleteAllAnimations()
{
	zhLog( "AnimationSet", "deleteAllAnimations",
		"Deleting all Animations from AnimationSet %u, %s.",
		mId, mName.c_str() );

	for( std::map<unsigned short, Animation*>::iterator ai = mAnimsById.begin();
		ai != mAnimsById.end(); ++ai )
	{
		delete ai->second;
	}

	mAnimsById.clear();
	mAnimsByName.clear();
}

bool AnimationSet::hasAnimation( unsigned short id ) const
{
	return mAnimsById.count(id) > 0;
}

bool AnimationSet::hasAnimation( const std::string& name ) const
{
	return mAnimsByName.count(name) > 0;
}

Animation* AnimationSet::getAnimation( unsigned short id ) const
{
	std::map<unsigned short, Animation*>::const_iterator ai = mAnimsById.find(id);

	if( ai != mAnimsById.end() )
		return ai->second;

	return NULL;
}

Animation* AnimationSet::getAnimation( const std::string& name ) const
{
	std::map<std::string, Animation*>::const_iterator ai = mAnimsByName.find(name);

	if( ai != mAnimsByName.end() )
		return ai->second;

	return NULL;
}

AnimationSet::AnimationIterator AnimationSet::getAnimationIterator()
{
	return AnimationIterator( mAnimsById );
}

AnimationSet::AnimationConstIterator AnimationSet::getAnimationConstIterator() const
{
	return AnimationConstIterator( mAnimsById );
}

unsigned int AnimationSet::getNumAnimations() const
{
	return mAnimsById.size();
}

AnimationSpace* AnimationSet::createAnimationSpace( unsigned short id, const std::string& name )
{
	zhAssert( !hasAnimationSpace(id) && !hasAnimationSpace(name) );

	AnimationSpace* anim_space = new AnimationSpace( id, name, AnimationSetPtr::DynamicCast<Resource>( getPtr() ) );
	mAnimSpacesById.insert( make_pair( id, anim_space ) );
	mAnimSpacesByName.insert( make_pair( name, anim_space ) );

	zhLog( "AnimationSet", "createAnimationSpace",
		"Creating AnimationSpace %u, %s in AnimationSet %u, %s.",
		id, name.c_str(), mId, mName.c_str() );

	return anim_space;
}

void AnimationSet::deleteAnimationSpace( unsigned short id )
{
	std::map<unsigned short, AnimationSpace*>::iterator asi = mAnimSpacesById.find(id);

	if( asi != mAnimSpacesById.end() )
	{
		zhLog( "AnimationSet", "deleteAnimationSpace",
			"Deleting AnimationSpace %u, %s from AnimationSet %u, %s.",
			asi->second->getId(), asi->second->getName().c_str(), mId, mName.c_str() );

		mAnimSpacesByName.erase( asi->second->getName() );
		delete asi->second;
		mAnimSpacesById.erase(asi);
	}
}

void AnimationSet::deleteAnimationSpace( const std::string& name )
{
	AnimationSpace* anim_space = getAnimationSpace(name);

	if( anim_space != NULL )
		deleteAnimationSpace( anim_space->getId() );
}

void AnimationSet::deleteAllAnimationSpaces()
{
	zhLog( "AnimationSet", "deleteAllAnimationSpaces",
		"Deleting all AnimationSpaces from AnimationSet %u, %s.",
		mId, mName.c_str() );

	for( std::map<unsigned short, AnimationSpace*>::iterator asi = mAnimSpacesById.begin();
		asi != mAnimSpacesById.end(); ++asi )
	{
		delete asi->second;
	}

	mAnimSpacesById.clear();
	mAnimSpacesByName.clear();
}

bool AnimationSet::hasAnimationSpace( unsigned short id ) const
{
	return mAnimSpacesById.count(id) > 0;
}

bool AnimationSet::hasAnimationSpace( const std::string& name ) const
{
	return mAnimSpacesByName.count(name) > 0;
}

AnimationSpace* AnimationSet::getAnimationSpace( unsigned short id ) const
{
	std::map<unsigned short, AnimationSpace*>::const_iterator asi = mAnimSpacesById.find(id);

	if( asi != mAnimSpacesById.end() )
		return asi->second;

	return NULL;
}

AnimationSpace* AnimationSet::getAnimationSpace( const std::string& name ) const
{
	std::map<std::string, AnimationSpace*>::const_iterator asi = mAnimSpacesByName.find(name);

	if( asi != mAnimSpacesByName.end() )
		return asi->second;

	return NULL;
}

AnimationSet::AnimationSpaceIterator AnimationSet::getAnimationSpaceIterator()
{
	return AnimationSpaceIterator( mAnimSpacesById );
}

AnimationSet::AnimationSpaceConstIterator AnimationSet::getAnimationSpaceConstIterator() const
{
	return AnimationSpaceConstIterator( mAnimSpacesById );
}

unsigned int AnimationSet::getNumAnimationSpaces() const
{
	return mAnimSpacesById.size();
}

size_t AnimationSet::_calcMemoryUsage() const
{
	size_t mem_usage = 0;

	AnimationConstIterator ai = getAnimationConstIterator();
	while( !ai.end() )
		mem_usage += ai.next()->_calcMemoryUsage();

	AnimationSpaceConstIterator asi = getAnimationSpaceConstIterator();
	while( !asi.end() )
		mem_usage += asi.next()->_calcMemoryUsage();

	return mem_usage;
}

void AnimationSet::_unload()
{
	deleteAllAnimations();
	deleteAllAnimationSpaces();
}

void AnimationSet::_clone( Resource* clonePtr ) const
{
	zhAssert( clonePtr != NULL );
	zhAssert( clonePtr->getClassId() == getClassId() );

	AnimationSet* clone = static_cast<AnimationSet*>(clonePtr);
	
	// clone each Animation
	AnimationConstIterator ai = getAnimationConstIterator();
	while( !ai.end() )
	{
		Animation* anim = ai.next();

		// create Animation and clone anim. data
		Animation* cl_anim = clone->createAnimation( anim->getId(), anim->getName() );
		anim->_clone( cl_anim );
	}

	// clone each AnimationSpace
	AnimationSpaceConstIterator asi = getAnimationSpaceConstIterator();
	while( !asi.end() )
	{
		AnimationSpace* anim_space = asi.next();

		// create Animation and clone anim. data
		AnimationSpace* cl_anim_space = clone->createAnimationSpace( anim_space->getId(), anim_space->getName() );
		anim_space->_clone( cl_anim_space );
	}
}

}
