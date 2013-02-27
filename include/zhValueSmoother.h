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

#ifndef __zhValueSmoother_h__
#define __zhValueSmoother_h__

#include "zhPrereq.h"

namespace zh
{

enum ValueSmootherClass
{
	ValueSmoother_LowPass
};

/**
* @brief Generic abstract value smoother
* which can be a used to smoothen a stream of values.
*
* Generic abstract value smoother
* which can be a used to smoothen a stream of values.
* To initialize the ValueSmoother, call update() with
* dt parameter set to 0.
*/
template <typename T>
class ValueSmoother
{

public:

	/**
	* Constructor.
	*/
	ValueSmoother() { }

	/**
	* Destructor.
	*/
	virtual ~ValueSmoother() { }

	/**
	* Gets the ValueSmoother class ID.
	*
	* This pure virtual function should be implemented in concrete
	* animation track classes.
	*/
	virtual ValueSmootherClass getClassId() const = 0;

	/**
	* Updates the smoothed value.
	*
	* @param x Next input value.
	* @param dt Elapsed time.
	*/
	virtual void update( const T& x, float dt ) = 0;

	/**
	* Gets the current smoothed value.
	*/
	const T& getValue() const
	{
		return mSV;
	}

protected:

	T mSV;

};

/**
* @brief Generic value smoother employing a low-pass filter.
*/
template <typename T>
class LowPassValueSmoother : public ValueSmoother<T>
{

public:

	/**
	* Constructor.
	*/
	LowPassValueSmoother() : mTC(0.f) { }

	/**
	* Destructor.
	*/
	~LowPassValueSmoother() { }

	/**
	* Gets the ValueSmoother class ID.
	*/
	ValueSmootherClass getClassId() const { return ValueSmoother_LowPass; }

	/**
	* Gets the current time constant.
	*/
	float getTimeConst() const
	{
		return mTC * 1.4426950f;
	}

	/**
	* Sets the current time constant.
	*/
	void setTimeConst( float tc )
	{
		mTC = tc * 0.6931471f;
	}

	/**
	* Updates the smoothed value.
	*
	* @param x Next input value.
	* @param dt Elapsed time.
	*/
	void update( const T& x, float dt )
	{	
		if( dt <= 0 )
		{
			mSV = x;
			return;
		}

		float a = dt / ( mTC + dt );
		mSV += ( ( x - mSV ) * a );
	}

protected:

	float mTC;

};

}

#endif // __zhValueSmoother_h__
