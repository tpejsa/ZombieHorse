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

#ifndef __zhPostureIKSolver_h__
#define __zhPostureIKSolver_h__

#include "zhPrereq.h"
#include "zhIKSolver.h"

namespace zh
{

/**
* @brief Class representing an IK solver for positioning the figure root.
*/
class zhDeclSpec PostureIKSolver : public IKSolver
{

public:

	zhDeclare_IKSolver( PostureIKSolver, zhPostureIKSolver_ClassId, zhPostureIKSolver_ClassName )

	/**
	* Constructor.
	*/
	PostureIKSolver();

	/**
	* Destructor.
	*/
	~PostureIKSolver();

	/**
	* Compute the skeletal pose that meets the current IK goals.
	*/
	void solve();

protected:

};

}

#endif // __zhPostureIKSolver_h__
