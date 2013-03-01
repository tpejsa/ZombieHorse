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

#include "zhAnimationTransitionBuilder.h"
#include "zhString.h"
#include "zhSkeleton.h"
#include "zhAnimationSpace.h"
#include "zhAnimationDistanceGrid.h"

namespace zh
{

AnimationTransitionBuilder::AnimationTransitionBuilder( Skeleton* skel )
: mSkel(skel)
{
	zhAssert( skel != NULL );
}

AnimationTransitionBuilder::~AnimationTransitionBuilder()
{
}

unsigned int AnimationTransitionBuilder::buildTransitions( AnimationSpace* srcAnim, AnimationSpace* trgAnim,
														  float transLength, float minDist )
{
	zhAssert( srcAnim != NULL );
	zhAssert( trgAnim != NULL );
	zhAssert( transLength > 0 );

	// delete existing transitions between the two anims
	for( unsigned int anim_i = 0; anim_i < srcAnim->getNumBaseAnimations(); ++anim_i )
	{
		Animation* anim = srcAnim->getBaseAnimation(anim_i);
		ParamTransitionAnnotationContainer *annots = anim->getParamTransitionAnnotations();
	
		for( unsigned int annot_i = 0; annot_i < annots->getNumAnnotations(); ++annot_i )
		{
			ParamTransitionAnnotation* annot = static_cast<ParamTransitionAnnotation*>( annots->getAnnotation(annot_i) );

			if( annot->getTargetSetId() == trgAnim->getAnimationSet()->getId() &&
				annot->getTargetId() == trgAnim->getId() )
			{
				annots->deleteAnnotation(annot_i);
				--annot_i;
			}
		}
	}

	// TODO: implement this properly, as in Heck07
	// (too lazy right now, plus this way is an order of magnitude faster)

	Animation* anim0 = srcAnim->getBaseAnimation(0);
	ParamTransitionAnnotationContainer* annots0 = anim0->getParamTransitionAnnotations();

	// compute transition for first anim.
	unsigned int num_trans = buildTransitions( anim0, trgAnim, transLength, minDist );

	// annotate the whole anim. space
	for( unsigned int anim_i = 0; anim_i < srcAnim->getNumBaseAnimations(); ++anim_i )
	{
		Animation* anim = srcAnim->getBaseAnimation(anim_i);
		ParamTransitionAnnotationContainer *annots = anim->getParamTransitionAnnotations();
	
		for( unsigned int annot_i = 0; annot_i < annots0->getNumAnnotations(); ++annot_i )
		{
			ParamTransitionAnnotation* annot0 = static_cast<ParamTransitionAnnotation*>(
				annots0->getAnnotation(annot_i) );

			if( annot0->getTargetSetId() != trgAnim->getAnimationSet()->getId() ||
				annot0->getTargetId() != trgAnim->getId() )
				continue;

			// compute transition times
			float nst, net;
			nst = annot0->getStartTime() / anim0->getLength();
			net = annot0->getEndTime() / anim0->getLength();

			// create transition on current base anim.
			ParamTransitionAnnotation* annot = static_cast<ParamTransitionAnnotation*>(
				annots->createAnnotation( nst * anim->getLength(), net * anim->getLength() ) );
			annot->setTargetSetId( annot0->getTargetSetId() );
			annot->setTargetId( annot0->getTargetId() );
			annot->setTargetTime( annot0->getTargetTime() );
			annot->setAlignTransf( annot0->getAlignTransf() ); // TODO: adjust align. transf. for this anim.
			annot->setLowerBound( annot0->getLowerBound() ); // TODO: param. bounds must be computed separately for each source anim.
			annot->setUpperBound( annot0->getUpperBound() );
		}
	}

	return num_trans;
}

unsigned int AnimationTransitionBuilder::buildTransitions( AnimationSpace* srcAnim, Animation* trgAnim,
														  float transLength, float minDist )
{
	zhAssert( srcAnim != NULL );
	zhAssert( trgAnim != NULL );
	zhAssert( transLength > 0 );

	// delete existing transitions between the two anims
	for( unsigned int anim_i = 0; anim_i < srcAnim->getNumBaseAnimations(); ++anim_i )
	{
		Animation* anim = srcAnim->getBaseAnimation(anim_i);
		TransitionAnnotationContainer *annots = anim->getTransitionAnnotations();
	
		for( unsigned int annot_i = 0; annot_i < annots->getNumAnnotations(); ++annot_i )
		{
			TransitionAnnotation* annot = static_cast<TransitionAnnotation*>( annots->getAnnotation(annot_i) );

			if( annot->getTargetSetId() == trgAnim->getAnimationSet()->getId() &&
				annot->getTargetId() == trgAnim->getId() )
			{
				annots->deleteAnnotation(annot_i);
				--annot_i;
			}
		}
	}

	// TODO: implement this properly, as in Heck07
	// (too lazy right now, plus this way is order of magnitude faster)

	Animation* anim0 = srcAnim->getBaseAnimation(0);
	TransitionAnnotationContainer* annots0 = anim0->getTransitionAnnotations();

	// compute transitions for first anim.
	unsigned int num_trans = buildTransitions( anim0, trgAnim, transLength, minDist );

	// annotate the whole anim. space
	for( unsigned int anim_i = 0; anim_i < srcAnim->getNumBaseAnimations(); ++anim_i )
	{
		Animation* anim = srcAnim->getBaseAnimation(anim_i);
		TransitionAnnotationContainer *annots = anim->getTransitionAnnotations();
	
		for( unsigned int annot_i = 0; annot_i < annots0->getNumAnnotations(); ++annot_i )
		{
			TransitionAnnotation* annot0 = static_cast<TransitionAnnotation*>(
				annots0->getAnnotation(annot_i) );

			if( annot0->getTargetSetId() != trgAnim->getAnimationSet()->getId() ||
				annot0->getTargetId() != trgAnim->getId() )
				continue;

			// compute transition times
			float nst, net;
			nst = annot0->getStartTime() / anim0->getLength();
			net = annot0->getEndTime() / anim0->getLength();

			// create transition on current base anim.
			TransitionAnnotation* annot = static_cast<TransitionAnnotation*>(
				annots->createAnnotation( nst * anim->getLength(), net * anim->getLength() ) );
			annot->setTargetSetId( annot0->getTargetSetId() );
			annot->setTargetId( annot0->getTargetId() );
			annot->setTargetTime( annot0->getTargetTime() );
			annot->setAlignTransf( annot0->getAlignTransf() ); // TODO: adjust align. transf. for this anim.
		}
	}

	return num_trans;
}

unsigned int AnimationTransitionBuilder::buildTransitions( Animation* srcAnim, AnimationSpace* trgAnim,
														  float transLength, float minDist )
{
	zhAssert( srcAnim != NULL );
	zhAssert( trgAnim != NULL );
	zhAssert( transLength > 0 );

	ParamTransitionAnnotationContainer* src_annots = srcAnim->getParamTransitionAnnotations();

	// delete existing transitions between the two anims
	for( unsigned int annot_i = 0; annot_i < src_annots->getNumAnnotations(); ++annot_i )
	{
		ParamTransitionAnnotation* annot = static_cast<ParamTransitionAnnotation*>( src_annots->getAnnotation(annot_i) );
		if( annot->getTargetSetId() == trgAnim->getAnimationSet()->getId() &&
			annot->getTargetId() == trgAnim->getId() )
		{
			src_annots->deleteAnnotation(annot_i);
			--annot_i;
		}
	}

	unsigned int num_trans = 0;

	// TODO: implement this properly, as in Heck07
	// (too lazy right now, plus this way is an order of magnitude faster)

	// compute anim. distance grid for these two animations
	AnimationSegment src_anim( srcAnim, 0, srcAnim->getLength() );
	AnimationSegment trg_anim( trgAnim->getBaseAnimation(0), 0, trgAnim->getBaseAnimation(0)->getLength() );
	AnimationDistanceGrid* grid = new AnimationDistanceGrid( mSkel, src_anim, trg_anim, zhAnimation_SampleRate );
	grid->build(transLength);

	// find transition points
	num_trans = grid->findLocalMinima(minDist);
	// TODO: remove this later
	int best_mini = INT_MAX, best_mini1 = 0, best_mini2 = INT_MAX;
	for( unsigned int min_i = 0; min_i < grid->getNumLocalMinima(); ++min_i )
	{
		const AnimationDistanceGrid::Point& tpt = grid->getPoint( grid->getLocalMinimum(min_i) );
		float time1 = 0, time2 = 0;
		grid->getTimesAtFrames( tpt.getIndex(), time1, time2 );
		if( time1 < transLength || time2 > trg_anim.getLength() - transLength )
			// skip minima too close to anim. boundaries
			continue;

		if( (int)tpt.getIndex().first - (int)tpt.getIndex().second >=
			best_mini1 - best_mini2 )
		{
			best_mini = min_i;
			best_mini1 = (int)tpt.getIndex().first;
			best_mini2 = (int)tpt.getIndex().second;
		}
	}
	num_trans = 1;
	//

	// annot. transitions
	// for( unsigned int min_i = 0; min_i < grid->getNumLocalMinima(); ++min_i ) // TODO: restore this later
	unsigned int min_i = best_mini;
	if( min_i < INT_MAX )
	{
		const AnimationDistanceGrid::Point& tpt = grid->getPoint( grid->getLocalMinimum(min_i) );
		/*float time1 = 0, time2 = 0; // TODO: restore this later
		grid->getTimesAtFrames( tpt.getIndex(), time1, time2 );
		if( time1 < transLength || time2 >= trg_anim.getLength() - transLength )
			// skip minima too close to anim. boundaries
			continue;*/
		
		// compute transition times
		float st, tt;
		grid->getTimesAtFrames( tpt.getIndex(), st, tt );
		st -= transLength/2;
		tt -= transLength/2;
		float ntt = tt/trg_anim.getLength();

		// create transition annot.
		ParamTransitionAnnotation* annot = static_cast<ParamTransitionAnnotation*>(
			src_annots->createAnnotation( st, st + transLength ) );
		annot->setTargetSetId( trgAnim->getAnimationSet()->getId() );
		annot->setTargetId( trgAnim->getId() );
		annot->setTargetTime(ntt);
		annot->setAlignTransf( tpt.getAlignTransf() ); // TODO: average for all target anims?
		if( trgAnim->hasParametrization() )
		{
			annot->setLowerBound( Vector( trgAnim->getParametrization()->getNumParams(), -FLT_MAX ) );
			annot->setUpperBound( Vector( trgAnim->getParametrization()->getNumParams(), FLT_MAX ) );
		}
		else
		{
			annot->setLowerBound( Vector( trgAnim->getNumBaseAnimations(), -FLT_MAX ) );
			annot->setUpperBound( Vector( trgAnim->getNumBaseAnimations(), FLT_MAX ) );
		}
	}

	delete grid;

	return num_trans;
}

unsigned int AnimationTransitionBuilder::buildTransitions( Animation* srcAnim, Animation* trgAnim,
														  float transLength, float minDist, bool twoWay )
{
	zhAssert( srcAnim != NULL );
	zhAssert( trgAnim != NULL );
	zhAssert( transLength > 0 );

	TransitionAnnotationContainer* src_annots = srcAnim->getTransitionAnnotations(),
		*trg_annots = trgAnim->getTransitionAnnotations();

	// delete existing transitions between the two anims
	for( unsigned int annot_i = 0; annot_i < src_annots->getNumAnnotations(); ++annot_i )
	{
		TransitionAnnotation* src_annot = static_cast<TransitionAnnotation*>(
			src_annots->getAnnotation(annot_i) );

		if( src_annot->getTargetSetId() == trgAnim->getAnimationSet()->getId() &&
			src_annot->getTargetId() == trgAnim->getId() )
		{
			src_annots->deleteAnnotation(annot_i);
			--annot_i;
		}
	}

	if( twoWay )
	{
		// also delete existing inverse transitions
		for( unsigned int annot_i = 0; annot_i < trg_annots->getNumAnnotations(); ++annot_i )
		{
			TransitionAnnotation* trg_annot = static_cast<TransitionAnnotation*>(
				trg_annots->getAnnotation(annot_i) );

			if( trg_annot->getTargetSetId() == srcAnim->getAnimationSet()->getId() &&
				trg_annot->getTargetId() == srcAnim->getId() )
			{
				trg_annots->deleteAnnotation(annot_i);
				--annot_i;
			}
		}
	}

	unsigned int num_trans = 0;

	// compute anim. distance grid for these two animations
	AnimationSegment src_anim( srcAnim, 0, srcAnim->getLength() );
	AnimationSegment trg_anim( trgAnim, 0, trgAnim->getLength() );
	AnimationDistanceGrid* grid = new AnimationDistanceGrid( mSkel, src_anim, trg_anim, zhAnimation_SampleRate );
	grid->build(transLength);

	// find transition points
	num_trans = grid->findLocalMinima(minDist);
	if( !twoWay )
	{
		// TODO: remove this later
		// create only *one* transition, from the end of the source anim. to the start of the target anim.
		int best_mini = INT_MAX, best_mini1 = 0, best_mini2 = INT_MAX;
		for( unsigned int min_i = 0; min_i < grid->getNumLocalMinima(); ++min_i )
		{
			const AnimationDistanceGrid::Point& tpt = grid->getPoint( grid->getLocalMinimum(min_i) );
			float time1 = 0, time2 = 0;
			grid->getTimesAtFrames( tpt.getIndex(), time1, time2 );
			if( time1 < transLength || time2 > trg_anim.getLength() - transLength )
				// skip minima too close to anim. boundaries
				continue;

			if( (int)tpt.getIndex().first - (int)tpt.getIndex().second >=
				best_mini1 - best_mini2 )
			{
				best_mini = min_i;
				best_mini1 = (int)tpt.getIndex().first;
				best_mini2 = (int)tpt.getIndex().second;
			}
		}
		num_trans = 1;
		//

		// annot. transitions
		// for( unsigned int min_i = 0; min_i < grid->getNumLocalMinima(); ++min_i ) // TODO: restore this later
		unsigned int min_i = best_mini;
		if( min_i < INT_MAX )
		{
			const AnimationDistanceGrid::Point& tpt = grid->getPoint( grid->getLocalMinimum(min_i) );
			
			// compute transition times
			float st, tt;
			grid->getTimesAtFrames( tpt.getIndex(), st, tt );
			st -= transLength/2;
			tt -= transLength/2;

			// create transition annot.
			TransitionAnnotation* annot = static_cast<TransitionAnnotation*>(
				src_annots->createAnnotation( st, st + transLength ) );
			annot->setTargetSetId( trgAnim->getAnimationSet()->getId() );
			annot->setTargetId( trgAnim->getId() );
			annot->setTargetTime(tt);
			annot->setAlignTransf( tpt.getAlignTransf() );
		}
	}
	else
	{
		// annot. transitions
		for( unsigned int min_i = 0; min_i < grid->getNumLocalMinima(); ++min_i )
		{
			const AnimationDistanceGrid::Point& tpt = grid->getPoint( grid->getLocalMinimum(min_i) );
			float time1 = 0, time2 = 0;
			grid->getTimesAtFrames( tpt.getIndex(), time1, time2 );
			if( time1 < transLength || time2 >= trg_anim.getLength() - transLength )
				// skip minima too close to anim. boundaries
				continue;
			
			// compute transition times
			float st, tt;
			grid->getTimesAtFrames( tpt.getIndex(), st, tt );
			st -= transLength/2;
			tt -= transLength/2;

			// create transition annot.
			TransitionAnnotation* annot = static_cast<TransitionAnnotation*>(
				src_annots->createAnnotation( st, st + transLength ) );
			annot->setTargetSetId( trgAnim->getAnimationSet()->getId() );
			annot->setTargetId( trgAnim->getId() );
			annot->setTargetTime(tt);
			annot->setAlignTransf( tpt.getAlignTransf() );

			if(twoWay)
			{
				// create inverse transition
				annot = static_cast<TransitionAnnotation*>(
					trg_annots->createAnnotation( tt, tt + transLength ) );
				annot->setTargetSetId( srcAnim->getAnimationSet()->getId() );
				annot->setTargetId( srcAnim->getId() );
				annot->setTargetTime(st);
				annot->setAlignTransf( Skeleton::Situation(
					-tpt.getAlignTransf().getPosition(),
					tpt.getAlignTransf().getOrientation().getInverse() ) );
			}
		}
	}

	delete grid;

	return num_trans;
}

}
