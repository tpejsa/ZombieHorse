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

#ifndef __zhMatchWeb_h__
#define __zhMatchWeb_h__

#include "zhPrereq.h"
#include "zhIterators.h"
#include "zhMath.h"
#include "zhSkeleton.h"
#include "zhAnimationSegment.h"
#include "zhAnimationDistanceGrid.h"

namespace zh
{

class Animation;
class AnimationIndex;

class zhDeclSpec MatchWeb
{

public:

	/**
	* @brief Match web index, consisting of indexes of
	* two indexed animation segments.
	*/
	struct Index
	{

	public:

		Index( unsigned long seg1Index, unsigned long seg2Index )
		{
			if( seg1Index < seg2Index ) { mSegIndex1 = seg1Index; mSegIndex2 = seg2Index; }
			else { mSegIndex1 = seg2Index; mSegIndex2 = seg1Index; }
		}

		unsigned long getSegIndex1() const { return mSegIndex1; }
		unsigned long getSegIndex2() const { return mSegIndex2; }

		bool operator<( const Index& mwIndex ) const
		{
			return mSegIndex1 < mwIndex.mSegIndex1 ||
				!( mwIndex.mSegIndex1 < mSegIndex1 ) && mSegIndex2 < mwIndex.mSegIndex2;
		}
	
	private:

		unsigned long mSegIndex1;
		unsigned long mSegIndex2;

	};

	/**
	* @brief Struct representing a path (sequence of points)
	* in the match web.
	*/
	struct zhDeclSpec Path
	{

		friend class MatchWeb;

	public:

		/**
		* Constructor.
		*/
		Path();

		/**
		* Constructor.
		*
		* @param points Points that constitute the path.
		*/
		Path( const std::vector<AnimationDistanceGrid::Point>& points );

		/**
		* Destructor.
		*/
		~Path();

		/**
		* Gets average animation distance at points on this path.
		*/
		float getAvgDistance() const;

		/**
		* Adds a point to the path.
		*/
		void addPoint( const AnimationDistanceGrid::Point& pt );

		/**
		* Inserts a point into the path before the point
		* at the specified index.
		*/
		void insertPoint( unsigned int ptIndex, const AnimationDistanceGrid::Point& pt );

		/**
		* Removes a point from the path at the specified index.
		*/
		void removePoint( unsigned int ptIndex );

		/**
		* Clears all points from the path.
		*/
		void clearPoints();

		/**
		* Gets a point on the path.
		*
		* @param ptIndex Point index.
		* @return Point.
		*/
		const AnimationDistanceGrid::Point& getPoint( unsigned int ptIndex ) const;

		/**
		* Gets the last point on the path.
		*
		* @return Point.
		*/
		const AnimationDistanceGrid::Point& getLastPoint() const;

		/**
		* Gets the number of points on the path.
		*/
		unsigned int getNumPoints() const;

		/**
		* Returns true if a path branches off this path
		* at the specified point, otherwise false.
		*
		* @param ptIndex Index of the point where the branch begins.
		* @return true if the branch exists, otherwise false.
		*/
		bool hasBranch( unsigned int ptIndex ) const;

		/**
		* Gets the index of the path that branches off this path
		* at the specified point.
		*
		* @param ptIndex Index of the point where the branch begins.
		* @return Index of the branch (in the match web) or UINT_MAX
		* if a branch does not exist.
		*/
		unsigned int getBranch( unsigned int ptIndex ) const;

		/**
		* Sets the index of the path that branches off this path
		* at the specified point.
		*
		* @param ptIndex Index of the point where the branch begins.
		* @param pathIndex Index of the branch (in the match web) or UINT_MAX
		* if a branch does not exist.
		*/
		void setBranch( unsigned int ptIndex, unsigned int pathIndex );

		/**
		* Returns true if this path joins with another path.
		*/
		bool hasNext() const;

		/**
		* Gets the index of the path that this path
		* joins into or UINT_MAX if such a path does not exist.
		* Also gets the index of the point in the next path
		* where the paths are joined.
		*/
		void getNext( unsigned int& pathIndex, unsigned int& ptIndex ) const;

		/**
		* Sets the index of the path that this path
		* joins into. UINT_MAX means such a path does not exist.
		* Also sets the index of the point in the next path
		* where the paths are joined.
		*/
		void setNext( unsigned int pathIndex, unsigned int ptIndex );

	private:

		std::vector<AnimationDistanceGrid::Point> mPoints;

		std::map<unsigned int, unsigned int> mBranches;
		unsigned int mNextPath, mNextPoint;

	};

	/**
	* @brief Specifies an animation segment that is similar
	* to another animation segment, along with the corresponding
	* match sequence (path in the match web).
	*/
	struct zhDeclSpec Match
	{

	public:

		/**
		* Constructor.
		*
		* @param Animation segment.
		* @param path Match web path.
		*/
		Match( const AnimationSegment& seg, const Path& path )
			: mSeg(seg), mPath(path) { }

		/**
		* Gets this match's animation segment.
		*/
		const AnimationSegment& getAnimationSegment() const { return mSeg; }

		/**
		* Gets the match web path that corresponds
		* to the animation segment.
		*/
		const Path& getPath() const { return mPath; }

		bool operator <( const Match& m ) const { return mPath.getAvgDistance() < m.mPath.getAvgDistance(); }

	private:

		AnimationSegment mSeg;
		Path mPath;

	};

