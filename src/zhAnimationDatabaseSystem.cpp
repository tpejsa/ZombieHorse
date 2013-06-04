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

#include "zhAnimationDatabaseSystem.h"
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
#include "KMlocal.h"

#include <boost/tokenizer.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>
#include <cctype>
#include <fstream>

namespace zh
{

AnimationDatabaseSystem::AnimationDatabaseSystem()
: mResampleFact(3), mWndLength(0.35f), mMinDist(0.05f), mMaxDistDiff(0.15f),
mMinChainLength(0.25f), mMaxBridgeLength(1.f),
mMaxOverlap(0.8f),
mMatchAnnots(true), mBuildBlendCurves(true), mKnotSpacing(3),
mMaxExtrap(0.15f), mMinSampleDist(0.00001f)
{
	mTrainSet = new AnimationFrameSet();
}

AnimationDatabaseSystem::~AnimationDatabaseSystem()
{
	delete mTrainSet;
	deleteAllMatchGraphs();
}

bool AnimationDatabaseSystem::init( const std::string& cfgPath )
{
	zhLog( "AnimationDatabaseSystem", "init",
		"Initializing animation search system. Using configuration file %s.",
		cfgPath.c_str() );

	// Create instances of various singletons
	AnimationIndexManager::Instance();
	
	// Register resource loaders and serializers
	zhRegister_ResourceLoader( getAnimationIndexManager(), ZHILoader );
	zhRegister_ResourceSerializer( getAnimationIndexManager(), ZHISerializer );

	// Load config.xml
	// TODO

	zhSetErrorCode(ADSE_None);
	return true;
}

void AnimationDatabaseSystem::buildTrainSet()
{
	mTrainSet->removeAllFrames();

	// Enumerate the animations in the database
	std::vector<Animation*> anims;
	typedef std::pair<unsigned int, unsigned int> FrameIndex;
	std::vector<FrameIndex> frames; // unprocessed frames
	unsigned int num_frames = 0; // number of unprocessed frames
	unsigned int num_tracks = 0; // number of bone tracks
	ResourceManager::ResourceConstIterator res_i =
		zhAnimationSystem->getAnimationManager()->getResourceConstIterator();
	unsigned int ani = 0;
	while( res_i.hasMore() )
	{
		AnimationSetPtr animset = AnimationSetPtr::DynamicCast<Resource>(res_i.next());
		AnimationSet::AnimationConstIterator anim_i = animset->getAnimationConstIterator();
		while( anim_i.hasMore() )
		{
			Animation* anim = anim_i.next();
			anims.push_back(anim);
			num_tracks = num_tracks <= 0 ? anim->getNumBoneTracks() : num_tracks;
			if( num_tracks != anim->getNumBoneTracks() )
				// This animation has an incompatible skeleton!
				continue;

			BoneAnimationTrack* btr = anim->getBoneTrack(0);
			unsigned int nkf = btr->getNumKeyFrames();
			num_frames += nkf;
			unsigned int fi = 0;
			while( fi < nkf ) frames.push_back( FrameIndex(ani, fi++) );
			++ani;
		}
	}
	
	// Select representative frame set
	boost::mt19937 rng;
	while( num_frames > 0 )
	{
		// Randomly select unprocessed frames from the database
		unsigned int nrfi = zhARFSS_NumClusters*4;
		if( num_frames < nrfi ) nrfi = num_frames;
		for( unsigned int rfii = 0; rfii < nrfi; ++rfii )
		{
			boost::uniform_int<> rnd(0, (unsigned int)frames.size()-1);
			boost::variate_generator<boost::mt19937&, boost::uniform_int<> > fi_rng(rng, rnd);
			unsigned int fii = fi_rng();
			FrameIndex fi = FrameIndex( frames[fii] ); // selected frame
			mTrainSet->addFrame( AnimationFrame(anims[fi.first], fi.second) );
			frames.erase(frames.begin() + fii);
		}
		num_frames -= nrfi;

		// Do k-means clustering of frames, and keep centers as new frames
		_computeFrameClusterCenters(mTrainSet, zhARFSS_NumClusters);
	}

	if( mTrainSet->getNumFrames() <= 0 )
		return;

	// Write out the representative frame set to a file
	/*std::ofstream ofs("TrainSet.csv");
	// Write out header
	ofs << "p0x,";
	ofs << "p0y,";
	ofs << "p0z,";
	ofs << "q0x,";
	ofs << "q0y,";
	ofs << "q0z,";
	const AnimationFrame& fr0 = mTrainSet->getFrame(0);
	for( unsigned int tri = 0; tri < (unsigned int)fr0.orientations.size(); ++tri )
	{
		ofs << "q" << tri+1 << "x,";
		ofs << "q" << tri+1 << "y,";
		ofs << "q" << tri+1 << "z";
		if( tri < (unsigned int)fr0.orientations.size()-1 )
			ofs << ",";
		else
			ofs << std::endl;
	}
	for( unsigned int fi = 0; fi < mTrainSet->getNumFrames(); ++fi )
	{
		const AnimationFrame& fr = mTrainSet->getFrame(fi);
		ofs << fr.rootPosition.x << ",";
		ofs << fr.rootPosition.y << ",";
		ofs << fr.rootPosition.z << ",";
		ofs << fr.rootOrientation.x << ",";
		ofs << fr.rootOrientation.y << ",";
		ofs << fr.rootOrientation.z << ",";
		for( unsigned int tri = 0; tri < (unsigned int)fr0.orientations.size(); ++tri )
		{
			ofs << fr.orientations[tri].x << ",";
			ofs << fr.orientations[tri].y << ",";
			ofs << fr.orientations[tri].z;
			if( tri < (unsigned int)fr0.orientations.size()-1 )
				ofs << ",";
			else
				ofs << std::endl;
		}
	}
	ofs.close();*/

	// Write out training data
	Skeleton* skel = zhAnimationSystem->getSkeletonConstIterator().next();
	std::ofstream ofs = std::ofstream("TrainSet.svml");
	mTrainSet->extractTargetPositions(skel);
	for( unsigned int fri = 0; fri < mTrainSet->getNumFrames(); ++fri )
	{
		ofs << "1 ";
		unsigned int ft = 1;
		const AnimationFrame& fr = mTrainSet->getFrame(fri);
		// Write out input values
		for( unsigned int ii = 0; ii < (unsigned int)fr.targetPositions.size(); ++ii )
		{
			ofs << (ft++) << ":" << fr.targetPositions[ii].x << " ";
			ofs << (ft++) << ":" << fr.targetPositions[ii].y << " ";
			ofs << (ft++) << ":" << fr.targetPositions[ii].z << " ";
		}
		// Write out output values
		ofs << (ft++) << ":" << fr.rootPosition.x << " ";
		ofs << (ft++) << ":" << fr.rootPosition.y << " ";
		ofs << (ft++) << ":" << fr.rootPosition.z << " ";
		ofs << (ft++) << ":" << fr.rootOrientation.x << " ";
		ofs << (ft++) << ":" << fr.rootOrientation.y << " ";
		ofs << (ft++) << ":" << fr.rootOrientation.z << " ";
		for( unsigned int oi = 0; oi < (unsigned int)fr.orientations.size(); ++oi )
		{
			ofs << (ft++) << ":" << fr.orientations[oi].x << " ";
			ofs << (ft++) << ":" << fr.orientations[oi].y << " ";
			std::string term = ( oi == (unsigned int)(fr.orientations.size()-1) ) ? "\n" : " ";
			ofs << (ft++) << ":" << fr.orientations[oi].z << term;
		}
	}
	ofs.close();
}

bool AnimationDatabaseSystem::loadTrainSet()
{
	// TODO

	return true;
}

AnimationFrameSet* AnimationDatabaseSystem::getTrainSet() const
{
	return mTrainSet;
}

AnimationSetPtr AnimationDatabaseSystem::initTrainPoses()
{
	// Get/create animation set for training poses
	AnimationManager* anim_mgr = zhAnimationSystem->getAnimationManager();
	AnimationSetPtr as_trainset = AnimationSetPtr::DynamicCast<zh::Resource>(
		!anim_mgr->hasResource("TrainSet") ?
		anim_mgr->createResource(0,"TrainSet") : anim_mgr->getResource("TrainSet") );
	as_trainset->deleteAllAnimations();
	
	// Build train set for currently loaded animations
	AnimationFrameSet* trainset = zhAnimationDatabaseSystem->getTrainSet();
	for( unsigned int fri = 0; fri < trainset->getNumFrames(); ++fri )
	{
		const AnimationFrame& fr = trainset->getFrame(fri);
		zh::Animation* anim = as_trainset->createAnimation(1000+fri, "RFrame"+toString<unsigned int>(fri));
		zh::Skeleton* skel = zhAnimationSystem->getOutputSkeleton();
		if( skel == NULL )
			continue;

		for( unsigned int tri = 0; tri < skel->getNumBones(); ++tri )
		{
			zh::BoneAnimationTrack* btr = anim->createBoneTrack(tri);
			zh::TransformKeyFrame* tkf = static_cast<zh::TransformKeyFrame*>( btr->createKeyFrame(0) );
			if( tri == 0 )
			{
				tkf->setTranslation(fr.rootPosition);
				tkf->setRotation( fr.rootOrientation.exp() );
			}
			else
				tkf->setRotation( fr.orientations[tri-1].exp() );
			zh::TransformKeyFrame* tkf1 = static_cast<zh::TransformKeyFrame*>( btr->createKeyFrame(1) );
			tkf1->setTranslation( tkf->getTranslation() );
			tkf1->setRotation( tkf->getRotation() );
		}

		// Add animation node to tree
		AnimationTree* anim_tree = zhAnimationSystem->getAnimationTree();
		std::string node_name = anim->getFullName();
		AnimationSampleNode* node = static_cast<AnimationSampleNode*>(
			anim_tree->createNode( AnimationSampleNode::ClassId(), anim_tree->getNumNodes(), node_name )
			);
		node->setAnimation( anim->getAnimationSet(), anim->getId() );
		node->createAdaptor(skel);
		anim_tree->getNode("Root")->addChild(node);
	}

	return as_trainset;
}

bool AnimationDatabaseSystem::trainGPLVM( const std::string& modelFile, unsigned int numLatentVars )
{
	// Train the model
	std::string cmd = "gplvm -L true -x " + toString<int>(numLatentVars) + 
		" TrainSet.svml " + modelFile;
	if( system( cmd.c_str() ) != 0 )
	{
		zhSetErrorCode(ADSE_ModelTrainingFailed);
		return false;
	}

	zhSetErrorCode(ADSE_None);
	return true;
}

AnimationIndexPtr AnimationDatabaseSystem::buildIndex( unsigned long id, const std::string& name, Skeleton* skel,
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

	return buildIndex( id, name, skel, ranims, labelFilter );
}

AnimationIndexPtr AnimationDatabaseSystem::buildIndex( unsigned long id, const std::string& name, Skeleton* skel,
													std::vector<AnimationSetPtr> rawAnims, const std::string& labelFilter )
{
	AnimationIndexManager* aimgr = getAnimationIndexManager();
	zhAssert( !aimgr->hasResource(id) && !aimgr->hasResource(name) );
	zhAssert( skel != NULL );

	// create empty anim. index
	AnimationIndexPtr anim_index = AnimationIndexPtr::DynamicCast<Resource>( aimgr->createResource( id, name ) );
	anim_index->setSkeleton(skel);

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

MatchGraph* AnimationDatabaseSystem::search( const AnimationSegment& animSeg )
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

MatchGraph* AnimationDatabaseSystem::search( unsigned long animIndexId, const AnimationSegment& animSeg )
{
	zhAssert( getAnimationIndexManager()->hasResource(animIndexId) );

	deleteAllMatchGraphs();

	AnimationIndexPtr anim_index = AnimationIndexPtr::DynamicCast<Resource>( getAnimationIndexManager()->getResource(animIndexId) );
	MatchGraph* mg = anim_index->search( animSeg, mMaxOverlap );
	if( mg != NULL )
		mMatchGraphs.push_back(mg);

	return mg;
}

MatchGraph* AnimationDatabaseSystem::search( const std::string& animIndexName, const AnimationSegment& animSeg )
{
	zhAssert( getAnimationIndexManager()->hasResource(animIndexName) );

	AnimationIndexPtr anim_index = AnimationIndexPtr::DynamicCast<Resource>( getAnimationIndexManager()->getResource(animIndexName) );

	return search( anim_index->getId(), animSeg );
}

void AnimationDatabaseSystem::deleteMatchGraph( unsigned int index )
{
	zhAssert( index < mMatchGraphs.size() );

	delete mMatchGraphs[index];
	mMatchGraphs.erase( mMatchGraphs.begin() + index );
}

void AnimationDatabaseSystem::deleteAllMatchGraphs()
{
	for( std::vector<MatchGraph*>::iterator mgi = mMatchGraphs.begin();
		mgi != mMatchGraphs.end(); ++mgi )
		delete *mgi;

	mMatchGraphs.clear();
}

MatchGraph* AnimationDatabaseSystem::getMatchGraph( unsigned int index ) const
{
	zhAssert( index < mMatchGraphs.size() );

	return mMatchGraphs[index];
}

unsigned int AnimationDatabaseSystem::getNumMatchGraphs() const
{
	return mMatchGraphs.size();
}

AnimationDatabaseSystem::MatchGraphIterator AnimationDatabaseSystem::getMatchGraphIterator()
{
	return MatchGraphIterator(mMatchGraphs);
}

AnimationDatabaseSystem::MatchGraphConstIterator AnimationDatabaseSystem::getMatchGraphConstIterator() const
{
	return MatchGraphConstIterator(mMatchGraphs);
}

AnimationSpace* AnimationDatabaseSystem::buildAnimationSpace( unsigned short id, const std::string& name,
														   Skeleton* skel, AnimationSetPtr animSet,
														   MatchGraph* matches, unsigned short refNodeHandle ) const
{
	zhAssert( skel != NULL );
	zhAssert( animSet != NULL );
	zhAssert( !animSet->hasAnimationSpace(id) && !animSet->hasAnimationSpace(name) );

	// create empty animation space
	AnimationSpace* anim_space = animSet->createAnimationSpace( id, name );

	// build animation space
	AnimationSpaceBuilder* asb = new AnimationSpaceBuilder( skel, anim_space );
	asb->setBuildBlendCurves(mBuildBlendCurves);
	asb->setKnotSpacing(mKnotSpacing);
	asb->setMatchAnnotations(mMatchAnnots);
	asb->build( matches, refNodeHandle );
	delete asb;

	return anim_space;
}

void AnimationDatabaseSystem::computeAnnotMatches( AnimationSpace* animSpace ) const
{
	zhAssert( animSpace != NULL );

	AnnotationMatchMaker* mm = new AnnotationMatchMaker(animSpace);
	mm->makeMatches();
}

void AnimationDatabaseSystem::parametrizeAnimSpace( Skeleton* skel, AnimationSpace* animSpace, const std::vector<AnimationParamSpec>& paramSpecs,
												 AnimationParamClass paramClass )
{
	zhAssert( animSpace != NULL );
	zhAssert( skel != NULL );

	DenseSamplingParamBuilder* pab = new DenseSamplingParamBuilder( skel, animSpace );
	pab->setMaxExtrapolation(mMaxExtrap);
	pab->setMinSampleDistance(mMinSampleDist);
	pab->parametrize(paramSpecs);
	delete pab;
}

float AnimationDatabaseSystem::getMaxExtrapolation() const
{
	return mMaxExtrap;
}

void AnimationDatabaseSystem::setMaxExtrapolation( float maxExtrap )
{
	mMaxExtrap = maxExtrap;
}

float AnimationDatabaseSystem::getMinSampleDistance() const
{
	return mMinSampleDist;
}

void AnimationDatabaseSystem::setMinSampleDistance( float minSampleDist )
{
	mMinSampleDist = minSampleDist;
}

void AnimationDatabaseSystem::buildTransitions( Skeleton* skel, const std::string& labelFilter )
{
	std::vector<AnimationSetPtr> ranims;

	// get raw animation sets
	AnimationManager::ResourceConstIterator rai = zhAnimationSystem->getAnimationManager()->getResourceConstIterator();
	while( !rai.end() )
	{
		AnimationSetPtr ras = AnimationSetPtr::DynamicCast<Resource>( rai.next() );
		ranims.push_back(ras);
	}

	return buildTransitions(skel, ranims, labelFilter );
}

void AnimationDatabaseSystem::buildTransitions( Skeleton* skel, std::vector<AnimationSetPtr> rawAnims,
											 const std::string& labelFilter )
{
	zhAssert( skel != NULL );

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

				if(skip)
					continue;
			}

			// add animation clip
			anims.push_back(anim);
		}
	}

	// build transitions between each two anims
	AnimationTransitionBuilder* ptb = new AnimationTransitionBuilder(skel);
	for( unsigned int anim_i1 = 0; anim_i1 < anims.size(); ++anim_i1 )
	{
		for( unsigned int anim_i2 = anim_i1; anim_i2 < anims.size(); ++anim_i2 )
		{
			ptb->buildTransitions( anims[anim_i1], anims[anim_i2], mWndLength, mMinDist, true );
		}
	}
	delete ptb;
}

