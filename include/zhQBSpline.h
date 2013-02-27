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

#ifndef __zhQBSpline_h__
#define __zhQBSpline_h__

#include "zhPrereq.h"
#include "zhMathMacros.h"
#include "zhVector.h"
#include "zhMatrix.h"
#include "zhVector3.h"
#include "zhVector2.h"
#include "zhQuat.h"

namespace zh
{

/**
* @brief Generic implementation of
* a uniform, quadratic, N-dimensional B-spline.
*/
template <class T>
class QBSpline
{

public:

	/**
	* Constructor.
	*/
	QBSpline()
	{
		// initialize const. matrix
		mCoeffs.set( 0, 0, 0.5f );
		mCoeffs.set( 0, 1, -1.f );
		mCoeffs.set( 0, 2, 0.5f );
		mCoeffs.set( 1, 0, -1.f );
		mCoeffs.set( 1, 1, 1.f );
		mCoeffs.set( 1, 2, 0.f );
		mCoeffs.set( 2, 0, 0.5f );
		mCoeffs.set( 2, 1, 0.5f );
		mCoeffs.set( 2, 2, 0.f );
	}

	/**
	* Destructor.
	*/
	~QBSpline()
	{
	}

	/**
	* Adds a control point to the spline.
	*/
	void addControlPoint( const T& pt )
	{
		mCtrlPoints.push_back(pt);
	}

	/**
	* Clears all control points on the spline.
	*/
	void clearControlPoints()
	{
		mCtrlPoints.clear();
	}

	/**
	* Gets a control point on the spline.
	*/
	const T& getControlPoint( unsigned int index ) const
	{
		zhAssert( index < getNumControlPoints() );

		return mCtrlPoints[index];
	}

	/**
	* Sets a control point on the spline.
	*/
	void setControlPoint( unsigned int index, const T& pt )
	{
		zhAssert( index < getNumControlPoints() );

		mCtrlPoints[index] = pt;
	}

	/**
	* Gets the number of control points on the spline.
	*/
	unsigned int getNumControlPoints() const
	{
		return mCtrlPoints.size();
	}

	/**
	* Gets the number of spline segments.
	*/
	unsigned int getNumSegments() const
	{
		return getNumControlPoints() - 2;
	}

	/**
	* Gets a point on the spline.
	*
	* @param t Interpolation parameter (in [0-1] range).
	* @return Interpolated point.
	*/
	T getPoint( float t ) const
	{
		unsigned int si;
		float st;

		// compute segment index and param.
		st = t * getNumSegments();
		si = unsigned int(st);
		if( si >= getNumSegments() )
			si = getNumSegments() - 1;
		st -= (float)si;

		return getPoint( si, st );
	}

	/**
	* Gets a point on the spline.
	*
	* @param segIndex Spline segment index.
	* @param t Interpolation parameter (in [0-1] range).
	* @return Interpolated point.
	*/
	T getPoint( unsigned int segIndex, float t ) const
	{
		if( segIndex >= getNumSegments() )
		{
			segIndex = getNumSegments() - 1;
			t = 1;
		}

		Vector3 tv( t*t, t, 1 );
		tv *= mCoeffs;

		return mCtrlPoints[segIndex] * tv.x + mCtrlPoints[segIndex+1] * tv.y + mCtrlPoints[segIndex+2] * tv.z;
	}

	/**
	* Gets a tangent on the spline.
	*
	* @param t Interpolation parameter (in [0-1] range).
	* @return Interpolated tangent.
	*/
	T getTangent( float t ) const
	{
		unsigned int si;
		float st;

		// compute segment index and param.
		st = t * getNumSegments();
		si = unsigned int(st);
		if( si >= getNumSegments() )
			si = getNumSegments() - 1;
		st -= (float)si;

		return getTangent( si, st );
	}

	/**
	* Gets a tangent on the spline.
	*
	* @param segIndex Spline segment index.
	* @param t Interpolation parameter (in [0-1] range).
	* @return Interpolated tangent.
	*/
	T getTangent( unsigned int segIndex, float t ) const
	{
		if( segIndex >= getNumSegments() )
		{
			segIndex = getNumSegments() - 1;
			t = 1;
		}

		Vector3 tv( 2*t, 1, 0 );
		tv *= mCoeffs;

		return mCtrlPoints[segIndex] * tv.x + mCtrlPoints[segIndex+1] * tv.y + mCtrlPoints[segIndex+2] * tv.z;
	}
	
	/**
	* Fits the spline to a set of data points using
	* a least-squares method.
	*/
	void fitToPoints( const std::vector<T>& samples )
	{
		zhAssert( samples.size() > 2 );

		mCtrlPoints.clear();

		// prepare sample points
		std::vector<T> samples1 = samples;
		samples1.insert( samples1.begin(), samples[1] - samples[0] );
		
		// initialize matrix of coefficients
		Matrix coeffs( samples1.size() );
		coeffs.set( 0, 0, -1.f );
		coeffs.set( 0, 1, 1.f );
		for( unsigned int sample_i = 1; sample_i < coeffs.size() - 1; ++sample_i )
		{
			coeffs.set( sample_i, sample_i - 1, 0.5f );
			coeffs.set( sample_i, sample_i, 0.5f );
		}
		coeffs.invert();

		// compute control points
		for( unsigned int cpi = 0; cpi < samples1.size(); ++cpi )
		{
			T cp = samples1[0];
			cp.null();

			for( unsigned int sample_i = 0; sample_i < samples1.size(); ++sample_i )
			{
				cp += samples1[sample_i] * coeffs.get( cpi, sample_i );
			}

			mCtrlPoints.push_back(cp);
		}

		// TODO: why is the last control point incorrect?
		mCtrlPoints[ mCtrlPoints.size() - 1 ] = mCtrlPoints[ mCtrlPoints.size() - 1 ] * 2.f - mCtrlPoints[ mCtrlPoints.size() - 2 ];
		//
	}

private:

	std::vector<T> mCtrlPoints; ///< Set of control points.
	Matrix4 mCoeffs; ///< Constant matrix.

};

}

#endif // __zhQBSpline_h__
