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

#ifndef __zhMemoryManager_h__
#define __zhMemoryManager_h__

#include "zhPrereq.h"

namespace zh
{

/**
* Generalized memory manager class.
*/
class zhDeclSpec MemoryManager
{

	zhDeclare_Mutex

public:

	/**
	* Allocates a block of memory.
	*
	* @size Size of the memory block to allocate (in bytes).
	* @return Pointer to the allocated memory.
	*/
	void* alloc( size_t size )
	{
		zhLock_Mutex;
		void* ptr = _alloc(size);
		zhUnlock_Mutex;
		return ptr;
	}

	/**
	* Deallocates a block of memory.
	*
	* @ptr Pointer to the memory block to deallocate.
	* @size Size of the memory block to deallocate (in bytes).
	*/
	void dealloc( void* ptr, size_t size )
	{
		zhLock_Mutex;
		_dealloc( ptr, size );
		zhUnlock_Mutex;
	}

protected:

	virtual void* _alloc( size_t size ) = 0; ///< Does the actual allocation.
	virtual void _dealloc( void* ptr, size_t size ) = 0; ///< Does the actual deallocation.

};

}

#endif // __zhMemoryManager_h__