unsigned int AnimationDatabaseSystem::buildTransitions( Skeleton* skel, AnimationSpace* srcAnim, AnimationSpace* trgAnim )
{
	zhAssert( skel != NULL );
	zhAssert( srcAnim != NULL );
	zhAssert( trgAnim != NULL );

	AnimationTransitionBuilder* ptb = new AnimationTransitionBuilder(skel);
	unsigned int num_built = ptb->buildTransitions( srcAnim, trgAnim, mWndLength, mMinDist );
	delete ptb;

	return num_built;
}

unsigned int AnimationDatabaseSystem::buildTransitions( Skeleton* skel, AnimationSpace* srcAnim, Animation* trgAnim )
{
	zhAssert( skel != NULL );
	zhAssert( srcAnim != NULL );
	zhAssert( trgAnim != NULL );

	AnimationTransitionBuilder* ptb = new AnimationTransitionBuilder(skel);
	unsigned int num_built = ptb->buildTransitions( srcAnim, trgAnim, mWndLength, mMinDist );
	delete ptb;

	return num_built;
}

unsigned int AnimationDatabaseSystem::buildTransitions( Skeleton* skel, Animation* srcAnim, AnimationSpace* trgAnim )
{
	zhAssert( skel != NULL );
	zhAssert( srcAnim != NULL );
	zhAssert( trgAnim != NULL );

	AnimationTransitionBuilder* ptb = new AnimationTransitionBuilder(skel);
	unsigned int num_built = ptb->buildTransitions( srcAnim, trgAnim, mWndLength, mMinDist );
	delete ptb;

	return num_built;
}

