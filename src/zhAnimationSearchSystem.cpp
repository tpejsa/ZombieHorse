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

#include "zhAnimationSearchSystem.h"
#include "zhString.h"
#include "zhAnimationSet.h"
#include "zhAnimation.h"
#include "zhAnimationSpace.h"
#include "zhBoneAnimationTrack.h"
#include "zhAnimationManager.h"
#include "zhZHILoader.h"
#include "zhZHISerializer.h"
#include "zhAnimationSpaceBuilder.h"
#include "zhAnnotationMatchMaker.h"
#include "zhDenseSamplingParamBuilder.h"
#include "zhAnimationTransitionBuilder.h"
#include <boost/tokenizer.hpp>
#include <cctype>

namespace zh
{

AnimationSearchSystem::AnimationSearchSystem()
: mResampleFact(3), mWndLength(0.35f), mMinDist(0.05f), mMaxDistDiff(0.15f),
mMinChainLength(0.25f), mMaxBridgeLength(1.f),
mMaxOverlap(0.8f),
mMatchAnnots(true), mBuildBlendCurves(true), mKnotSpacing(3),
mMaxExtrap(0.15f), mMinSampleDist(0.00001f)
{
}

AnimationSearchSystem::~AnimationSearchSystem()
{
	deleteAllMatchGraphs();
}

bool AnimationSearchSystem::init( const std::string& cfgPath )
{
	zhLog( "AnimationSearchSystem", "init",
		"Initializing animation search system. Using configuration file %s.",
		cfgPath.c_str() );

	// create instances of various singletons
	AnimationIndexManager::Instance();
	
	// register resource loaders and serializers
	zhRegister_ResourceLoader( getAnimationIndexManager(), ZHILoader );
	zhRegister_ResourceSerializer( getAnimationIndexManager(), ZHISerializer );

	// load config.xml
	// TODO

	zhSetErrorCode( AnimSearchSystemError_None );
	return true;
}

AnimationIndexPtr AnimationSearchSystem::buildIndex( unsigned long id, const std::string& name, Model* mdl,
													const std::string& labelFilter )
{
	std::vector<AnimationSetPtr> ranims;

	// get raw animation sets
	AnimationManager::ResourceConstIterator rai = zhAnimationSystem->getAnimationManager()->getResourceConstIterator();
	while( !rai.end() )
	{
		AnimationSetPtr ras = AnimationSetPtr::DynamicCast<Resource>( rai.next() );
		ranims.push_back(ras);
	}

	return buildIndex( id, name, mdl, ranims, labelFilter );
}

AnimationIndexPtr AnimationSearchSystem::buildIndex( unsigned long id, const std::string& name, Model* mdl,
													std::vector<AnimationSetPtr> rawAnims, const std::string& labelFilter )
{
	AnimationIndexManager* aimgr = getAnimationIndexManager();
	zhAssert( !aimgr->hasResource(id) && !aimgr->hasResource(name) );
	zhAssert( mdl != NULL );

	// create empty anim. index
	AnimationIndexPtr anim_index = AnimationIndexPtr::DynamicCast<Resource>( aimgr->createResource( id, name ) );
	anim_index->setModel(mdl);

	// get anim. labels
	std::vector<std::string> labels;
	_parseLabelFilter( labelFilter, labels );

	// add raw animations to the anim. index
	for( unsigned int rasi = 0; rasi < rawAnims.size(); ++rasi )
	{
		AnimationSet::AnimationConstIterator rai = rawAnims[rasi]->getAnimationConstIterator();
		while( !rai.end() )
		{
			Animation* anim = rai.next();

			// filter animations by labels
			if( labels.size() > 0 )
			{
				bool skip = false;
				std::string anim_name = anim->getName();

				std::transform( anim_name.begin(), anim_name.end(), anim_name.begin(),
					( int(*)(int) )std::tolower );

				for( unsigned int li = 0; li < labels.size(); ++li )
				{
					if( anim_name.find( labels[li] ) == std::string::npos )
					{
						skip = true;
						break;
					}
				}

				if( skip )
					continue;
			}

			// add animation to the index
			anim_index->addAnimationSegment( AnimationSegment( anim, 0, anim->getLength() ) );
		}
	}

	// TODO: add filtering by annotations

	// build anim. index
	anim_index->buildIndex( mResampleFact, mWndLength, mMinDist,
		mMaxDistDiff, mMinChainLength, mMaxBridgeLength );

	return anim_index;
}

MatchGraph* AnimationSearchSystem::search( const AnimationSegment& animSeg )
{
	deleteAllMatchGraphs();

	MatchGraph* best_mg = NULL;
	AnimationIndexManager* mgr = getAnimationIndexManager();
	AnimationIndexManager::ResourceConstIterator res_i = mgr->getResourceConstIterator();
	while( !res_i.end() )
	{
		AnimationIndexPtr anim_index = AnimationIndexPtr::DynamicCast<Resource>( res_i.next() );
		MatchGraph* mg = anim_index->search( animSeg, mMaxOverlap );

		if( mg == NULL )
		{
			continue;
		}
		else
		{
			mMatchGraphs.push_back(mg);

			if( best_mg == NULL || best_mg->getNumNodes() < mg->getNumNodes() )
				best_mg = mg;
		}
	}

	return best_mg;
}

MatchGraph* AnimationSearchSystem::search( unsigned long animIndexId, const AnimationSegment& animSeg )
{
	zhAssert( getAnimationIndexManager()->hasResource(animIndexId) );

	deleteAllMatchGraphs();

	AnimationIndexPtr anim_index = AnimationIndexPtr::DynamicCast<Resource>( getAnimationIndexManager()->getResource(animIndexId) );
	MatchGraph* mg = anim_index->search( animSeg, mMaxOverlap );
	if( mg != NULL )
		mMatchGraphs.push_back(mg);

	return mg;
}

MatchGraph* AnimationSearchSystem::search( const std::string& animIndexName, const AnimationSegment& animSeg )
{
	zhAssert( getAnimationIndexManager()->hasResource(animIndexName) );

	AnimationIndexPtr anim_index = AnimationIndexPtr::DynamicCast<Resource>( getAnimationIndexManager()->getResource(animIndexName) );

	return search( anim_index->getId(), animSeg );
}

void AnimationSearchSystem::deleteMatchGraph( unsigned int index )
{
	zhAssert( index < mMatchGraphs.size() );

	delete mMatchGraphs[index];
	mMatchGraphs.erase( mMatchGraphs.begin() + index );
}

void AnimationSearchSystem::deleteAllMatchGraphs()
{
	for( std::vector<MatchGraph*>::iterator mgi = mMatchGraphs.begin();
		mgi != mMatchGraphs.end(); ++mgi )
		delete *mgi;

	mMatchGraphs.clear();
}

MatchGraph* AnimationSearchSystem::getMatchGraph( unsigned int index ) const
{
	zhAssert( index < mMatchGraphs.size() );

	return mMatchGraphs[index];
}

unsigned int AnimationSearchSystem::getNumMatchGraphs() const
{
	return mMatchGraphs.size();
}

AnimationSearchSystem::MatchGraphIterator AnimationSearchSystem::getMatchGraphIterator()
{
	return MatchGraphIterator(mMatchGraphs);
}

AnimationSearchSystem::MatchGraphConstIterator AnimationSearchSystem::getMatchGraphConstIterator() const
{
	return MatchGraphConstIterator(mMatchGraphs);
}

AnimationSpace* AnimationSearchSystem::buildAnimationSpace( unsigned short id, const std::string& name,
														   Model* mdl, AnimationSetPtr animSet,
														   MatchGraph* matches, unsigned short refNodeHandle ) const
{
	zhAssert( mdl != NULL );
	zhAssert( animSet != NULL );
	zhAssert( !animSet->hasAnimationSpace(id) && !animSet->hasAnimationSpace(name) );

	// create empty animation space
	AnimationSpace* anim_space = animSet->createAnimationSpace( id, name );

	// build animation space
	AnimationSpaceBuilder* asb = new AnimationSpaceBuilder( mdl, anim_space );
	asb->setBuildBlendCurves(mBuildBlendCurves);
	asb->setKnotSpacing(mKnotSpacing);
	asb->setMatchAnnotations(mMatchAnnots);
	asb->build( matches, refNodeHandle );
	delete asb;

	return anim_space;
}

void AnimationSearchSystem::computeAnnotMatches( AnimationSpace* animSpace ) const
{
	zhAssert( animSpace != NULL );

	AnnotationMatchMaker* mm = new AnnotationMatchMaker(animSpace);
	mm->makeMatches();
}

void AnimationSearchSystem::parametrizeAnimSpace( Model* mdl, AnimationSpace* animSpace, const std::vector<AnimationParamSpec>& paramSpecs,
												 AnimationParamClass paramClass )
{
	zhAssert( animSpace != NULL );
	zhAssert( mdl != NULL );

	DenseSamplingParamBuilder* pab = new DenseSamplingParamBuilder( mdl, animSpace );
	pab->setMaxExtrapolation(mMaxExtrap);
	pab->setMinSampleDistance(mMinSampleDist);
	pab->parametrize(paramSpecs);
	delete pab;
}

float AnimationSearchSystem::getMaxExtrapolation() const
{
	return mMaxExtrap;
}

void AnimationSearchSystem::setMaxExtrapolation( float maxExtrap )
{
	mMaxExtrap = maxExtrap;
}

float AnimationSearchSystem::getMinSampleDistance() const
{
	return mMinSampleDist;
}

void AnimationSearchSystem::setMinSampleDistance( float minSampleDist )
{
	mMinSampleDist = minSampleDist;
}

void AnimationSearchSystem::buildTransitions( Model* mdl, const std::string& labelFilter )
{
	std::vector<AnimationSetPtr> ranims;

	// get raw animation sets
	AnimationManager::ResourceConstIterator rai = zhAnimationSystem->getAnimationManager()->getResourceConstIterator();
	while( !rai.end() )
	{
		AnimationSetPtr ras = AnimationSetPtr::DynamicCast<Resource>( rai.next() );
		ranims.push_back(ras);
	}

	return buildTransitions(mdl, ranims, labelFilter );
}

void AnimationSearchSystem::buildTransitions( Model* mdl, std::vector<AnimationSetPtr> rawAnims,
											 const std::string& labelFilter )
{
	zhAssert( mdl != NULL );

	// get anim. labels
	std::vector<std::string> labels;
	_parseLabelFilter( labelFilter, labels );

	// filter animation clips
	std::vector<Animation*> anims;
	for( unsigned int rasi = 0; rasi < rawAnims.size(); ++rasi )
	{
		AnimationSet::AnimationConstIterator rai = rawAnims[rasi]->getAnimationConstIterator();
		while( !rai.end() )
		{
			Animation* anim = rai.next();

			// filter animations by labels
			if( labels.size() > 0 )
			{
				bool skip = false;
				std::string anim_name = anim->getName();

				std::transform( anim_name.begin(), anim_name.end(), anim_name.begin(),
					( int(*)(int) )std::tolower );

				for( unsigned int li = 0; li < labels.size(); ++li )
				{
					if( anim_name.find( labels[li] ) == std::string::npos )
					{
						skip = true;
						break;
					}
				}

				if( skip )
					continue;
			}

			// add animation clip
			anims.push_back(anim);
		}
	}

	// build transitions between each two anims
	AnimationTransitionBuilder* ptb = new AnimationTransitionBuilder(mdl);
	for( unsigned int anim_i1 = 0; anim_i1 < anims.size(); ++anim_i1 )
	{
		for( unsigned int anim_i2 = anim_i1; anim_i2 < anims.size(); ++anim_i2 )
		{
			ptb->buildTransitions( anims[anim_i1], anims[anim_i2], mWndLength, mMinDist, true );
		}
	}
	delete ptb;
}

unsigned int AnimationSearchSystem::buildTransitions( Model* mdl, AnimationSpace* srcAnim, AnimationSpace* trgAnim )
{
	zhAssert( mdl != NULL );
	zhAssert( srcAnim != NULL );
	zhAssert( trgAnim != NULL );

	AnimationTransitionBuilder* ptb = new AnimationTransitionBuilder(mdl);
	unsigned int num_built = ptb->buildTransitions( srcAnim, trgAnim, mWndLength, mMinDist );
	delete ptb;

	return num_built;
}

unsigned int AnimationSearchSystem::buildTransitions( Model* mdl, AnimationSpace* srcAnim, Animation* trgAnim )
{
	zhAssert( mdl != NULL );
	zhAssert( srcAnim != NULL );
	zhAssert( trgAnim != NULL );

	AnimationTransitionBuilder* ptb = new AnimationTransitionBuilder(mdl);
	unsigned int num_built = ptb->buildTransitions( srcAnim, trgAnim, mWndLength, mMinDist );
	delete ptb;

	return num_built;
}

unsigned int AnimationSearchSystem::buildTransitions( Model* mdl, Animation* srcAnim, AnimationSpace* trgAnim )
{
	zhAssert( mdl != NULL );
	zhAssert( srcAnim != NULL );
	zhAssert( trgAnim != NULL );

	AnimationTransitionBuilder* ptb = new AnimationTransitionBuilder(mdl);
	unsigned int num_built = ptb->buildTransitions( srcAnim, trgAnim, mWndLength, mMinDist );
	delete ptb;

	return num_built;
}

unsigned int AnimationSearchSystem::buildTransitions( Model* mdl, Animation* srcAnim, Animation* trgAnim )
{
	zhAssert( mdl != NULL );
	zhAssert( srcAnim != NULL );
	zhAssert( trgAnim != NULL );

	AnimationTransitionBuilder* ptb = new AnimationTransitionBuilder(mdl);
	unsigned int num_built = ptb->buildTransitions( srcAnim, trgAnim, mWndLength, mMinDist );
	delete ptb;

	return num_built;
}

void AnimationSearchSystem::_parseLabelFilter( const std::string& labelFilter, std::vector<std::string>& labels ) const
{
	boost::char_separator<char> sep( ",;" );
	boost::tokenizer< boost::char_separator<char> > label_tok( labelFilter, sep );
	for( boost::tokenizer< boost::char_separator<char> >::const_iterator ltok_i = label_tok.begin();
		ltok_i != label_tok.end(); ++ltok_i )
	{
		std::string label = *ltok_i;
		if( label != "" )
		{
			std::transform( label.begin(), label.end(), label.begin(),
				( int(*)(int) )std::tolower );
			labels.push_back(label);
		}
	}
}

}
