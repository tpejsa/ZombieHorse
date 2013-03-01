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

#ifndef __zhAnimationDistanceGrid_h__
#define __zhAnimationDistanceGrid_h__

#include "zhPrereq.h"
#include "zhMath.h"
#include "zhSkeleton.h"
#include "zhAnimationSegment.h"

namespace zh
{

class Animation;

/**
* @brief Animation distance grid for comparing
* two animations.
*/
class zhDeclSpec AnimationDistanceGrid
{

public:

	typedef std::pair<unsigned int, unsigned int> Index; ///< Index of a point in the distance grid,
	///< consisting of indexes of sample frames in the first and second animation, respectively.

	/**
	* @brief Struct representing a point in the distance grid.
	*/
	struct zhDeclSpec Point
	{

	public:

		/**
		* Constructor.
		*/
		Point() : mInd( 0, 0 ), mDist(FLT_MAX)
		{
		}

		/**
		* Constructor.
		*
		* @param index Sample pair index.
		* @param dist Animation distance at the point.
		* @param alignTransf Aligning 2D transformation at the point.
		*/
		Point( const Index& index, float dist, const Skeleton::Situation& alignTransf )
			: mInd(index), mDist(dist), mAlignTransf(alignTransf)
		{
		}

		/**
		* Gets the sample pair index.
		*/
		const Index& getIndex() const { return mInd; }

		/**
		* Gets the animation distance at the point.
		*/
		float getDistance() const { return mDist; }

		/**
		* Gets the aligning 2D transformation at the point.
		*/
		const Skeleton::Situation& getAlignTransf() const { return mAlignTransf; }

		/**
		* Gets the Manhattan distance to another point.
		*/
		unsigned int distanceToPoint( const Point& pt ) const
		{
			unsigned int i1_0 = mInd.first, i2_0 = mInd.second,
				i1 = pt.mInd.first, i2 = pt.mInd.second;
			
			if( pt.mInd.first < mInd.first )
				std::swap( i1_0, i1 );
			
			if( pt.mInd.second < mInd.second )
				std::swap( i2_0, i2 );

			return ( i1 - i1_0 ) + ( i2 - i2_0 );
		}

	private:

		Index mInd;
		float mDist;
		Skeleton::Situation mAlignTransf;

	};

	/**
	* Constructor.
	*
	* @param skel Pointer to the skeleton.
	* animated by the compared animations.
	* @param anim1 The first animation segment.
	* @param anim2 The second animation segment.
	* @param sampleRate Animation sample rate.
	*/
	AnimationDistanceGrid( Skeleton* skel, const AnimationSegment& anim1, const AnimationSegment& anim2, unsigned int sampleRate = zhAnimation_SampleRate );

	/**
	* Destructor.
	*/
	~AnimationDistanceGrid();

	/**
	* Gets a pointer to the skeleton.
	*/
	Skeleton* getSkeleton() const;

	/**
	* Gets the weight of the specified bone.
	*/
	float getBoneWeight( unsigned short boneId ) const;

	/**
	* Sets the weight of the specified bone.
	*
	* @remark The sum of all bone weights must be 1.
	* By default, all bones are given the same weight.
	*/
	void setBoneWeight( unsigned short boneId, float weight );

	/**
	* Gets a pointer to the first animation.
	*/
	const AnimationSegment& getAnimation1() const;

	/**
	* Gets the number of samples taken from the first animation.
	*/
	unsigned int getNumSamples1() const;

	/**
	* Gets a pointer to the second animation.
	*/
	const AnimationSegment& getAnimation2() const;

	/**
	* Gets the number of samples taken from the second animation.
	*/
	unsigned int getNumSamples2() const;

	/**
	* Gets the index corresponding to the specified animation times.
	*/
	Index getFramesAtTimes( float time1, float time2 ) const;

	/**
	* Gets the animations times at the specified index.
	*/
	void getTimesAtFrames( const Index& index, float& time1, float& time2 ) const;

	/**
	* Gets the animation sample rate (in frames per second).
	*/
	unsigned int getSampleRate() const;

	/**
	* Builds the animation distance grid.
	*
	* @param wndLength Length of the animation window
	* used in animation comparison.
	* @remark When comparing a pair of sample frames,
	* the distance computation algorithm takes into account
	* not just character poses at those specific
	* frames, but also at the frames in their neighborhood.
	* wndLength parameter specifies the length of the animation window
	* centered on the compared frames and encompassing their neighborhood.
	* It typically does not need to be longer than
	* animation transition length.
	*/
	void build( float wndLength = 0.35 );

	/**
	* Gets a point in the distance grid.
	*
	* @param index Frame pair index.
	* @return Point in the distance grid.
	*/
	const Point& getPoint( const Index& index ) const;

