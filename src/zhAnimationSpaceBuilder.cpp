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

#include "zhAnimationSpaceBuilder.h"
#include "zhString.h"
#include "zhMath.h"
#include "zhBoneAnimationTrack.h"
#include "zhAnimationIndex.h"
#include "zhAnnotationMatchMaker.h"

namespace zh
{

AnimationSpaceBuilder::AnimationSpaceBuilder( Skeleton* skel, AnimationSpace* animSpace )
: mSkel(skel), mAnimSpace(animSpace)
{
	zhAssert( skel != NULL );
	zhAssert( animSpace != NULL );
}

AnimationSpaceBuilder::~AnimationSpaceBuilder()
{
}

void AnimationSpaceBuilder::build( const std::vector<AnimationSegment>& animSegs )
{
	// TODO
}

void AnimationSpaceBuilder::build( MatchGraph* matches, unsigned short refNodeHandle )
{
	zhAssert( matches != NULL );
	zhAssert( matches->hasNode(refNodeHandle) );

	//
	// 1. Create base animations:
	//

	// compute optimal paths through the match graph from ref. base anim. to every other base anim.
	std::map< unsigned short, std::vector<MatchGraph::Node*> > paths; // optimal paths through the match graph
	std::vector<unsigned short> trg_handles;
	MatchGraph::NodeConstIterator node_i = matches->getNodeConstIterator();
	while( !node_i.end() )
	{
		MatchGraph::Node* node = node_i.next();
		trg_handles.push_back( node->getHandle() );
	}
	matches->computeOptimalPath( refNodeHandle, trg_handles, paths );

	// create base animations for every reachable match
	AnimationSetPtr banim_set = mAnimSpace->getAnimationSet(); // animation set for base animations
	std::map<unsigned short, unsigned int> banim_indexes; // base animation indexes (in anim. space), indexed by match graph node handle of the original anim. segment
	unsigned int banim_namenum = 0, banim_id = 0;
	std::string banim_name;
	unsigned int num_banims = 0;
	for( std::map<unsigned short, std::vector<MatchGraph::Node*> >::const_iterator node_i = paths.begin();
		node_i != paths.end(); ++num_banims, ++node_i )
	{
		MatchGraph::Node* node = matches->getNode( node_i->first );
		const AnimationSegment& anim_seg = node->getAnimationSegment();
		Animation* ranim = anim_seg.getAnimation();
		
		// find free ID for base anim.
		while( banim_set->hasAnimation(banim_id) ) ++banim_id;

		// find free name for base anim.
		do
		{
			banim_name = mAnimSpace->getName() + toString<unsigned int>(banim_namenum++) + "_b";
		}
		while( banim_set->hasAnimation(banim_name) );
		
		// create base anim.
		Animation* banim = banim_set->createAnimation( banim_id, banim_name );
		banim->setKFInterpolationMethod( ranim->getKFInterpolationMethod() );

		// add base anim. to anim. space
		mAnimSpace->addBaseAnimation(banim);
		banim_indexes.insert( make_pair( node->getHandle(), num_banims ) );
		
		// init base anim.
		Animation::BoneTrackConstIterator bti = ranim->getBoneTrackConstIterator();
		while( !bti.end() )
		{
			BoneAnimationTrack* bat = bti.next();
			BoneAnimationTrack* bbat = banim->createBoneTrack( bat->getBoneId() );

			// create first key-frame
			KeyFrame* bkf0 = bbat->createKeyFrame(0);
			bat->getInterpolatedKeyFrame( anim_seg.getStartTime(), bkf0 );

			// create last key-frame
			KeyFrame* bkf1 = bbat->createKeyFrame( anim_seg.getEndTime() - anim_seg.getStartTime() );
			bat->getInterpolatedKeyFrame( anim_seg.getEndTime(), bkf1 );

			// create intermediate key-frames
			BoneAnimationTrack::KeyFrameConstIterator kfi = bat->getKeyFrameConstIterator();
			while( !kfi.end() )
			{
				KeyFrame* kf = kfi.next();

				if( kf->getTime() <= anim_seg.getStartTime() || kf->getTime() >= anim_seg.getEndTime() )
					continue;

				KeyFrame* bkf = bbat->createKeyFrame( kf->getTime() - anim_seg.getStartTime() );
				TransformKeyFrame *btkf = static_cast<TransformKeyFrame*>(bkf),
					*tkf = static_cast<TransformKeyFrame*>(kf);
				btkf->setTranslation( tkf->getTranslation() );
				btkf->setRotation( tkf->getRotation() );
				btkf->setScale( tkf->getScale() );
			}
		}

		// recreate annotations on base anims
		//_copyAnnotsToBaseAnim( anim_seg, anim_seg.getAnimation()->getTransitionAnnotations(), banim->getTransitionAnnotations() );
		//_copyAnnotsToBaseAnim( anim_seg, anim_seg.getAnimation()->getParamTransitionAnnotations(), banim->getParamTransitionAnnotations() );
		_copyAnnotsToBaseAnim( anim_seg, anim_seg.getAnimation()->getPlantConstraintAnnotations(), banim->getPlantConstraintAnnotations() );
		_copyAnnotsToBaseAnim( anim_seg, anim_seg.getAnimation()->getSimEventAnnotations(), banim->getSimEventAnnotations() );
	}

	//
	// 2. Compute blend curve knots (TODO: optimize this!)
	//

	if( mBuildBlendCurves )
	{
		Animation* ref_anim = mAnimSpace->getBaseAnimation( banim_indexes[refNodeHandle] );
		unsigned int num_samples = unsigned int( ref_anim->getLength() * ((float)zhAnimation_SampleRate)/mKnotSpacing ) + 2;
		std::vector<Vector> tw_samples( num_samples, Vector( banim_indexes.size() ) ),
			align_samples( num_samples, Vector( 3 * banim_indexes.size() ) );

		// take initial set of samples in ref. anim.
		for( unsigned int sample_i = 0; sample_i < num_samples; ++sample_i )
		{
			unsigned int banim_i = banim_indexes[refNodeHandle];

			if( sample_i < num_samples - 1 )
				tw_samples[sample_i].set( banim_i, sample_i / ((float)zhAnimation_SampleRate) * mKnotSpacing );
			else
				tw_samples[sample_i].set( banim_i, ref_anim->getLength() );
			align_samples[sample_i].set( 3 * banim_i, Skeleton::Situation::Identity.getPosX() );
			align_samples[sample_i].set( 3 * banim_i + 1, Skeleton::Situation::Identity.getPosZ() );
			align_samples[sample_i].set( 3 * banim_i + 2, Skeleton::Situation::Identity.getOrientY() );
		}

		for( std::map<unsigned short, std::vector<MatchGraph::Node*> >::const_iterator node_i = paths.begin();
			node_i != paths.end(); ++node_i )
		{
			const std::vector<MatchGraph::Node*>& path = node_i->second;
			unsigned int tbanim_i = banim_indexes[ node_i->first ]; // target base anim. index
			Animation* tbanim = mAnimSpace->getBaseAnimation(tbanim_i); // target base anim.

			// compute a set of frame correspondences to ref. anim. for each base anim.
			MatchGraph::Node* prev_node = NULL, // node from prev. iteration
				*cur_node = NULL; // node from current iteration
			for( std::vector<MatchGraph::Node*>::const_iterator path_i = path.begin();
				path_i != path.end(); ++path_i )
			{
				cur_node = *path_i;
				unsigned int banim_i = banim_indexes[ cur_node->getHandle() ];
				Animation* banim = mAnimSpace->getBaseAnimation(banim_i);

				if( prev_node == NULL )
				{
					// this is the ref. node, initialize time correspondences and align. transf.'s

					for( unsigned int sample_i = 0; sample_i < num_samples; ++sample_i )
					{
						if( sample_i < num_samples - 1 )
							tw_samples[sample_i].set( tbanim_i, sample_i / ((float)zhAnimation_SampleRate) * mKnotSpacing );
						else
							tw_samples[sample_i].set(tbanim_i, ref_anim->getLength() );
						align_samples[sample_i].set( 3 * tbanim_i, Skeleton::Situation::Identity.getPosX() );
						align_samples[sample_i].set( 3 * tbanim_i + 1, Skeleton::Situation::Identity.getPosZ() );
						align_samples[sample_i].set( 3 * tbanim_i + 2, Skeleton::Situation::Identity.getOrientY() );
					}
				}
				else
				{
					// update time correspondences and align. transf.'s

					unsigned int pbanim_i = banim_indexes[ prev_node->getHandle() ];
					Animation* pbanim = mAnimSpace->getBaseAnimation(pbanim_i);

					MatchGraph::Edge* edge = matches->getEdge( prev_node->getHandle(), cur_node->getHandle() );

					for( unsigned int sample_i = 0; sample_i < num_samples; ++sample_i )
					{
						MatchGraph::MatchPoint align_inf = edge->sample( tw_samples[sample_i].get( tbanim_i ),
							prev_node == edge->getNode1() ? true : false );

						tw_samples[sample_i].set( tbanim_i, cur_node == edge->getNode2() ? align_inf.getTime2() : align_inf.getTime1() );

						Skeleton::Situation transf( align_samples[sample_i].get( 3 * tbanim_i ),
							align_samples[sample_i].get( 3 * tbanim_i + 1 ),
							align_samples[sample_i].get( 3 * tbanim_i + 2 ) );
						transf = transf.transform( align_inf.getAlignTransf() ); /// TODO: is this OK?

						align_samples[sample_i].set( 3 * tbanim_i, transf.getPosX() );
						align_samples[sample_i].set( 3 * tbanim_i + 1, transf.getPosZ() );
						align_samples[sample_i].set( 3 * tbanim_i + 2, transf.getOrientY() );
					}
				}

				prev_node = cur_node;
			}
		}

		// correct angle discontinuities in align. transf.'s
		for( unsigned int sample_i = 1; sample_i < num_samples; ++sample_i )
		{
			for( unsigned int banim_i = 0; banim_i < mAnimSpace->getNumBaseAnimations(); ++banim_i )
			{
				float d_orienty = align_samples[sample_i].get( 3 * banim_i + 2 ) -
					align_samples[ sample_i - 1 ].get( 3 * banim_i + 2 );
				
				while( fabs(d_orienty) > zhPI )
				{
					float angle_shift = d_orienty > 0 ? -2 * zhPI : 2 * zhPI;
					d_orienty += angle_shift;
					align_samples[sample_i].set( 3 * banim_i + 2,
						align_samples[sample_i].get( 3 * banim_i + 2 ) + angle_shift );
				}
			}
		}
		// TODO: apply a median filter to align. transf.'s (that's what Kovar advises)?

		// compute blend curves
		CatmullRomSpline<Vector> tw_curve, align_curve;
		for( unsigned int sample_i = 1; sample_i < num_samples; ++sample_i )
		{
			tw_curve.addControlPoint( tw_samples[sample_i] );
			align_curve.addControlPoint( align_samples[sample_i] );
		}
		tw_curve.calcTangents();
		align_curve.calcTangents();

		// adjust TW curve knots to make the curve strictly increasing
		for( unsigned int cpi = 1; cpi < tw_curve.getNumControlPoints() - 1; ++cpi )
		{
			for( unsigned int banim_i = 0; banim_i < mAnimSpace->getNumBaseAnimations(); ++banim_i )
			{
				Vector cp0 = tw_curve.getControlPoint(cpi-1),
					cp1 = tw_curve.getControlPoint(cpi),
					cp2 = tw_curve.getControlPoint(cpi+1);

				float dcp = cp2[banim_i] - cp1[banim_i] - 0.001f,
					prev_dcp = cp1[banim_i] - cp0[banim_i] - 0.001f;

				if( dcp < 0.f )
				{
					float cpinc = std::max( 0.5f, prev_dcp/dcp );
					cp1[banim_i] = cp1[banim_i] - fabs( cpinc * dcp );
					cp2[banim_i] = cp2[banim_i] + fabs( ( 1.f - cpinc ) * dcp );

					tw_curve.setControlPoint( cpi, cp1 );
					tw_curve.setControlPoint( cpi+1, cp2 );
				}
			}
		}

		// set blend curves
		mAnimSpace->setTimewarpCurve(tw_curve);
		mAnimSpace->setAlignmentCurve(align_curve);

		/* TODO: remove this
		zhLog( "AnimationSpaceBuilder", "build", "TIMEWARP CURVE" );

		for( unsigned int segi = 0; segi < tw_curve.getNumSegments(); ++segi )
		{
			std::string tw_str = "", tw_str2 = "", twtang_str = "", twtang_str2 = "";

			for( unsigned int banim_i = 0; banim_i < mAnimSpace->getNumBaseAnimations(); ++banim_i )
			{
				tw_str += toString<float>( tw_curve.getControlPoint(segi).get(banim_i) );
				tw_str += "  ";
				tw_str2 += toString<float>( tw_curve.getPoint( segi, 0 ).get(banim_i) );
				tw_str2 += "  ";
				twtang_str2 += toString<float>( tw_curve.getTangent( segi, 0 ).get(banim_i) );
				twtang_str2 += "  ";
			}

			zhLog( "AnimationSpaceBuilder", "build", "%s -_ %s -- %s _- %s", tw_str.c_str(), tw_str2.c_str(), twtang_str.c_str(), twtang_str2.c_str() );
		}

		zhLog( "AnimationSpaceBuilder", "build", "ALIGNMENT CURVE" );

		for( unsigned int segi = 0; segi < tw_curve.getNumControlPoints(); ++segi )
		{
			std::string align_str = "";

			for( unsigned int banim_i = 0; banim_i < mAnimSpace->getNumBaseAnimations(); ++banim_i )
			{
				align_str += toString<float>( align_curve.getControlPoint(segi).get( 3 * banim_i ) ) + ";" +
					toString<float>( align_curve.getControlPoint(segi).get( 3 * banim_i + 1 ) ) + ";" +
					toString<float>( align_curve.getControlPoint(segi).get( 3 * banim_i + 2 ) );
				align_str += "  ";
			}

			zhLog( "AnimationSpaceBuilder", "build", "%s", align_str.c_str() );
		}
		*/
	}

	//
	// 3. Identify matching annotations in base animations:
	//

	if( mMatchAnnots )
	{
		AnnotationMatchMaker* mm = new AnnotationMatchMaker(mAnimSpace);
		mm->makeMatches();
		delete mm;
	}
}

void AnimationSpaceBuilder::_copyAnnotsToBaseAnim( const AnimationSegment& animSeg, AnimationAnnotationContainer* srcAnnots,
												  AnimationAnnotationContainer* trgAnnots )
{
	AnimationAnnotationContainer::AnnotationConstIterator annot_i = srcAnnots->getAnnotationConstIterator();
	while( annot_i.hasMore() )
	{
		AnimationAnnotation* annot = annot_i.next();

		float st = 0, et = 0;

		if( annot->getEndTime() <= animSeg.getStartTime() || annot->getStartTime() >= animSeg.getEndTime() )
		{
			continue;
		}

		st = annot->getStartTime() >= animSeg.getStartTime() ? annot->getStartTime() : animSeg.getStartTime();
		et = annot->getEndTime() <= animSeg.getEndTime() ? annot->getEndTime() : animSeg.getEndTime();
		st -= animSeg.getStartTime();
		et -= animSeg.getStartTime();

		AnimationAnnotation* new_annot = trgAnnots->createAnnotation( st, et );
		annot->_clone(new_annot);
	}
}

}
