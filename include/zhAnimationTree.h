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
#include "zhSmartPtr.h"
#include "zhObjectFactory.h"
#include "zhResourceManager.h"
#include "zhModel.h"
#include "zhAnimationSet.h"

#define zhAnimationTree_ClassId 2
#define zhAnimationTree_ClassName "AnimationTree"

namespace zh
{

class AnimationTree;
class AnimationTreeInstance;
class AnimationController;
class AnimationNode;
class BoneController;

typedef SmartPtr<AnimationTree> AnimationTreePtr;

/**
* @brief Animation tree resource.
*/
class zhDeclSpec AnimationTree : public Resource
{

public:

	typedef MapIterator< std::map<unsigned short, AnimationNode*> > NodeIterator;
	typedef MapConstIterator< std::map<unsigned short, AnimationNode*> > NodeConstIterator;
	typedef MapIterator< std::map<unsigned short, BoneController*> > BoneControllerIterator;
	typedef MapConstIterator< std::map<unsigned short, BoneController*> > BoneControllerConstIterator;

	/**
	* Constructor.
	*
	* @param id AnimationTree ID.
	* @param name AnimationTree name.
	* @param mgr Owning ResourceManager.
	*/
	AnimationTree( unsigned long id, const std::string& name, ResourceManager* mgr );

	/**
	* Destructor.
	*/
	~AnimationTree();

	/**
	* Gets the character model affected by this animation tree.
	*
	* @return Character model or NULL if no model is
	* associated with this anim. tree.
	*/
	virtual Model* getModel() const;

	/**
	* Sets the character model affected by this animation tree.
	*
	* @param mdl Character model or NULL if no model should be
	* associated with this anim. tree.
	*/
	virtual void setModel( Model* mdl );

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
	* Gets the first bone controller in the chain.
	*/
	virtual BoneController* getFirst() const;

	/**
	* Sets the first bone controller in the chain.
	*
	* @param id Bone controller ID.
	*/
	virtual void setFirst( unsigned short id );

	/**
	* Sets the first bone controller in the chain.
	*
	* @param name Bone controller name.
	*/
	virtual void setFirst( const std::string& name );

	/**
	* Sets the first bone controller in the chain.
	*
	* @param bc Bone controller or
	* NULL to unset current first.
	*/
	virtual void setFirst( BoneController* bc );

	/**
	* Gets the last bone controller in the chain.
	*/
	virtual BoneController* getLast() const;

	/**
	* Creates a new bone controller.
	*
	* @param classId Bone controller class ID.
	* @param id Bone controller ID.
	* @param name Bone controller name.
	* @return Pointer to the bone controller.
	*/
	virtual BoneController* createBoneController( unsigned long classId,
		unsigned short id, const std::string& name );

	/**
	* Deletes the specified bone controller.
	*
	* @param id Bone controller ID.
	*/
	virtual void deleteBoneController( unsigned short id );

	/**
	* Deletes the specified bone controller.
	*
	* @param name Bone controller name.
	*/
	virtual void deleteBoneController( const std::string& name );

	/**
	* Deletes all bone controllers in the animation tree.
	*/
	virtual void deleteAllBoneControllers();

	/**
	* Checks if the specified animation bone controller exists in the animation tree.
	*
	* @param id Bone controller ID.
	* @return true if the bone controller exists, false otherwise.
	*/
	virtual bool hasBoneController( unsigned short id ) const;

	/**
	* Checks if the specified bone controller exists in the animation tree.
	*
	* @param name Bone controller name.
	* @return true if the bone controller exists, false otherwise.
	*/
	virtual bool hasBoneController( const std::string& name ) const;

	/**
	* Gets a pointer to the specified bone controller.
	*
	* @param id Bone controller ID.
	* @return Pointer to specified bone controller
	* or NULL if the bone controller doesn't exist.
	*/
	virtual BoneController* getBoneController( unsigned short id ) const;

