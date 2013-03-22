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

#ifndef __zhRootIKSolver_h__
#define __zhRootIKSolver_h__

#include "zhPrereq.h"
#include "zhIKSolver.h"

namespace zh
{

/**
* @brief Class representing an IK solver for positioning the figure root.
*/
class zhDeclSpec RootIKSolver : public IKSolver
{

public:

	struct Sphere
	{
		Vector3 center;
		float radius;

		Sphere( const Vector3& center, float radius )
		{
			this->center = center;
			this->radius = radius;
		}
	};

	struct Circle
	{
		Vector3 center;
		float radius;
		Vector3 normal;

		Circle( const Vector3& center, float radius, const Vector3& normal )
		{
			this->center = center;
			this->radius = radius;
			this->normal = normal;
		}
	};

	zhDeclare_IKSolver( RootIKSolver, zhRootIKSolver_ClassId, zhRootIKSolver_ClassName )

	/**
	* Constructor.
	*/
	RootIKSolver();

	/**
	* Destructor.
	*/
	~RootIKSolver();

	/**
	* Compute the skeletal pose that meets the current IK goals.
	*/
	void solve();

protected:

	bool _isPointInSphere( const Sphere& sphere, const Vector3& pt ) const;
	bool _isPointInSphereIntersection( const std::vector<Sphere>& spheres, const Vector3& pt ) const;
	Vector3 _findClosestPointOnSphere( const Sphere& sphere, const Vector3& pt0 ) const;
	bool _findClosestPointOnSphereIntersection( const std::vector<Sphere>& spheres,
		const Vector3& pt0, Vector3& pt ) const;
	void _computeSphereIntersectCircles( const std::vector<Sphere>& spheres, std::vector<Circle>& circles ) const;
	Vector3 _findClosestPointOnCircle( const Circle& circle, const Vector3& pt0 ) const;
	bool _findClosestPointOnSphereIntersectCircles( const std::vector<Sphere>& spheres, const std::vector<Circle>& circles,
		const Vector3& pt0, Vector3& pt ) const;
	void _computeSphereIntersectVertices( const std::vector<Circle>& circles, std::vector<Vector3>& vertices ) const;
	bool _findClosestPointOnSphereIntersectVertices( const std::vector<Sphere>& spheres, const std::vector<Vector3>& vertices,
		const Vector3& pt0, Vector3& pt ) const;
};

}

#endif // __zhRootIKSolver_h__
