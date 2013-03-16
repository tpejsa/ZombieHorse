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

#include "zhIKSolver.h"
#include "zhSkeleton.h"

namespace zh
{

IKSolver::IKSolver() : mSkel(NULL), mPriority(5)
{
}

IKSolver::~IKSolver()
{
}

const std::string& IKSolver::getName() const
{
	return mName;
}

Skeleton* IKSolver::getSkeleton() const
{
	return mSkel;
}

Bone* IKSolver::getBone( unsigned int index ) const
{
	zhAssert( index < getNumBones() );

	return mChain[(size_t)index];
}

unsigned int IKSolver::getNumBones() const
{
	return (unsigned int)mChain.size();
}

IKSolver::BoneIterator IKSolver::getBoneIterator()
{
	return BoneIterator(mChain);
}

IKSolver::BoneConstIterator IKSolver::getBoneConstIterator() const
{
	return BoneConstIterator(mChain);
}

unsigned short IKSolver::getPriority() const
{
	return mPriority;
}

void IKSolver::setPriority( unsigned short priority )
{
	mPriority = priority;
}

bool IKSolver::hasGoal( unsigned short boneId ) const
{
	zhAssert( mSkel->hasBone(boneId) );

	return mGoals.count(boneId) > 0;
}

const IKGoal& IKSolver::getGoal( unsigned int boneId ) const
{
	zhAssert( mSkel->hasBone(boneId) );
	zhAssert( hasGoal(boneId) );

	return mGoals.find(boneId)->second;
}

void IKSolver::setGoal( const IKGoal& goal )
{
	zhAssert( mSkel->hasBone(goal.boneId) );

	mGoals[goal.boneId] = goal;
}

void IKSolver::removeGoal( unsigned int boneId )
{
	zhAssert( mSkel->hasBone(boneId) );
	
	mGoals.erase(boneId);
}

void IKSolver::removeAllGoals()
{
	mGoals.clear();
}

IKSolver::GoalConstIterator IKSolver::getGoalConstIterator() const
{
	return GoalConstIterator(mGoals);
}

IKSolver::GoalIterator IKSolver::getGoalIterator()
{
	return GoalIterator(mGoals);
}

void IKSolver::updateGoal( unsigned int boneId, const Vector3& pos, float weight )
{
	zhAssert( hasGoal(boneId) );

	mGoals[boneId] = IKGoal( boneId, pos, weight );
}

float IKSolver::getGoalError( unsigned int boneId ) const
{
	return getGoal(boneId).position.distance( mSkel->getBone(boneId)->getWorldPosition() );
}

float IKSolver::getTotalGoalError() const
{
	float err = 0;

	GoalConstIterator goal_i = getGoalConstIterator();
	while( goal_i.hasMore() )
		err += getGoalError( goal_i.next().boneId );

	return err;
}

void IKSolver::_init( const std::string& name, const std::vector<Bone*>& chain,
	Skeleton* skel )
{
	zhAssert( !chain.empty() );
	zhAssert( skel != NULL );

	mName = name;
	mChain = chain;
	mSkel = skel;
}

}
