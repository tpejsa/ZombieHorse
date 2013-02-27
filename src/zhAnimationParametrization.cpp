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

#include "zhAnimationParametrization.h"
#include "zhString.h"
#include "zhAnimationSpace.h"

namespace zh
{

AnimationParametrization::AnimationParametrization( unsigned int numParams, unsigned int numBaseSamples )
{
	zhAssert( numParams > 0 && numBaseSamples > 0 );

	mParams = std::vector<std::string>( numParams, "" );
	mBaseSamples = std::vector<Vector>( numBaseSamples, Vector(numParams) );
}

AnimationParametrization::~AnimationParametrization()
{
}

unsigned int AnimationParametrization::findParam( const std::string& paramName ) const
{
	for( unsigned int param_i = 0; param_i < mParams.size(); ++param_i )
		if( mParams[param_i] == paramName )
			return param_i;

	return UINT_MAX;
}

const std::string& AnimationParametrization::getParam( unsigned int paramIndex ) const
{
	zhAssert( paramIndex < mParams.size() );

	return mParams[paramIndex];
}

void AnimationParametrization::setParam( unsigned int paramIndex, const std::string& paramName )
{
	zhAssert( paramIndex < mParams.size() );

	mParams[paramIndex] = paramName;
}

unsigned int AnimationParametrization::getNumParams() const
{
	return mParams.size();
}

const Vector& AnimationParametrization::getBaseSample( unsigned int animIndex ) const
{
	zhAssert( animIndex < getNumBaseSamples() );

	return mBaseSamples[animIndex];
}

void AnimationParametrization::setBaseSample( unsigned int animIndex, const Vector& paramValues )
{
	zhAssert( animIndex < getNumBaseSamples() );
	zhAssert( paramValues.size() == getNumParams() );

	mBaseSamples[animIndex] = paramValues;
}

unsigned int AnimationParametrization::getNumBaseSamples() const
{
	return mBaseSamples.size();
}

void DenseSamplingParametrization::addSample( const Vector& paramValues, const Vector& weights )
{
	zhAssert( paramValues.size() == getNumParams() );
	zhAssert( weights.size() == getNumBaseSamples() );

	mSamples.push_back( make_pair( paramValues, weights ) );
}

void DenseSamplingParametrization::removeSample( unsigned int sampleIndex )
{
	zhAssert( sampleIndex < getNumSamples() );

	mSamples.erase( mSamples.begin() + sampleIndex );
}

void DenseSamplingParametrization::getSample( unsigned int sampleIndex, Vector& paramValues, Vector& weights ) const
{
	zhAssert( sampleIndex < getNumSamples() );

	paramValues = mSamples[sampleIndex].first;
	weights = mSamples[sampleIndex].second;
}

void DenseSamplingParametrization::setSample( unsigned int sampleIndex, const Vector& paramValues, const Vector& weights )
{
	zhAssert( sampleIndex < getNumSamples() );

	mSamples[sampleIndex] = make_pair( paramValues, weights );
}

const Vector& DenseSamplingParametrization::getSampleWeights( unsigned int sampleIndex ) const
{
	zhAssert( sampleIndex < getNumSamples() );

	return mSamples[sampleIndex].second;
}

void DenseSamplingParametrization::setSampleWeights( unsigned int sampleIndex, const Vector& weights )
{
	zhAssert( sampleIndex < getNumSamples() );

	mSamples[sampleIndex] = make_pair( mSamples[sampleIndex].first, weights );
}

unsigned int DenseSamplingParametrization::getNumSamples() const
{
	return mSamples.size();
}

void DenseSamplingParametrization::buildKDTree()
{
	// TODO: build the kd-tree over all samples
}

Vector DenseSamplingParametrization::sample( const Vector& paramValues ) const
{
	zhAssert( paramValues.size() == getNumParams() );

	// TODO: implement kd-tree search? using linear search for now

	// compute Euclidean distance between each param. sample and specified param. vector
	std::set<SortableSample> sorted_samples;
	for( unsigned int sample_i = 0; sample_i < mSamples.size(); ++sample_i )
		sorted_samples.insert( SortableSample( sample_i, mSamples[sample_i].first.distanceSq(paramValues) ) );

	// get k nearest samples
	std::vector<SortableSample> nearest_samples(zhAnimationParam_SampleInterpK);
	unsigned int num_samples = 0;
	for( std::set<SortableSample>::const_iterator sample_i = sorted_samples.begin();
		sample_i != sorted_samples.end() && num_samples < zhAnimationParam_SampleInterpK;
		++sample_i, ++num_samples )
	{
		nearest_samples[num_samples] = *sample_i;
	}

	// compute interpolation weights
	Vector knn_weights( num_samples );
	float inv_maxdist = 1.f / sqrt( nearest_samples[ num_samples - 1 ].dist ); // TODO: how about an efficient InvSqrt impl.? there is a nice one in id Tech 3
	for( unsigned int sample_i = 0; sample_i < num_samples; ++sample_i )
	{
		knn_weights[sample_i] = 1.f / sqrt( nearest_samples[sample_i].dist ) - inv_maxdist;
	}
	knn_weights = knn_weights / knn_weights.sum();

	// interpolate k nearest samples
	Vector weights( getNumBaseSamples() ); // interpolated blend weights
	for( unsigned int sample_i = 0; sample_i < num_samples; ++sample_i )
	{
		weights += mSamples[ nearest_samples[sample_i].sampleIndex ].second * knn_weights[sample_i];
	}

	return weights;
}

}
