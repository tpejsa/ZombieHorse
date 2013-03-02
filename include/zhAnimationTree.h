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

#ifndef __zhAnimationTree_h__
#define __zhAnimationTree_h__

#include "zhPrereq.h"
#include "zhLogger.h"
#include "zhObjectFactory.h"
#include "zhSkeleton.h"
#include "zhAnimationSet.h"
#include "zhAnimationNode.h"
#include "zhAnimationSampler.h"
#include "zhAnimationBlender.h"
#include "zhAnimationTransitionBlender.h"

namespace zh
{

class AnimationTree;

/**
* @brief Animation tree resource.
*/
class zhDeclSpec AnimationTree
{

public:

	typedef MapIterator< std::map<unsigned short, AnimationNode*> > NodeIterator;
	typedef MapConstIterator< std::map<unsigned short, AnimationNode*> > NodeConstIterator;

	typedef ObjectFactory< AnimationNode, unsigned short > NodeFactory;

	/**
	* Constructor.
	*
	* @param name Animation tree name.
	*/
	AnimationTree( const std::string& name );

	/**
	* Destructor.
	*/
	virtual ~AnimationTree();

	/**
	* Gets the animation tree name.
	*/
	virtual const std::string& getName() const;

	/**
	* Gets the root animation node.
	*/
	virtual AnimationNode* getRoot() const;

	/**
	* Sets the root animation node.
	*/
	virtual void setRoot( unsigned short id );
	
	/**
	* Sets the root animation node.
	*/
	virtual void setRoot( const std::string& name );

	/**
	* Sets the root animation node.
	*
	* @param node Animation node or
	* NULL to unset current root.
	*/
	virtual void setRoot( AnimationNode* node );

	/**
	* Creates a new animation node.
	*
	* @param classId Node class ID.
	* @param id Node ID.
	* @param name Node name.
	* @return Pointer to the animation node.
	*/
	virtual AnimationNode* createNode( unsigned long classId,
		unsigned short id, const std::string& name );

	/**
	* Deletes the specified animation node.
	*
	* @param id Node ID.
	*/
	virtual void deleteNode( unsigned short id );

	/**
	* Deletes the specified animation node.
	*
	* @param name Node name.
	*/
	virtual void deleteNode( const std::string& name );

	/**
	* Deletes all nodes in the animation tree.
	*/
	virtual void deleteAllNodes();

	/**
	* Checks if the specified animation node exists in the animation tree.
	*
	* @param id Node ID.
	* @return true if the node exists, false otherwise.
	*/
	virtual bool hasNode( unsigned short id ) const;

	/**
	* Checks if the specified animation node exists in the animation tree.
	*
	* @param name Node name.
	* @return true if the node exists, false otherwise.
	*/
	virtual bool hasNode( const std::string& name ) const;

	/**
	* Gets a pointer to the specified animation node.
	*
	* @param id Node ID.
	* @return Pointer to specified node or NULL if the node doesn't exist.
	*/
	virtual AnimationNode* getNode( unsigned short id ) const;

	/**
	* Gets a pointer to the specified animation node.
	*
	* @param name Node name.
	* @return Pointer to specified node or NULL if the node doesn't exist.
	*/
	virtual AnimationNode* getNode( const std::string& name ) const;

	/**
	* Gets the number of animation nodes in the animation tree.
	*/
	virtual unsigned int getNumNodes() const;

	/**
	* Gets an iterator over the map of animation nodes
	* in the animation tree.
	*/
	virtual NodeIterator getNodeIterator();

	/**
	* Gets a const iterator over the map of animation nodes
	* in the animation tree.
	*/
	virtual NodeConstIterator getNodeConstIterator() const;

	/**
	* If false, mover channel in sampled animations is applied directly,
	* otherwise it is left to individual animation nodes
	* to recompute the mover channel.
	*/
	virtual bool getApplyMover() const;

	/**
	* If set to false, mover channel in sampled animations is applied directly,
	* otherwise it is left to individual animation nodes
	* to recompute the mover channel.
	*/
	virtual void setApplyMover( bool applyMover = true );

	/**
	* Updates the animation tree instance with elapsed time.
	*
	* @param dt Elapsed time.
	*/
	virtual void update( float dt );

	/**
	* Applies animation in this instance to the specified character skeleton.
	*
	* @param skel Pointer to the Skeleton that animation should be applied to.
	*/
	virtual void apply( Skeleton* skel ) const;

	/**
	* Creates a deep copy of the animation tree.
	*
	* @param clonePtr Pointer to the copy.
	*/
	void _clone( AnimationTree* clonePtr ) const;

	/**
	* Updates the name of the specified animation node.
	*
	* @remark This function is not meant to be called directly.
	* To rename a node, use AnimationNode::setName().
	*/
	virtual void _renameNode( AnimationNode* node, const std::string& newName );

	/**
	* Gets the cumulative blend weight of animations applied to the skeleton
	* so far.
	*
	* @remark This property is useful for additive blending and is used
	* e.g. in AnimationSampler::_applyNode().
	*/
	virtual float _getTotalWeight() const;

	/**
	* Sets the cumulative blend weight of animations applied to the skeleton
	* so far.
	*/
	virtual void _setTotalWeight( float weight );

	/**
	* Gets a pointer to the skeleton to which animation is being applied.
	*/
	virtual Skeleton* _getCurrentSkeleton() const;

	/**
	* Sets pointer to the skeleton to which animation is being applied.
	*/
	virtual void _setCurrentSkeleton( Skeleton* skel );

	/**
	* Gets the character situation at the end of previous frame.
	*/
	virtual const Skeleton::Situation& _getPrevSituation() const;

	/**
	* Sets the character situation at the end of previous frame.
	*/
	virtual void _setPrevSituation( const Skeleton::Situation& sit );

	/**
	* Gets the factory that produces nodes for the animation tree.
	*/
	virtual NodeFactory* _getNodeFactory() const;

protected:

	std::string mName;
	AnimationNode* mRoot;
	std::map<unsigned short, AnimationNode*> mNodesById;
	std::map<std::string, AnimationNode*> mNodesByName;
	bool mApplyMover;
	float mTotalWeight;
	mutable Skeleton* mCurSkel;
	mutable Skeleton::Situation mPrevSit;

	NodeFactory* mNodeFact;

};

}

#endif // __zhAnimationTree_h__
