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

	Vector3 root_pos1 = root_pos;
	while( !spheres.empty() )
	{
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
			break;

		// Try finding new root position on spheres
		if( _findClosestPointOnSphereIntersection(spheres, root_pos, root_pos1) )
			break;

		// Try finding new root position on circles (sphere-sphere intersections)
		std::vector<Circle> circles;
		_computeSphereIntersectCircles(spheres, circles);
		if( _findClosestPointOnSphereIntersectCircles(spheres, circles, root_pos, root_pos1) )
			break;
		
		// Try finding new root position on vertices (multi-sphere intersections)
		std::vector<Vector3> vertices;
		_computeSphereIntersectVertices(spheres, vertices);
		if( _findClosestPointOnSphereIntersectVertices(spheres, vertices, root_pos, root_pos1) )
			break;

		// Discard sphere with lowest end-effector weight
		size_t min_i;
		float min_w = FLT_MAX;
		for( size_t sphere_i = 0; sphere_i < spheres.size(); ++sphere_i )
		{
			if( sphere_weights[sphere_i] < min_w )
			{
				min_i = sphere_i;
				min_w = sphere_weights[sphere_i];
			}
		}
		spheres.erase( spheres.begin() + min_i );
		sphere_weights.erase( sphere_weights.begin() + min_i );
	}

	root->setPosition(root_pos1);
}

bool RootIKSolver::_isPointInSphere( const Sphere& sphere, const Vector3& pt ) const
{
	return (sphere.center - pt).lengthSq() <= sphere.radius*sphere.radius + 0.00001f;
}

bool RootIKSolver::_compute2SphereIntersection( const Sphere& sphere1,
	const Sphere& sphere2,
	Circle& circle ) const
{
	float d = sphere1.center.distance(sphere2.center);
	if( d > sphere1.radius+sphere2.radius )
		// Spheres don't intersect
		return false;

	// Compute circle radius
	float d_2 = d*d;
	float r1 = sphere1.radius;
	float r1_2 = r1*r1;
	float r2 = sphere2.radius;
	float r2_2 = r2*r2;
	float A = d_2 - r2_2 + r1_2;
	circle.radius = 1.f/(2.f*d)*sqrt( 4.f*d_2*r1_2 - A*A );

	// Compute circle normal
	circle.normal = (sphere2.center-sphere1.center).getNormalized();

	// Compute circle center
	circle.center = sphere1.center + circle.normal*(d_2 - r2_2 + r1_2)/(2.f*d);

	return true;
}

bool RootIKSolver::_compute3SphereIntersection( const Sphere& sphere1,
	const Sphere& sphere2, const Sphere& sphere3,
	Vector3& v1, Vector3& v2 ) const
{
	Circle circ1;
	if( !_compute2SphereIntersection(sphere1, sphere2, circ1) )
		// These spheres don't intersect
		return false;

	Circle circ2;
	if( !_computeSpherePlaneIntersection(sphere3, Plane(circ1.center, circ1.normal), circ2) )
		// These spheres don't intersect
		return false;

	Vector3 d = circ2.center - circ1.center;
	float r1 = circ1.radius;
	float r1_2 = r1*r1;
	float r2 = circ2.radius;
	float r2_2 = r2*r2;
	float ld = d.length();
	float ld_2 = ld*ld;
	float A = r1_2 - r2_2 + ld_2;
	float rv = sqrt( r1_2 - A*A/(4.f*ld_2) );
	Vector3 cv = d*A/(2.f*ld_2) + circ1.center;
	Vector3 u = circ1.normal.cross(d).getNormalized();

	v1 = cv + u*rv;
	v2 = cv - u*rv;

	return true;
}

bool RootIKSolver::_computeSpherePlaneIntersection( const Sphere& sphere,
	const Plane& plane, Circle& circle ) const
{
	Vector3 h = plane.normal * plane.normal.dot(plane.point - sphere.center);
	circle.center = sphere.center + h;
	float r_2 = sphere.radius*sphere.radius - h.lengthSq();
	if( r_2 < 0 )
		return false;
	circle.radius = sqrt(r_2);

	return true;
}

