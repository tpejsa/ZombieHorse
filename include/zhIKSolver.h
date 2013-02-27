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

#ifndef __zhIKSolver_h__
#define __zhIKSolver_h__

#include "zhPrereq.h"
#include "zhMath.h"
#include "zhModel.h"

namespace zh
{

enum IKSolverClass
{
	IKSolver_CCD
};

/**
* @brief Abstract class representing an inverse kinematics solver.
*/
class zhDeclSpec IKSolver
{

public:

	/**
	* Constructor.
	*/
	IKSolver();

	/**
	* Destructor.
	*/
	virtual ~IKSolver();

	/**
	* Gets the IK solver class.
	*/
	virtual IKSolverClass getClassId() const = 0;

	/**
	* Gets the end-effector bone ID.
	*/
	virtual unsigned short getBoneId() const;

	/**
	* Sets the end-effector bone ID.
	*/
	virtual void setBoneId( unsigned short boneId );

	/**
	* Gets the target position of the end-effector bone
	* (in world coordinates).
	*/
	virtual const Vector3& getTargetPosition() const;

	/**
	* Gets the target position of the end-effector bone
	* (in world coordinates).
	*/
	virtual void setTargetPosition( const Vector3& pos );

	/**
	* Gets the IK chain length.
	*/
	virtual unsigned int getChainLength() const = 0;

	/**
	* Solves the skeletal configuration for the specified
	* end-effector position.
	*
	* @param mdl Affected model.
	* @param weight Influence of the bone controller upon the character pose
	* (0 - character pose unchanged, 1 - modified skeletal configuration
	* fully applied).
	*/
	virtual void solve( Model* mdl, float weight = 1.f ) = 0;

protected:

	unsigned short mBoneId;
	Vector3 mTargetPos;

};

}

#endif // __zhIKSolver_h__
