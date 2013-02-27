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

#ifndef __zhCCDSolver_h__
#define __zhCCDSolver_h__

#include "zhPrereq.h"
#include "zhIKSolver.h"

namespace zh
{

/**
* @brief Simple inverse kinematics solver employing
* the cyclic coordinate descent (CCD) technique.
*/
class zhDeclSpec CCDSolver : public IKSolver
{

public:

	/**
	* Constructor.
	*/
	CCDSolver();

	/**
	* Destructor.
	*/
	~CCDSolver();

	/**
	* Gets the IK solver class.
	*/
	IKSolverClass getClassId() const { return IKSolver_CCD; }

	/**
	* Gets the IK chain length.
	*/
	unsigned int getChainLength() const;

	/**
	* Sets the IK chain length.
	*
	* @remark CCD method works best for short chains (1-3 links).
	*/
	void setChainLength( unsigned int length = 2 );

	/**
	* Gets the maximum angle change that the solver
	* may make on bones.
	*/
	virtual float getMaxAngleChange() const;

	/**
	* Sets the maximum angle change that the solver
	* may make on bones.
	*
	* @remark Restricting angle changes to a reasonable value
	* can help avoid unnatural bone configurations.
	*/
	virtual void setMaxAngleChange( float change = zhPI4 );

	/**
	* Gets the maximum number of iterations for solving
	* the limb configuration.
	*/
	virtual unsigned int getNumIterations() const;

	/**
	* Sets the maximum number of iterations for solving
	* the limb configuration.
	*/
	virtual void setNumIterations( unsigned int numIters = zhIK_CCDNumIters );

	/**
	* Solves the skeletal configuration for the specified
	* end-effector position.
	*
	* @param mdl Affected model.
	*/
	void solve( Model* mdl, float weight = 1.f );

protected:

	unsigned int mChainLength;
	float mMaxAngleChange;
	unsigned int mNumIters;

};

}

#endif // __zhCCDSolver_h__