Vector3 RootIKSolver::_findClosestPointOnSphere( const Sphere& sphere, const Vector3& pt0 ) const
{
	return ( sphere.center + (pt0 - sphere.center).getNormalized()*sphere.radius );
}

Vector3 RootIKSolver::_findClosestPointOnCircle( const Circle& circle, const Vector3& pt0 ) const
{
	Vector3 pc = circle.center - pt0;
	Vector3 cp = -pc;
	Vector3 h = circle.normal*circle.normal.dot(pc);
	Vector3 pt0p = pt0+h;
	Vector3 cpt0p = pt0p - circle.center;
	float d = cpt0p.length();
	if( zhEqualf(d,0) )
		// Any point on the circle is valid
		return circle.center + Vector3::XAxis.cross(circle.normal)*circle.radius;
	
	return circle.center + cpt0p/d*circle.radius;
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

bool RootIKSolver::_findClosestPointOnSphereIntersection( const std::vector<Sphere>& spheres,
	const Vector3& pt0, Vector3& pt ) const
{
	std::vector<Vector3> closest_pts;
	for( std::vector<Sphere>::const_iterator sphere_i = spheres.begin();
		sphere_i != spheres.end(); ++sphere_i )
		closest_pts.push_back( _findClosestPointOnSphere(*sphere_i, pt0) );

	float min_dist = FLT_MAX;
	bool found = false;
	for( int pti = 0; pti < (int)closest_pts.size(); ++pti )
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

void RootIKSolver::_computeSphereIntersectCircles( const std::vector<Sphere>& spheres,
	std::vector<Circle>& circles ) const
{
	for( size_t sphere_i = 0; sphere_i < spheres.size(); ++sphere_i )
	{
		for( size_t sphere_j = sphere_i+1; sphere_j < spheres.size(); ++sphere_j )
		{
			Circle circle;
			if( _compute2SphereIntersection( spheres[sphere_i], spheres[sphere_j], circle ) )
				circles.push_back(circle);
		}
	}
}

bool RootIKSolver::_findClosestPointOnSphereIntersectCircles( const std::vector<Sphere>& spheres,
	const std::vector<Circle>& circles,
	const Vector3& pt0, Vector3& pt ) const
{
	std::vector<Vector3> closest_pts;
	for( std::vector<Circle>::const_iterator circle_i = circles.begin();
		circle_i != circles.end(); ++circle_i )
		closest_pts.push_back( _findClosestPointOnCircle(*circle_i, pt0) );

	float min_dist = FLT_MAX;
	bool found = false;
	for( int pti = 0; pti < (int)closest_pts.size(); ++pti )
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

void RootIKSolver::_computeSphereIntersectVertices( const std::vector<Sphere>& spheres,
	std::vector<Vector3>& vertices ) const
{
	for( size_t sphere_i = 0; sphere_i < spheres.size(); ++sphere_i )
	{
		for( size_t sphere_j = sphere_i+1; sphere_j < spheres.size(); ++sphere_j )
		{
			for( size_t sphere_k = sphere_j+1; sphere_k < spheres.size(); ++sphere_k )
			{
				Sphere sphere1 = spheres[sphere_i];
				Sphere sphere2 = spheres[sphere_j];
				Sphere sphere3 = spheres[sphere_k];
				Vector3 v1, v2;
				if( _compute3SphereIntersection(sphere1, sphere2, sphere3, v1, v2) )
				{
					vertices.push_back(v1);
					vertices.push_back(v2);
				}
			}
		}
	}
}

bool RootIKSolver::_findClosestPointOnSphereIntersectVertices( const std::vector<Sphere>& spheres,
	const std::vector<Vector3>& vertices,
	const Vector3& pt0, Vector3& pt ) const
{
	float min_dist = FLT_MAX;
	bool found = false;
	for( int pti = 0; pti < (int)vertices.size(); ++pti )
	{
		Vector3 cpt = vertices[pti];

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

}
