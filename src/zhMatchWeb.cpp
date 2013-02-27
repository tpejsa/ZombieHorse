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

#include "zhMatchWeb.h"
#include "zhAnimation.h"
#include "zhAnimationDistanceGrid.h"
#include "zhAnimationIndex.h"

namespace zh
{

MatchWeb::Path::Path() : mNextPath(UINT_MAX), mNextPoint(UINT_MAX)
{
}

MatchWeb::Path::Path( const std::vector<AnimationDistanceGrid::Point>& points ) : mNextPath(UINT_MAX), mNextPoint(UINT_MAX)
{
	mPoints = points;
}

MatchWeb::Path::~Path()
{
}

float MatchWeb::Path::getAvgDistance() const
{
	float dist = 0;

	for( unsigned int pti = 0; pti < mPoints.size(); ++pti )
		dist += mPoints[pti].getDistance();

	return dist / mPoints.size();
}

void MatchWeb::Path::addPoint( const AnimationDistanceGrid::Point& pt )
{
	mPoints.push_back(pt);
}

void MatchWeb::Path::insertPoint( unsigned int ptIndex, const AnimationDistanceGrid::Point& pt )
{
	zhAssert( ptIndex < getNumPoints() );

	mPoints.insert( mPoints.begin() + ptIndex, pt );
}

void MatchWeb::Path::removePoint( unsigned int ptIndex )
{
	zhAssert( ptIndex < getNumPoints() );

	mPoints.erase( mPoints.begin() + ptIndex );
}

void MatchWeb::Path::clearPoints()
{
	mPoints.clear();
	mBranches.clear();
}

const AnimationDistanceGrid::Point& MatchWeb::Path::getPoint( unsigned int ptIndex ) const
{
	zhAssert( ptIndex < getNumPoints() );

	return mPoints[ptIndex];
}

const AnimationDistanceGrid::Point& MatchWeb::Path::getLastPoint() const
{
	zhAssert( !mPoints.empty() );

	return mPoints[ mPoints.size() - 1 ];
}

unsigned int MatchWeb::Path::getNumPoints() const
{
	return mPoints.size();
}

bool MatchWeb::Path::hasBranch( unsigned int ptIndex ) const
{
	std::map<unsigned int, unsigned int>::const_iterator bri = mBranches.find(ptIndex);

	if( bri != mBranches.end() )
		return true;

	return false;
}

unsigned int MatchWeb::Path::getBranch( unsigned int ptIndex ) const
{
	std::map<unsigned int, unsigned int>::const_iterator bri = mBranches.find(ptIndex);

	if( bri != mBranches.end() )
		return bri->second;

	return UINT_MAX;
}

void MatchWeb::Path::setBranch( unsigned int ptIndex, unsigned int pathIndex )
{
	mBranches[ptIndex] = pathIndex;
}

bool MatchWeb::Path::hasNext() const
{
	return mNextPath != UINT_MAX;
}

void MatchWeb::Path::getNext( unsigned int& pathIndex, unsigned int& ptIndex ) const
{
	pathIndex = mNextPath;
	ptIndex = mNextPoint;
}

void MatchWeb::Path::setNext( unsigned int pathIndex, unsigned int ptIndex )
{
	mNextPath = pathIndex;
	mNextPoint = ptIndex;
}

MatchWeb::MatchWeb( Index index, AnimationIndex* animIndex, unsigned int sampleRate )
: mInd(index), mAnimIndex(animIndex), mMdl(NULL), mSampleRate(sampleRate), mDistGrid(NULL)
{
	zhAssert( animIndex != NULL && index.getSegIndex1() < animIndex->getNumAnimationSegments() &&
		index.getSegIndex2() < animIndex->getNumAnimationSegments() );
	zhAssert( sampleRate > 0 );

	const AnimationSegment& seg1 = mAnimIndex->getAnimationSegment( mInd.getSegIndex1() );
	const AnimationSegment& seg2 = mAnimIndex->getAnimationSegment( mInd.getSegIndex2() );
	mNumSamples1 = (unsigned int)( seg1.getLength() * mSampleRate ) + 1;
	mNumSamples2 = (unsigned int)( seg2.getLength() * mSampleRate ) + 1;
}

MatchWeb::~MatchWeb()
{
	if( mDistGrid != NULL )
		delete mDistGrid;
}

MatchWeb::Index MatchWeb::getIndex() const
{
	return mInd;
}

AnimationIndex* MatchWeb::getAnimationIndex() const
{
	return mAnimIndex;
}

Model* MatchWeb::getModel() const
{
	return mMdl;
}

void MatchWeb::setModel( Model* mdl )
{
	mMdl = mdl;
}

const AnimationSegment& MatchWeb::getAnimation1() const
{
	return mAnimIndex->getAnimationSegment( mInd.getSegIndex1() );
}

unsigned int MatchWeb::getNumSamples1() const
{
	return mNumSamples1;
}

unsigned int MatchWeb::getFrameAtTime1( float time ) const
{
	const AnimationSegment& seg = getAnimation1();

	if( time < 0 )
		time = 0;
	else if( time > seg.getLength() )
		time = seg.getLength();

	unsigned int fi = unsigned int( time * mSampleRate + 0.5f );
	return fi < mNumSamples1 ? fi : mNumSamples1 - 1;
}

float MatchWeb::getTimeAtFrame1( unsigned int index ) const
{
	zhAssert( index < mNumSamples1 );

	return index / ((float)mSampleRate);
}

const AnimationSegment& MatchWeb::getAnimation2() const
{
	return mAnimIndex->getAnimationSegment( mInd.getSegIndex2() );
}

unsigned int MatchWeb::getNumSamples2() const
{
	return mNumSamples2;
}

unsigned int MatchWeb::getFrameAtTime2( float time ) const
{
	const AnimationSegment& seg = getAnimation2();

	if( time < 0 )
		time = 0;
	else if( time > seg.getLength() )
		time = seg.getLength();

	unsigned int fi = unsigned int( time * mSampleRate + 0.5f );
	return fi < mNumSamples2 ? fi : mNumSamples2 - 1;
}

float MatchWeb::getTimeAtFrame2( unsigned int index ) const
{
	zhAssert( index < mNumSamples2 );

	return index / ((float)mSampleRate);
}

unsigned int MatchWeb::getSampleRate() const
{
	return mSampleRate;
}

void MatchWeb::build( unsigned int resampleFactor, float wndLength, float minDist, float maxDistDiff,
					 float minChainLength, float maxBridgeLength )
{
	zhAssert( mMdl != NULL );
	zhAssert( resampleFactor > 0 );

	zhLog( "MatchWeb", "build", "Building match web for animation segments %u and %u.",
		mInd.getSegIndex1(), mInd.getSegIndex2() );

	const AnimationSegment& seg1 = getAnimation1();
	const AnimationSegment& seg2 = getAnimation2();

	//
	// Match web build algorithm:
	//
	// threshold values
	// - animation window length (for comparing two animation frames)
	// - maximum distance for local minima
	// - maximum distance difference (for time alignment computation) - 15%
	// - minimum chain length - 0.25s in Manhattan distance?)
	// - maximum bridge length - 2s (in Manhattan distance)
	// 
	// 
	// 1. build anim. distance grid for M1 and M2 at specified resolution
	// 2. find all local 1D minima, along with blend region
	// 3. for each local minimum, build a chain C (using time alignment algorithm)
	//     - search in bottom-left direction (left, bottom-left, bottom cells)
	//     - search in top-right direction (right, top-right, top cells)
	//     - use regular time alignment algorithm
	//     - discard chains shorter than minimum chain length
	// 4. for each chain C, identify nearby chains C' and connect to them with bridges
	//     - compute optimal path between each cell of C and each cell of C'
	//     - the path must lie entirely in the valid region
	//     - again, use time alignment algorithm
	//     - path length (in Manhattan distance) must be lower than maximum bridge length
	//      - retain the bridge with the smallest *average distance*
	// 5. resample the match web to high resolution
	//     - pad each chain C with 1 extra cell in each direction
	//     - upsample the padded chain to form a search region
	//     - in the vicinity of each end-point, find the minimum-value cell
	//     - build an optimal (now hi-res) path between new end-points (using time alignment!)
	//     - do the same for each bridge B, except now each new end-point
	//       must also lie on the corresponding chain
	// 6. in the end, we discard all data except paths
	//    (consisting of series of grid positions,
	//     with associated distance values and 2D alignment transforms.)
	//

	//
	// 1. Build anim. distance grid and find local minima:
	//

	//unsigned int sample_rate = mSampleRate/resampleFactor;
	unsigned int sample_rate = mSampleRate;
	AnimationDistanceGrid* grid = new AnimationDistanceGrid( mMdl, seg1, seg2, sample_rate );
	grid->build(wndLength);
	grid->findLocalMinima( minDist, false, true, maxDistDiff );

	//
	// 2. For each local minimum without left, bottom or bottom-left neighbors, build a chain C:
	//

	zhLog( "MatchWeb", "build", "Building chains..." );

	std::map< AnimationDistanceGrid::Index, unsigned int> pathi_assoc; // to prevent points from being added to multiple paths,
	// we note which path a point belongs to
	std::map< AnimationDistanceGrid::Index, unsigned int> pti_assoc; // also note the index of the point in that path
	for( unsigned int mini = 0; mini < grid->getNumLocalMinima(); ++mini )
	{
		Path chain;
		unsigned int mini1, mini2;
		AnimationDistanceGrid::Index minpti = grid->getLocalMinimum( mini );
		mini1 = minpti.first;
		mini2 = minpti.second;
		float d = grid->getDistance( AnimationDistanceGrid::Index( mini1, mini2 ) );

		// if it isn't the first in chain, ignore it
		// (we assume minima are sorted by indexes in ascending order)
		if( mini > 0 )
		{
			unsigned int prev_mini1, prev_mini2;
			AnimationDistanceGrid::Index prev_minpti;
			prev_minpti = grid->getLocalMinimum( mini - 1 );
			prev_mini1 = prev_minpti.first;
			prev_mini2 = prev_minpti.second;

			if( ( prev_mini1 == mini1 || prev_mini1 == mini1 - 1 ) &&
				( prev_mini2 == mini2 || prev_mini2 == mini2 - 1 ) )
				continue;
		}

		chain.addPoint(
			AnimationDistanceGrid::Point( AnimationDistanceGrid::Index( mini1, mini2 ),
			d, grid->getAlignTransf( AnimationDistanceGrid::Index( mini1, mini2 ) ) )
			);
		
		// search for other minima
		float td, trd, rd;
		unsigned int prev_mini1, prev_mini2,
			deg_ct1 = 1, deg_ct2 = 1;
		while(true)
		{
			AnimationDistanceGrid::Point pt( AnimationDistanceGrid::Index( 0, 0 ), 0, Model::Situation() );;

			prev_mini1 = mini1;
			prev_mini2 = mini2;

			td = grid->getDistance( AnimationDistanceGrid::Index( mini1, mini2 + 1 ) );
			trd = grid->getDistance( AnimationDistanceGrid::Index( mini1 + 1, mini2 + 1 ) );
			rd = grid->getDistance( AnimationDistanceGrid::Index( mini1 + 1, mini2 ) );

			// find the next local minimum in the chain
			if( grid->isLocalMinimum( AnimationDistanceGrid::Index( mini1 + 1, mini2 + 1 ), minDist, false ) )
			{
				pt = AnimationDistanceGrid::Point(
					AnimationDistanceGrid::Index( mini1 + 1, mini2 + 1 ),
					trd, grid->getAlignTransf( AnimationDistanceGrid::Index( mini1 + 1, mini2 + 1 ) )
					);
				++mini1;
				++mini2;
			}
			else if( grid->isLocalMinimum( AnimationDistanceGrid::Index( mini1 + 1, mini2 ), minDist, false ) )
			{
				pt = AnimationDistanceGrid::Point(
					AnimationDistanceGrid::Index( mini1 + 1, mini2 ),
					td, grid->getAlignTransf( AnimationDistanceGrid::Index( mini1 + 1, mini2 ) )
					);
				++mini1;
			}
			else if( grid->isLocalMinimum( AnimationDistanceGrid::Index( mini1, mini2 + 1 ), minDist, false ) )
			{
				pt = AnimationDistanceGrid::Point(
					AnimationDistanceGrid::Index( mini1, mini2 + 1 ),
					trd, grid->getAlignTransf( AnimationDistanceGrid::Index( mini1, mini2 + 1 ) )
					);
				++mini2;
			}
			else
			{
				break;
			}

			// if we have take a non-diagonal step in right direction,
			// update the degenerate frame counter, otherwise reset it
			if( mini2 == prev_mini2 )
				++deg_ct1;
			else
				deg_ct1 = 1;

			// if the chain contains too many degenerate frames,
			// discard last frame
			if( deg_ct1 > zhBlend_DegeneracyLimit )
			{
				break;
			}

			// if we have take a non-diagonal step in top direction,
			// update the degenerate frame counter, otherwise reset it
			if( mini1 == prev_mini1 )
				++deg_ct2;
			else
				deg_ct2 = 1;

			// if the chain contains too many degenerate frames,
			// discard last frame
			if( deg_ct2 > zhBlend_DegeneracyLimit )
			{
				break;
			}

			// add the minimum to the chain or join chains
			if( pathi_assoc.count( pt.getIndex() ) <= 0 )
			{
				chain.addPoint(pt);
			}
			else
			{
				chain.mNextPath = pathi_assoc[ pt.getIndex() ];
				chain.mNextPoint = pti_assoc[ pt.getIndex() ];
				break;
			}
		}

		// if the chain is long enough, add it as a path
		if( ((float)chain.getNumPoints()) / sample_rate >= minChainLength )
			mPaths.push_back(chain);

		// associate added points with the chain
		for( unsigned int pti = 0; pti < chain.getNumPoints(); ++pti )
		{
			pathi_assoc[ chain.getPoint(pti).getIndex() ] = mPaths.size() - 1;
			pti_assoc[ chain.getPoint(pti).getIndex() ] = pti;
		}
	}

	pathi_assoc.clear();
	pti_assoc.clear();

	//
	// 3. For each chain C, try to connect it to nearby chains
	// with an optimal path (bridge) B
	//

	zhLog( "MatchWeb", "build", "Connecting chains with bridges..." );

	std::vector<Path> bridges; // this is where we will temp. store bridges
	std::map< unsigned int, std::pair<unsigned int, unsigned int> > branches; // this is where we will temp. store branch mappings ( bridge index, ( source path index, source path point index ) )
	unsigned int max_brlen = (unsigned int)( maxBridgeLength * sample_rate ); // maximum bridge length in frames
	for( unsigned int ci = 0; ci < mPaths.size(); ++ci )
	{
		if( mPaths[ci].getNumPoints() <= 0 )
			continue;

		// compute this chain's AABB
		unsigned int lb0, bb0, rb0, tb0;
		_computePathAABB( ci, lb0, bb0, rb0, tb0, max_brlen );

		// find nearby chains
		for( unsigned int nci = 0; nci < mPaths.size(); ++nci )
		{
			if( ci == nci )
				continue;

			unsigned int lb, bb, rb, tb;
			_computePathAABB( nci, lb, bb, rb, tb, max_brlen );

			// if the AABBs intersect, the chains are nearby - try to make a bridge between each two points!
			if( !( rb0 < lb || rb < lb0 ||
				tb0 < bb || tb < bb0 ) )
			{
				std::vector<Path> bridges0;

				// also find the shortest bridge
				unsigned int min_blen = UINT_MAX;
				Path min_bridge;
				unsigned int min_pti, min_npti;
				for( unsigned int pti = 0; pti < mPaths[ci].getNumPoints(); ++pti )
				{
					for( unsigned int npti = 0; npti < mPaths[nci].getNumPoints(); ++npti )
					{
						AnimationDistanceGrid::Point pt = mPaths[ci].getPoint(pti);
						AnimationDistanceGrid::Point npt = mPaths[nci].getPoint(npti);

						if( pt.getIndex().first < npt.getIndex().first &&
							pt.getIndex().second < npt.getIndex().second &&
							pt.distanceToPoint(npt) < max_brlen )
						{
							std::vector<AnimationDistanceGrid::Point> bridge_pts;
							grid->computeOptimalPath( pt.getIndex(), npt.getIndex(), bridge_pts );
							Path bridge(bridge_pts);

							if( bridge.getNumPoints() > 0 &&
								bridge.getNumPoints() < min_blen )
							{
								bridge.removePoint( bridge.getNumPoints() - 1 );

								min_blen = bridge.getNumPoints();
								min_bridge = bridge;
								min_pti = pti;
								min_npti = npti;
							}
						}
					}
				}

				if( min_bridge.getNumPoints() <= 0 )
					continue;

				// add the shortest bridge to temp. container
				bridges.push_back( min_bridge );
				Path* bridge = &bridges[ bridges.size() - 1 ];

				// connect the bridge to source chain
				if( bridge->getPoint(0).getIndex().first == mPaths[ci].getPoint( mPaths[ci].getNumPoints() - 1 ).getIndex().first &&
					bridge->getPoint(0).getIndex().second == mPaths[ci].getPoint( mPaths[ci].getNumPoints() - 1 ).getIndex().second )
				{
					for( unsigned int pti = 1; pti < bridge->getNumPoints(); ++pti )
						mPaths[ci].addPoint( bridge->getPoint(pti) );

					bridge = &mPaths[ci];
					bridges.pop_back();
				}
				else
				{
					bridge->removePoint(0);

					if( bridge->getNumPoints() <= 0 )
						bridge->addPoint( mPaths[nci].getPoint(min_npti++) );

					branches.insert( make_pair( bridges.size() - 1, make_pair( ci, min_pti ) ) );

					// TODO: how to connect tangent chains?
					// bridge (line any path) can't have length 0!
					// for now, we connect them using length 1 bridge
					// that overlaps the target chain
				}

				// connect the shortest bridge to the destination chain
				if( mPaths[nci].getNumPoints() > min_npti )
				{
					bridge->mNextPath = nci;
					bridge->mNextPoint = min_npti;
				}
			}
		}
	}

	unsigned int num_chains = mPaths.size(); // number of chains

	// add bridges to path container
	mPaths.reserve( mPaths.size() + bridges.size() );
	mPaths.insert( mPaths.end(), bridges.begin(), bridges.end() );

	// update branch connections
	for( std::map< unsigned int, pair<unsigned int, unsigned int> >::iterator brai = branches.begin();
		brai != branches.end(); ++brai )
	{
		mPaths[ brai->second.first ].mBranches[ brai->second.second ] = mPaths.size() - bridges.size() + brai->first;
	}

	bridges.clear();
	branches.clear();

	//
	// 4. Upsample the match web to high resolution:
	//

	// TODO

	zhLog( "MatchWeb", "build", "Finished building match web for animation segments %u and %u.",
		mInd.getSegIndex1(), mInd.getSegIndex2() );

	//#ifdef _DEBUG
	mDistGrid = grid;
	//#else
	//delete grid;
	//#endif
}

void MatchWeb::addPath( const Path& path )
{
	mPaths.push_back(path);
}

void MatchWeb::removePath( unsigned int pathIndex )
{
	zhAssert( pathIndex < getNumPaths() );

	mPaths.erase( mPaths.begin() + pathIndex );
}

const MatchWeb::Path& MatchWeb::getPath( unsigned int pathIndex ) const
{
	zhAssert( pathIndex < getNumPaths() );

	return mPaths[pathIndex];
}

void MatchWeb::setPath( unsigned int pathIndex, const Path& path )
{
	zhAssert( pathIndex < getNumPaths() );

	mPaths[pathIndex] = path;
}

unsigned int MatchWeb::getNumPaths() const
{
	return mPaths.size();
}

unsigned int MatchWeb::search( const AnimationSegment& animSeg, std::vector<Match>& matches,
							  float maxOverlap ) const
{
	const AnimationSegment& seg1 = getAnimation1();
	const AnimationSegment& seg2 = getAnimation2();

	zhAssert( animSeg.getAnimation() == seg1.getAnimation() ||
		animSeg.getAnimation() == seg2.getAnimation() );

	if( mPaths.size() <= 0 )
		return 0;

	zhLog( "MatchWeb", "search", "Searching match web of animation segments %u and %u for matches to animation segment %u, %s [ %f, %f ].",
		mInd.getSegIndex1(), mInd.getSegIndex2(),
		animSeg.getAnimation()->getId(), animSeg.getAnimation()->getName().c_str(),
		animSeg.getStartTime(), animSeg.getEndTime() );

	unsigned int start_fr, end_fr;
	start_fr = animSeg.getAnimation() == seg1.getAnimation() ? getFrameAtTime1( animSeg.getStartTime() ) : getFrameAtTime2( animSeg.getStartTime() );
	end_fr = animSeg.getAnimation() == seg1.getAnimation() ? getFrameAtTime1( animSeg.getEndTime() ) : getFrameAtTime2( animSeg.getEndTime() );
	std::vector<Path> match_segs; // intersecting match web path segments
	
	// compute intersecting path segments
	for( unsigned int pti = 0; pti < getNumPaths(); ++pti )
	{
		_intersectPath( animSeg, getPath(pti), Path(), match_segs );
	}

	std::set<Match> all_matches;

	// compute matches
	for( unsigned int seg_i = 0; seg_i < match_segs.size(); ++seg_i )
	{
		unsigned int seg_endfr = animSeg.getAnimation() == seg1.getAnimation() ?
			match_segs[seg_i].getLastPoint().getIndex().first : match_segs[seg_i].getLastPoint().getIndex().second;

		float start_time = animSeg.getAnimation() == seg1.getAnimation() ? getTimeAtFrame2( match_segs[seg_i].getPoint(0).getIndex().second ) :
			getTimeAtFrame1( match_segs[seg_i].getPoint(0).getIndex().first ),
			end_time = animSeg.getAnimation() == seg1.getAnimation() ? getTimeAtFrame2( match_segs[seg_i].getLastPoint().getIndex().second ) :
			getTimeAtFrame1( match_segs[seg_i].getLastPoint().getIndex().first );
		all_matches.insert(
			Match(
			AnimationSegment( animSeg.getAnimation() == seg1.getAnimation() ? seg2.getAnimation() : seg1.getAnimation(),
			start_time, end_time ),
			match_segs[seg_i] )
			);
	}

	// discard overlapping matches
	while( !all_matches.empty() )
	{
		matches.push_back( *all_matches.begin() );
		all_matches.erase( all_matches.begin() );

		for( std::set<Match>::iterator match_i = all_matches.begin();
			match_i != all_matches.end(); )
		{
			if( match_i->getAnimationSegment().overlap(
				matches[ matches.size() - 1 ].getAnimationSegment() ) > maxOverlap )
			{
				std::set<Match>::iterator rmatch_i = match_i;
				++match_i;
				all_matches.erase( rmatch_i );
			}
			else
			{
				++match_i;
			}
		}
	}

	zhLog( "MatchWeb", "search", "Finished searching match web of animation segments %u and %u for matches to animation segment %u, %s [ %f, %f ]. Found %u matches.",
		mInd.getSegIndex1(), mInd.getSegIndex2(),
		animSeg.getAnimation()->getId(), animSeg.getAnimation()->getName().c_str(),
		animSeg.getStartTime(), animSeg.getEndTime(), matches.size() );
	
	return matches.size();
}

void MatchWeb::_computePathAABB( unsigned int pathIndex, unsigned int& lBound, unsigned int& bBound, 
		unsigned int& rBound, unsigned int& tBound, unsigned int extendBy ) const
{
	zhAssert( pathIndex < getNumPaths() );

	const Path& path = mPaths[pathIndex];

	// compute the AABB
	
	lBound = path.getPoint(0).getIndex().first;
	bBound = path.getPoint(0).getIndex().second;
	rBound = path.getPoint( path.getNumPoints() - 1 ).getIndex().first;
	tBound = path.getPoint( path.getNumPoints() - 1 ).getIndex().second;

	// extend the AABB
	
	rBound += extendBy;
	if( rBound >= mNumSamples1 )
		rBound = mNumSamples1 - 1;
	
	tBound += extendBy;
	if( tBound >= mNumSamples2 )
		tBound = mNumSamples2 - 1;
	
	if( lBound > extendBy )
		lBound -= extendBy;
	else
		lBound = 0;
	
	if( bBound > extendBy )
		bBound -= extendBy;
	else
		bBound = 0;
}

bool MatchWeb::_intersectPath( const AnimationSegment& animSeg, const Path& path, const Path& prevSeg,
							  std::vector<Path>& matchSegs ) const
{
	const AnimationSegment& seg1 = getAnimation1();
	const AnimationSegment& seg2 = getAnimation2();

	bool first_anim = animSeg.getAnimation() == seg1.getAnimation();
	unsigned int start_fr, end_fr;
	start_fr = first_anim ? getFrameAtTime1( animSeg.getStartTime() ) : getFrameAtTime2( animSeg.getStartTime() );
	end_fr = first_anim ? getFrameAtTime1( animSeg.getEndTime() ) : getFrameAtTime2( animSeg.getEndTime() );
	
	// ensure the start frame of the query anim. segment intersects this path
	if( path.getNumPoints() <= 0 ||
		first_anim && !( start_fr >= path.getPoint(0).getIndex().first && start_fr <= path.getLastPoint().getIndex().first ) ||
		!first_anim && !( start_fr >= path.getPoint(0).getIndex().second && start_fr <= path.getLastPoint().getIndex().second ) )
		return false;

	// TODO: optimize this algorithm

	// advance along the path and compute intersecting match segments
	bool in_seg = false;
	Path seg = prevSeg;
	for( unsigned int pti = 0; pti < path.getNumPoints(); ++pti )
	{
		const AnimationDistanceGrid::Point& pt = path.getPoint(pti);
		unsigned int cur_fr = first_anim ? pt.getIndex().first : pt.getIndex().second;

		if( cur_fr == start_fr )
		{
			// we have entered the intersecting segment
			in_seg = true;
		}

		if( in_seg )
		{
			// we are in the intersecting segment, extend it
			seg.addPoint(pt);

			if( path.hasBranch(pti) )
			{
				// a path branches off this one, new intersecting segment?

				unsigned int bri = path.getBranch(pti);
				unsigned int start_fr0 = first_anim ? getPath(bri).getPoint(0).getIndex().first :
					getPath(bri).getPoint(0).getIndex().second;
				AnimationSegment anim_seg( animSeg.getAnimation(),
					first_anim ? getTimeAtFrame1( start_fr0 ) : getTimeAtFrame2( start_fr0 ),
					animSeg.getEndTime() );

				_intersectPath( anim_seg, getPath(bri), seg, matchSegs );
			}

			if( pti == path.getNumPoints() - 1 && path.hasNext() )
			{
				// this path joins into another path, new intersecting segment?

				unsigned int npath_i, npti;
				path.getNext( npath_i, npti );
				unsigned int start_fr0 = first_anim ? getPath(npath_i).getPoint(npti).getIndex().first :
					getPath(npath_i).getPoint(npti).getIndex().second;
				AnimationSegment anim_seg( animSeg.getAnimation(),
					first_anim ? getTimeAtFrame1( start_fr0 ) : getTimeAtFrame2( start_fr0 ),
					animSeg.getEndTime() );

				_intersectPath( anim_seg, getPath(npath_i), seg, matchSegs );
			}
		}

		if( cur_fr == end_fr )
		{
			// we have reached the end of the intersection
			matchSegs.push_back(seg);
			break;
		}
	}

	return in_seg;
}

}
