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

#ifndef __zhIterators_h__
#define __zhIterators_h__

#include "zhPrereq.h"

namespace zh
{

/**
* Template for a wrapper over STL vector iterators.
*/
template <class T>
class VectorIterator
{

public:

	/**
	* Constructor.
	*
	* @param v Vector reference.
	*/
	explicit VectorIterator( T& v )
	{
		mCurIter = v.begin();
		mEndIter = v.end();
	}

	/**
	* Returns true if the iterator has reached the end of the vector,
	* otherwise false.
	*/
	bool end() const
	{
		return mCurIter == mEndIter;
	}
	
	/**
	* Returns true if the iterator has not yet reached the end of the map,
	* otherwise false.
	*/
	bool hasMore() const
	{
		return mCurIter != mEndIter;
	}

	/**
	* Moves the iterator to the next value
	* and returns the current one.
	*/
	typename T::value_type next()
	{
		return *( mCurIter++ );
	}

	/**
	* Gets the current value.
	*/
	typename T::value_type value() const
	{
		return *mCurIter;
	}

	/**
	* Gets a pointer to the current value.
	*/
	typename T::pointer valuePtr() const
	{
		return &(*mCurIter);
	}

private:

	typename T::iterator mCurIter; ///< Current iterator.
	typename T::iterator mEndIter; ///< End iterator.

};

/**
* Template for a wrapper over STL vector const iterators.
*/
template <class T>
class VectorConstIterator
{

public:

	/**
	* Constructor.
	*
	* @param v Vector reference.
	*/
	explicit VectorConstIterator( const T& v )
	{
		mCurIter = v.begin();
		mEndIter = v.end();
	}

	/**
	* Returns true if the iterator has reached the end of the vector,
	* otherwise false.
	*/
	bool end() const
	{
		return mCurIter == mEndIter;
	}

	/**
	* Returns true if the iterator has not yet reached the end of the map,
	* otherwise false.
	*/
	bool hasMore() const
	{
		return mCurIter != mEndIter;
	}

	/**
	* Moves the iterator to the next value
	* and returns the current one.
	*/
	typename T::value_type next() const
	{
		return *( mCurIter++ );
	}

	/**
	* Gets the current value.
	*/
	typename T::value_type value() const
	{
		return *mCurIter;
	}

	/**
	* Gets a pointer to the current value.
	*/
	typename T::const_pointer valuePtr() const
	{
		return &(*mCurIter);
	}

private:

	mutable typename T::const_iterator mCurIter; ///< Current iterator.
	typename T::const_iterator mEndIter; ///< End iterator.

};

/**
* Template for a wrapper over STL map iterators.
*/
template <class T>
class MapIterator
{

public:

	/**
	* Constructor.
	*
	* @param m Map reference.
	*/
	explicit MapIterator( T& m )
	{
		mCurIter = m.begin();
		mEndIter = m.end();
	}

	/**
	* Returns true if the iterator has reached the end of the map,
	* otherwise false.
	*/
	bool end() const
	{
		return mCurIter == mEndIter;
	}

	/**
	* Returns true if the iterator has not yet reached the end of the map,
	* otherwise false.
	*/
	bool hasMore() const
	{
		return mCurIter != mEndIter;
	}

	/**
	* Moves the iterator to the next key-value pair
	* and returns the current value.
	*/
	typename T::mapped_type next()
	{
		return ( mCurIter++ )->second;
	}

	/**
	* Gets the current key.
	*/
	typename T::key_type key() const
	{
		return mCurIter->first;
	}

	/**
	* Gets the current value.
	*/
	typename T::mapped_type value() const
	{
		return mCurIter->second;
	}

	/**
	* Gets a pointer to the current value.
	*/
	typename T::mapped_type* valuePtr() const
	{
		return &(mCurIter->second);
	}

private:

	typename T::iterator mCurIter; ///< Current iterator.
	typename T::iterator mEndIter; ///< End iterator.

};

/**
* Template for a wrapper over STL map const iterators.
*/
template <class T>
class MapConstIterator
{

public:

	/**
	* Constructor.
	*
	* @param m Map reference.
	*/
	explicit MapConstIterator( const T& m )
	{
		mCurIter = m.begin();
		mEndIter = m.end();
	}

	/**
	* Returns true if the iterator has reached the end of the map,
	* otherwise false.
	*/
	bool end() const
	{
		return mCurIter == mEndIter;
	}

	/**
	* Returns true if the iterator has not yet reached the end of the map,
	* otherwise false.
	*/
	bool hasMore() const
	{
		return mCurIter != mEndIter;
	}

	/**
	* Moves the iterator to the next key-value pair
	* and returns the current value.
	*/
	typename T::mapped_type next() const
	{
		return ( mCurIter++ )->second;
	}

	/**
	* Gets the current key.
	*/
	typename T::key_type key() const
	{
		return mCurIter->first;
	}

	/**
	* Gets the current value.
	*/
	typename T::mapped_type value() const
	{
		return mCurIter->second;
	}

	/**
	* Gets a pointer to the current value.
	*/
	const typename T::mapped_type* valuePtr() const
	{
		return &(mCurIter->second);
	}

private:

	mutable typename T::const_iterator mCurIter; ///< Current iterator.
	typename T::const_iterator mEndIter; ///< End iterator.

};

}

#endif // __zhIterators_h__
