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

#ifndef __zhAnimationIndex_h__
#define __zhAnimationIndex_h__

#include "zhPrereq.h"
#include "zhIterators.h"
#include "zhError.h"
#include "zhResourceManager.h"
#include "zhAnimationSet.h"
#include "zhAnimationSystem.h"
#include "zhAnimationSegment.h"
#include "zhMatchWeb.h"
#include "zhMatchGraph.h"

namespace zh
{

// TODO: add events for the following:
// - when a match web has been built
// - when a set of matches has been found
// - when a match web has been loaded
// - when a match web has been saved
// - 

class AnimationIndex;

typedef SmartPtr<AnimationIndex> AnimationIndexPtr;

/**
* @brief Class representing an animation index,
* enabling high-performance animation retrieval
* and parametrization.
*/
class zhDeclSpec AnimationIndex : public Resource
{

public:

	typedef MapIterator< std::map<unsigned long, AnimationSegment> > AnimationSegmentIterator;
	typedef MapConstIterator< std::map<unsigned long, AnimationSegment> > AnimationSegmentConstIterator;

	typedef MapIterator< std::map<MatchWeb::Index, MatchWeb*> > MatchWebIterator;
	typedef MapConstIterator< std::map<MatchWeb::Index, MatchWeb*> > MatchWebConstIterator;

	/**
	* Constructor.
	*
	* @param id Animation index ID.
	* @param name Animation index name.
	* @param mgr Owning resource manager.
	*/
	AnimationIndex( unsigned long id, const std::string& name, ResourceManager* mgr );

	/**
	* Destructor.
	*/
	~AnimationIndex();

	/**
	* Gets the character model.
	*
	* @return Pointer to the model.
	*/
	Model* getModel() const;

	/**
	* Sets the character model.
	*
	* @param mdl Pointer to the model.
	*/
	void setModel( Model* mdl );

	/**
	* Adds an animation segment to the animation index.
	*
	* @param animSeg Animation segment.
	* @remark If the new animation segment overlaps fully or partially
	* with another animation segment, the two segments will be merged.
	*/
	void addAnimationSegment( const AnimationSegment& animSeg );

	/**
	* Removes an animation segment from the animation index.
	*
	* @param segIndex Animation segment index.
	*/
	void removeAnimationSegment( unsigned int segIndex );

	/**
	* Removes all animation segments from the animation index.
	*/
	void removeAllAnimationSegments();

	/**
	* Gets an indexed animation segment.
	*
	* @param segIndex Animation segment index.
	* @return Animation segment.
	*/
	const AnimationSegment& getAnimationSegment( unsigned int segIndex ) const;

	/**
	* Gets the number of indexed animation segments.
	*/
	unsigned int getNumAnimationSegments() const;

	// TODO: index raw animation *segments*, rather than whole animations
	// match web ID should be *segment index pair*, not raw anim. index pair
	// later add pre-indexing by semantic labels

	/**
	* Builds the animation index.
	*
	* @param resampleFactor The factor by which animation sample rate
	* should be reduced while building the match web. Once the match web
	* is built, it is upsampled back to the original sample rate.
	* @param wndLength Length of the frame window
	* used in animation comparison (should be long enough to incorporate
	* first- and second-order derivatives of animation parameters and
	* no longer than animation transition length).
	* @param minDist Maximum distance value for local minima, normalized
	* (lower value yields fewer matches, but they are of higher quality).
	* @param maxDistDiff Maximum difference in animation distance allowed
	* for two pairs of frames in a minima chain (default value is 15%).
	* @param minChainLength Minimum length of a minima chain.
	* Shorter chains are discarded (default value is 0.25s).
	* @param maxBridgeLength Maximum length of a bridge between a pair
	* of minima chains (default value is 2s).
	*/
	void buildIndex( unsigned int resampleFactor = 3,
		float wndLength = 0.35f, float minDist = 0.05f,
		float maxDistDiff = 0.15f, float minChainLength = 0.25f, float maxBridgeLength = 1.f );

	/**
	* Deletes the current animation index.
	*/
	void dropIndex();

	/**
	* Creates a new, empty match web.
	*
	* @param mwIndex Match web index.
	* @return Pointer to the match web.
	*/
	MatchWeb* createMatchWeb( MatchWeb::Index mwIndex, unsigned int sampleRate = zhAnimation_SampleRate );

	/**
	* Deletes a match web.
	*
	* @param mwIndex Match web index.
	*/
	void deleteMatchWeb( MatchWeb::Index mwIndex );

	/**
	* Deletes all match webs.
	*/
	void deleteAllMatchWebs();

	/**
	* Returns true if the match web exists, otherwise false.
	*
	* @param mwIndex Match web index.
	* @return true if the match web exists, false otherwise.
	*/
	bool hasMatchWeb( MatchWeb::Index mwIndex ) const;

	/**
	* Gets the specified match web.
	*
	* @param mwIndex Match web index.
	* @return Pointer to the match web or NULL
	* if the match web does not exist.
	*/
	MatchWeb* getMatchWeb( MatchWeb::Index mwIndex ) const;

	/**
	* Gets the number of match webs in the index.
	*/
	unsigned int getNumMatchWebs() const;

	/**
	* Gets an iterator over the map of match webs.
	*/
	MatchWebIterator getMatchWebIterator();

	/**
	* Gets a const iterator over the map of match webs.
	*/
	MatchWebConstIterator getMatchWebConstIterator() const;

	/**
	* Searches the raw animation dataset for animation segments
	* similar to the specified query animation segment.
	*
	* @param animSeg Query animation segment
	* @param maxOverlap Maximum permitted overlap between
	* matching animation segments.
	* @return Pointer to the match graph of search results
	* or NULL if there are no results.
	*/
	MatchGraph* search( const AnimationSegment& animSeg, float maxOverlap = 0.8f ) const;

	/**
	* Calculates the resource memory usage.
	*/
	size_t _calcMemoryUsage() const;

	/**
	* Unloads the resource, freeing up the memory it occupies.
	*/
	void _unload();

	/**
	* Creates a deep copy of the resource.
	*
	* @param clonePtr Pointer to the copy.
	*/
	void _clone( Resource* clonePtr ) const;

private:

	Model* mMdl;

	std::vector<AnimationSegment> mAnimSegs;
	std::map<MatchWeb::Index, MatchWeb*> mMatchWebs;

};

}

#endif // __zhAnimationIndex_h__
