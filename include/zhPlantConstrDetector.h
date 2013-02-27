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

#ifndef __zhPlantConstrDetector_h__
#define __zhPlantConstrDetector_h__

#include "zhPrereq.h"
#include "zhIKSolver.h"

namespace zh
{

class Model;
class Animation;

/**
* @brief Class that provides an implementation of the algorithm
* for detection of end-effector plant constraints
* in animation clips and performs optional footskate cleanup.
*/
class zhDeclSpec PlantConstrDetector
{

public:

	/**
	* Constructor.
	*
	* @param mdl Pointer to the character model.
	* @param anim Pointer to 
	*/
	PlantConstrDetector( Model* mdl, Animation* anim );

	/**
	* Destructor.
	*/
	virtual ~PlantConstrDetector();

	/**
	* Gets the pointer to the character model.
	*/
	Model* getModel() const { return mMdl; }

	/**
	* Gets the pointer to the animation.
	*/
	Animation* getAnimation() const { return mAnim; }

	/**
	* Gets the maximum end-effector position gradient.
	*/
	float getMaxPosChange() const { return mMaxPosChange; }

	/**
	* Sets the maximum end-effector position gradient.
	* @remark If end-effector moves in space by less than this value
	* across two frames, then it is planted.
	*/
	void setMaxPosChange( float posChange = 0.1f ) { mMaxPosChange = posChange; }

	/**
	* Gets the minimum length of a plant constraint.
	*/
	float getMinConstrLength() const { return mMinConstrLength; }

	/**
	* Sets the minimum length of a plant constraint.
	* @remark The detector will generate a plant constraint only
	* if end-effector is planted for longer than this value.
	*/
	void setMinConstrLength( float constrLength = 1.f ) { mMinConstrLength = constrLength; }

	/**
	* Detects plant constraints in the animation.
	*
	* @param boneId End-effector bone ID.
	*/
	virtual void detect( unsigned short boneId );

	/**
	* Detects plant constraints in the animation.
	*
	* @param boneName End-effector bone name.
	*/
	virtual void detect( const std::string& boneName );

	/**
	* Gets the IK solver that is used for
	* footskate cleanup.
	*/
	IKSolver* getIKSolver() const;

	/**
	* Sets the IK solver that is used for
	* footskate cleanup.
	*/
	void setIKSolver( IKSolverClass solver );

	/**
	* Cleans up footskate artifacts in the animation.
	*/
	virtual void cleanup();

protected:

	void _createPlantConstr( float startTime, float endTime, unsigned short boneId );

	Model* mMdl;
	Animation* mAnim;

	float mMaxPosChange;
	float mMinConstrLength;

	IKSolver* mSolver;

};

}

#endif // __zhPlantConstrDetector_h__


