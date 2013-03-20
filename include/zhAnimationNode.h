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

#ifndef __zhAnimationNode_h__
#define __zhAnimationNode_h__

#include "zhPrereq.h"
#include "zhIterators.h"
#include "zhObjectFactory.h"
#include "zhSkeleton.h"
#include "zhAnimation.h"
#include "zhAnimationNodeEvents.h"
#include "zhAnimationAdaptor.h"

#define zhAnimationSampleNode_ClassId 1
#define zhAnimationSampleNode_ClassName "AnimationSampleNode"
#define zhAnimationBlendNode_ClassId 2
#define zhAnimationBlendNode_ClassName "AnimationBlendNode"
#define zhAnimationQueueNode_ClassId 3
#define zhAnimationQueueNode_ClassName "AnimationQueueNode"

#define zhDeclare_AnimationNode( AN, classId, className ) \
	zhDeclare_Class( AnimationNode, AN, classId, className, unsigned short )
#define zhRegister_AnimationNode( AN ) \
	AnimationSystem::Instance()->_getAnimationNodeFactory().registerClass( AN::ClassId(), AN::ClassName(), &AN::Create )
#define zhUnregister_AnimationNode( AN ) \
	AnimationSystem::Instance()->_getAnimationNodeFactory().unregisterClass( AN::ClassId() )

