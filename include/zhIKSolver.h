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
#include "zhIterators.h"
#include "zhObjectFactory.h"
#include "zhMath.h"
#include "zhLogger.h"

#define zhRootIKSolver_ClassId 1
#define zhRootIKSolver_ClassName "RootIKSolver"
#define zhPostureIKSolver_ClassId 2
#define zhPostureIKSolver_ClassName "PostureIKSolver"
#define zhLimbIKSolver_ClassId 3
#define zhLimbIKSolver_ClassName "LimbIKSolver"

#define zhDeclare_IKSolver( IKS, classId, className ) \
	zhDeclare_Class( IKSolver, IKS, classId, className, unsigned short )
#define zhRegister_IKSolver(IKS) \
	AnimationSystem::Instance()->_getIKSolverFactory().registerClass( IKS::ClassId(), IKS::ClassName(), &IKS::Create )
#define zhUnregister_IKSolver(IKS) \
	AnimationSystem::Instance()->_getIKSolverFactory().unregisterClass( IKS::ClassId() )

namespace zh
{

class Bone;
class Skeleton;

/**
* @brief Descriptor for specifying IK positional goals.
*/
struct IKGoal
{
	unsigned short boneId; ///< End effector ID.
	Vector3 position; ///< Goal position.
	float weight; ///< Goal weight/importance.

	IKGoal() : boneId(0), weight(0) { }

	IKGoal( unsigned short boneId, const Vector3& position, float weight )
	{
		this->boneId = boneId;
		this->position = position;
		this->weight = weight;
	}
};

/**
* @brief Base class for inverse kinematics solvers.
*/
class zhDeclSpec IKSolver
{

public:

	zhDeclare_BaseClass( IKSolver, 0, "IKSolver", unsigned short )

	typedef VectorIterator< std::vector<Bone*> > BoneIterator;
	typedef VectorConstIterator< std::vector<Bone*> > BoneConstIterator;
	typedef MapIterator< std::map<unsigned short, IKGoal> > GoalIterator;
	typedef MapConstIterator< std::map<unsigned short, IKGoal> > GoalConstIterator;

	/**
	* Constructor.
	*/
	IKSolver();

	/**
	* Destructor.
	*/
	virtual ~IKSolver();

	/**
	* Get the IK solver name.
	*/
	virtual const std::string& getName() const;

	/**
	* Get the owning skeleton.
	*
	* @return Pointer to the owning skeleton.
	*/
	virtual Skeleton* getSkeleton() const;

	/**
	* Check if the IK solver is enabled.
	*
	* @return true if the solver is enabled, false otherwise.
	*/
	virtual bool getEnabled() const;

	/**
	* Enable/disable the IK solver.
	*
	* @param enabled If true, the solver will be enabled, otherwise it will be disabled.
	*/
	virtual void setEnabled( bool enabled = true );

	/**
	* Add a bone to the end of the chain.
	*
	* @param bone Pointer to the bone.
	*/
	virtual void pushBone( Bone* bone );

	/**
	* Remove a bone from the end of the chain.
	*/
	virtual void popBone();

	/**
	* Get a bone in the IK chain.
	*
	* @param index Index of the bone in the chain.
	* @return Pointer to the bone.
	*/
	virtual Bone* getBone( unsigned int index ) const;

	/**
	* Get the number of bones in the chain.
	*/
	virtual unsigned int getNumBones() const;

	/**
	* Get an iterator over the vector of bones in the IK chain.
	*
	* @return Bone iterator.
	*/
	virtual BoneIterator getBoneIterator();

	/**
	* Get an iterator over the vector of bones in the IK chain.
	*
	* @return Bone iterator.
	*/
	virtual BoneConstIterator getBoneConstIterator() const;

	/**
	* Find a bone by ID.
	*
	* @param boneId Bone Id.
	* @return Pointer to the bone.
	*/
	virtual Bone* findBone( unsigned short boneId ) const;

	/**
	* Find a bone by name.
	*
	* @param boneName Bone name.
	* @return Pointer to the bone.
	*/
	virtual Bone* findBone( const std::string& boneName ) const;

	/**
	* Get the IK solver priority, which determines the order in which solvers
	* are executed.
	*
	* @remark Default value is 5, but can differ in a concrete IKSolver
	* implementation. For instance, RootIKSolver has default priority 1,
	* since it is typically executed first, while LimbIKSolver has default
	* priority 10, since it is typically executed last.
	*/
	virtual unsigned short getPriority() const;

	/**
	* Set the IK solver priority, which determines the order in which solvers
	* are executed.
	*
	* @remark Default value is 5, but can differ in a concrete IKSolver
	* implementation. For instance, RootIKSolver has default priority 1,
	* since it is typically executed first, while LimbIKSolver has default
	* priority 10, since it is typically executed last.
	*/
	virtual void setPriority( unsigned short priority );

	/**
	* Check if a positional goal is defined on the specified end-effector.
	*
	* @param boneId End-effector ID.
	* @return true if a goal exists, false otherwise.
	*/
	virtual bool hasGoal( unsigned short boneId ) const;

	/**
	* Get a positional goal from the IK solver.
	*
	* @param boneId End-effector ID.
	* @return Goal descriptor.
	*/
	virtual const IKGoal& getGoal( unsigned int boneId ) const;

	/**
	* Set a positional goal on the specified end-effector.
	*
	* @param Goal descriptor.
	*/
	virtual void setGoal( const IKGoal& goal );

	/**
	* Remove a positional goal from the specified end-effector.
	*
	* @param boneId End-effector ID.
	*/
	virtual void removeGoal( unsigned int boneId );

	/**
	* Remove all positional goals from the IK solver.
	*/
	virtual void removeAllGoals();

	/**
	* Get an iterator  over the map of positional goals.
	*/
	virtual GoalConstIterator getGoalConstIterator() const;

	/**
	* Get an iterator  over the map of positional goals.
	*/
	virtual GoalIterator getGoalIterator();

	/**
	* Update the goal position and weight for the specified end-effector.
	*
	* @param boneId End-effector ID.
	* @param pos New goal position.
	* @param weight New goal weight.
	*/
	virtual void updateGoal( unsigned int boneId, const Vector3& pos, float weight );

	/**
	* Get distance between the goal position and the actual
	* position of the corresponding end-effector.
	*
	* @param boneId End-effector ID.
	* @return Distance between goal position and end-effector.
	* @remark After calling solve, this can be used to check how successful
	* the solver was in meeting the specified goals.
	*/
	virtual float getGoalError( unsigned int boneId ) const;

	/**
	* Get cumulative distance between goal positions and actual
	* positions of the corresponding end-effectors.
	*
	* @return Distance between goal positions and end-effectors.
	* @remark After calling solve, this can be used to check how successful
	* the solver was in meeting the specified goals.
	*/
	virtual float getTotalGoalError() const;

	/**
	* Compute the skeletal pose that meets the current IK goals.
	*/
	virtual void solve() = 0;

	virtual void _init( const std::string& name, const std::vector<Bone*>& chain,
		Skeleton* skel ); ///< Initialize the IK solver (called only by Skeleton class).

protected:

	std::string mName;
	std::vector<Bone*> mChain;
	Skeleton* mSkel;
	bool mEnabled;
	unsigned short mPriority;
	std::map<unsigned short, IKGoal> mGoals;
};

}

#endif // __zhIKSolver_h__
