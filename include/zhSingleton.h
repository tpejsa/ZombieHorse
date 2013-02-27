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

#ifndef __zhSingleton_h__
#define __zhSingleton_h__

#include "zhPrereq.h"

namespace zh
{

/**
* Template for a singleton class.
*/
template <class T>
class Singleton
{

protected:

	/**
	* Constructor.
	*/
	Singleton(){}

	/**
	* Destructor.
	*/
	virtual ~Singleton()
	{
	}

public:

	/**
	* Gets the singleton instance.
	*/
	static T* Instance()
	{
		if( mInst == NULL )
			mInst = new T();

		return mInst;
	}

	/**
	* Deletes the singleton instance, this will typically be called
	* at the end of the program execution.
	*/
	static void Release()
	{
		if( mInst != NULL )
			delete mInst;
		mInst = NULL;
	}

protected:

	static T* mInst; ///< The singleton instance.

};

template <class T> T* Singleton<T>::mInst = NULL;

}

#endif // __zhSingleton_h__
