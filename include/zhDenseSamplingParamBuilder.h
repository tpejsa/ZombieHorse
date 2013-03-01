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

#ifndef __zhDenseSamplingParamBuilder_h__
#define __zhDenseSamplingParamBuilder_h__

#include "zhPrereq.h"
#include "zhParamAnimationBuilder.h"

namespace zh
{

class zhDeclSpec DenseSamplingParamBuilder : public ParamAnimationBuilder
{

public:

	/**
	* Constructor.
	*
	* @param skel Pointer to the character skeleton.
	* @param animSpace Pointer to the animation space.
	*/
	DenseSamplingParamBuilder( Skeleton* skel, AnimationSpace* animSpace );

	/**
	* Destructor.
	*/
	~DenseSamplingParamBuilder();

	/**
	* Gets the parametrization class ID.
	*/
	AnimationParamClass getClassId() const { return AnimationParam_DenseSampling; }

	/**
	* Gets the maximum extrapolation allowed in the animation space.
	*/
	float getMaxExtrapolation() const;

	/**
	* Sets the maximum extrapolation allowed in the animation space.
	*/
	void setMaxExtrapolation( float maxExtrap = 0.15f );

	/**
	* Gets the minimum Euclidean distance between
	* two samples in the parametric space.
	*/
	float getMinSampleDistance() const;

	/**
	* Sets the minimum Euclidean distance between
	* two samples in the parametric space.
	*/
	void setMinSampleDistance( float minSampleDist = 0.00001f );

protected:

	void _buildParametrization();

	struct SortableSample
	{
		unsigned int sampleIndex;
		float dist;

		SortableSample() { }
		SortableSample( unsigned int sampleIndex, float dist ) { this->sampleIndex = sampleIndex; this->dist = dist; }
		bool operator <( const SortableSample& sample ) const { return dist < sample.dist; }
	};

	float mMaxExtrap;
	float mMinSampleDist;

};

}

#endif // __zhDenseSamplingParamBuilder_h__
