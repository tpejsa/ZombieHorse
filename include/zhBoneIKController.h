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

#ifndef __zhBoneIKController_h__
#define __zhBoneIKController_h__

#include "zhPrereq.h"
#include "zhObjectFactory.h"
#include "zhEvent.h"
#include "zhValueSmoother.h"
#include "zhBoneController.h"
#include "zhCCDSolver.h"

namespace zh
{

class Vector3;
class Quat;

/**
* @brief BoneController which manipulates a bone chain
* using inverse kinematics.
*/
class zhDeclSpec BoneIKController : public BoneController
{

public:

	zhDeclare_BoneController( BoneIKController, zhBoneIKController_ClassId, zhBoneIKController_ClassName )

	/**
	* Constructor.
	*/
	BoneIKController();

	/**
	* Destructor.
	*/
	~BoneIKController();

	/**
	* Gets the target position of the end-effector (in world coordinates).
	*/
	virtual const Vector3& getTargetPosition() const;

	/**
	* Sets the target position of the end-effector (in world coordinates).
	*/
	virtual void setTargetPosition( const Vector3& pos ) const;

	/**
	* Gets the offset rotation that should be applied to the end-effector
	* relative to the target pose.
	*/
	virtual const Quat& getOffsetRotation() const;

	/**
	* Sets the offset rotation that should be applied to the end-effector
	* relative to the target pose.
	*/
	virtual void setOffsetRotation( const Quat& rot );

	/**
	* Gets a pointer to the IKSolver employed by this
	* BoneIKController.
	*/
	virtual IKSolver* getIKSolver() const;

	/**
	* Specifies which IKSolver this BoneIKController should use.
	*/
	virtual void setIKSolver( IKSolverClass solver );

	/**
	* Calculates the BoneController memory usage.
	*/
	size_t _calcMemoryUsage() const;

	/**
	* Unloads the BoneController, freeing up the memory it occupies.
	*/
	void _unload();

	/**
	* Creates a deep copy of the BoneController.
	*
	* @param clonePtr Pointer to the copy.
	* @param shareData Specifies if the clone
	* should share data with the original.
	*/
	void _clone( BoneController* clonePtr, bool shareData = false ) const;

protected:

	/**
	* Updates the bone controller's state. Any subclass
	* that wishes to implement its own update method should do so
	* by overriding this method.
	*
	* @param dt Elapsed time.
	*/
	void _update( float dt );

	/**
	* Applies this bone controller to the specified model.
	* Any subclass that wishes to implement its own apply method 
	* should do so by overriding this method.
	*
	* @param weight Influence of the bone controller upon the character pose
	* (0 - character pose unchanged, 1 - modified skeletal configuration
	* fully applied).
	*/
	void _applyController( float weight ) const;

	IKSolver* mSolver;
	Quat mOffsetRot;

};

}

#endif // __zhBoneIKController_h__
