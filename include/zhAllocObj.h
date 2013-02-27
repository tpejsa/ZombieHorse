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

#ifndef __zhAllocObj_h__
#define __zhAllocObj_h__

#include "zhPrereq.h"
#include "zhMemoryPool.h"

namespace zh
{

/**
* Class template for objects with custom memory allocation.
* Any class that wants to use custom memory allocation should inherit
* from this class. Template parameter specifies the memory manager to use.
* The default memory manager is MemoryPool.
*/
template <class MemMgr = MemoryPool>
class AllocObj
{

public:

	static void* operator new( size_t size )
	{
		return MemMgr::Instance()->alloc(size);
	}

	static void operator delete( void* ptr, size_t size )
	{
		MemMgr::Instance()->dealloc(ptr,size);
	}

};

}

#endif // __zhAllocObj_h__
