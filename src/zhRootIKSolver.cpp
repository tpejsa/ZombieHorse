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

#include "zhRootIKSolver.h"
#include "zhSkeleton.h"

namespace zh
{

RootIKSolver::RootIKSolver()
{
	mPriority = 1;
}

RootIKSolver::~RootIKSolver()
{
}

void RootIKSolver::solve()
{
	zhAssert( getNumBones() > 0 );
	Bone* root = getBone(0);
	zhAssert( root->getSkeleton()->getRoot() == root );

	// Compute end-effector goal displacements
	Vector3 disp(0,0,0);
	float w = 0;
	GoalConstIterator goal_i = getGoalConstIterator();
	while( goal_i.hasMore() )
	{
		const IKGoal& goal = goal_i.next();
		Vector3 goal_wpos = goal.position;
		Vector3 cur_wpos = mSkel->getBone(goal.boneId)->getWorldPosition();
		disp += ( goal_wpos - cur_wpos )*goal.weight;
		w += goal.weight;
	}
	if( zhEqualf(w,0) )
		// None of the goals are important, nothing to solve here
		return;
	disp /= w;

	// Displace root to get the end-effectors closer to goals
	root->translate(disp);

	// TODO: compute intersections of end-effector reachable areas (spheres),
	// and project the displaced root into the intersection region
}

}