namespace zh
{

class AnimationTree;

/**
* @brief Base class for animation tree nodes.
*/
class zhDeclSpec AnimationNode : public TransitionAnnotEmitter,
	public ParamTransitionAnnotEmitter,
	public PlantConstraintAnnotEmitter,
	public SimEventAnnotEmitter
{

	friend class AnimationTree;

public:

	typedef MapIterator< std::map<unsigned short, AnimationNode*> > ChildIterator;
	typedef MapConstIterator< std::map<unsigned short, AnimationNode*> > ChildConstIterator;

	zhDeclare_BaseClass( AnimationNode, 0, "AnimationNode", unsigned short )

	/**
	* Constructor.
	*/
	AnimationNode();

	/**
	* Destructor.
	*/
	virtual ~AnimationNode();

	/**
	* Gets the name of the animation node.
	*/
	virtual const std::string& getName() const;

	/**
	* Sets the name of the animation node.
	*/
	virtual void setName( const std::string& name );

	/**
	* Gets a pointer to the animation tree instance
	* which owns this animation node.
	*/
	virtual AnimationTree* getAnimationTree() const;

	/**
	* Returns true if the current animation node is a leaf
	* (and therefore accepts no child nodes) or false otherwise.
	*
	* This pure virtual function should be implemented
	* in concrete animation node classes.
	*/
	virtual bool isLeaf() const = 0;

	/**
	* Gets the node's parent.
	*/
	virtual AnimationNode* getParent() const;

	/**
	* Attaches a node to this one as a child node.
	*/
	virtual void addChild( AnimationNode* node );

	/**
	* Removes one of this node's children.
	* 
	* @param id Child node ID.
	* @remark The node won't be deleted - to actually delete it,
	* use AnimationTree::deleteNode().
	*/
	virtual void removeChild( unsigned short id );

	/**
	* Removes one of this node's children.
	* 
	* @param name Child node name.
	* @remark The node won't be deleted - to actually delete it,
	* use AnimationTree::deleteNode().
	*/
	virtual void removeChild( const std::string& name );

	/**
	* Removes all of this node's children.
	*
	* @remark The nodes won't be deleted - to actually delete them,
	* use AnimationTree::deleteNode().
	*/
	virtual void removeAllChildren();

	/**
	* Detaches the specified child node from this node
	* and attaches it to another node.
	*
	* @param id Child node ID.
	*/
	virtual void moveChild( unsigned short id, AnimationNode* node );

	/**
	* Detaches the specified child node from this node
	* and attaches it to another node.
	*
	* @param name Child node name.
	*/
	virtual void moveChild( const std::string& name, AnimationNode* node );

	/**
	* Returns true if the specified node is the child of this node,
	* otherwise false.
	*
	* @param id Child node ID.
	* @return true if the child node exists, otherwise false.
	*/
	virtual bool hasChild( unsigned short id ) const;

	/**
	* Returns true if the specified node is the child of this node,
	* otherwise false.
	*
	* @param name Child node name.
	* @return true if the child node exists, otherwise false.
	*/
	virtual bool hasChild( const std::string& name ) const;

	/**
	* Gets the specified child node.
	*
	* @param id Child node ID.
	* @return Pointer to the child node or NULL if the child node
	* does not exist.
	*/
	virtual AnimationNode* getChild( unsigned short id ) const;

	/**
	* Gets the specified child node.
	*
	* @param name Child node name.
	* @return Pointer to the child node or NULL if the child node
	* does not exist.
	*/
	virtual AnimationNode* getChild( const std::string& name ) const;

	/**
	* Gets the number of the node's children.
	*/
	virtual unsigned int getNumChildren() const;

	/**
	* Gets an iterator over the map of children.
	*/
	virtual ChildIterator getChildIterator();

	/**
	* Gets an iterator over the map of children.
	*/
	virtual ChildConstIterator getChildConstIterator() const;

	/**
	* Gets the main child node.
	*/
	virtual AnimationNode* getMainChild() const;

	/**
	* Sets the specified child node as
	* the main child.
	*/
	virtual void setMainChild( unsigned short id );

	/**
	* Sets the specified child node as
	* the main child.
	*/
	virtual void setMainChild( const std::string& name );

	/**
	* Sets the specified child node as
	* the main child.
	*/
	virtual void setMainChild( AnimationNode* node );

	/**
	* Create an animation adaptor on this node.
	*
	* @param origSkel Skeleton for the original motion, played/synthesized
	* by this animation node's subtree.
	* @return Pointer to the adaptor.
	*/
	virtual AnimationAdaptor* createAdaptor( Skeleton* origSkel );

	/**
	* Delete the animation adaptor on this node.
	*/
	virtual void deleteAdaptor();

	/**
	* Get the animation adaptor on this node.
	*
	* @return Pointer to the adaptor, or NULL if adaptor does not exist.
	*/
	virtual AnimationAdaptor* getAdaptor() const;

	/**
	* Check if animation adaptation is enabled on this node
	* (otherwise original animation is applied directly to the target
	* skeleton).
	*
	* @return true if adaptation is enabled, false otherwise.
	*/
	virtual bool getAdaptationEnabled() const;

	/**
	* Enable/disable animation adaptation on this node.
	*
	* @param enabled If true, adaptation will be enabled.
	*/
	virtual void setAdaptationEnabled( bool enabled = true );

	/**
	* Gets the play status of this animation node and its children.
	*
	* @return true if this animation node is playing, false otherwise.
	*/
	virtual bool getPlaying() const;

	/**
	* Sets the play status of this animation node and its children.
	*
	* @param play If true, this animation node will be playing.
	*/
	virtual void setPlaying( bool playing = true );

	/**
	* Gets the pause status of this animation node and its children.
	*
	* @return true if this animation node is paused, false otherwise.
	*/
	virtual bool getPaused() const;

	/**
	* Sets the pause status of this animation node and its children.
	*
	* @param pause If true, this animation node will be paused.
	*/
	virtual void setPaused( bool paused = true );

	/**
	* Gets the current play time of this animation node.
	*/
	virtual float getPlayTime() const;

	/**
	* Sets the current play time of this animation node.
	*/
	virtual void setPlayTime( float time );

	/**
	* Gets the current play time of this animation node,
	* normalized to range 0 to 1.
	*/
	virtual float getNormalizedPlayTime() const;

	/**
	* Sets the current play time of this animation node,
	* normalized to range 0 to 1.
	*/
	virtual void setNormalizedPlayTime( float time );

	/**
	* Gets the playback rate of this animation node.
	*/
	virtual float getPlayRate() const;

	/**
	* Sets the playback rate of this animation node.
	*/
	virtual void setPlayRate( float rate );

	/**
	* Gets the total playback length of this animation node.
	*/
	virtual float getPlayLength() const;

	/**
	* Gets the annotation event triggering status of this animation node.
	*
	* @return true if annotations are enabled, false otherwise.
	*/
	virtual bool getAnnotationsEnabled() const;

	/**
	* Sets the annotation event triggering status for this animation node.
	*
	* @enabled If true, annotations will be enabled.
	*/
	virtual void setAnnotationsEnabled( bool enabled = true );

	/**
	* Gets the container of transition annotations.
	*/
	virtual TransitionAnnotationContainer* getTransitionAnnotations() const;

	/**
	* Gets the container of param. transition annotations.
	*/
	virtual ParamTransitionAnnotationContainer* getParamTransitionAnnotations() const;

	/**
	* Gets the container of plant constraint annotations.
	*/
	virtual PlantConstraintAnnotationContainer* getPlantConstraintAnnotations() const;

	/**
	* Gets the container of simulation event annotations.
	*/
	virtual SimEventAnnotationContainer* getSimEventAnnotations() const;

	/**
	* Gets this animation's origin in space.
	*
	* @remark This property enables the animation to be
	* translated and rotated in space.
	*/
	virtual const Skeleton::Situation& getOrigin() const;

	/**
	* Sets this animation's origin in space.
	*/
	virtual void setOrigin( const Skeleton::Situation& origin );

	/**
	* Masks the specified bone, setting its blend weight to 0.
	*/
	virtual void maskBone( unsigned short boneId );

	/**
	* Unmasks the specified bone.
	*/
	virtual void unmaskBone( unsigned short boneId );

	/**
	* Unmasks all bones.
	*/
	virtual void unmaskAllBones();

	/**
	* Returns true if the specified bone is masked, otherwise false.
	*/
	virtual bool isBoneMasked( unsigned short boneId );

	/**
	* Gets the bone mask used on this animation node.
	*/
	virtual const std::set<unsigned short>& getBoneMask() const;

	/**
	* Updates this animation node with elapsed time.
	*
	* @param dt Elapsed time.
	*/
	virtual void update( float dt );

	/**
	* Applies this animation node to the current model.
	*
	* @param weight Blend weight.
	* @param boneMask Bone mask. Animation is not applied to masked bones.
	*/
	virtual void apply( float weight = 1.f, const std::set<unsigned short>& boneMask = Animation::EmptyBoneMask ) const;

	/**
	* Get the previous play time of this animation node.
	*/
	virtual float _getPrevTime() const;

	/**
	* Get the delta time of the current frame.
	*/
	virtual float _getDeltaTime() const;

	/**
	* Samples this animation's mover channel at the current time.
	*
	* @return Mover value (with origin transformation applied)
	* @remark This is a helper method used for alignment of consecutive animations.
	* Any node that represents an animation with a mover channel
	* should override this method, otherwise alignment won't work correctly.
	*/
	virtual Skeleton::Situation _sampleMover() const;

	/**
	* Computes the origin that realigns this animation to the specified character situation.
	*
	* @return Mover value (with origin transformation applied).
	* @remark This is a helper function used for alignment of consecutive animations.
	*/
	virtual Skeleton::Situation _getRealignedOrigin( const Skeleton::Situation& sit ) const;

	/**
	* Creates a deep copy of the AnimationNode.
	*
	* @param clonePtr Pointer to the copy.
	* @param shareData Specifies if the clone
	* should share data with the original.
	*/
	virtual void _clone( AnimationNode* clonePtr, bool shareData = false ) const;

protected:

	/**
	* Updates this animation node's playback state. Any subclass
	* that wishes to implement its own update method should do so
	* by overriding this method.
	*
	* @param dt Elapsed time.
	*/
	virtual void _updateNode( float dt );

	/**
	* Applies this animation node to the current model.
	* Any subclass that wishes to implement its own apply method 
	* should do so by overriding this method.
	*
	* @param weight Blend weight.
	* @param boneMask Bone mask. Animation is not applied to masked bones.
	*/
	virtual void _applyNode( float weight = 1.f, const std::set<unsigned short>& boneMask = Animation::EmptyBoneMask ) const;

	/**
	* Determines which annotations are active, which are no longer active,
	* and handles each case by emitting appropriate events.
	* Any subclass that wishes to handle annotations differently should do so
	* by overriding this method.
	*/
	virtual void _applyAnnotations() const;

	std::string mName; ///< init'ed by AnimationTree::createNode()
	AnimationTree* mOwner; ///< init'ed by AnimationTree::createNode()
	AnimationNode* mParent;
	std::map<unsigned short, AnimationNode*> mChildrenById;
	std::map<std::string, AnimationNode*> mChildrenByName;
	mutable AnimationNode* mMainChild;
	AnimationAdaptor* mAnimAdaptor;
	bool mAdaptEnabled;

	bool mPlaying;
	bool mPaused;
	float mPlayRate;

	bool mAnnotsEnabled;
	TransitionAnnotationContainer* mTransAnnots;
	ParamTransitionAnnotationContainer* mParamTransAnnots;
	PlantConstraintAnnotationContainer* mPlantConstrAnnots;
	SimEventAnnotationContainer* mSimEventAnnots;

	std::set<unsigned short> mBoneMask;

	float mDt;

};

}

#endif // __zhAnimationNode_h__
