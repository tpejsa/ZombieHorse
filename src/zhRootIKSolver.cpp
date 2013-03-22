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

	//
	return;
	//
	// Compute end-effector reachable areas (spheres)
	Vector3 root_pos = root->getWorldPosition();
	std::vector<Sphere> spheres;
	std::vector<float> sphere_weights;
	goal_i = getGoalConstIterator();
	while( goal_i.hasMore() )
	{
		const IKGoal& goal = goal_i.next();
		if( zhEqualf(goal.weight,0) )
			continue;

		Bone* wrist = mSkel->getBone(goal.boneId);
		Bone* elbow = wrist->getParent();
		Bone* shoulder = elbow->getParent();
		Vector3 center = goal.position + root_pos - shoulder->getWorldPosition();
		float r = wrist->getWorldPosition().distance( elbow->getWorldPosition() ) + 
			shoulder->getWorldPosition().distance( elbow->getWorldPosition() );
		r /= goal.weight;
		spheres.push_back( Sphere(center, r) );
		sphere_weights.push_back(goal.weight);
	}

	// Is the root in the reachable area?
	size_t num_contain = 0;
	for( std::vector<Sphere>::const_iterator sphere_i = spheres.begin();
		sphere_i != spheres.end(); ++sphere_i )
	{
		if( _isPointInSphere(*sphere_i, root_pos) )
			++num_contain;
	}
	if( num_contain == spheres.size() )
		// All goals are reachable
		return;

	// TODO: compute intersections of end-effector reachable areas (spheres),
	// and project the displaced root into the intersection region
}

bool RootIKSolver::_isPointInSphere( const Sphere& sphere, const Vector3& pt ) const
{
	return (sphere.center - pt).lengthSq() <= sphere.radius*sphere.radius + 0.00001f;
}

bool RootIKSolver::_isPointInSphereIntersection( const std::vector<Sphere>& spheres,
	const Vector3& pt ) const
{
	for( std::vector<Sphere>::const_iterator sphere_i = spheres.begin();
		sphere_i != spheres.end(); ++sphere_i )
	{
		if( !_isPointInSphere(*sphere_i, pt) )
			return false;
	}

	return true;
}

Vector3 RootIKSolver::_findClosestPointOnSphere( const Sphere& sphere, const Vector3& pt0 ) const
{
	return ( sphere.center + (pt0 - sphere.center).getNormalized()*sphere.radius );
}

bool RootIKSolver::_findClosestPointOnSphereIntersection( const std::vector<Sphere>& spheres,
	const Vector3& pt0, Vector3& pt ) const
{
	std::vector<Vector3> closest_pts;
	for( std::vector<Sphere>::const_iterator sphere_i = spheres.begin();
		sphere_i != spheres.end(); ++sphere_i )
		closest_pts.push_back( _findClosestPointOnSphere(*sphere_i, pt0) );

	float min_dist = FLT_MAX;
	bool found = false;
	for( int pti = 0; pti < (int)spheres.size(); ++pti )
	{
		Vector3 cpt = closest_pts[pti];

		size_t num_contain = 0;
		for( std::vector<Sphere>::const_iterator sphere_j = spheres.begin();
			sphere_j != spheres.end(); ++sphere_j )
		{
			if( _isPointInSphere(*sphere_j, cpt) )
				++num_contain;
		}

		if( num_contain == spheres.size() )
		{
			found = true;
			float dist = pt0.distance(cpt);
			if( dist < min_dist )
			{
				pt = cpt;
				min_dist = dist;
			}
		}
	}

	return found;
}

void RootIKSolver::_computeSphereIntersectCircles( const std::vector<Sphere>& spheres, std::vector<Circle>& circles ) const
{
}

Vector3 RootIKSolver::_findClosestPointOnCircle( const Circle& circle, const Vector3& pt0 ) const
{
	return Vector3();
}

bool RootIKSolver::_findClosestPointOnSphereIntersectCircles( const std::vector<Sphere>& spheres, const std::vector<Circle>& circles,
	const Vector3& pt0, Vector3& pt ) const
{
	return false;
}

void RootIKSolver::_computeSphereIntersectVertices( const std::vector<Circle>& circles, std::vector<Vector3>& vertices ) const
{
}

bool RootIKSolver::_findClosestPointOnSphereIntersectVertices( const std::vector<Sphere>& spheres, const std::vector<Vector3>& vertices,
	const Vector3& pt0, Vector3& pt ) const
{
	return false;
}

}
