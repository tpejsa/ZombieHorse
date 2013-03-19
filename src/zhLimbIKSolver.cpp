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

#include "zhLimbIKSolver.h"
#include "zhSkeleton.h"

namespace zh
{

LimbIKSolver::LimbIKSolver() : mElbowAxis(Vector3::XAxis)
{
	mPriority = 10;
}

LimbIKSolver::~LimbIKSolver()
{
}

void LimbIKSolver::solve()
{
	// Using arm terminology here (for legs everything's identical)
	// Get all three joints and store the unadapted pose
	zhAssert( getNumBones() >= 3 );
	Bone* shoulder = getBone(0);
	Bone* elbow = getBone(1);
	Bone* wrist = getBone(2);
	Quat sq0 = shoulder->getOrientation();
	Quat eq0 = elbow->getOrientation();
	Quat wq0 = wrist->getOrientation();
	zhAssert( hasGoal(wrist->getId()) );
	const IKGoal& goal = getGoal(wrist->getId());

	// Flex elbow to make goal achievable
	Vector3 cur_n = wrist->getWorldPosition() - shoulder->getWorldPosition();
	Vector3 goal_n = goal.position - shoulder->getWorldPosition();
	float goal_l = goal_n.length(); // distance from shoulder to goal
	Vector3 cur_es = shoulder->getWorldPosition() - elbow->getWorldPosition();
	Vector3 cur_ew = wrist->getWorldPosition() - elbow->getWorldPosition();
	float les = cur_es.length();
	float lew = cur_ew.length();
	cur_es.normalize();
	cur_ew.normalize();
	// If elbow is completely extended or collapsed, we reuse previous rot. axis
	if( !zhEqualf( fabs(cur_es.dot(cur_ew)), 1 ) )
		mElbowAxis = cur_es.cross(cur_ew).getNormalized();
	float eth = cur_es.angle(cur_ew);
	float goal_coseth = ( les*les + lew*lew - goal_l*goal_l )/( 2.f*les*lew );
	float goal_eth = acos( zhClamp(goal_coseth,-1,1) );
	Quat eq = elbow->getWorldOrientation()*Quat( mElbowAxis, eth-goal_eth );
	elbow->setOrientation( shoulder->getWorldOrientation().getInverse()*eq );

	// Rotate shoulder to align wrist with goal
	cur_n = ( wrist->getWorldPosition() - shoulder->getWorldPosition() ).getNormalized();
	goal_n = goal_n.getNormalized();
	Quat sq = shoulder->getWorldOrientation()*goal_n.getRotationTo(cur_n);
	shoulder->setOrientation( shoulder->getParent()->getWorldOrientation().getInverse()*sq );

	// Compute shoulder swivel angle
	// TODO

	// Blend based on goal weight
	shoulder->setOrientation( sq0.slerp( shoulder->getOrientation(), goal.weight ) );
	elbow->setOrientation( eq0.slerp( elbow->getOrientation(), goal.weight ) );
}

}
