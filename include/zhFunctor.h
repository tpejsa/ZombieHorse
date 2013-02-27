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

#ifndef __zhFunctor_h__
#define __zhFunctor_h__

#include "zhPrereq.h"

namespace zh
{

template <typename RT>
class Functor0
{

public:

	virtual RT operator()() = 0;
	virtual RT call() = 0;

};

template <typename RT, typename PT0>
class Functor1
{

public:

	virtual RT operator()( PT0 p0 ) = 0;
	virtual RT call( PT0 p0 ) = 0;

};

template <typename RT, typename PT0, typename PT1>
class Functor2
{

public:

	virtual RT operator()( PT0 p0, PT1 p1 ) = 0;
	virtual RT call( PT0 p0, PT1 p1 ) = 0;

};

}

#endif // __zhFunctor_h__
