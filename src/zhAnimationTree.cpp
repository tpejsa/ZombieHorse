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
#include "zhAnimationNode.h"

namespace zh
{

AnimationTree::AnimationTree( const std::string& name ) :
mName(name), mRoot(NULL), mApplyMover(true), mTotalWeight(0)
{
	mNodeFact = new NodeFactory();
}

AnimationTree::~AnimationTree()
{
	deleteAllNodes();
	delete mNodeFact;
}

const std::string& AnimationTree::getName() const
{
	return mName;
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

	AnimationNode* node = _getNodeFactory()->createObject( classId, id );
	node->mOwner = this;
	node->mName = name;

	zhLog( "AnimationTree", "createNode",
		"Creating node %s %u, %s on animation tree %s.",
		node->getClassName().c_str(), node->getId(), node->getName().c_str(),
		mName.c_str() );

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
			"Deleting node %s %u, %s from animation tree %s.",
			node->getClassName().c_str(), node->getId(),
			mName.c_str() );

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
			"Deleting node %s %u, %s from animation tree %s.",
			node->getClassName().c_str(), node->getId(), node->getName().c_str(),
			mName.c_str() );

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
		"Deleting all nodes from animation tree %s.",
		mName.c_str() );

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
	// reset total weight
	mTotalWeight = 0;

	// update node tree
	if( mRoot != NULL )
		mRoot->update(dt);
}

void AnimationTree::apply( Skeleton* skel) const
{
	zhAssert( skel != NULL );
	mCurSkel = skel;

	// reset character to initial pose
	mPrevSit = mCurSkel->getSituation();
	mCurSkel->resetToInitialPose();

	if( mApplyMover )
	{
		// reset root transformation
		Bone* root = mCurSkel->getRoot();
		root->setPosition( Vector3::Null );
		root->setOrientation( Quat::Identity );
	}

	// apply node tree
	AnimationNode* root = getRoot();
	if( root != NULL )
		root->apply(1);
}

void AnimationTree::_clone( AnimationTree* clonePtr ) const
{
	zhAssert( clonePtr != NULL );

	AnimationTree* clone = static_cast<AnimationTree*>(clonePtr);

	// Create empty nodes
	NodeConstIterator ani = getNodeConstIterator();
	while( !ani.end() )
	{
		AnimationNode* node = ani.next();
		clone->createNode( node->getClassId(), node->getId(), node->getName() );
	}

	// Clone data from original nodes
	ani = getNodeConstIterator();
	while( !ani.end() )
	{
		AnimationNode* node = ani.next();
		AnimationNode* new_node = clone->getNode( ani.key() );
		node->_clone( new_node );
	}

	// Set anim. tree root
	if( mRoot != NULL )
		clone->mRoot = clone->getNode( mRoot->getId() );

	// Copy settings
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

float AnimationTree::_getTotalWeight() const
{
	return mTotalWeight;
}

void AnimationTree::_setTotalWeight( float weight )
{
	mTotalWeight = weight;
}

Skeleton* AnimationTree::_getCurrentSkeleton() const
{
	return mCurSkel;
}

void AnimationTree::_setCurrentSkeleton( Skeleton* skel )
{
	mCurSkel = skel;
}

const Skeleton::Situation& AnimationTree::_getPrevSituation() const
{
	return mPrevSit;
}

void AnimationTree::_setPrevSituation( const Skeleton::Situation& sit )
{
	mPrevSit = sit;
}

AnimationTree::NodeFactory* AnimationTree::_getNodeFactory() const
{
	return mNodeFact;
}

}
