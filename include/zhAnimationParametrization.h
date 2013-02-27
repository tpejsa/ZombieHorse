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

#ifndef __zhAnimationParametrization_h__
#define __zhAnimationParametrization_h__

#include "zhPrereq.h"
#include "zhIterators.h"
#include "zhMath.h"

namespace zh
{

class AnimationSpace;

enum AnimationParamClass
{
	AnimationParam_DenseSampling
};

/**
* @brief Generic abstract class representing
* an animation parametrization, used for computing
* animation blend weights from a set of arbitrary
* parameter values.
*/
class zhDeclSpec AnimationParametrization
{

public:

	/**
	* Constructor.
	* 
	* @param numParams Number of parameters.
	* @param numBaseSamples Number of base samples (i.e. base animations
	* in the animation space).
	*/
	AnimationParametrization( unsigned int numParams, unsigned int numBaseSamples );

	/**
	* Destructor.
	*/
	virtual ~AnimationParametrization();

	/**
	* Gets the animation parametrization class ID.
	*
	* This pure virtual function should be implemented in concrete
	* animation parametrization classes.
	*/
	virtual AnimationParamClass getClassId() const = 0;

	/**
	* Searches for the specified parameter.
	*
	* @param paramName Parameter name.
	* @return Parameter index if the parameter is defined,
	* UINT_MAX otherwise.
	*/
	unsigned int findParam( const std::string& paramName ) const;

	/**
	* Gets the name of a parameter.
	*
	* @param paramIndex Parameter index.
	* @return Parameter name.
	*/
	const std::string& getParam( unsigned int paramIndex ) const;

	/**
	* Sets the name of a parameter.
	*
	* @param paramIndex Parameter index.
	* @param paramName Parameter name.
	*/
	void setParam( unsigned int paramIndex, const std::string& paramName );

	/**
	* Gets the number of parameters (i.e. parameter vector dimensionality).
	*/
	unsigned int getNumParams() const;

	/**
	* Gets the parameter values associated with the specified
	* base animation sample.
	*
	* @param animIndex Base animation index.
	* @return Set of parameter values.
	*/
	const Vector& getBaseSample( unsigned int animIndex ) const;

	/**
	* Sets the parameter values associated with the specified
	* base animation sample.
	*
	* @param id Base animation node ID.
	* @param paramValues Set of parameter values.
	*/
	void setBaseSample( unsigned int animIndex, const Vector& paramValues );

	/**
	* Gets the number of base samples (i.e. base animations
	* in the animation space).
	*/
	unsigned int getNumBaseSamples() const;

	/**
	* Gets the blend weights for the specified arbitrary parameter value set.
	*
	* @param paramValues Set of parameter values.
	* @return Set of blend weights.
	*/
	virtual Vector sample( const Vector& paramValues ) const = 0;

protected:

	std::vector<std::string> mParams;
	std::vector<Vector> mBaseSamples;

};

/**
* @brief Generic class representing an animation parametrization
* based on scattered data interpolation from a dense set of
* parameter-weight samples.
*/
class zhDeclSpec DenseSamplingParametrization : public AnimationParametrization
{

public:

	/**
	* Constructor.
	* 
	* @param numParams Number of parameters.
	* @param numBaseSamples Number of base samples (i.e. base animations
	* in the animation space).
	*/
	DenseSamplingParametrization( unsigned int numParams, unsigned int numBaseSamples )
		: AnimationParametrization( numParams, numBaseSamples ) { }

	/**
	* Gets the animation parametrization class ID.
	*/
	static AnimationParamClass ClassId() { return AnimationParam_DenseSampling; }

	/**
	* Gets the animation parametrization class ID.
	*/
	AnimationParamClass getClassId() const { return ClassId(); }

	/**
	* Adds a new parameter value sample.
	* 
	* @param paramValues Set of parameter values.
	* @param weights Set of blend weights.
	*/
	void addSample( const Vector& paramValues, const Vector& weights );

	/**
	* Removes the specified parameter value sample.
	* 
	* @param sampleIndex Sample index.
	*/
	void removeSample( unsigned int sampleIndex );

	/**
	* Gets the parameter values and blend weights associated
	* with the specified sample.
	*
	* @param sampleIndex Sample index.
	* @param paramValues Set of parameter values.
	* @param weights Set of blend weights.
	*/
	void getSample( unsigned int sampleIndex, Vector& paramValues, Vector& weights ) const;

	/**
	* Sets the parameter values and blend weights associated
	* with the specified sample.
	*
	* @param sampleIndex Sample index.
	* @param paramValues Set of parameter values.
	* @param weights Set of blend weights.
	*/
	void setSample( unsigned int sampleIndex, const Vector& paramValues, const Vector& weights );

	/**
	* Gets the blend weights associated
	* with the specified sample.
	*
	* @param sampleIndex Sample index.
	* @param weights Set of blend weights.
	*/
	const Vector& getSampleWeights( unsigned int sampleIndex ) const;
	
	/**
	* Sets the blend weights associated
	* with the specified sample.
	*
	* @param sampleIndex Sample index.
	* @param weights Set of blend weights.
	*/
	void setSampleWeights( unsigned int sampleIndex, const Vector& weights );

	/**
	* Gets the number of parameter value samples.
	*/
	unsigned int getNumSamples() const;

	/**
	* Builds a kd-tree supporting data structure
	* for faster sample interpolation.
	*/
	void buildKDTree(); // TODO: make this private and have it invoked automatically on first sample call?

	/**
	* Gets the blend weights for the specified arbitrary parameter value set.
	* k-nearest-neighbor interpolation is employed
	* to compute the blend weights.
	*
	* @param paramValues Set of parameter values.
	* @return Set of blend weights.
	*/
	Vector sample( const Vector& paramValues ) const;

private:

	struct SortableSample
	{
		unsigned int sampleIndex;
		float dist;

		SortableSample() { }
		SortableSample( unsigned int sampleIndex, float dist ) { this->sampleIndex = sampleIndex; this->dist = dist; }
		bool operator <( const SortableSample& sample ) const { return dist < sample.dist; }
	};

	std::vector< std::pair<Vector, Vector> > mSamples;

};

}

#endif // __zhAnimationParametrization_h__
