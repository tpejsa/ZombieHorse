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

#ifndef __zhSmartPtr_h__
#define __zhSmartPtr_h__

#include "zhPrereq.h"

// Boost
#include "boost/detail/atomic_count.hpp"

namespace zh
{

template <typename T>
class SmartPtr;

/**
* @brief Smart pointer locking proxy, enables exclusive access to
* the smart pointer.
*/
template <typename T>
class LockingProxy
{

public:

	LockingProxy( SmartPtr<T>* ptr )
	{
		this->mPtr = ptr;
		zhLock_SharedMutex(*ptr);
	}

	~LockingProxy()
	{
		zhUnlock_SharedMutex(*mPtr);
	}

	T& operator*() const
	{
		return *mPtr->getRawPtr();
	}

	T* operator->() const
	{
		return mPtr->getRawPtr();
	}

private:

	SmartPtr<T>* mPtr;

};

/**
* @brief Simple generic smart pointer that uses reference counting.
*
* SmartPtr is a thread-safe implementation using a shared mutex lock
* to protect the pointer from concurrent access.
*/
template <typename T>
class SmartPtr
{

public:

	zhDeclare_SharedMutex ///< Mutex for protecting the smart pointer.

	SmartPtr() : mRawPtr(NULL), mRefCount(NULL)
	{
		zhCreate_SharedMutex;
	}

	SmartPtr( T* rawPtr )
	{
		zhCreate_SharedMutex;

		mRawPtr = rawPtr;
		if( rawPtr != NULL )
		{
			mRefCount = new boost::detail::atomic_count(0);
			++*mRefCount;
		}
		else
		{
			mRefCount = NULL;
		}
	}

	SmartPtr( const SmartPtr& ptr )
	{
		zhLockCopy_SharedMutex(ptr);

		mRawPtr = ptr.mRawPtr;
		mRefCount = ptr.mRefCount;
		if( mRawPtr != NULL )
		{
			++*mRefCount;
		}
		else
		{
			zhCreate_SharedMutex;
		}

		zhUnlock_SharedMutex(*this);
	}

	template <typename U>
    SmartPtr( SmartPtr<U> const & ptr )
    {
		zhLockCopy_SharedMutex(ptr);

		mRawPtr = ptr.getRawPtr();
		mRefCount = ptr.getRefCountPtr();
		if( mRawPtr != NULL )
		{
			++*mRefCount;
		}
		else
		{
			zhCreate_SharedMutex;
		}

		zhUnlock_SharedMutex(*this);
    }

	// support for SmartPtr down-casting
	template <typename U>
	static SmartPtr DynamicCast( SmartPtr<U> const & ptr )
	{
		SmartPtr rptr;

		#if zhMultiThreading_Enabled
		ptr.mMtx->lock();
		rptr.mMtx = ptr.mMtx;
		#endif

		rptr.mRawPtr = dynamic_cast<T*>( ptr.getRawPtr() );
		rptr.mRefCount = ptr.getRefCountPtr();
		if( rptr.mRawPtr != NULL )
		{
			++*rptr.mRefCount;
		}
		else
		{
			zhCreate_SharedMutex;
		}

		#if zhMultiThreading_Enabled
		rptr.mMtx->unlock();
		#endif

		return rptr;
	}

	~SmartPtr()
	{
		release();
	}

	SmartPtr& operator=( const SmartPtr& ptr )
	{
		if( mRawPtr == ptr.getRawPtr() )
			return *this;
		
		release();

		zhLockCopy_SharedMutex(ptr);

		mRawPtr = ptr.mRawPtr;
		mRefCount = ptr.mRefCount;
		if( mRawPtr != NULL )
		{
			++*mRefCount;
		}
		else
		{
			zhCreate_SharedMutex;
		}

		zhUnlock_SharedMutex(ptr);

		return *this;
	}

	T& operator*() const
	{
		return *LockingProxy<T>((SmartPtr<T>*)this);
	}

	LockingProxy<T> operator->() const
	{
		return LockingProxy<T>((SmartPtr<T>*)this);
	}

	T* getRawPtr() const
	{
		return mRawPtr;
	}

	const boost::detail::atomic_count& getRefCount() const
	{
		return *mRefCount;
	}

	boost::detail::atomic_count* getRefCountPtr() const
	{
		return mRefCount;
	}

	bool operator!() const
	{
		return mRawPtr == NULL;
	}

	inline friend bool operator==( const SmartPtr& ptr1, const T* ptr2 )
	{
		return ptr1.mRawPtr == ptr2;
	}

	inline friend bool operator==( const T* ptr1, const SmartPtr& ptr2 )
	{
		return ptr1 == ptr2.mRawPtr;
	}

	inline friend bool operator!=( const SmartPtr& ptr1, const T* ptr2 )
	{
		return ptr1.mRawPtr != ptr2;
	}

	inline friend bool operator!=( const T* ptr1, const SmartPtr& ptr2 )
	{
		return ptr1 != ptr2.mRawPtr;
	}

	template <class U>
	inline friend bool operator==( const SmartPtr& ptr1, const U* ptr2 )
	{
		return ptr1.mRawPtr == ptr2;
	}

	template <class U>
	inline friend bool operator==( const U* ptr1, const SmartPtr& ptr2 )
	{
		return ptr1 == ptr2.mRawPtr;
	}

	template <class U>
	inline friend bool operator!=( const SmartPtr& ptr1, const U* ptr2 )
	{
		return ptr1.mRawPtr != ptr2;
	}

	template <class U>
	inline friend bool operator!=( const U* ptr1, const SmartPtr& ptr2 )
	{
		return ptr1 != ptr2.mRawPtr;
	}

	template <class U>
	bool operator==( const SmartPtr<U>& ptr ) const
	{
		return mRawPtr == ptr.mRawPtr;
	}

	template <class U>
	bool operator!=( const SmartPtr<U>& ptr ) const
	{
		return mRawPtr != ptr.mRawPtr;
	}

private:

	class IfTester
	{
		void operator delete( void* );
	};

public:

	operator IfTester*() const
	{
		if( mRawPtr == NULL ) return NULL;
		static IfTester test;
		return &test;
	}

protected:

	void release()
	{
		zhLock_SharedMutex(*this);

		if( mRawPtr == NULL )
		{
			zhUnlock_SharedMutex(*this);
			zhDelete_SharedMutex;
		}
		else
		{
			--*mRefCount;
			if( *mRefCount == 0 )
			{
				delete mRefCount;
				delete mRawPtr;
				zhUnlock_SharedMutex(*this);
				zhDelete_SharedMutex;
			}
			else
			{
				zhUnlock_SharedMutex(*this);
			}
		}
	}

private:

	T* mRawPtr; ///< Raw pointer that underlies the smart pointer.
	boost::detail::atomic_count* mRefCount; ///< Reference counter.

};

}

#endif // __zhSmartPtr_h__