unsigned int AnimationDatabaseSystem::buildTransitions( Skeleton* skel, Animation* srcAnim, Animation* trgAnim )
{
	zhAssert( skel != NULL );
	zhAssert( srcAnim != NULL );
	zhAssert( trgAnim != NULL );

	AnimationTransitionBuilder* ptb = new AnimationTransitionBuilder(skel);
	unsigned int num_built = ptb->buildTransitions( srcAnim, trgAnim, mWndLength, mMinDist );
	delete ptb;

	return num_built;
}

void AnimationDatabaseSystem::_computeFrameClusterCenters( AnimationFrameSet* frames, unsigned int numClusters )
{
	zhAssert( frames != NULL && frames->getNumFrames() > 0 );

	// Prepare data points
	unsigned int ntr = frames->getNumTracks();
	unsigned int nfr = frames->getNumFrames();
	KMdata data(ntr, nfr);
	for( unsigned int fri = 0; fri < nfr; ++fri )
	{
		const AnimationFrame& fr = frames->getFrame(fri);
		data[fri][0] = fr.rootPosition.x;
		data[fri][1] = fr.rootPosition.y;
		data[fri][2] = fr.rootPosition.z;
		data[fri][3] = fr.rootOrientation.x;
		data[fri][4] = fr.rootOrientation.y;
		data[fri][5] = fr.rootOrientation.z;
		for( unsigned int qi = 0; qi < (unsigned int)fr.orientations.size(); ++qi )
		{
			data[fri][6+qi*3] = fr.orientations[qi].x;
			data[fri][6+qi*3+1] = fr.orientations[qi].y;
			data[fri][6+qi*3+2] = fr.orientations[qi].z;
		}
	}
	data.buildKcTree();

	// Perform k-means clustering
	KMfilterCenters ctrs(numClusters, data);
	KMterm term(100, 0, 0, 0, 0.1, 0.1, 3, 0.5, 10, 0.95);
	KMlocalLloyds alg(ctrs, term);
	ctrs = alg.execute();

	// Add cluster centers as new frames
	frames->removeAllFrames();
	for( int ctri = 0; ctri < ctrs.getK(); ++ctri )
	{
		AnimationFrame fr;
		fr.rootPosition.x = ctrs[ctri][0];
		fr.rootPosition.y = ctrs[ctri][1];
		fr.rootPosition.z = ctrs[ctri][2];
		fr.rootOrientation.w = 0;
		fr.rootOrientation.x = ctrs[ctri][3];
		fr.rootOrientation.y = ctrs[ctri][4];
		fr.rootOrientation.z = ctrs[ctri][5];

		unsigned int nqtr = ctrs.getDim()/3-2;
		fr.orientations = std::vector<Quat>(nqtr);
		for( unsigned int qi = 0; qi < nqtr; ++qi )
		{
			fr.orientations[qi].w = 0;
			fr.orientations[qi].x = ctrs[ctri][6+qi*3];
			fr.orientations[qi].y = ctrs[ctri][6+qi*3+1];
			fr.orientations[qi].z = ctrs[ctri][6+qi*3+2];
		}

		frames->addFrame(fr);
	}
}

void AnimationDatabaseSystem::_parseLabelFilter( const std::string& labelFilter, std::vector<std::string>& labels ) const
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