	/**
	* Constructor.
	*
	* @param index Match web index.
	* @param animIndex Pointer to the animation index.
	* @param sampleRate Animation sample rate.
	*/
	MatchWeb( Index index, AnimationIndex* animIndex, unsigned int sampleRate = zhAnimation_SampleRate );

	/**
	* Destructor.
	*/
	virtual ~MatchWeb();

	/**
	* Gets the match web index.
	*/
	Index getIndex() const;

	/**
	* Gets a pointer to the owning animation index.
	*/
	AnimationIndex* getAnimationIndex() const;

	/**
	* Gets the character skeleton.
	*
	* @return Pointer to the skeleton.
	*/
	Skeleton* getSkeleton() const;

	/**
	* Sets the character skeleton.
	*
	* @param skel Pointer to the skeleton.
	*/
	void setSkeleton( Skeleton* skel );

	/**
	* Gets the first indexed animation.
	*/
	const AnimationSegment& getAnimation1() const;

	/**
	* Gets the number of samples taken from the first animation.
	*/
	unsigned int getNumSamples1() const;

	/**
	* Gets the sample frame index corresponding to the specified
	* time on the first animation.
	*/
	unsigned int getFrameAtTime1( float time ) const;

	/**
	* Gets the time of sample frame in the second animation
	* at the specified index.
	*/
	float getTimeAtFrame1( unsigned int index ) const;

	/**
	* Gets the second indexed animation.
	*/
	const AnimationSegment& getAnimation2() const;

	/**
	* Gets the number of samples taken from the second animation.
	*/
	unsigned int getNumSamples2() const;

	/**
	* Gets the sample frame index corresponding to the specified
	* time on the second animation.
	*/
	unsigned int getFrameAtTime2( float time ) const;

	/**
	* Gets the time of sample frame in the first animation
	* at the specified index.
	*/
	float getTimeAtFrame2( unsigned int index ) const;

	/**
	* Gets the animation sample rate (in frames per second).
	*/
	unsigned int getSampleRate() const;

	/**
	* Builds the match web.
	*
	* @param resampleFactor The factor by which animation sample rate
	* should be reduced while building the match web. Once the match web
	* is built, it is upsampled back to the original sample rate.
	* @param wndLength Length of the animation window
	* used in animation comparison (should be long enough to incorporate
	* first- and second-order derivatives af animation parameters and
	* no longer than animation transition length).
	* @param minDist Maximum distance value for local minima, normalized
	* (lower value yields fewer matches, but they are of higher quality).
	* @param maxDistDiff Maximum difference in animation distance allowed
	* for two pairs of frames in a minima chain (default value is 15%).
	* @param minChainLength Minimum length of a minima chain.
	* Shorter chains are discarded. Default value is 0.25s.
	* @param maxBridgeLength Maximum length of a bridge between a pair
	* of minima chains. Default value is 2s.
	*/
	virtual void build( unsigned int resampleFactor = 3, float wndLength = 0.35f, float minDist = 0.05f,
		float maxDistDiff = 0.15f, float minChainLength = 0.25f, float maxBridgeLength = 1.f );

	/**
	* Gets the animation distance grid.
	*/
	AnimationDistanceGrid* getDistanceGrid() { return mDistGrid; }

	/**
	* Adds a path through the match web.
	*
	* @param path Match web path.
	*/
	void addPath( const Path& path );

	/**
	* Removes a path from the match web.
	*
	* @param pathIndex Match web path index.
	*/
	void removePath( unsigned int pathIndex );

	/**
	* Gets a path through the match web.
	*
	* @param pathIndex Match web path index.
	*/
	const Path& getPath( unsigned int pathIndex ) const;

	/**
	* Sets a path through the match web.
	*
	* @param pathIndex Match web path index.
	*/
	void setPath( unsigned int pathIndex, const Path& path );

	/**
	* Gets the number of paths in the match web.
	*/
	unsigned int getNumPaths() const;

	/**
	* Searches for animation segments in one animation
	* that match the specified animation segment from the other animation,
	* along with corresponding match web paths.
	*
	* @param seg Query animation segment.
	* @param matches Vector for storing all the matches
	* that have been found.
	* @param maxOverlap Maximum permitted overlap between
	* matching animation segments.
	* If overlap is greater than this value, one of the segments is discarded.
	* @return Number of matches found.
	*/
	virtual unsigned int search( const AnimationSegment& animSeg, std::vector<Match>& matches,
		float maxOverlap = 0.8f ) const;

protected:

	void _computePathAABB( unsigned int pathIndex, unsigned int& lBound, unsigned int& bBound, 
		unsigned int& rBound, unsigned int& tBound, unsigned int extendBy = 0 ) const;
	
	/**
	* Computes an intersection between the match web paths and 
	* an animation segment.
	*
	* @param animSeg Query animation segment.
	* @param path Match web path.
	* @param prevSeg Previous intersecting match web path segment
	* (for continuing intersection with branching/joining paths).
	* @param matchSegs Intersecting match web path segments.
	* @return true if an intersection exists, otherwise false.
	*/
	bool _intersectPath( const AnimationSegment& animSeg, const Path& path, const Path& prevSeg,
		std::vector<Path>& matchSegs ) const;

	Index mInd;
	AnimationIndex* mAnimIndex;
	Skeleton* mSkel;
	unsigned int mNumSamples1;
	unsigned int mNumSamples2;
	unsigned int mSampleRate;

	std::vector<Path> mPaths;
	AnimationDistanceGrid* mDistGrid;

};

}

#endif // __zhMatchWeb_h__
