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

#include "zhMemoryPool.h"

namespace zh
{

void MemoryPool::ChunkAllocator::Chunk::init( size_t blockSize, unsigned char numBlocks )
{
	zhAssert( blockSize > 0 && numBlocks > 0 );
	zhAssert( ( blockSize * numBlocks ) / blockSize == numBlocks );

	mData = new unsigned char[ blockSize * numBlocks ];
	mNextFreeBlock = 0;
	mNumFreeBlocks = numBlocks;
	unsigned char i = 0;
	unsigned char* ptr = mData;
	for( ; i != numBlocks; ptr += blockSize )
		*ptr = ++i;
}

void MemoryPool::ChunkAllocator::Chunk::destroy()
{
	delete[] mData;
}

void* MemoryPool::ChunkAllocator::Chunk::alloc( size_t blockSize )
{
	zhAssert( blockSize > 0 );

	if( mNumFreeBlocks == 0 )
		return NULL;

	unsigned char* ptr = mData + mNextFreeBlock * blockSize;
	mNextFreeBlock = *ptr;
	--mNumFreeBlocks;

	return ptr;
}

void MemoryPool::ChunkAllocator::Chunk::dealloc( void* ptr, size_t blockSize )
{
	zhAssert( ptr != NULL && blockSize > 0 && ptr >= mData );

	unsigned char* dptr = static_cast<unsigned char*>(ptr);
	zhAssert( ( dptr - mData ) % blockSize == 0 );
	*dptr = mNextFreeBlock;
	mNextFreeBlock = static_cast<unsigned char>(
		( dptr - mData ) / blockSize );
	zhAssert( mNextFreeBlock == ( dptr - mData ) / blockSize );
	++mNumFreeBlocks;
}

MemoryPool::ChunkAllocator::ChunkAllocator( size_t blockSize, size_t chunkSize )
{
	zhAssert( blockSize > 0 && chunkSize > 0 );

	// remember, mPrev and mNext are needed for correct handling of chunk copies
	mPrev = mNext = this;

	this->mBlockSize = blockSize;
	
	size_t num_blocks = chunkSize / blockSize;
	if( num_blocks > UCHAR_MAX ) num_blocks = UCHAR_MAX;
	else if( num_blocks == 0 ) num_blocks = 8 * blockSize; // TODO: WTF? I don't get this.
	this->mNumBlocks = static_cast<unsigned char>( num_blocks );
	zhAssert( num_blocks == mNumBlocks );

	mLastAllocChunk = NULL;
	mLastDeallocChunk = NULL;
}

MemoryPool::ChunkAllocator::~ChunkAllocator()
{
	// remember, mPrev and mNext are needed for correct handling of chunk copies
	if( mPrev != this )
	{
		mPrev->mNext = mNext;
		mNext->mPrev = mPrev;
		return;
	}
	zhAssert( mPrev == mNext );

	for( std::vector<Chunk>::iterator i = mChunks.begin(); i != mChunks.end(); ++i )
		(*i).destroy();
}

size_t MemoryPool::ChunkAllocator::getBlockSize() const
{
	return mBlockSize;
}

void* MemoryPool::ChunkAllocator::alloc()
{
	if( mLastAllocChunk == NULL || mLastAllocChunk->mNumFreeBlocks == 0 )
	{
		for( std::vector<Chunk>::iterator i = mChunks.begin();; ++i )
		{
			if( i == mChunks.end() )
			{
				// all chunks full, add another one
				mChunks.reserve( mChunks.size() + 1 );
				Chunk nc;
				nc.init( mBlockSize, mNumBlocks );
				mChunks.push_back(nc);
				mLastAllocChunk = &mChunks.back();
				mLastDeallocChunk = &mChunks.front();
				break;
			}
			if( i->mNumFreeBlocks > 0 )
			{
				// found a chunk that isn't filled up
				mLastAllocChunk = &(*i);
				break;
			}
		}
	}
	zhAssert( mLastAllocChunk != NULL );
	zhAssert( mLastAllocChunk->mNumFreeBlocks > 0 );

	return mLastAllocChunk->alloc( mBlockSize );
}

void MemoryPool::ChunkAllocator::dealloc( void* ptr )
{
	zhAssert( ptr != NULL );
	zhAssert( !mChunks.empty() );
	zhAssert( &mChunks.front() <= mLastDeallocChunk );
	zhAssert( &mChunks.back() >= mLastDeallocChunk );

	// find chunk holding the data block
	size_t cs = mNumBlocks * mBlockSize;
	Chunk* lc = mLastDeallocChunk,
		*hc = mLastDeallocChunk + 1,
		*lbc = &mChunks.front(),
		*hbc = &mChunks.back() + 1;
	if( hc == hbc ) hc = NULL;
	while(true)
	{
		if( lc != NULL )
		{
			if( ptr >= lc->mData && ptr < lc->mData + cs )
			{
				mLastDeallocChunk = lc;
				break;
			}

			if( lc == lbc )
				lc = NULL;
			else
				--lc;
		}
		if( hc != NULL )
		{
			if( ptr >= hc->mData && ptr < hc->mData + cs )
			{
				mLastDeallocChunk = hc;
				break;
			}

			if( ++hc == hbc )
				hc = NULL;
		}
	}

	zhAssert( mLastDeallocChunk != NULL );

	// deallocate the block
	mLastDeallocChunk->dealloc( ptr, mBlockSize );
	if( mLastDeallocChunk->mNumFreeBlocks == mNumBlocks )
	{
		Chunk* lc = &mChunks.back();

		if( lc == mLastDeallocChunk )
		{
			if( mChunks.size() > 1 &&
				( mLastDeallocChunk - 1 )->mNumFreeBlocks == mNumBlocks )
			{
				// we're down to two free chunks, destroy one
				lc->destroy();
				mChunks.pop_back();
				mLastDeallocChunk = mLastAllocChunk = &mChunks.front();
			}

			return;
		}

		if( lc->mNumFreeBlocks == mNumBlocks )
		{
			// two chunks are free, destroy one
			lc->destroy();
			mChunks.pop_back();
			mLastAllocChunk = mLastDeallocChunk;
		}
		else
		{
			// move the free chunk to the back
			// (to avoid std::vector<Chunk>::erase() in the future)
			std::swap( *mLastDeallocChunk, *lc );
			mLastAllocChunk = &mChunks.back();
		}
	}
}

MemoryPool::ChunkAllocator::ChunkAllocator( const MemoryPool::ChunkAllocator& chAlloc )
{
	this->mBlockSize = chAlloc.mBlockSize;
	this->mNumBlocks = chAlloc.mNumBlocks;
	this->mChunks = chAlloc.mChunks;

	this->mPrev = &chAlloc;
	this->mNext = chAlloc.mNext;
	chAlloc.mNext->mPrev = this;
	chAlloc.mNext = this;
	this->mLastAllocChunk = chAlloc.mLastAllocChunk
		? &mChunks.front() + ( chAlloc.mLastAllocChunk - &chAlloc.mChunks.front() )
		: 0;
	this->mLastDeallocChunk = chAlloc.mLastDeallocChunk
		? &mChunks.front() + ( chAlloc.mLastDeallocChunk - &chAlloc.mChunks.front() )
		: 0;
}

MemoryPool::ChunkAllocator& MemoryPool::ChunkAllocator::operator=( const MemoryPool::ChunkAllocator& chAlloc )
{
	ChunkAllocator copied_ca(chAlloc);
	copied_ca.swap(*this);
	return *this;
}

void MemoryPool::ChunkAllocator::swap( MemoryPool::ChunkAllocator& chAlloc )
{
	std::swap( this->mBlockSize, chAlloc.mBlockSize );
	std::swap( this->mNumBlocks, chAlloc.mNumBlocks );
	std::swap( this->mLastAllocChunk, chAlloc.mLastAllocChunk );
	std::swap( this->mLastDeallocChunk, chAlloc.mLastDeallocChunk );
}

MemoryPool::MemoryPool( size_t chunkSize, size_t maxObjSize )
{
	zhAssert( chunkSize > 0 && maxObjSize > 0 &&
		chunkSize >= maxObjSize );

	this->mChunkSize = chunkSize;
	this->mMaxObjSize = maxObjSize;
	mLastAlloc = NULL;
	mLastDealloc = NULL;
}

unsigned int MemoryPool::getNumChunkAllocators() const
{
	return mPool.size();
}

const MemoryPool::ChunkAllocator& MemoryPool::getChunkAllocator( unsigned int index ) const
{
	zhAssert( index < mPool.size() );

	return mPool[index];
}

struct ChunkAllocatorCompare
	: std::binary_function<const MemoryPool::ChunkAllocator&, size_t, bool>
{
	bool operator ()( const MemoryPool::ChunkAllocator& ca, size_t size ) const
	{
		return ca.getBlockSize() < size;
	}

	#ifdef zhCompiler_MSVC // this is needed because MSVC is a buggy POS
	bool operator ()( const MemoryPool::ChunkAllocator& ca1,
		const MemoryPool::ChunkAllocator& ca2 ) const
	{
		return ca1.getBlockSize() < ca2.getBlockSize();
	}

	bool operator ()( size_t size, const MemoryPool::ChunkAllocator& ca ) const
	{
		return size < ca.getBlockSize();
	}
	#endif

};

void* MemoryPool::_alloc( size_t size )
{
	zhAssert( size > 0 );

	if( size > mMaxObjSize )
		// not a small object, use default allocator
		return operator new(size);

	if( mLastAlloc != NULL && mLastAlloc->getBlockSize() == size )
		// how fortunate, last used allocator happens to be the one we need
		return mLastAlloc->alloc();

	// find the first allocator for blocks equal to or larger than needed
	std::vector<ChunkAllocator>::iterator i =
		lower_bound( mPool.begin(), mPool.end(), size, ChunkAllocatorCompare() );

	if( i == mPool.end() || i->getBlockSize() != size )
	{
		// no appropriate allocator found
		i = mPool.insert( i, ChunkAllocator( size, mChunkSize ) );
		mLastDealloc = &(*mPool.begin());
	}

	mLastAlloc = &(*i);
	return mLastAlloc->alloc();
}

void MemoryPool::_dealloc( void* ptr, size_t size )
{
	zhAssert( ptr != NULL );
	zhAssert( size > 0 );

	if( size > mMaxObjSize )
		// not a small object, use default deallocator
		return operator delete(ptr);

	if( mLastDealloc != NULL && mLastDealloc->getBlockSize() == size )
	{
		// how fortunate, last used deallocator happens to be the one we need
		mLastDealloc->dealloc(ptr);
		return;
	}

	// find the first allocator for blocks equal to or larger than needed
	std::vector<ChunkAllocator>::iterator i =
		lower_bound( mPool.begin(), mPool.end(), size, ChunkAllocatorCompare() );

	zhAssert( i != mPool.end() &&
		i->getBlockSize() == size ); // that block must have been allocated *somewhere*
	mLastDealloc = &(*i);
	mLastDealloc->dealloc(ptr);
}

}