	/**
	* Gets a pointer to the specified animation bone controller.
	*
	* @param name BoneController name.
	* @return Pointer to specified bone controller
	* or NULL if the bone controller doesn't exist.
	*/
	virtual BoneController* getBoneController( const std::string& name ) const;

	/**
	* Gets the number of bone controllers in the animation tree.
	*/
	virtual unsigned int getNumBoneControllers() const;

	/**
	* Gets an iterator over the map of bone controllers
	* in the animation tree.
	*/
	virtual BoneControllerIterator getBoneControllerIterator();

	/**
	* Gets a const iterator over the map of bone controllers
	* in the animation tree.
	*/
	virtual BoneControllerConstIterator getBoneControllerConstIterator() const;

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
	* Applies animation in this instance to the specified character model.
	*
	* @param model Pointer to the Model that
	* this animation tree instance should apply to.
	*/
	virtual void apply() const;
	
	/**
	* Calculates the resource memory usage.
	*/
	size_t _calcMemoryUsage() const;

	/**
	* Unloads the resource, freeing up the memory it occupies.
	*/
	void _unload();

	/**
	* Creates a deep copy of the resource.
	*
	* @param clonePtr Pointer to the copy.
	*/
	void _clone( Resource* clonePtr ) const;

	/**
	* Updates the name of the specified animation node.
	*
	* @remark This function is not meant to be called directly.
	* To rename a node, use AnimationNode::setName().
	*/
	virtual void _renameNode( AnimationNode* node, const std::string& newName );

	/**
	* Updates the name of the specified bone controller.
	*
	* @remark This function is not meant to be called directly.
	* To rename a bone controller, use BoneController::setName().
	*/
	virtual void _renameBoneController( BoneController* boneCtrl, const std::string& newName );

	/**
	* Gets the cumulative blend weight of animations applied to the model
	* so far.
	*
	* @remark This property is useful for additive blending and is used
	* e.g. in AnimationSampler::_applyNode().
	*/
	virtual float _getTotalWeight() const;

	/**
	* Sets the cumulative blend weight of animations applied to the model
	* so far.
	*/
	virtual void _setTotalWeight( float weight );

	/**
	* Gets the current character situation.
	*/
	virtual const Model::Situation& _getPrevSituation() const;

	/**
	* Sets the current character situation.
	*/
	virtual void _setPrevSituation( const Model::Situation& sit );

protected:

	Model* mMdl;

	AnimationNode* mRoot;
	std::map<unsigned short, AnimationNode*> mNodesById;
	std::map<std::string, AnimationNode*> mNodesByName;

	BoneController* mFirst;
	std::map<unsigned short, BoneController*> mBoneControllersById;
	std::map<std::string, BoneController*> mBoneControllersByName;

	bool mApplyMover;

	float mTotalWeight;
	mutable Model::Situation mPrevSit;

};

/**
* @brief Class representing an animation tree instance.
*/
class zhDeclSpec AnimationTreeInstance : public AnimationTree
{

public:

	/**
	* Constructor.
	*
	* @param animTree Original animation tree resource.
	* @param ctrl Owning animation controller.
	*/
	AnimationTreeInstance( AnimationTreePtr animTree, AnimationController* ctrl );

	/**
	* Destructor.
	*/
	~AnimationTreeInstance();

	/**
	* Gets the original animation tree resource from which
	* this instance was created.
	*/
	virtual AnimationTreePtr getAnimationTree() const;

	/**
	* Gets the owning animation controller.
	*/
	virtual AnimationController* getController() const;

	/**
	* This override will never be called and always returns 0.
	*/
	size_t _calcMemoryUsage() const;

	/**
	* This override will never be called and does nothing.
	*/
	void _unload();

	/**
	* This override will never be called and does nothing.
	*/
	void _clone( Resource* clonePtr ) const;

protected:

	AnimationTreePtr mAnimTreeRes;
	AnimationController* mOwner;

};

}

#endif // __zhAnimationTree_h__
