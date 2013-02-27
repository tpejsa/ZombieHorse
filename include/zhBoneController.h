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

#ifndef __zhBoneController_h__
#define __zhBoneController_h__

#include "zhPrereq.h"
#include "zhIterators.h"
#include "zhObjectFactory.h"
#include "zhModel.h"
#include "zhAnimationTree.h"
#include "zhAnimationTreeManager.h"

#define zhBoneTransformController_ClassId 1
#define zhBoneTransformController_ClassName "BoneTransformController"
#define zhBoneIKController_ClassId 2
#define zhBoneIKController_ClassName "BoneIKController"

namespace zh
{

/**
* @brief Base class for bone controllers.
*/
class zhDeclSpec BoneController
{

	friend class AnimationTree;

public:

	zhDeclare_BaseClass( BoneController, 0, "BoneController", unsigned short )

	/**
	* Constructor.
	*/
	BoneController();

	/**
	* Destructor.
	*/
	virtual ~BoneController();

	/**
	* Gets the name of the bone controller.
	*/
	virtual const std::string& getName() const;

	/**
	* Sets the name of the bone controller.
	*/
	virtual void setName( const std::string& name );

	/**
	* Gets a pointer to the animation tree instance
	* which owns this bone controller.
	*/
	virtual AnimationTree* getAnimationTree() const;

	/**
	* Gets the previous bone controller in the chain.
	*/
	virtual BoneController* getPrevious() const;

	/**
	* Gets the next bone controller in the chain.
	*/
	virtual BoneController* getNext() const;

	/**
	* Sets the next bone controller in the chain.
	*/
	virtual void setNext( BoneController* next );

	/**
	* Gets the ID of the bone upon which this
	* BoneController acts.
	*/
	virtual unsigned short getBoneId() const;

	/**
	* Sets the ID of the bone upon which this
	* BoneController acts.
	*/
	virtual void setBoneId( unsigned short id );

	/**
	* Gets the activity status of the BoneController.
	*
	* @return true if the BoneController is enabled and active,
	* false otherwise.
	*/
	virtual bool getEnabled() const;

	/**
	* Sets the activity status of the BoneController.
	*
	* @param enabled If true, the BoneController will be enabled and active.
	*/
	virtual void setEnabled( bool enabled );

	/**
	* Gets the bone controller's response time
	* (i.e. the time it takes for the skeleton to
	* assume the target configuration upon activation of
	* the bone controller).
	*/
	virtual float getResponseTime() const;

	/**
	* Sets the bone controller's response time
	* (i.e. the time it takes for the skeleton to
	* assume the target configuration upon activation of
	* the bone controller).
	*/
	virtual void setResponseTime( float time );

	// TODO: implement transformation smoothing (for smooth motion instead of snapping)

	/**
	* Updates this bone controller with elapsed time.
	*/
	virtual void update( float dt );

	/**
	* Applies this bone controller to the current model's skeleton.
	*/
	virtual void apply() const;

	/**
	* Calculates the BoneController memory usage.
	*/
	virtual size_t _calcMemoryUsage() const;

	/**
	* Unloads the BoneController, freeing up the memory it occupies.
	*/
	virtual void _unload();

	/**
	* Creates a deep copy of the BoneController.
	*
	* @param clonePtr Pointer to the copy.
	* @param shareData Specifies if the clone
	* should share data with the original.
	*/
	virtual void _clone( BoneController* clonePtr, bool shareData = false ) const;

protected:

	virtual void _setAnimationTree( AnimationTree* treePtr );

	/**
	* Updates the bone controller's state. Any subclass
	* that wishes to implement its own update method should do so
	* by overriding this method.
	*
	* @param dt Elapsed time.
	*/
	virtual void _update( float dt );

	/**
	* Applies this bone controller to the specified model.
	* Any subclass that wishes to implement its own apply method 
	* should do so by overriding this method.
	*
	* @param weight Influence of the bone controller upon the character pose
	* (0 - character pose unchanged, 1 - modified skeletal configuration
	* fully applied).
	*/
	virtual void _applyController( float weight ) const;

	std::string mName;
	AnimationTree* mOwner;

	BoneController* mPrev;
	BoneController* mNext;

	unsigned short mBoneId;
	bool mEnabled;
	float mRespTime;
	float mWeight;

};

}

#endif // __zhBoneController_h__
