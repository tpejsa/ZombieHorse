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

#ifndef __zhMemoryPool_h__
#define __zhMemoryPool_h__

#include "zhPrereq.h"
#include "zhSingleton.h"
#include "zhMemoryManager.h"

namespace zh
{

/**
* Class representing a memory pool.
* The pool is organized into memory chunks, each chunk capable of
* holding memory blocks of a specific size. New chunks are created
* as needed.
* MemoryPool is best used for managing a large number of small objects.
* To change default chunk size and maximum object size supported
* by the MemoryPool, modify zhMemoryPool_ChunkSize and zhMemoryPool_MaxObjSize
* macros is zhPrereq.h.
*/
class zhDeclSpec MemoryPool : public MemoryManager, public Singleton<MemoryPool>
{

	friend class Singleton<MemoryPool>;

public:

	class ChunkAllocator
	{

	public:

		explicit ChunkAllocator( size_t blockSize, size_t chunkSize = zhMemoryPool_ChunkSize );
		~ChunkAllocator();
		inline size_t getBlockSize() const;
		inline void* alloc();
		inline void dealloc( void* ptr );

		// needed to prevent semantic errors due to temporary chunk copies getting destroyed
		ChunkAllocator( const ChunkAllocator& );
		ChunkAllocator& operator=( const ChunkAllocator& );
		inline void swap( ChunkAllocator& chAlloc );

	private:

		struct Chunk
		{

			inline void init( size_t blockSize, unsigned char numBlocks );
			inline void destroy();
			inline void* alloc( size_t blockSize );
			inline void dealloc( void* ptr, size_t blockSize );
			
			unsigned char* mData;
			unsigned char mNextFreeBlock;
			unsigned char mNumFreeBlocks;

		};
		
		size_t mBlockSize;
		unsigned char mNumBlocks;
		std::vector<Chunk> mChunks;
		Chunk* mLastAllocChunk;
		Chunk* mLastDeallocChunk;

		// needed to prevent errors due to temporary chunk copies getting destroyed
		// changes made to copies need to propagate to originals
		mutable const ChunkAllocator* mPrev;
		mutable const ChunkAllocator* mNext;

	};

private:

	/**
	* Constructor.
	*
	* @chunkSize Default chunk size.
	* @maxObjSize Maximum object size supported by the memory pool.
	*/
	MemoryPool( size_t chunkSize = zhMemoryPool_ChunkSize,
		size_t maxObjSize = zhMemoryPool_MaxObjSize
		);

public:

	inline unsigned int getNumChunkAllocators() const; ///< Gets the number of different-sized chunk allocators in the memory pool.
	inline const ChunkAllocator& getChunkAllocator( unsigned int index ) const; ///< Gets a pointer to the chunk allocator at the specified index.

protected:

	/**
	* Allocates a block of memory.
	*
	* @size Size of the memory block to allocate (in bytes).
	* @return Pointer to the allocated memory.
	*/
	void* _alloc( size_t size );

	/**
	* Deallocates a block of memory.
	*
	* @ptr Pointer to the memory block to deallocate.
	* @size Size of the memory block to deallocate (in bytes).
	*/
	void _dealloc( void* ptr, size_t size );

private:

	size_t mChunkSize; ///< Chunk size.
	size_t mMaxObjSize; ///< Maximum object size.
	std::vector<ChunkAllocator> mPool; ///< Pool of chunk allocators for different-sized blocks.
	ChunkAllocator* mLastAlloc; ///< Pointer to the last chunk allocator used for allocation.
	ChunkAllocator* mLastDealloc; ///< Pointer to the last chunk allocator used for deallocation.
	
};

}

#endif // __zhMemoryPool_h__
