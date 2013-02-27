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

#include "zhAnimationIndex.h"
#include "zhAnimationSearchSystem.h"
#include "zhAnimation.h"
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include <queue>

namespace zh
{

AnimationIndex::AnimationIndex( unsigned long id, const std::string& name, ResourceManager* mgr )
: Resource( id, name, mgr ), mMdl(NULL)
{
}

AnimationIndex::~AnimationIndex()
{
	dropIndex();
}

Model* AnimationIndex::getModel() const
{
	return mMdl;
}

void AnimationIndex::setModel( Model* mdl )
{
	mMdl = mdl;
}

void AnimationIndex::addAnimationSegment( const AnimationSegment& animSeg )
{
	AnimationSegment seg = animSeg;

	for( unsigned int segi = 0; segi < mAnimSegs.size(); ++segi )
	{
		if( mAnimSegs[segi].overlap(seg) > 0 )
		{
			seg = mAnimSegs[segi].merge(seg);
			mAnimSegs.erase( mAnimSegs.begin() + segi );
			--segi;
		}
	}

	mAnimSegs.push_back(seg);
}

void AnimationIndex::removeAnimationSegment( unsigned int segIndex )
{
	zhAssert( segIndex < mAnimSegs.size() );

	mAnimSegs.erase( mAnimSegs.begin() + segIndex );
}

void AnimationIndex::removeAllAnimationSegments()
{
	mAnimSegs.clear();
}

const AnimationSegment& AnimationIndex::getAnimationSegment( unsigned int segIndex ) const
{
	zhAssert( segIndex < mAnimSegs.size() );

	return mAnimSegs[segIndex];
}

unsigned int AnimationIndex::getNumAnimationSegments() const
{
	return mAnimSegs.size();
}

void AnimationIndex::buildIndex( unsigned int resampleFactor,
								float wndLength, float minDist, float maxDistDiff,
								float minChainLength, float maxBridgeLength )
{
	zhAssert( mMdl != NULL );

	dropIndex();

	zhLog( "AnimationIndex", "buildIndex", "Building animation index %u.", mId );

	// build match webs
	for( unsigned int seg1i = 0; seg1i < mAnimSegs.size(); ++seg1i )
	{
		for( unsigned int seg2i = 0; seg2i < mAnimSegs.size(); ++seg2i )
		{
			MatchWeb::Index mwi( seg1i, seg2i );

			if( hasMatchWeb(mwi) )
				continue;

			// we don't have a match web for this pair of animations, build one
			MatchWeb* mw = new MatchWeb( mwi, this, zhAnimation_SampleRate );
			mw->setModel(mMdl);
			mw->build( resampleFactor, wndLength, minDist, maxDistDiff, minChainLength, maxBridgeLength );
			mMatchWebs[mwi] = mw;

			// notify listeners
			MatchWebBuiltEvent evt( zhAnimationSearchSystem, mw );
			evt.emit();
		}
	}

	zhLog( "AnimationIndex", "buildIndex", "Finished building animation index %u. %u match webs built.", mId, getNumMatchWebs() );
}

void AnimationIndex::dropIndex()
{
	zhLog( "AnimationIndex", "dropIndex", "Dropping animation index %u.", mId );

	for( std::map<MatchWeb::Index, MatchWeb*>::iterator mwi = mMatchWebs.begin();
		mwi != mMatchWebs.end(); ++mwi )
		delete mwi->second;

	mMatchWebs.clear();
}

MatchWeb* AnimationIndex::createMatchWeb( MatchWeb::Index mwIndex, unsigned int sampleRate )
{
	zhAssert( !hasMatchWeb(mwIndex) );

	MatchWeb* mw = new MatchWeb( mwIndex, this, sampleRate );
	mMatchWebs[mwIndex] = mw;

	return mw;
}

void AnimationIndex::deleteMatchWeb( MatchWeb::Index mwIndex )
{
	std::map<MatchWeb::Index, MatchWeb*>::iterator mwi = mMatchWebs.find(mwIndex);

	if( mwi != mMatchWebs.end() )
	{
		delete mwi->second;
		mMatchWebs.erase(mwi);
	}
}

void AnimationIndex::deleteAllMatchWebs()
{
	for( std::map<MatchWeb::Index, MatchWeb*>::iterator mwi = mMatchWebs.begin();
		mwi != mMatchWebs.end(); ++mwi )
		delete mwi->second;

	mMatchWebs.clear();
}

bool AnimationIndex::hasMatchWeb( MatchWeb::Index mwIndex ) const
{
	return mMatchWebs.count(mwIndex) > 0;
}

MatchWeb* AnimationIndex::getMatchWeb( MatchWeb::Index mwIndex ) const
{
	std::map<MatchWeb::Index, MatchWeb*>::const_iterator mwi = mMatchWebs.find(mwIndex);

	if( mwi == mMatchWebs.end() )
		return NULL;

	return mwi->second;
}

unsigned int AnimationIndex::getNumMatchWebs() const
{
	return mMatchWebs.size();
}

AnimationIndex::MatchWebIterator AnimationIndex::getMatchWebIterator()
{
	return MatchWebIterator( mMatchWebs );
}

AnimationIndex::MatchWebConstIterator AnimationIndex::getMatchWebConstIterator() const
{
	return MatchWebConstIterator( mMatchWebs );
}

MatchGraph* AnimationIndex::search( const AnimationSegment& animSeg, float maxOverlap ) const
{
	zhAssert( animSeg.getAnimation() != NULL );

	zhLog( "AnimationIndex", "search", "Searching animation index %u, %s for animation segments similar to %u, %s [ %f, %f ].",
		mId, mName.c_str(),
		animSeg.getAnimation()->getId(), animSeg.getAnimation()->getName().c_str(),
		animSeg.getStartTime(), animSeg.getEndTime() );

	std::map<unsigned short, unsigned int> node_segs; // maps result subsegments in match graph nodes to corresponding indexed segments

	// create a match graph for search results
	MatchGraph* mg = new MatchGraph();
	MatchGraph::Node* root_node = mg->createRoot(animSeg);

	// find corresponding indexed segment
	for( unsigned int segi = 0; segi < mAnimSegs.size(); ++segi )
	{
		if( mAnimSegs[segi].getAnimation() == animSeg.getAnimation() &&
			animSeg.getStartTime() >= mAnimSegs[segi].getStartTime() &&
			animSeg.getEndTime() <= mAnimSegs[segi].getEndTime() )
		{
			node_segs[ root_node->getHandle() ] = segi;
			break;
		}
	}

	if( node_segs.empty() )
	{
		// query segment not indexed
		delete mg;
		return NULL;
	}

	// search for similar animation segments (breadth-first expansion of the match graph)
	std::queue<MatchGraph::Node*> queries;
	queries.push(root_node);
	while( !queries.empty() )
	{
		MatchGraph::Node* node = queries.front();
		queries.pop();
		const AnimationSegment& qseg = node->getAnimationSegment();

		for( unsigned int segi = 0; segi < mAnimSegs.size(); ++segi )
		{
			std::vector<MatchWeb::Match> matches;

			// get each match web that includes the query animation
			MatchWeb* mw = getMatchWeb( MatchWeb::Index( segi, node_segs[ node->getHandle() ] ) );
			if( mw == NULL )
				continue;

			// search the match web for similar animation segments
			mw->search( qseg, matches, maxOverlap );

			// create match graph nodes for new anim. segments
			for( unsigned int match_i = 0; match_i < matches.size(); ++match_i )
			{
				const MatchWeb::Path& mw_path = matches[match_i].getPath();
				std::vector<MatchGraph::MatchPoint> match_seq;
				float st1 = mw->getTimeAtFrame1( mw_path.getPoint(0).getIndex().first ),
					st2 = mw->getTimeAtFrame2( mw_path.getPoint(0).getIndex().second );
				for( unsigned int pti = 0; pti < mw_path.getNumPoints(); ++pti )
				{
					const AnimationDistanceGrid::Point& pt = mw_path.getPoint(pti);
					match_seq.push_back( MatchGraph::MatchPoint(
						mw->getTimeAtFrame1( pt.getIndex().first ) - st1,
						mw->getTimeAtFrame2( pt.getIndex().second ) - st2,
						pt.getAlignTransf() )
						);
				}

				// find potential duplicate matches:
				// node - query node
				// dnode - potential duplicate node
				// mdnode - potential duplicate node with max. overlap
				// mol - overlap for mdnode
				MatchGraph::Node* mdnode = NULL;
				float mol = 0;
				MatchGraph::NodeIterator node_i = mg->getNodeIterator();
				while( !node_i.end() )
				{
					MatchGraph::Node* dnode = node_i.next();

					float ol = dnode->getAnimationSegment().overlap( matches[match_i].getAnimationSegment() );
					if( ol > mol )
					{
						mol = ol;
						mdnode = dnode;
					}
				}

				// handle duplicate match
				if( mol >= 1.f - maxOverlap )
				{
					continue;
				}
				
				// match is legit, add new result
				MatchGraph::Node* new_node = mg->createNode( node->getHandle(), matches[match_i].getAnimationSegment(),
					match_seq, matches[match_i].getPath().getAvgDistance() );
				if( new_node != NULL )
				{
					queries.push(new_node);
					node_segs[ new_node->getHandle() ] = segi;

					// notify listeners
					MatchFoundEvent evt( zhAnimationSearchSystem, new_node );
					evt.emit();
				}

				// TODO: current impl. cuts corners, this is more correct:
				// if O <= Omin
				//    add new node for mw_path
				// else if O >= Omax
				//    M = average of M and Mnew
				//    add edge between Mq and Mnew
				// else
				//    discard Mnew
			}
		}
	}

	zhLog( "AnimationIndex", "search", "Finished searching animation index %u, %s for animation segments similar to %u, %s [ %f, %f ]. %u matches found",
		mId, mName.c_str(),
		animSeg.getAnimation()->getId(), animSeg.getAnimation()->getName().c_str(),
		animSeg.getStartTime(), animSeg.getEndTime(),
		mg->getNumNodes() );

	return mg;
}

size_t AnimationIndex::_calcMemoryUsage() const
{
	size_t mem_usage = 0;

	// compute memory usage of match webs
	MatchWebConstIterator mwi = getMatchWebConstIterator();
	while( !mwi.end() )
	{
		MatchWeb* mw = mwi.next();
		
		for( unsigned int path_i = 0; path_i < mw->getNumPaths(); ++path_i )
		{
			mem_usage += mw->getPath(path_i).getNumPoints() * sizeof( AnimationDistanceGrid::Point );
		}
	}

	return mem_usage;
}

void AnimationIndex::_unload()
{
	dropIndex();
}

void AnimationIndex::_clone( Resource* clonePtr ) const
{
	zhAssert( clonePtr != NULL );
	zhAssert( clonePtr->getClassId() == getClassId() );

	AnimationIndex* clone = static_cast<AnimationIndex*>(clonePtr);

	clone->setModel(mMdl);
	clone->mAnimSegs = mAnimSegs;

	// clone match webs
	MatchWebConstIterator mwi = getMatchWebConstIterator();
	while( !mwi.end() )
	{
		MatchWeb* mw = mwi.next();

		MatchWeb* clone_mw = new MatchWeb( mw->getIndex(), clone, mw->getSampleRate() );
		clone_mw->setModel( mw->getModel() );
		clone->mMatchWebs.insert( make_pair( clone_mw->getIndex(), clone_mw ) );
		
		for( unsigned int path_i = 0; path_i < mw->getNumPaths(); ++path_i )
		{
			clone_mw->addPath( mw->getPath(path_i) );
		}
	}
}

}
