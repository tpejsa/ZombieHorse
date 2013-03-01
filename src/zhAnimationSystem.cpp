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

#include "zhAnimationSystem.h"
#include "zhMemoryPool.h"
#include "zhAnimationManager.h"
#include "zhAnimation.h"
#include "zhAnimationTree.h"
#include "zhZHALoader.h"
#include "zhZHASerializer.h"
#include "zhBVHLoader.h"

namespace zh
{

AnimationSystem::AnimationSystem()
{
}

AnimationSystem::~AnimationSystem()
{
	deleteAllSkeletons();
}

bool AnimationSystem::init( const std::string& cfgPath )
{
	zhLog( "AnimationSystem", "init",
		"Initializing AnimationSystem. Using configuration file %s.",
		cfgPath.c_str() );

	// create instances of various singletons
	MemoryPool::Instance();
	AnimationManager::Instance();
	AnimationTree::Instance();
	
	// register resource loaders and serializers
	zhRegister_ResourceLoader( getAnimationManager(), ZHALoader );
	zhRegister_ResourceSerializer( getAnimationManager(), ZHASerializer );
	zhRegister_ResourceLoader( getAnimationManager(), BVHLoader );

	// register animation nodes and bone controllers
	zhRegister_AnimationNode( AnimationSampler );
	zhRegister_AnimationNode( AnimationBlender );
	zhRegister_AnimationNode( AnimationTransitionBlender );

	// load config.xml (configuration settings, res. dirs, plugin dir, plugins)
	// TODO

	zhSetErrorCode( SystemError_None );
	return true;
}

Skeleton* AnimationSystem::createSkeleton( const std::string& name )
{
	zhAssert( !hasSkeleton(name) );

	zhLog( "AnimationSystem", "createSkeleton",
		"Creating Skeleton  %s.", name.c_str() );

	Skeleton* ch = new Skeleton(name);
	mSkeletons[name] = ch;

	return ch;
}

void AnimationSystem::deleteSkeleton( const std::string& name )
{
	zhLog( "AnimationSystem", "deleteSkeleton", "Deleting skeleton %s.", name.c_str() );

	Skeleton* ch = getSkeleton(name);

	if( ch == NULL )
		return;

	mSkeletons.erase(name);
	delete ch;
}

void AnimationSystem::deleteAllSkeletons()
{
	zhLog( "AnimationSystem", "deleteSkeleton",
		"Deleting all skeletons." );

	for( std::map<std::string, Skeleton*>::iterator ci = mSkeletons.begin(); ci != mSkeletons.end(); ++ci )
		delete ci->second;

	mSkeletons.clear();
}

bool AnimationSystem::hasSkeleton( const std::string& name ) const
{
	return mSkeletons.count(name) > 0;
}

Skeleton* AnimationSystem::getSkeleton( const std::string& name ) const
{
	std::map<std::string, Skeleton*>::const_iterator ci = mSkeletons.find(name);

	if( ci != mSkeletons.end() )
		return ci->second;

	return NULL;
}

AnimationSystem::SkeletonIterator AnimationSystem::getSkeletonIterator()
{
	return SkeletonIterator( mSkeletons );
}

AnimationSystem::SkeletonConstIterator AnimationSystem::getSkeletonConstIterator() const
{
	return SkeletonConstIterator( mSkeletons );
}

unsigned int AnimationSystem::getNumSkeletons() const
{
	return mSkeletons.size();
}

AnimationSetPtr AnimationSystem::loadAnimationSet( const std::string& path, const std::string& skel )
{
}

void AnimationSystem::deleteAnimationSet( const std::string& name ) const
{
}

void AnimationSystem::deleteAllAnimations( const std::string& name ) const
{
}

AnimationSetPtr AnimationSystem::getAnimationSet( const std::string& name ) const
{
}

bool AnimationSystem::hasAnimationSet( const std::string& name ) const
{
}

AnimationSystem::AnimationSetIterator AnimationSystem::getAnimationSetIterator()
{
}

AnimationSystem::AnimationSetConstIterator AnimationSystem::getAnimationSetConstIterator() const
{
}

Animation* AnimationSystem::getAnimation( const std::string& animName ) const
{
}

bool AnimationSystem::hasAnimation( const std::string& animName ) const
{
}

AnimationManager* AnimationSystem::getAnimationManager() const
{
	return AnimationManager::Instance();
}

void AnimationSystem::createAnimationFromSegment( const std::string& newAnimName,
	const std::string& origAnimName, float startTime, float length )
{
	/*
	// create new animation
	unsigned short anim_id = 0;
	while( anim_set->hasAnimation(anim_id) ) ++anim_id;
	anim = anim_set->createAnimation( anim_id, anim_name );

	// fill up animation with key-frames
	// first create bone tracks
	zh::Animation::BoneTrackConstIterator bti = raw_anim->getBoneTrackConstIterator();
	while( !bti.end() )
	{
		BoneAnimationTrack* rbat = bti.next();
		BoneAnimationTrack* bat = anim->createBoneTrack( rbat->getBoneId() );
		
		// create initial key-frame
		zh::TransformKeyFrame* tkf = static_cast<zh::TransformKeyFrame*>( bat->createKeyFrame(0) );
		rbat->getInterpolatedKeyFrame( anim_seg.getStartTime(), tkf );

		// create final key-frame
		tkf = static_cast<zh::TransformKeyFrame*>( bat->createKeyFrame( anim_seg.getEndTime() - anim_seg.getStartTime() ) );
		rbat->getInterpolatedKeyFrame( anim_seg.getEndTime(), tkf );

		// copy intervening key-frames
		for( unsigned int kfi = 0; kfi < rbat->getNumKeyFrames(); ++kfi )
		{
			zh::TransformKeyFrame* rtkf = static_cast<zh::TransformKeyFrame*>( rbat->getKeyFrame(kfi) );
			
			if( rtkf->getTime() <= anim_seg.getStartTime() )
				continue;
			if( rtkf->getTime() >= anim_seg.getEndTime() )
				break;

			tkf = static_cast<zh::TransformKeyFrame*>( bat->createKeyFrame( rtkf->getTime() - anim_seg.getStartTime() ) );
			tkf->setTranslation( rtkf->getTranslation() );
			tkf->setRotation( rtkf->getRotation() );
			tkf->setScale( rtkf->getScale() );
		}
	}
	// then create mesh tracks
	zh::Animation::MeshTrackConstIterator mti = raw_anim->getMeshTrackConstIterator();
	while( !mti.end() )
	{
		MeshAnimationTrack* rmat = mti.next();
		MeshAnimationTrack* mat = anim->createMeshTrack( rmat->getMeshId() );
		
		// create initial key-frame
		zh::MorphKeyFrame* mkf = static_cast<MorphKeyFrame*>( mat->createKeyFrame(0) );
		rmat->getInterpolatedKeyFrame( anim_seg.getStartTime(), mkf );

		// create final key-frame
		mkf = static_cast<MorphKeyFrame*>( mat->createKeyFrame( anim_seg.getEndTime() - anim_seg.getStartTime() ) );
		rmat->getInterpolatedKeyFrame( anim_seg.getEndTime(), mkf );

		// copy intervening key-frames
		for( unsigned int kfi = 0; kfi < rmat->getNumKeyFrames(); ++kfi )
		{
			zh::MorphKeyFrame* rmkf = static_cast<MorphKeyFrame*>( rmat->getKeyFrame(kfi) );
			
			if( rmkf->getTime() <= anim_seg.getStartTime() )
				continue;
			if( rmkf->getTime() >= anim_seg.getEndTime() )
				break;

			mkf = static_cast<MorphKeyFrame*>( mat->createKeyFrame( rmkf->getTime() - anim_seg.getStartTime() ) );
			mkf->setMorphTargetWeights( rmkf->getMorphTargetWeights() );
		}
	}

	// TODO: copy annots as well
	*/
}

Skeleton* AnimationSystem::getOutputSkeleton() const
{
}

void AnimationSystem::setOutputSkeleton( const std::string& name )
{
}

void AnimationSystem::playAnimation( const std::string& animName )
{
}

void AnimationSystem::playAnimationNow( const std::string& animName )
{
}

void AnimationSystem::stopAnimation()
{
}

bool AnimationSystem::isAnimationPlaying() const
{
}

void AnimationSystem::pauseAnimation()
{
}

void AnimationSystem::unpauseAnimation()
{
}
	
bool AnimationSystem::isAnimationPaused() const
{
}

float AnimationSystem::getAnimationTime() const
{
}

void AnimationSystem::setAnimationTime( float time )
{
}

float AnimationSystem::getAnimationRate() const
{
}

void AnimationSystem::setAnimationRate( float rate )
{
}

float AnimationSystem::getAnimationLength() const
{
}

void AnimationSystem::update( float dt ) const
{
}

AnimationTree* AnimationSystem::getAnimationTree() const
{
	return AnimationTree::Instance();
}

MemoryPool* AnimationSystem::getMemoryPool() const
{
	return MemoryPool::Instance();
}

}
