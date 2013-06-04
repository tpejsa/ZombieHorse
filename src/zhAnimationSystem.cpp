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
#include "zhFileSystem.h"
#include "zhAnimationManager.h"
#include "zhAnimation.h"
#include "zhAnimationTree.h"
#include "zhZHALoader.h"
#include "zhZHASerializer.h"
#include "zhBVHLoader.h"
#include "zhRootIKSolver.h"
#include "zhPostureIKSolver.h"
#include "zhLimbIKSolver.h"
#include "zhGPLVMIKSolver.h"

#include <boost/algorithm/string.hpp>

namespace zh
{

AnimationSystem::AnimationSystem() : mOutSkel(NULL)
{
	mAnimTree = new AnimationTree("MainTree");
}

AnimationSystem::~AnimationSystem()
{
	delete mAnimTree;
}

bool AnimationSystem::init( const std::string& cfgPath )
{
	zhLog( "AnimationSystem", "init",
		"Initializing AnimationSystem. Using configuration file %s.",
		cfgPath.c_str() );

	// Create instances of various singletons
	MemoryPool::Instance();
	AnimationManager::Instance();
	
	// Register resource loaders and serializers
	zhRegister_ResourceLoader( getAnimationManager(), ZHALoader );
	zhRegister_ResourceSerializer( getAnimationManager(), ZHASerializer );
	zhRegister_ResourceLoader( getAnimationManager(), BVHLoader );

	// Register animation nodes
	zhRegister_AnimationNode(AnimationSampleNode);
	zhRegister_AnimationNode(AnimationBlendNode);
	zhRegister_AnimationNode(AnimationQueueNode);

	// Register IK solvers
	zhRegister_IKSolver(RootIKSolver);
	zhRegister_IKSolver(PostureIKSolver);
	zhRegister_IKSolver(LimbIKSolver);
	zhRegister_IKSolver(GPLVMIKSolver);

	// Load config.xml (configuration settings, res. dirs, plugin dir, plugins)
	// TODO

	// Create the environment "skeleton"
	Skeleton* skel = createSkeleton("Environment");
	mOutSkel = NULL;
	skel->createBone( 0, "Root" );

	zhSetErrorCode( SystemError_None );
	return true;
}

Skeleton* AnimationSystem::createSkeleton( const std::string& name )
{
	zhAssert( !hasSkeleton(name) );

	zhLog( "AnimationSystem", "createSkeleton",
		"Creating Skeleton  %s.", name.c_str() );

	Skeleton* skel = new Skeleton(name);
	mSkeletons[name] = skel;

	if( mOutSkel == NULL )
		mOutSkel = skel;

	return skel;
}

void AnimationSystem::deleteSkeleton( const std::string& name )
{
	zhLog( "AnimationSystem", "deleteSkeleton", "Deleting skeleton %s.", name.c_str() );

	Skeleton* skel = getSkeleton(name);

	if( skel == NULL )
		return;

	mSkeletons.erase(name);
	delete skel;
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

Skeleton* AnimationSystem::createIKSolversOnSkeleton( const std::string& name )
{
	zhAssert( hasSkeleton(name) );

	zhLog( "AnimationSystem", "createIKSolversOnSkeleton",
		"Creating default IK solvers on skeleton %s.", name.c_str() );

	Skeleton* skel = getSkeleton(name);

	// Create root solver
	if( skel->hasBoneWithTag(BT_Root) )
		skel->createIKSolver( RootIKSolver::ClassId(), 0, "RootIK", BT_Root, BT_Root );

	// Create posture solver
	if( skel->hasBoneWithTag(BT_Root) && skel->hasBoneWithTag(BT_Chest) )
	{
		IKSolver* solver = skel->createIKSolver( PostureIKSolver::ClassId(), 1, "PostureIK", BT_Root, BT_Chest );
		if( skel->hasBoneWithTag(BT_LCollar) )
			solver->pushBone( skel->getBoneByTag(BT_LCollar) );
		if( skel->hasBoneWithTag(BT_RCollar) )
			solver->pushBone( skel->getBoneByTag(BT_RCollar) );
		if( skel->hasBoneWithTag(BT_LHipJoint) )
			solver->pushBone( skel->getBoneByTag(BT_LHipJoint) );
		if( skel->hasBoneWithTag(BT_RHipJoint) )
			solver->pushBone( skel->getBoneByTag(BT_RHipJoint) );
	}

	// Create limb solvers
	if( skel->hasBoneWithTag(BT_LShoulder) && skel->hasBoneWithTag(BT_LWrist) )
		skel->createIKSolver( LimbIKSolver::ClassId(), 2, "LArmIK", BT_LShoulder, BT_LWrist );
	if( skel->hasBoneWithTag(BT_RShoulder) && skel->hasBoneWithTag(BT_RWrist) )
		skel->createIKSolver( LimbIKSolver::ClassId(), 3, "RArmIK", BT_RShoulder, BT_RWrist );
	if( skel->hasBoneWithTag(BT_LHip) && skel->hasBoneWithTag(BT_LAnkle) )
		skel->createIKSolver( LimbIKSolver::ClassId(), 4, "LLegIK", BT_LHip, BT_LAnkle );
	if( skel->hasBoneWithTag(BT_RHip) && skel->hasBoneWithTag(BT_RAnkle) )
		skel->createIKSolver( LimbIKSolver::ClassId(), 5, "RLegIK", BT_RHip, BT_RAnkle );

	// Create data-driven solvers
	if( skel->hasBoneWithTag(BT_Root) )
		skel->createIKSolver( GPLVMIKSolver::ClassId(), 6, "GPLVMIK", BT_Root, BT_Root );
	
	return skel;
}

Skeleton* AnimationSystem::getEnvironment() const
{
	return getSkeleton("Environment");
}

float AnimationSystem::getGroundHeightAt( float posX, float posZ ) const
{
	// TODO: one day there will be different ground heights!
	return 0;
}

AnimationSetPtr AnimationSystem::loadAnimationSet( const std::string& path, const std::string& skel )
{
	zhLog( "AnimationSystem", "loadAnimationSet",
		"Loading animation set from path %s.", path.c_str() );

	AnimationManager* anim_mgr = getAnimationManager();

	// Determine name for animation set
	std::string dir, filename, animset_name, ext;
	parsePathStr( path, dir, filename, animset_name, ext );
	int asi = 0;
	std::string banimset_name = animset_name;
	if( anim_mgr->hasResource(animset_name) )
	{
		zhLog( "AnimationSystem", "loadAnimationSet",
			"WARNING: Unable to load animation set %s from path %s. Animation set by that name already exists.",
			animset_name.c_str(), path.c_str() );
		
		return AnimationSetPtr::DynamicCast<zh::Resource>( anim_mgr->getResource(animset_name) );
	}

	// Determine animation set ID
	unsigned int animset_id = 100;
	while( anim_mgr->hasResource(animset_id) ) ++animset_id;
	
	// Create and load animation set
	AnimationSetPtr anim_set = AnimationSetPtr::DynamicCast<zh::Resource>( 
		anim_mgr->createResource( animset_id, animset_name ) );
	if( !anim_mgr->loadResource( animset_id, path ) )
	{
		zhLog( "AnimationSystem", "loadAnimationSet",
			"ERROR: Failed to load animation set (%d, %s) from path %s.",
			animset_id, animset_name.c_str(), path.c_str() );
		anim_mgr->deleteResource(animset_id);
		
		return NULL;
	}

	// Add animation nodes to tree
	if( mAnimTree->getRoot() == NULL )
	{
		mAnimTree->createNode( AnimationQueueNode::ClassId(), 0, "Root" );
		mAnimTree->setRoot("Root");
	}
	AnimationSet::AnimationConstIterator anim_i = anim_set->getAnimationConstIterator();
	while( anim_i.hasMore() )
	{
		Animation* anim = anim_i.next();
		std::string node_name = anim_set->getName() + "::" + anim->getName();
		if( mAnimTree->hasNode(node_name) )
			continue;

		AnimationSampleNode* node = static_cast<AnimationSampleNode*>(
			mAnimTree->createNode( AnimationSampleNode::ClassId(), mAnimTree->getNumNodes(), node_name )
			);
		node->setAnimation( anim->getAnimationSet(), anim->getId() );
		Skeleton* skel = getSkeleton( anim_set->getName() );
		if( skel != NULL )
			node->createAdaptor(skel);
		mAnimTree->getNode("Root")->addChild(node);
	}
	mAnimTree->getNode("Root")->setPlaying(false);
	// TODO: Add nodes under correct retargetting node (specified by skel param.)

	zhLog( "AnimationSystem", "loadAnimationSet",
		"Loaded animation set (%d, %s) from path %s.",
		animset_id, animset_name.c_str(), path.c_str() );

	return anim_set;
}

void AnimationSystem::deleteAnimationSet( const std::string& name )
{
	zhLog( "AnimationSystem", "deleteAnimationSet",
		"Deleting animation set %s.", name.c_str() );

	AnimationManager* anim_mgr = getAnimationManager();

	// Remove animation nodes from tree
	AnimationSetPtr anim_set = AnimationSetPtr::DynamicCast<zh::Resource>(
		anim_mgr->getResource(name) );
	AnimationSet::AnimationConstIterator anim_i = anim_set->getAnimationConstIterator();
	while( anim_i.hasMore() )
	{
		Animation* anim = anim_i.next();
		mAnimTree->deleteNode( zhA( name, anim->getName() ) );
	}

	anim_mgr->deleteResource(name);
}

void AnimationSystem::deleteAllAnimations( const std::string& name )
{
	zhLog( "AnimationSystem", "deleteAnimationSet",
		"Deleting all currently loaded animation sets.", name.c_str() );

	AnimationManager* anim_mgr = getAnimationManager();

	mAnimTree->deleteAllNodes();
	anim_mgr->deleteAllResources();
}

AnimationSetPtr AnimationSystem::getAnimationSet( const std::string& name ) const
{
	return AnimationSetPtr::DynamicCast<zh::Resource>( getAnimationManager()->getResource(name) );
}

bool AnimationSystem::hasAnimationSet( const std::string& name ) const
{
	return getAnimationManager()->hasResource(name);
}

Animation* AnimationSystem::createAnimationFromSegment( const std::string& newAnimName,
	const std::string& origAnimName, float startTime, float length )
{
	zhLog( "AnimationSystem", "createAnimationFromSegment",
		"Creating a new animation %s from segment of existing animation %s.",
		newAnimName.c_str(), origAnimName.c_str() );

	if( !hasAnimation(origAnimName) )
	{
		zhLog( "AnimationSystem", "createAnimationFromSegment",
		"ERROR: Unable to create new animation %s from segment of existing animation %s. Original animation does not exist.",
		newAnimName.c_str(), origAnimName.c_str() );
		return NULL;
	}

	Animation* orig_anim = getAnimation(origAnimName);
	AnimationSetPtr anim_set = orig_anim->getAnimationSet();

	if( hasAnimation( zhA(anim_set->getName(),newAnimName) ) )
	{
		zhLog( "AnimationSystem", "createAnimationFromSegment",
		"ERROR: Unable to create new animation %s from segment of existing animation %s. Animation by that name already exists.",
		newAnimName.c_str(), origAnimName.c_str() );
		return NULL;
	}

	// Clamp start and end times if necessary
	startTime = startTime < 0 ? 0 : startTime;
	length = startTime+length > orig_anim->getLength() ? orig_anim->getLength()-startTime : length;

	// Create new animation
	unsigned short anim_id = 0;
	while( anim_set->hasAnimation(anim_id) ) ++anim_id;
	Animation* anim = anim_set->createAnimation( anim_id, newAnimName );

	// Fill up animation with key-frames
	zh::Animation::BoneTrackConstIterator bti = orig_anim->getBoneTrackConstIterator();
	while( !bti.end() )
	{
		BoneAnimationTrack* rbat = bti.next();
		BoneAnimationTrack* bat = anim->createBoneTrack( rbat->getBoneId() );
		
		// create initial key-frame
		zh::TransformKeyFrame* tkf = static_cast<zh::TransformKeyFrame*>( bat->createKeyFrame(0) );
		rbat->getInterpolatedKeyFrame( startTime, tkf );

		// create final key-frame
		tkf = static_cast<zh::TransformKeyFrame*>( bat->createKeyFrame(length) );
		rbat->getInterpolatedKeyFrame( startTime + length, tkf );

		// copy intervening key-frames
		for( unsigned int kfi = 0; kfi < rbat->getNumKeyFrames(); ++kfi )
		{
			zh::TransformKeyFrame* rtkf = static_cast<zh::TransformKeyFrame*>( rbat->getKeyFrame(kfi) );
			
			if( rtkf->getTime() <= startTime )
				continue;
			if( rtkf->getTime() >= startTime+length )
				break;

			tkf = static_cast<zh::TransformKeyFrame*>( bat->createKeyFrame( rtkf->getTime() - startTime ) );
			tkf->setTranslation( rtkf->getTranslation() );
			tkf->setRotation( rtkf->getRotation() );
			tkf->setScale( rtkf->getScale() );
		}
	}

	// Add the new animation to the animation tree
	AnimationSampleNode* node = static_cast<AnimationSampleNode*>(
		mAnimTree->createNode( AnimationSampleNode::ClassId(), mAnimTree->getNumNodes(), anim->getFullName() )
		);
	node->setAnimation( anim->getAnimationSet(), anim->getId() );
	mAnimTree->getNode("Root")->addChild(node);
	// TODO: Add under correct retargetting node

	// TODO: Copy annots. as well

	return anim;
}

void AnimationSystem::deleteAnimation( const std::string& name )
{
	zhLog( "AnimationSystem", "deleteAnimation",
		"Deleting animation %s.", name.c_str() );

	std::string animset_name, anim_name;
	ParseAnimationName( name, animset_name, anim_name );

	// Remove animation node from tree
	mAnimTree->deleteNode(name);

	AnimationSetPtr anim_set = getAnimationSet(animset_name);
	if( anim_set != NULL )
		anim_set->deleteAnimation(anim_name);
}

Animation* AnimationSystem::getAnimation( const std::string& animName ) const
{
	std::string animset_name, anim_name;
	ParseAnimationName( animName, animset_name, anim_name );

	AnimationSetPtr anim_set = getAnimationSet(animset_name);
	return anim_set == NULL ? NULL : anim_set->getAnimation(anim_name);
}

bool AnimationSystem::hasAnimation( const std::string& animName ) const
{
	std::string animset_name, anim_name;
	ParseAnimationName( animName, animset_name, anim_name );

	AnimationSetPtr anim_set = getAnimationSet(animset_name);
	return anim_set == NULL ? false : anim_set->hasAnimation(anim_name);
}

void AnimationSystem::getAnimationList( std::vector<Animation*>& animList ) const
{
	animList.clear();

	ResourceManager::ResourceConstIterator res_i =
		getAnimationManager()->getResourceConstIterator();
	while( res_i.hasMore() )
	{
		AnimationSetPtr anim_set = AnimationSetPtr::DynamicCast<zh::Resource>( res_i.next() );
		AnimationSet::AnimationConstIterator anim_i = anim_set->getAnimationConstIterator();
		while( anim_i.hasMore() )
			animList.push_back( anim_i.next() );
	}
}

AnimationManager* AnimationSystem::getAnimationManager() const
{
	return AnimationManager::Instance();
}

Skeleton* AnimationSystem::getOutputSkeleton() const
{
	return mOutSkel;
}

void AnimationSystem::setOutputSkeleton( const std::string& name )
{
	zhAssert( hasSkeleton(name) && name != "Environment" );

	mOutSkel = getSkeleton(name);
	
	zhLog( "AnimationSystem", "setOutputSkeleton",
		"Output skeleton is now %s.", name.c_str() );
}

void AnimationSystem::playAnimation( const std::string& animName )
{
	if( !mAnimTree->hasNode(animName) )
	{
		zhLog( "AnimationSystem", "playAnimationNow",
			"Unable to play animation %s, animation node does not exist.", animName.c_str() );
		return;
	}

	// Play the animation
	mAnimTree->setApplyMover();
	AnimationQueueNode* root =
		static_cast<AnimationQueueNode*>( mAnimTree->getRoot() );
	root->addTransition(animName);
	root->setDefaultNode((zh::AnimationNode*)NULL);
	root->setPlaying();
}

void AnimationSystem::playAnimationNow( const std::string& animName )
{
	stopAnimation();
	
	if( !mAnimTree->hasNode(animName) )
	{
		zhLog( "AnimationSystem", "playAnimationNow",
			"Unable to play animation %s, animation node does not exist.", animName.c_str() );
		return;
	}

	// Play the animation
	mAnimTree->setApplyMover(false);
	AnimationQueueNode* root =
		static_cast<AnimationQueueNode*>( mAnimTree->getRoot() );
	root->setDefaultNode(animName);
	root->setPlaying();
}

void AnimationSystem::stopAnimation()
{
	AnimationNode* root = mAnimTree->getRoot();
	if( root != NULL )
	{
		root->setPlaying(false);
		static_cast<AnimationQueueNode*>(root)->removeAllTransitions();
	}
}

Animation* AnimationSystem::getCurrentAnimation() const
{
	AnimationTree::NodeConstIterator node_i = mAnimTree->getNodeConstIterator();
	while( node_i.hasMore() )
	{
		AnimationNode* node = node_i.next();
		if( node->getClassId() == AnimationSampleNode::ClassId() &&
			node->getPlaying() )
			return static_cast<AnimationSampleNode*>(node)->getAnimation();
	}

	return NULL;
}

bool AnimationSystem::isAnimationPlaying() const
{
	AnimationNode* root = mAnimTree->getRoot();

	return root != NULL && root->getPlaying();
}

void AnimationSystem::pauseAnimation()
{
	AnimationNode* root = mAnimTree->getRoot();
	if( root != NULL )
		root->setPaused(true);
}

void AnimationSystem::unpauseAnimation()
{
	AnimationNode* root = mAnimTree->getRoot();
	if( root != NULL )
		root->setPaused(false);
}
	
bool AnimationSystem::isAnimationPaused() const
{
	AnimationNode* root = mAnimTree->getRoot();

	return root != NULL && root->getPaused();
}

float AnimationSystem::getAnimationTime() const
{
	AnimationNode* root = mAnimTree->getRoot();
	if( root != NULL )
		return root->getPlayTime();

	return 0;
}

void AnimationSystem::setAnimationTime( float time )
{
	AnimationNode* root = mAnimTree->getRoot();
	if( root != NULL )
		root->setPlayTime(time);
}

float AnimationSystem::getAnimationRate() const
{
	AnimationNode* root = mAnimTree->getRoot();
	if( root != NULL )
		return root->getPlayRate();

	return 1;
}

void AnimationSystem::setAnimationRate( float rate )
{
	AnimationNode* root = mAnimTree->getRoot();
	if( root != NULL )
		root->setPlayRate(rate);
}

float AnimationSystem::getAnimationLength() const
{
	AnimationNode* root = mAnimTree->getRoot();
	if( root != NULL )
		return root->getPlayLength();

	return 0;
}

void AnimationSystem::update( float dt ) const
{
	if( mOutSkel == NULL )
		return;

	mAnimTree->update(dt);
	mAnimTree->apply(mOutSkel);
}

AnimationTree* AnimationSystem::getAnimationTree() const
{
	return mAnimTree;
}

AnimationSystem::AnimationNodeFactory& AnimationSystem::_getAnimationNodeFactory()
{
	return mAnimNodeFact;
}

AnimationSystem::IKSolverFactory& AnimationSystem::_getIKSolverFactory()
{
	return mIKSolverFact;
}

MemoryPool* AnimationSystem::_getMemoryPool() const
{
	return MemoryPool::Instance();
}

void AnimationSystem::ParseAnimationName( const std::string& fullName,
	std::string& animSetName, std::string& animName )
{
	size_t dli = fullName.find("::");
	if( dli == std::string::npos )
		animSetName = animName = "";
	else
	{
		animSetName = fullName.substr(0,dli);
		animName = fullName.substr(dli+2);
	}
}

}