	/**
	* Sets a point in the distance grid.
	*
	* @param pt Point in the distance grid.
	*/
	void setPoint( const Point& pt );

	/**
	* Gets the distance between two sample animation frames.
	*
	* @param index Frame pair index.
	* @return Distance value.
	*/
	float getDistance( const Index& index ) const;

	/**
	* Gets the normalized distance between two sample animation frames.
	*
	* @param index Frame pair index.
	* @return Distance value.
	*/
	float getNormDistance( const Index& index ) const;

	/**
	* Sets the distance between two sample animation frames.
	*
	* @param index Frame pair index.
	* @param dist Distance value.
	*/
	void setDistance( const Index& index, float dist );

	/**
	* Gets the aligning 2D transformation
	* between two sample animation frames.
	*
	* @param index Frame pair index.
	* @return Aligning 2D transformation.
	*/
	const Skeleton::Situation& getAlignTransf( const Index& index ) const;

	/**
	* Sets the aligning 2D transformation
	* between two sample animation frames.
	*
	* @param index Frame pair index.
	* @param transf Aligning 2D transformation.
	*/
	void setAlignTransf( const Index& index, const Skeleton::Situation& transf );

	/**
	* Gets the length of the animation window used
	* when comparing frames.
	*/
	float getAnimationWindowLength() const;

	/**
	* Finds the local minima in the animation distance grid.
	* 
	* @param minDist Maximum distance value for local minima, normalized
	* (lower value yields more realistic results).
	* @param only2d If false, the search will also locate 1D minima.
	* @param computeBlendRegion If true, computes the so-called blend region
	* in the distance grid by searching neighborhoods of local minima
	* for frame pairs that are similar enough to be blendable.
	* @param maxDistDiff Maximum difference in animation distance allowed
	* for two pairs of frames in the blend region (default value is 15%).
	* @return Number of local minima found.
	*/
	unsigned int findLocalMinima( float minDist = 0.05f, bool only2d = true,
		bool computeBlendRegion = false, float maxDistDiff = 0.15f );

	/**
	* Gets a local minimum.
	*
	* @param minIndex Index of the local minimum.
	* @return Frame pair index of the local minimum.
	*/
	const Index& getLocalMinimum( unsigned int minIndex ) const;

	/**
	* Gets the number of local minima found by findLocalMinima().
	*/
	unsigned int getNumLocalMinima() const;

	/**
	* Returns true if there is a local minimum in the distance grid
	* for the specified frame pair.
	*
	* @param index Frame pair index.
	* @param minDist Maximum distance value for local minima, normalized
	* (lower value yields more realistic results).
	* @param only2d If false, the search will also locate 1D minima.
	* @return true if the specified point is a local minimum, false otherwise.
	*/
	bool isLocalMinimum( const Index& index, float minDist = 0.05f, bool only2d = true ) const;

	/**
	* Returns true if the specified frame pair lies in the blend region,
	* otherwise false.
	*/
	bool isBlendRegion( const Index& index ) const;

	/**
	* Computes an optimal path through the distance grid between two points.
	*
	* @param srcIndex Index of the source point.
	* @param dstIndex Index of the destination point.
	* @param path Computed path (as a sequence of neighboring points).
	* @return Total cost of the optimal path.
	* @remark Optimal path minimizes the sum of distances along the path.
	* We employ an optimization algorithm called dynamic timewarping (DTW).
	*/
	float computeOptimalPath( const Index& srcIndex, const Index& dstIndex,
		std::vector<Point>& path ) const;

private:

	void _computeBoneWeights( Bone* bone );

	typedef std::pair<unsigned int, unsigned int> MarkerIndex; ///< Index of a marker,
	///< consisting of index of a sample frame and bone index.

	/**
	* @brief Annotation of a frame pair with precomputed timewarping data.
	*/
	struct _DTWAnnot
	{
		float cost;
		Index prev;
		_DTWAnnot() : cost(0), prev( 0, 0 ) { }
		_DTWAnnot( float cost, const Index& prev ) { this->cost = cost; this->prev = prev; }
	};

	float _computeOptimalPathSegment( const Index& srcIndex, const Index& dstIndex,
		std::vector<Point>& path ) const;
	_DTWAnnot _DTW( const Index& ptIndex, const std::map<Index,_DTWAnnot>& dtwAnnots ) const;

	Skeleton* mSkel;
	std::map<unsigned short, float> mBoneWeights;
	AnimationSegment mAnim1;
	unsigned int mNumSamples1;
	AnimationSegment mAnim2;
	unsigned int mNumSamples2;
	unsigned int mSampleRate;

	std::vector<Point> mGrid;
	float mMinDist, mMaxDist;
	float mWndLen;

	std::vector<Index> mMinima;
	std::set<Index> mBlendRegion;

};

}

#endif // __zhAnimationDistanceGrid_h__
