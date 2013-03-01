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

#include "zhDenseSamplingParamBuilder.h"
#include "zhMath.h"
#include "zhString.h"
#include "zhSkeleton.h"
#include "zhAnimationSpace.h"

#include "ctime"
#include "boost/random.hpp"
#include "boost/generator_iterator.hpp"

namespace zh
{

DenseSamplingParamBuilder::DenseSamplingParamBuilder( Skeleton* skel, AnimationSpace* animSpace )
: ParamAnimationBuilder( skel, animSpace ), mMaxExtrap(0.15f), mMinSampleDist(0.00001f)
{
}

DenseSamplingParamBuilder::~DenseSamplingParamBuilder()
{
}

float DenseSamplingParamBuilder::getMaxExtrapolation() const
{
	return mMaxExtrap;
}

void DenseSamplingParamBuilder::setMaxExtrapolation( float maxExtrap )
{
	mMaxExtrap = maxExtrap;
}

float DenseSamplingParamBuilder::getMinSampleDistance() const
{
	return mMinSampleDist;
}

void DenseSamplingParamBuilder::setMinSampleDistance( float minSampleDist )
{
	mMinSampleDist = minSampleDist;
}

void DenseSamplingParamBuilder::_buildParametrization()
{
	DenseSamplingParametrization* animparam = static_cast<DenseSamplingParametrization*>( mAnimSpace->getParametrization() );
	unsigned int num_banim = animparam->getNumBaseSamples(),
		num_param = animparam->getNumParams();

	// add base samples as dense samples, compute AABB for these samples
	Vector param_lb( num_param, FLT_MAX ), param_ub( num_param, -FLT_MAX );
	for( unsigned int banim_i = 0; banim_i < num_banim; ++banim_i )
	{
		const Vector& params = animparam->getBaseSample(banim_i);
		Vector weights(num_banim);
		weights[banim_i] = 1;
		animparam->addSample( params, weights );

		for( unsigned int param_i = 0; param_i < num_param; ++param_i )
		{
			if( params[param_i] < param_lb[param_i] )
				param_lb[param_i] = params[param_i];
			
			if( params[param_i] > param_ub[param_i] )
				param_ub[param_i] = params[param_i];
		}
	}

	// expand AABB
	for( unsigned int param_i = 0; param_i < num_param; ++param_i )
	{
		param_lb[param_i] += ( param_ub[param_i] - param_lb[param_i] ) * mMaxExtrap;
		param_ub[param_i] += ( param_ub[param_i] - param_lb[param_i] ) * mMaxExtrap;
	}

	// initialize random number generators
	boost::mt19937 rng( std::time(NULL) );
	boost::uniform_real<float> distr( 0, 1.f );
	boost::variate_generator< boost::mt19937, boost::uniform_real<float> > get_randf( rng, distr );

	// generate a dense sampling of the param. space
	unsigned int num_samples = zhAnimationParam_SampleDensity * num_banim;
	unsigned int conv_ct = 0; // convergence counter - because it seems the algorithm can converge before reaching num_samples
	while( animparam->getNumSamples() < num_samples )
	{
		// take random sample from AABB
		Vector params( num_param );
		for( unsigned int param_i = 0; param_i < num_param; ++param_i )
			params[param_i] = param_lb[param_i] + get_randf() * ( param_ub[param_i] - param_lb[param_i] );

		// compute Euclidean distance to each base sample
		std::set<SortableSample> sorted_samples;
		for( unsigned int banim_i = 0; banim_i < num_banim; ++banim_i )
			sorted_samples.insert( SortableSample( banim_i, animparam->getBaseSample(banim_i).distanceSq(params) ) );

		// get nearest base samples
		std::vector<SortableSample> nearest_samples( num_param + 1 );
		std::vector<unsigned int> unweighted_samples( num_param + 1 );
		unsigned int num_nbsamples = 0;
		for( std::set<SortableSample>::const_iterator nbsample_i = sorted_samples.begin();
			nbsample_i != sorted_samples.end() && num_nbsamples < num_param + 1;
			++nbsample_i, ++num_nbsamples )
		{
			nearest_samples[num_nbsamples] = *nbsample_i;
			unweighted_samples[num_nbsamples] = (*nbsample_i).sampleIndex;
		}

		// assign random weights to sample
		Vector weights( num_banim );
		float weight_sum = 0;
		while( !unweighted_samples.empty() )
		{
			unsigned int uws_i = zhRoundi( get_randf() * ( unweighted_samples.size() - 1 ) );
			unsigned int bsample_i = unweighted_samples[uws_i];
			unweighted_samples.erase( unweighted_samples.begin() + uws_i );

			if( unweighted_samples.empty() )
			{
				weights[bsample_i] = 1.f - weight_sum;
			}
			else
			{
				float lw = std::max<float>( - mMaxExtrap, - mMaxExtrap - weight_sum ),
					uw = std::min<float>( 1.f + mMaxExtrap, 1.f + mMaxExtrap - weight_sum );
				weights[bsample_i] = lw + get_randf() * ( uw - lw );			
			}

			weight_sum += weights[bsample_i];
		}

		// compute param. values for sample
		// TODO: is this the way to compute param. values of samples? I have no idea!
		params = Vector(num_param);
		for( unsigned int nbsample_i = 0; nbsample_i < num_nbsamples; ++nbsample_i )
		{
			unsigned int bsample_i = nearest_samples[nbsample_i].sampleIndex;
			const Vector& bparam = animparam->getBaseSample(bsample_i);
			params += bparam * weights[bsample_i];
		}

		// determine if the sample is too close to an existing one
		bool discard = false;
		for( unsigned int sample_i = 0; sample_i < animparam->getNumSamples(); ++sample_i )
		{
			Vector params0, weights0;
			animparam->getSample( sample_i, params0, weights0 );

			if( params0.distance(params) < mMinSampleDist )
			{
				discard = true;
				++conv_ct;
				break;
			}
		}

		if( conv_ct >= 10 )
			// can't generate more samples?
			break;

		// finally, add the sample
		if(!discard)
			animparam->addSample( params, weights );
	}
}

}
