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

#include "zhAnimationTree.h"
#include "zhAnimationTreeManager.h"
#include "zhAnimationNode.h"
#include "zhBoneController.h"
#include "zhAnimationController.h"

namespace zh
{

AnimationTree::AnimationTree( unsigned long id, const std::string& name, ResourceManager* mgr )
: Resource( id, name, mgr ),
mMdl(NULL), mRoot(NULL), mFirst(NULL), mApplyMover(true), mTotalWeight(0)
{
}

AnimationTree::~AnimationTree()
{
	deleteAllNodes();
	deleteAllBoneControllers();
}

Model* AnimationTree::getModel() const
{
	return mMdl;
}

void AnimationTree::setModel( Model* mdl )
{
	mMdl = mdl;
}

AnimationNode* AnimationTree::getRoot() const
{
	return mRoot;
}

void AnimationTree::setRoot( unsigned short id )
{
	mRoot = getNode(id);
}

void AnimationTree::setRoot( const std::string& name )
{
	mRoot = getNode(name);
}

void AnimationTree::setRoot( AnimationNode* node )
{
	zhAssert( node == NULL ||
		hasNode( node->getId() ) && node->getParent() == NULL );

	mRoot = node;
}

AnimationNode* AnimationTree::createNode( unsigned long classId,
										 unsigned short id, const std::string& name )
{
	zhAssert( !hasNode(id) && !hasNode(name) );

	AnimationTreeManager* atmgr = static_cast<AnimationTreeManager*>( this->getManager() );
	AnimationNode* node = atmgr->_getNodeFactory()->createObject( classId, id );
	node->mOwner = this;
	node->mName = name;

	zhLog( "AnimationTree", "createNode",
		"Creating node %s %u, %s on animation tree %u, %s.",
		node->getClassName().c_str(), node->getId(), node->getName().c_str(),
		mId, mName.c_str() );

	mNodesById.insert( make_pair( id, node ) );
	mNodesByName.insert( make_pair( name, node ) );

	return node;
}

void AnimationTree::deleteNode( unsigned short id )
{
	std::map<unsigned short, AnimationNode*>::iterator ani = mNodesById.find(id);

	if( ani != mNodesById.end() )
	{
		AnimationNode* node = ani->second;

		zhLog( "AnimationTree", "deleteNode",
			"Deleting node %s %u, %s from animation tree %u, %s.",
			node->getClassName().c_str(), node->getId(), node->getName().c_str(),
			mId, mName.c_str() );

		// detach node from the hierarchy
		if( node->getParent() != NULL )
			node->getParent()->removeChild(id);
		node->removeAllChildren();
		if( node == mRoot )
			mRoot = NULL;

		mNodesById.erase(ani);
		mNodesByName.erase( node->getName() );
		delete node;
	}
}

void AnimationTree::deleteNode( const std::string& name )
{
	std::map<std::string, AnimationNode*>::iterator ani = mNodesByName.find(name);

	if( ani != mNodesByName.end() )
	{
		AnimationNode* node = ani->second;

		zhLog( "AnimationTree", "deleteNode",
			"Deleting node %s %u, %s from animation tree %u, %s.",
			node->getClassName().c_str(), node->getId(), node->getName().c_str(),
			mId, mName.c_str() );

		// detach node from the hierarchy
		if( node->getParent() != NULL )
			node->getParent()->removeChild( node->getId() );
		node->removeAllChildren();
		if( node == mRoot )
			mRoot = NULL;

		mNodesById.erase( node->getId() );
		mNodesByName.erase(name);
		delete node;
	}
}

void AnimationTree::deleteAllNodes()
{
	for( std::map<unsigned short, AnimationNode*>::iterator ani = mNodesById.begin();
		ani != mNodesById.end(); ++ani )
		delete ani->second;

	zhLog( "AnimationTree", "deleteAllNodes",
		"Deleting all nodes from animation tree %u, %s.",
		mId, mName.c_str() );

	mNodesById.clear();
	mNodesByName.clear();
	mRoot = NULL;
}

bool AnimationTree::hasNode( unsigned short id ) const
{
	return mNodesById.count(id) > 0;
}

bool AnimationTree::hasNode( const std::string& name ) const
{
	return mNodesByName.count(name) > 0;
}

AnimationNode* AnimationTree::getNode( unsigned short id ) const
{
	std::map<unsigned short, AnimationNode*>::const_iterator ani = mNodesById.find(id);

	if( ani != mNodesById.end() )
		return ani->second;

	return NULL;
}

AnimationNode* AnimationTree::getNode( const std::string& name ) const
{
	std::map<std::string, AnimationNode*>::const_iterator ani = mNodesByName.find(name);

	if( ani != mNodesByName.end() )
		return ani->second;

	return NULL;
}

unsigned int AnimationTree::getNumNodes() const
{
	return mNodesById.size();
}

AnimationTree::NodeIterator AnimationTree::getNodeIterator()
{
	return NodeIterator( mNodesById );
}

AnimationTree::NodeConstIterator AnimationTree::getNodeConstIterator() const
{
	return NodeConstIterator( mNodesById );
}

BoneController* AnimationTree::getFirst() const
{
	return mFirst;
}

void AnimationTree::setFirst( BoneController* bc )
{
	zhAssert( bc == NULL ||
		hasBoneController( bc->getId() ) && bc->getPrevious() == NULL );

	mFirst = bc;
}

void AnimationTree::setFirst( unsigned short id )
{
	mFirst = getBoneController(id);
}

void AnimationTree::setFirst( const std::string& name )
{
	mFirst = getBoneController(name);
}

BoneController* AnimationTree::getLast() const
{
	if( mFirst == NULL )
		return NULL;

	BoneController* bc = mFirst;
	while( bc->getNext() != NULL )
		bc = bc->getNext();

	return bc;
}

BoneController* AnimationTree::createBoneController( unsigned long classId,
										 unsigned short id, const std::string& name )
{
	zhAssert( !hasBoneController(id) && !hasBoneController(name) );

	AnimationTreeManager* atmgr = static_cast<AnimationTreeManager*>( this->getManager() );
	BoneController* bc = atmgr->_getBoneControllerFactory()->createObject( classId, id );
	bc->_setAnimationTree(this);
	bc->setName(name);

	zhLog( "AnimationTree", "createBoneController",
		"Creating bone controller %s %u, %s on animation tree %u, %s.",
		bc->getClassName().c_str(), bc->getId(), bc->getName().c_str(),
		mId, mName.c_str() );

	mBoneControllersById.insert( make_pair( id, bc ) );
	mBoneControllersByName.insert( make_pair( name, bc ) );

	return bc;
}

void AnimationTree::deleteBoneController( unsigned short id )
{
	std::map<unsigned short, BoneController*>::iterator bci = mBoneControllersById.find(id);

	if( bci != mBoneControllersById.end() )
	{
		BoneController* bc = bci->second;

		zhLog( "AnimationTree", "deleteBoneController",
			"Deleting bone controller %s %u, %s from animation tree %u, %s.",
			bc->getClassName().c_str(), bc->getId(), bc->getName().c_str(),
			mId, mName.c_str() );

		// detach bone controller from the chain
		if( bc->getPrevious() != NULL )
			bc->getPrevious()->setNext(
			bc->getNext() != NULL ? bc->getNext() : NULL );
		if( bc == mFirst )
			mFirst = NULL;
		bc->setNext(NULL);

		mBoneControllersById.erase(bci);
		mBoneControllersByName.erase( bc->getName() );
		delete bc;
	}
}

void AnimationTree::deleteBoneController( const std::string& name )
{
	std::map<std::string, BoneController*>::iterator bci = mBoneControllersByName.find(name);

	if( bci != mBoneControllersByName.end() )
	{
		BoneController* bc = bci->second;

		zhLog( "AnimationTree", "deleteBoneController",
			"Deleting bone controller %s %u, %s from animation tree %u, %s.",
			bc->getClassName().c_str(), bc->getId(), bc->getName().c_str(),
			mId, mName.c_str() );

		// detach bone controller from the chain
		if( bc->getPrevious() != NULL )
			bc->getPrevious()->setNext(
			bc->getNext() != NULL ? bc->getNext() : NULL );
		if( bc == mFirst )
			mFirst = NULL;
		bc->setNext(NULL);

		mBoneControllersById.erase( bc->getId() );
		mBoneControllersByName.erase(name);
		delete bc;
	}
}

void AnimationTree::deleteAllBoneControllers()
{
	for( std::map<unsigned short, BoneController*>::iterator bci = mBoneControllersById.begin();
		bci != mBoneControllersById.end(); ++bci )
		delete bci->second;

	zhLog( "AnimationTree", "deleteAllBoneControllers",
		"Deleting all bone controllers from animation tree %u, %s.",
		mId, mName.c_str() );

	mBoneControllersById.clear();
	mBoneControllersByName.clear();
}

bool AnimationTree::hasBoneController( unsigned short id ) const
{
	return mBoneControllersById.count(id) > 0;
}

bool AnimationTree::hasBoneController( const std::string& name ) const
{
	return mBoneControllersByName.count(name) > 0;
}

BoneController* AnimationTree::getBoneController( unsigned short id ) const
{
	std::map<unsigned short, BoneController*>::const_iterator bci = mBoneControllersById.find(id);

	if( bci != mBoneControllersById.end() )
		return bci->second;

	return NULL;
}

BoneController* AnimationTree::getBoneController( const std::string& name ) const
{
	std::map<std::string, BoneController*>::const_iterator bci = mBoneControllersByName.find(name);

	if( bci != mBoneControllersByName.end() )
		return bci->second;

	return NULL;
}

unsigned int AnimationTree::getNumBoneControllers() const
{
	return mBoneControllersById.size();
}

AnimationTree::BoneControllerIterator AnimationTree::getBoneControllerIterator()
{
	return BoneControllerIterator( mBoneControllersById );
}

AnimationTree::BoneControllerConstIterator AnimationTree::getBoneControllerConstIterator() const
{
	return BoneControllerConstIterator( mBoneControllersById );
}

bool AnimationTree::getApplyMover() const
{
	return mApplyMover;
}

void AnimationTree::setApplyMover( bool applyMover )
{
	mApplyMover = applyMover;
}

void AnimationTree::update( float dt )
{
	if( mMdl == NULL )
		return;

	// reset total weight
	mTotalWeight = 0;

	// update node tree
	if( mRoot != NULL )
		mRoot->update(dt);

	// update bone controller chain
	if( mFirst != NULL )
		mFirst->update(dt);
}

void AnimationTree::apply() const
{
	if( mMdl == NULL )
		return;

	Skeleton* skel = mMdl->getSkeleton();

	// reset character to initial pose
	mPrevSit = mMdl->getSituation();
	skel->resetToInitialPose();

	if( mApplyMover )
	{
		// reset root transformation
		Bone* root = skel->getRoot();
		root->setPosition( Vector3::Null );
		root->setOrientation( Quat::Identity );
	}

	// apply node tree
	AnimationNode* root = getRoot();
	if( root != NULL )
		root->apply(1);

	// apply bone controller chain
	BoneController* first = getFirst();
	if( first != NULL )
		first->apply();
}
	
size_t AnimationTree::_calcMemoryUsage() const
{
	size_t usage = 0;

	NodeConstIterator ani = getNodeConstIterator();
	while( !ani.end() )
	{
		AnimationNode* node = ani.next();
		usage += node->_calcMemoryUsage();
	}

	BoneControllerConstIterator bci = getBoneControllerConstIterator();
	while( !bci.end() )
	{
		BoneController* bc = bci.next();
		usage += bc->_calcMemoryUsage();
	}

	return usage;
}

void AnimationTree::_unload()
{
	zhLog( "AnimationTree", "_unload",
		"Unloading animation tree %u, %s.",
		mId, mName.c_str() );

	NodeConstIterator ani = getNodeConstIterator();
	while( !ani.end() )
	{
		AnimationNode* node = ani.next();
		node->_unload();
	}

	BoneControllerConstIterator bci = getBoneControllerConstIterator();
	while( !bci.end() )
	{
		BoneController* bc = bci.next();
		bc->_unload();
	}
}

void AnimationTree::_clone( Resource* clonePtr ) const
{
	zhAssert( clonePtr != NULL );
	zhAssert( clonePtr->getClassId() == getClassId() );

	AnimationTree* clone = static_cast<AnimationTree*>(clonePtr);

	// create empty nodes and bone controllers

	NodeConstIterator ani = getNodeConstIterator();
	while( !ani.end() )
	{
		AnimationNode* node = ani.next();
		clone->createNode( node->getClassId(), node->getId(), node->getName() );
	}

	BoneControllerConstIterator bci = getBoneControllerConstIterator();
	while( !bci.end() )
	{
		BoneController* bc = bci.next();
		clone->createBoneController( bc->getClassId(), bc->getId(), bc->getName() );
	}

	// clone data from original nodes and bone controllers

	ani = getNodeConstIterator();
	while( !ani.end() )
	{
		AnimationNode* node = ani.next();
		AnimationNode* new_node = clone->getNode( ani.key() );
		node->_clone( new_node );
	}

	bci = getBoneControllerConstIterator();
	while( !bci.end() )
	{
		BoneController* bc = bci.next();
		BoneController* new_bc = clone->getBoneController( bci.key() );
		bc->_clone( new_bc );
	}

	// set anim. tree root and first bone ctrl.
	if( mRoot != NULL )
		clone->mRoot = clone->getNode( mRoot->getId() );
	if( mFirst != NULL )
		clone->mFirst = clone->getBoneController( mFirst->getId() );

	clone->mApplyMover = mApplyMover;
}

void AnimationTree::_renameNode( AnimationNode* node, const std::string& newName )
{
	zhAssert( node != NULL );
	zhAssert( !hasNode(newName) );

	// remove node from name-indexed maps
	AnimationNode* pnode = node->getParent();
	mNodesByName.erase( node->getName() );
	if( pnode != NULL )
		pnode->mChildrenByName.erase( node->getName() );

	// rename node and update name-indexed maps
	node->mName = newName;
	mNodesByName[newName] = node;
	if( pnode != NULL )
		pnode->mChildrenByName[newName] = node;
}

void AnimationTree::_renameBoneController( BoneController* boneCtrl, const std::string& newName )
{
	zhAssert( boneCtrl != NULL );
	zhAssert( !hasBoneController(newName) );

	// remove bone controller from name-indexed maps
	mBoneControllersByName.erase( boneCtrl->getName() );

	// rename bone controller and update name-indexed maps
	boneCtrl->mName = newName;
	mBoneControllersByName[newName] = boneCtrl;
}

float AnimationTree::_getTotalWeight() const
{
	return mTotalWeight;
}

void AnimationTree::_setTotalWeight( float weight )
{
	mTotalWeight = weight;
}

const Model::Situation& AnimationTree::_getPrevSituation() const
{
	return mPrevSit;
}

void AnimationTree::_setPrevSituation( const Model::Situation& sit )
{
	mPrevSit = sit;
}

AnimationTreeInstance::AnimationTreeInstance( AnimationTreePtr animTree, AnimationController* ctrl )
: AnimationTree( animTree->getId(), animTree->getName(), animTree->getManager() ), mOwner(ctrl)
{
	zhAssert( animTree != NULL );
	zhAssert( ctrl != NULL );

	// copy data members and shared pointers
	this->mMgr = animTree->getManager();
	this->mPath = animTree->getPath();
	this->mState = animTree->getState();
	this->mId = animTree->getId();
	this->mName = animTree->getName();
	this->mAnimTreeRes = animTree;

	// create empty nodes and bone controllers

	NodeConstIterator ani = animTree->getNodeConstIterator();
	while( !ani.end() )
	{
		AnimationNode* node = ani.next();
		this->createNode( node->getClassId(), node->getId(), node->getName() );
	}

	BoneControllerConstIterator bci = animTree->getBoneControllerConstIterator();
	while( !bci.end() )
	{
		BoneController* bc = bci.next();
		this->createBoneController( bc->getClassId(), bc->getId(), bc->getName() );
	}

	// clone data from original nodes and bone controllers

	ani = animTree->getNodeConstIterator();
	while( !ani.end() )
	{
		AnimationNode* node = ani.next();
		AnimationNode* new_node = this->getNode( node->getId() );
		node->_clone( new_node, true );
	}

	bci = animTree->getBoneControllerConstIterator();
	while( !bci.end() )
	{
		BoneController* bc = bci.next();
		BoneController* new_bc = this->getBoneController( bc->getId() );
		bc->_clone( new_bc, true );
	}

	// set anim. tree root and first bone ctrl.
	if( animTree->getRoot() != NULL )
		mRoot = getNode( animTree->getRoot()->getId() );
	if( animTree->getFirst() != NULL )
		mFirst = getBoneController( animTree->getFirst()->getId() );

	mApplyMover = animTree->getApplyMover();
}

AnimationTreeInstance::~AnimationTreeInstance()
{
}

AnimationTreePtr AnimationTreeInstance::getAnimationTree() const
{
	return mAnimTreeRes;
}

AnimationController* AnimationTreeInstance::getController() const
{
	return mOwner;
}

size_t AnimationTreeInstance::_calcMemoryUsage() const
{
	return 0;
}

void AnimationTreeInstance::_unload()
{
}

void AnimationTreeInstance::_clone( Resource* clonePtr ) const
{
}

}
