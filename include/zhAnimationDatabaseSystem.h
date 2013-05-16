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

#ifndef __zhAnimationDatabaseSystem_h__
#define __zhAnimationDatabaseSystem_h__

#include "zhPrereq.h"
#include "zhIterators.h"
#include "zhSingleton.h"
#include "zhError.h"
#include "zhAnimationIndexManager.h"
#include "zhAnimationDatabaseEvents.h"
#include "zhParamAnimationBuilder.h"
#include "zhAnimationFrame.h"

#define zhAnimationDatabaseSystem AnimationDatabaseSystem::Instance()

namespace zh
{

enum SearchSystemError
{
	AnimDatabaseSystemError_None,
	AnimDatabaseSystemError_FileNotFound
};

/**
* @brief The animation search system, tasked with
* managing animation indexes.
*/
class zhDeclSpec AnimationDatabaseSystem : public Singleton<AnimationDatabaseSystem>,
	public EventEmitter<MatchWebBuiltEvent>,
	public EventEmitter<MatchFoundEvent>
{

	friend class Singleton<AnimationDatabaseSystem>;

public:

	zhDeclare_ErrorState

	typedef VectorIterator< std::vector<MatchGraph*> > MatchGraphIterator;
	typedef VectorConstIterator< std::vector<MatchGraph*> > MatchGraphConstIterator;

private:

	/**
	* Constructor.
	*/
	AnimationDatabaseSystem();

	/**
	* Destructor.
	*/
	~AnimationDatabaseSystem();

public:

	/**
	* Initializes the animation search system.
	*
	* @param cfgPath Configuration file path.
	* @return true if initialization has been successful, false otherwise.
	* Sets error codes:
	* - AnimDatabaseSystemError_None - no errors
	* - AnimDatabaseSystemError_FileNotFound - config. file not found or invalid
	*/
	bool init( const std::string& cfgPath = "config.xml" );

	/**
	* Select a representative subset of frames from the current animation
	* dabatase for training priors.
	*
	* @remark kd-clustering is used to select a representative frame set from
	* all the currently loaded animations. Warning: this process takes a LONG time.
	*/
	void buildTrainSet();

	/**
	* Get a pointer to the current training set of animation frames.
	*/
	AnimationFrameSet* getTrainSet() const;

	/**
	* Gets a pointer to the animation index manager instance.
	*/
	AnimationIndexManager* getAnimationIndexManager() const { return AnimationIndexManager::Instance(); }

	// TODO: add index build settings get/set functions

	/**
	* Gets the factor by which animation sample rate should be reduced
	* while building the match web.
	*/
	unsigned int getResampleFactor() const { return mResampleFact; }

	/**
	* Sets the factor by which animation sample rate should be reduced
	* while building the match web.
	*/
	void setResampleFactor( unsigned int resampleFactor = 3 ) { mResampleFact = resampleFactor; }

	/**
	* Gets the length of the frame window
	* used in animation comparison.
	*/
	float getWindowLength() const { return mWndLength; }

	/**
	* Sets the length of the frame window
	* used in animation comparison (should be long enough to incorporate
	* first- and second-order derivatives of animation parameters and
	* no longer than animation transition length).
	*/
	void setWindowLength( float wndLength = 0.35f ) { mWndLength = wndLength; }

	/**
	* Gets the distance value threshold for local minima, normalized.
	*/
	float getMinDistance() const { return mMinDist; }

	/**
	* Sets the distance value threshold for local minima, normalized
	* (lower value yields fewer matches, but they are of higher quality).
	*/
	void setMinDistance( float minDist = 0.05f ) { mMinDist = minDist; }

	/**
	* Gets the maximum difference in animation distance allowed
	* for two pairs of frames in a single minima chain.
	*/
	float getMaxDistanceDiff() const { return mMaxDistDiff; }

	/**
	* Sets the maximum difference in animation distance allowed
	* for two pairs of frames in a single minima chain
	* (default value is 15%).
	*/
	void setMaxDistanceDiff( float maxDistDiff = 0.15f ) { mMaxDistDiff = maxDistDiff; }

	/**
	* Gets the minimum length of a minima chain.
	* Shorter chains are discarded.
	*/
	float getMinChainLength() const { return mMinChainLength; }

	/**
	* Sets the minimum length of a minima chain.
	* Shorter chains are discarded (default value is 0.25s).
	*/
	void setMinChainLength( float minChainLength = 0.25f ) { mMinChainLength = minChainLength; }

	/**
	* Gets the maximum length of a bridge between a pair
	* of minima chains.
	*/
	float getMaxBridgeLength() const { return mMaxBridgeLength; }

	/**
	* Sets the maximum length of a bridge between a pair
	* of minima chains (default value is 2s).
	*/
	void setMaxBridgeLength( float maxBridgeLength = 1.f ) { mMaxBridgeLength = maxBridgeLength; }

	/**
	* Builds an index over the currently loaded animation data
	* for efficient animation search.
	*
	* @param id Animation index ID.
	* @param name Animation index name.
	* @param skel Pointer to the character skeleton.
	* @param labelFilter Comma- or semicolon-separated list of labels.
	* Animation index will be built only over animations that have
	* one of these labels in their names.
	* @return Pointer to the animation index.
	*/
	AnimationIndexPtr buildIndex( unsigned long id, const std::string& name, Skeleton* skel,
		const std::string& labelFilter = "" );

	/**
	* Builds an index over the given set of animations
	* for efficient animation search.
	*
	* @param id Animation index ID.
	* @param name Animation index name.
	* @param skel Pointer to the character skeleton.
	* @param rawAnims Raw animation sets.
	* @param labelFilter Comma- or semicolon-separated list of labels.
	* Animation index will be built only over animations that have
	* one of these labels in their names.
	* @return Pointer to the animation index.
	*/
	AnimationIndexPtr buildIndex( unsigned long id, const std::string& name, Skeleton* skel,
		std::vector<AnimationSetPtr> rawAnims, const std::string& labelFilter = "" );

	/**
	* Gets the maximum permitted overlap between
	* matching animation segments.
	*/
	float getMaxOverlap() const { return mMaxOverlap; }

	/**
	* Sets the maximum permitted overlap between
	* matching animation segments.
	*/
	void setMaxOverlap( float maxOverlap = 0.8f ) { mMaxOverlap = maxOverlap; }

	/**
	* Searches the raw animation dataset for animation segments
	* similar to the specified query animation segment.
	*
	* @param animSeg Query animation segment
	* @param maxOverlap Maximum permitted overlap between
	* matching animation segments.
	* @return Pointer to the match graph of search results
	* or NULL if there are no results.
	* @remark This variant of the search function searches *every*
	* animation index and builds a match graph for each.
	* It returns the match graph that contains
	* the most results. The other match graphs are still accessible.
	*/
	MatchGraph* search( const AnimationSegment& animSeg );

	/**
	* Searches the raw animation dataset for animation segments
	* similar to the specified query animation segment.
	*
	* @param animIndexId ID of the animation index that should be used
	* for searching.
	* @param animSeg Query animation segment
	* @param maxOverlap Maximum permitted overlap between
	* matching animation segments.
	* @return Pointer to the match graph of search results
	* or NULL if there are no results.
	*/
	MatchGraph* search( unsigned long animIndexId, const AnimationSegment& animSeg );

	/**
	* Searches the raw animation dataset for animation segments
	* similar to the specified query animation segment.
	*
	* @param animIndexName Name of the animation index that should be used
	* for searching.
	* @param animSeg Query animation segment
	* @param maxOverlap Maximum permitted overlap between
	* matching animation segments.
	* @return Pointer to the match graph of search results
	* or NULL if there are no results.
	*/
	MatchGraph* search( const std::string& animIndexName, const AnimationSegment& animSeg );

	/**
	* Deletes the specified match graph.
	*
	* @param index Match graph index.
	*/
	void deleteMatchGraph( unsigned int index );

	/**
	* Deletes all match graphs.
	*/
	void deleteAllMatchGraphs();

	/**
	* Gets the specified match graph.
	*
	* @param index Match graph index.
	* @return Pointer to the match graph.
	*/
	MatchGraph* getMatchGraph( unsigned int index ) const;

	/**
	* Gets the number of match graphs.
	*/
	unsigned int getNumMatchGraphs() const;

	/**
	* Gets an iterator over the vector of match graphs.
	*/
	MatchGraphIterator getMatchGraphIterator();

	/**
	* Gets a const iterator over the vector of match graphs.
	*/
	MatchGraphConstIterator getMatchGraphConstIterator() const;

	/**
	* true if the animation space build algorithm will identify matching
	* annotations in base animations, false otherwise.
	*/
	bool getMatchAnnots() const { return mMatchAnnots; }

	/**
	* If true, the animation space build algorithm will identify matching
	* annotations in base animations.
	*/
	void setMatchAnnots( bool matchAnnots = true ) { mMatchAnnots = matchAnnots; }

	/**
	* true if the animation space build algorithm will build
	* timewarp and alignment curves for improved visual quality of blends.
	*/
	bool getBuildBlendCurves() const { return mBuildBlendCurves; }

	/**
	* If true, the animation space build algorithm will build
	* timewarp and alignment curves for improved visual quality of blends.
	*/
	void setBuildBlendCurves( bool buildBlendCurves = true ) { mBuildBlendCurves = buildBlendCurves; }

	/**
	* Gets the knot spacing in timewarp and alignment curves.
	* If 1, a knot will be generated for every frame correspondence
	* in the base animations. Greater knot
	* spacing will reduce computational and memory requirements of blending,
	* at the expense of visual quality.
	*/
	unsigned int getKnotSpacing() const { return mKnotSpacing; }

	/**
	* Sets the knot spacing in timewarp and alignment curves.
	* If 1, a knot will be generated for every frame correspondence
	* in the base animations. Greater knot
	* spacing will reduce computational and memory requirements of blending,
	* at the expense of visual quality.
	*/
	void setKnotSpacing( unsigned int knotSpacing = 3 ) { mKnotSpacing = knotSpacing > 0 ? knotSpacing : 1; }

	/**
	* Builds an animation space from a set of matching animation segments,
	* allowing efficient real-time blending of the base animations.
	*
	* @param id Animation space ID.
	* @param name Animation space name.
	* @param skel Pointer to the character skeleton.
	* @param animSet Animation set to which the animation space
	* should be added.
	* @param matches Match graph specifying a set of matching
	* animation segments that will be used as base animations.
	* @param refNodeHandle Handle of the match graph node corresponding
	* to the animation segment that will be used as reference animation
	* in construction of the animation space.
	* @return Pointer to the animation space.
	*/
	AnimationSpace* buildAnimationSpace( unsigned short id, const std::string& name,
		Skeleton* skel, AnimationSetPtr animSet,
		MatchGraph* matches, unsigned short refNodeHandle = 0 ) const;

	/**
	* Detects and creates annotation matches in the animation space.
	*
	* @param animSpace Animation space.
	*/
	void computeAnnotMatches( AnimationSpace* animSpace ) const;

	/**
	* Builds a parametrization over the animation space.
	*
	* @param skel Character skeleton.
	* @param animSpace Animation space.
	* @param paramSpecs Vector containing specifications of all parameters.
	* @param paramClass Parametrization class ID.
	*/
	void parametrizeAnimSpace( Skeleton* skel, AnimationSpace* animSpace, const std::vector<AnimationParamSpec>& paramSpecs,
		AnimationParamClass paramClass = AnimationParam_DenseSampling );

	/**
	* Gets the maximum extrapolation allowed in the animation space.
	*/
	float getMaxExtrapolation() const;

	/**
	* Sets the maximum extrapolation allowed in the animation space.
	*/
	void setMaxExtrapolation( float maxExtrap = 0.15f );

	/**
	* Gets the minimum Euclidean distance between
	* two samples in the parametric space.
	*/
	float getMinSampleDistance() const;

	/**
	* Sets the minimum Euclidean distance between
	* two samples in the parametric space.
	*/
	void setMinSampleDistance( float minSampleDist = 0.00001f );

	/**
	* Builds transitions between animations in the specified dataset.
	*
	* @param skel Character skeleton.
	* @param labelFilter Comma- or semicolon-separated list of labels.
	* Animation index will be built only over animations that have
	* one of these labels in their names.
	*/
	void buildTransitions( Skeleton* skel, const std::string& labelFilter = "" );

	/**
	* Builds transitions between animations in the specified dataset.
	*
	* @param skel Character skeleton.
	* @param rawAnims Raw animations sets.
	* @param labelFilter Comma- or semicolon-separated list of labels.
	* Animation index will be built only over animations that have
	* one of these labels in their names.
	*/
	void buildTransitions( Skeleton* skel, std::vector<AnimationSetPtr> rawAnims,
		const std::string& labelFilter = "" );

	/**
	* Builds a transition between two animations.
	*
	* @param skel Character skeleton.
	* @param srcAnim Source parametrized animation.
	* @param trgAnim Target parametrized animation.
	* @return Number of transitions that have been built.
	*/
	unsigned int buildTransitions( Skeleton* skel, AnimationSpace* srcAnim, AnimationSpace* trgAnim );

	/**
	* Builds a transition between two animations.
	*
	* @param skel Character skeleton.
	* @param srcAnim Source parametrized animation.
	* @param trgAnim Target animation.
	* @return Number of transitions that have been built.
	*/
	unsigned int buildTransitions( Skeleton* skel, AnimationSpace* srcAnim, Animation* trgAnim );

	/**
	* Builds a transition between two animations.
	*
	* @param skel Character skeleton.
	* @param srcAnim Source animation.
	* @param trgAnim Target parametrized animation.
	* @return Number of transitions that have been built.
	*/
	unsigned int buildTransitions( Skeleton* skel, Animation* srcAnim, AnimationSpace* trgAnim );

	/**
	* Builds a transition between two animations.
	*
	* @param skel Character skeleton.
	* @param srcAnim Source animation.
	* @param trgAnim Target animation.
	* @return Number of transitions that have been built.
	*/
	unsigned int buildTransitions( Skeleton* skel, Animation* srcAnim, Animation* trgAnim );

private:

	void _computeFrameClusterCenters( AnimationFrameSet* frames, unsigned int numClusters );
	void _parseLabelFilter( const std::string& labelFilter, std::vector<std::string>& labels ) const;

	AnimationFrameSet* mTrainSet;

	unsigned int mResampleFact;
	float mWndLength;
	float mMinDist;
	float mMaxDistDiff;
	float mMinChainLength;
	float mMaxBridgeLength;

	float mMaxOverlap;

	bool mMatchAnnots;
	bool mBuildBlendCurves;
	unsigned int mKnotSpacing;
	std::vector<MatchGraph*> mMatchGraphs;

	float mMaxExtrap;
	float mMinSampleDist;

};

}

#endif // __zhAnimationDatabaseSystem_h__
