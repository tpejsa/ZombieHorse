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

#include "zhAnnotationMatchMaker.h"

namespace zh
{

AnnotationMatchMaker::AnnotationMatchMaker( AnimationSpace* animSpace )
: mAnimSpace(animSpace)
{
	zhAssert( animSpace != NULL );
}

AnnotationMatchMaker::~AnnotationMatchMaker()
{
}

AnimationSpace* AnnotationMatchMaker::getAnimationSpace() const
{
	return mAnimSpace;
}

void AnnotationMatchMaker::makeMatches()
{
	zhAssert( mAnimSpace != NULL );

	makeMatches(AnimAnnot_Transition);
	makeMatches(AnimAnnot_ParamTransition);
	makeMatches(AnimAnnot_PlantConstraint);
	makeMatches(AnimAnnot_SimEvent);
}

void AnnotationMatchMaker::makeMatches( AnimationAnnotationClass annotClass )
{
	zhAssert( mAnimSpace != NULL );

	if( mAnimSpace->getNumBaseAnimations() <= 0 )
		return;

	// copy annotations from base animations to temporary container
	std::vector<AnimationAnnotationContainer*> base_annots( mAnimSpace->getNumBaseAnimations() );
	for( unsigned int banim_i = 0; banim_i < mAnimSpace->getNumBaseAnimations(); ++banim_i )
	{
		Animation* banim = mAnimSpace->getBaseAnimation(banim_i);
		AnimationAnnotationContainer* annots;

		if( annotClass == AnimAnnot_Transition )
		{
			base_annots[banim_i] = new TransitionAnnotationContainer();
			annots = banim->getTransitionAnnotations();
		}
		else if( annotClass == AnimAnnot_ParamTransition )
		{
			base_annots[banim_i] = new ParamTransitionAnnotationContainer();
			annots = banim->getParamTransitionAnnotations();
		}
		else if( annotClass == AnimAnnot_PlantConstraint )
		{
			base_annots[banim_i] = new PlantConstraintAnnotationContainer();
			annots = banim->getPlantConstraintAnnotations();
		}
		else // if( annotClass == AnimAnnot_SimEvent )
		{
			base_annots[banim_i] = new SimEventAnnotationContainer();
			annots = banim->getSimEventAnnotations();
		}

		annots->_clone( base_annots[banim_i] );

		// delete original annots
		if( annotClass == AnimAnnot_Transition )
			banim->getTransitionAnnotations()->deleteAllAnnotations();
		else if( annotClass == AnimAnnot_ParamTransition )
			banim->getParamTransitionAnnotations()->deleteAllAnnotations();
		else if( annotClass == AnimAnnot_PlantConstraint )
			banim->getPlantConstraintAnnotations()->deleteAllAnnotations();
		else // if( annotClass == AnimAnnot_SimEvent )
			banim->getSimEventAnnotations()->deleteAllAnnotations();
	}

	// identify annotation groups (annotations of same class that refer to the same thing, e.g. constraints on the same end-effector)
	std::vector<AnimationAnnotation*> annot_groups;
	for( unsigned int annot_i = 0; annot_i < base_annots[0]->getNumAnnotations(); ++annot_i )
	{
		AnimationAnnotation* annot = base_annots[0]->getAnnotation(annot_i);

		unsigned int group_i = 0;
		for( group_i = 0; group_i < annot_groups.size(); ++group_i )
		{
			if( _areMatchable( annot, annot_groups[group_i] ) )
				break;
		}

		if( group_i == annot_groups.size() )
		{
			annot_groups.push_back(annot);
		}
	}

	for( unsigned int group_i = 0; group_i < annot_groups.size(); ++group_i )
	{
		// extract annotations of each group and store them in temporary container
		std::vector<AnimationAnnotationContainer*> base_annots0( mAnimSpace->getNumBaseAnimations() );
		unsigned int banim_i = 0;
		for( banim_i = 0; banim_i < mAnimSpace->getNumBaseAnimations(); ++banim_i )
		{
			Animation* banim = mAnimSpace->getBaseAnimation(banim_i);

			if( annotClass == AnimAnnot_Transition )
				base_annots0[banim_i] = new TransitionAnnotationContainer();
			else if( annotClass == AnimAnnot_ParamTransition )
				base_annots0[banim_i] = new ParamTransitionAnnotationContainer();
			else if( annotClass == AnimAnnot_PlantConstraint )
				base_annots0[banim_i] = new PlantConstraintAnnotationContainer();
			else // if( annotClass == AnimAnnot_SimEvent )
				base_annots0[banim_i] = new SimEventAnnotationContainer();

			for( unsigned int annot_i = 0; annot_i < base_annots[banim_i]->getNumAnnotations(); ++annot_i )
			{
				AnimationAnnotation* annot = base_annots[banim_i]->getAnnotation(annot_i);

				if( _areMatchable( annot_groups[group_i], annot ) )
				{
					// annotation belongs to current group, add it to the temporary container (and convert to global timeframe)
					AnimationAnnotation* annot0 = base_annots0[banim_i]->createAnnotation(
						_toGlobalTime( banim, annot->getStartTime() ), _toGlobalTime( banim, annot->getEndTime() ) );
					annot->_clone(annot0);
				}
			}
		}

		// every anim has at least one annot of each group
		// compute annotation matches
		_makeMatches(base_annots0);

		// clean up extracted annotations
		for( banim_i = 0; banim_i < mAnimSpace->getNumBaseAnimations(); ++banim_i )
		{
			if( annotClass == AnimAnnot_Transition )
				delete base_annots0[banim_i];
			else if( annotClass == AnimAnnot_ParamTransition )
				delete base_annots0[banim_i];
			else if( annotClass == AnimAnnot_PlantConstraint )
				delete base_annots0[banim_i];
			else if( annotClass == AnimAnnot_SimEvent )
				delete base_annots0[banim_i];
			else // if( annotClass == AnimAnnot_GesturePhase )
				delete base_annots0[banim_i];
		}
	}

	// we're done, clean up
	for( unsigned int banim_i = 0; banim_i < mAnimSpace->getNumBaseAnimations(); ++banim_i )
	{
		if( annotClass == AnimAnnot_Transition )
			delete base_annots[banim_i];
		else if( annotClass == AnimAnnot_ParamTransition )
			delete base_annots[banim_i];
		else if( annotClass == AnimAnnot_PlantConstraint )
			delete base_annots[banim_i];
		else if( annotClass == AnimAnnot_SimEvent )
			delete base_annots[banim_i];
		else // if( annotClass == AnimAnnot_GesturePhase )
			delete base_annots[banim_i];
	}
}

void AnnotationMatchMaker::_makeMatches( const std::vector<AnimationAnnotationContainer*>& annotSet )
{
	AnimationAnnotationClass annot_class = annotSet[0]->getClassId();
	std::vector<AnimationAnnotationContainer*> target_annots( mAnimSpace->getNumBaseAnimations() );

	// get pointers to target annot containers
	for( unsigned int anim_i = 0; anim_i < mAnimSpace->getNumBaseAnimations(); ++anim_i )
	{
		Animation* anim = mAnimSpace->getBaseAnimation(anim_i);

		if( annot_class == AnimAnnot_Transition )
			target_annots[anim_i] = anim->getTransitionAnnotations();
		else if( annot_class == AnimAnnot_ParamTransition )
			target_annots[anim_i] = anim->getParamTransitionAnnotations();
		else if( annot_class == AnimAnnot_PlantConstraint )
			target_annots[anim_i] = anim->getPlantConstraintAnnotations();
		else // if( annot_class == AnimAnnot_SimEvent )
			target_annots[anim_i] = anim->getSimEventAnnotations();
	}

	// get first unprocessed annot for each anim
	std::vector<unsigned int> annot_indexes; // for each anim, earliest unprocessed annot index
	std::vector<AnimationAnnotation*> annots; // for each anim, earliest unprocessed annot
	Animation* anim = NULL;
	for( unsigned int anim_i = 0; anim_i < mAnimSpace->getNumBaseAnimations(); ++anim_i )
	{
		anim = mAnimSpace->getBaseAnimation(anim_i);

		if( annotSet[anim_i]->getNumAnnotations() <= 0 )
			// no annotations on current anim, can't make matches
			return;

		annot_indexes.push_back(0);
		annots.push_back( annotSet[anim_i]->getAnnotation(0) );
	}

	while(true)
	{
		if( _areConnected(annots) )
		{
			// annots are connected, compute match annots

			// determine which annots should be split
			std::vector<unsigned int> annots_split, annots_nosplit; // best annot partition
			float apq = 0; // annot partition quality
			for( unsigned int anim_i1 = 0; anim_i1 < annot_indexes.size(); ++anim_i1 )
			{
				std::vector<unsigned int> annots_split0, annots_nosplit0;
				annots_split0.push_back(anim_i1);

				for( unsigned int anim_i2 = 0; anim_i2 < annot_indexes.size(); ++anim_i2 )
				{
					if( anim_i2 != anim_i1 )
						annots_nosplit0.push_back(anim_i2);
				}

				float stable = false; // indicates if annot partition is stable (i.e. no changes have been made on last iteration)
				do
				{
					stable = true;

					for( unsigned int nsi = 0; nsi < annots_nosplit0.size(); ++nsi )
					{
						unsigned int nsanim_i = annots_nosplit0[nsi];
						bool sub_all = true;

						for( unsigned si = 0; si < annots_split0.size(); ++si )
						{
							unsigned int sanim_i = annots_split0[si];

							if( !_subsumes( annotSet[sanim_i], annot_indexes[sanim_i],
								annotSet[nsanim_i], annot_indexes[nsanim_i] ) )
							{
								sub_all = false;
								break;
							}
						}

						if( !sub_all )
						{
							annots_nosplit0.erase( annots_nosplit0.begin() + (nsi--) );
							annots_split0.push_back(nsanim_i);
							stable = false;

							break;
						}
					}
				}
				while( !stable );

				// compute effective annot intervals
				std::vector<float> annot_st, annot_et;
				for( unsigned int si = 0; si < annots_split0.size(); ++si )
				{
					unsigned int anim_i = annots_split0[si];

					annot_st.push_back( annots[anim_i]->getStartTime() );
					annot_et.push_back( annots[anim_i]->getEndTime() );
				}
				for( unsigned int nsi = 0; nsi < annots_nosplit0.size(); ++nsi )
				{
					unsigned int anim_i = annots_nosplit0[nsi];
					
					annot_st.push_back( annots[anim_i]->getStartTime() );
					annot_et.push_back( annotSet[anim_i]->getAnnotation( annot_indexes[anim_i] + 1 )->getEndTime() );
				}

				// compute annot partition quality
				float apq0 = 0;
				for( unsigned int int_i1 = 0; int_i1 < annot_st.size(); ++int_i1 )
				{
					TransitionAnnotation annot1( annot_st[int_i1], annot_et[int_i1] );
					// annot1 and annot2 are just "dummy" annots, so we can compute overlap

					for( unsigned int int_i2 = 0; int_i2 < annot_st.size(); ++int_i2 )
					{
						TransitionAnnotation annot2( annot_st[int_i2], annot_et[int_i2] );
						
						apq0 += _overlap( &annot1, &annot2 );
					}
				}

				if( annots_nosplit0.empty() )
				{
					// no annots need to be split

					annots_nosplit0 = annots_split0;
					annots_split0.clear();
				}

				// retain better partition
				if( apq0 > apq )
				{
					apq = apq0;
					annots_split = annots_split0;
					annots_nosplit = annots_nosplit0;
				}
			}

			// split annots
			for( unsigned int si = 0; si < annots_split.size(); ++si )
			{
				unsigned int anim_i1 = annots_split[si];
				
				// determine split times
				float let = 0, rst = 0;
				float w = 1.f / annots_nosplit.size();
				for( unsigned int nsi = 0; nsi < annots_nosplit.size(); ++nsi )
				{
					unsigned int anim_i2 = annots_nosplit[nsi];
					float let0, rst0;

					_getSplitTimes( annotSet[anim_i1], annot_indexes[anim_i1],
						annotSet[anim_i2], annot_indexes[anim_i2], let0, rst0 );
					let += w * let0;
					rst += w * rst0;
				}

				// split annot
				AnimationAnnotation* lannot = annotSet[anim_i1]->getAnnotation( annot_indexes[anim_i1] );
				float ret = lannot->getEndTime();
				lannot->setEndTime(let);
				AnimationAnnotation* rannot = annotSet[anim_i1]->createAnnotation( rst, ret );
				lannot->_clone(rannot);
			}

			// add new annot match
			for( unsigned int anim_i = 0; anim_i < annots.size(); ++anim_i )
			{
				Animation* anim = mAnimSpace->getBaseAnimation(anim_i);

				AnimationAnnotation* new_annot = target_annots[anim_i]->createAnnotation(
					_toLocalTime( anim, annots[anim_i]->getStartTime() ),
					_toLocalTime( anim, annots[anim_i]->getEndTime() ) );
				annots[anim_i]->_clone(new_annot);
			}

			// move to next annots
			unsigned int anim_i = 0;
			for( anim_i = 0; anim_i < annot_indexes.size(); ++anim_i )
			{
				++annot_indexes[anim_i];
				if( annot_indexes[anim_i] >= annotSet[anim_i]->getNumAnnotations() )
					// no more annots to be found
					break;
				annots[anim_i] = annotSet[anim_i]->getAnnotation( annot_indexes[anim_i] );
			}

			if( anim_i < annot_indexes.size() )
			{
				// no more annots to be found, we're done
				break;
			}
		}
		else
		{
			// annots are not connected, discard earliest annot

			float st_min = FLT_MAX;
			unsigned int ani_min = 0;

			// find earliest annot and discard it
			for( unsigned int anim_i = 0; anim_i < annot_indexes.size(); ++anim_i )
			{
				if( annots[anim_i]->getStartTime() < st_min )
				{
					st_min = annots[anim_i]->getStartTime();
					ani_min = anim_i;
				}
			}

			// move to next annot for that anim
			++annot_indexes[ani_min];
			if( annot_indexes[ani_min] >= annotSet[ani_min]->getNumAnnotations() )
				// no more annots to be found, we're done
				break;
			annots[ani_min] = annotSet[ani_min]->getAnnotation( annot_indexes[ani_min] );
		}
	}
}

bool AnnotationMatchMaker::_areMatchable( AnimationAnnotation* annot1, AnimationAnnotation* annot2 )
{
	zhAssert( annot1->getClassId() == annot2->getClassId() );

	AnimationAnnotationClass annot_class = annot1->getClassId();

	if( annot_class == AnimAnnot_Transition )
	{
		return matchAnnotations<TransitionAnnotation>(
		static_cast<TransitionAnnotation*>(annot1),
		static_cast<TransitionAnnotation*>(annot2) );
	}
	else if( annot_class == AnimAnnot_ParamTransition )
	{
		return matchAnnotations<ParamTransitionAnnotation>(
		static_cast<ParamTransitionAnnotation*>(annot1),
		static_cast<ParamTransitionAnnotation*>(annot2) );
	}
	else if( annot_class == AnimAnnot_PlantConstraint )
	{
		return matchAnnotations<PlantConstraintAnnotation>(
		static_cast<PlantConstraintAnnotation*>(annot1),
		static_cast<PlantConstraintAnnotation*>(annot2) );
	}
	else // if( annot_class == AnimAnnot_SimEvent )
	{
		return matchAnnotations<SimEventAnnotation>(
		static_cast<SimEventAnnotation*>(annot1),
		static_cast<SimEventAnnotation*>(annot2) );
	}

	return false;
}

float AnnotationMatchMaker::_toGlobalTime( Animation* anim, float time ) const
{
	// TODO: make conversion using timewarp curve if available
	return time / anim->getLength();
}

float AnnotationMatchMaker::_toLocalTime( Animation* anim, float time ) const
{
	// TODO: make conversion using timewarp curve if available
	return time * anim->getLength();
}

bool AnnotationMatchMaker::_areConnected( const std::vector<AnimationAnnotation*>& annots ) const
{
	// determine if a set of annotations are connected

	if( annots.empty() )
		return false;

	std::vector<AnimationAnnotation*> annots0 = annots;
	float st = annots[0]->getStartTime(),
		et = annots[0]->getEndTime(); // interval of annot. union
	bool overlap = true;

	while(overlap)
	{
		overlap = false;

		for( unsigned int annot_i = 0; annot_i < annots0.size(); ++annot_i )
		{
			AnimationAnnotation* annot = annots0[annot_i];

			if( annot->getEndTime() >= st && annot->getStartTime() <= et )
			{
				// annot overlaps current union, expand union

				if( annot->getStartTime() < st )
					st = annot->getStartTime();
				
				if( annot->getEndTime() > et )
					et = annot->getEndTime();

				// mark annot as connected
				annots0.erase( annots0.begin() + (annot_i--) );
				overlap = true;
			}
		}
	}

	if( !annots0.empty() )
		// some annots aren't part of the union
		return false;

	return true;		
}

bool AnnotationMatchMaker::_subsumes( AnimationAnnotationContainer* annots1, unsigned int annotIndex1, AnimationAnnotationContainer* annots2, unsigned int annotIndex2 ) const
{
	// determine if the first annot subsumes the other annot

	AnimationAnnotation* annot1 = annots1->getAnnotation(annotIndex1);
	AnimationAnnotation* next_annot1 = NULL;
	if( annotIndex1 + 1 < annots1->getNumAnnotations() )
		next_annot1 = annots1->getAnnotation( annotIndex1 + 1 );

	AnimationAnnotation* annot2 = annots2->getAnnotation(annotIndex2);
	AnimationAnnotation* next_annot2 = NULL;
	if( annotIndex2 + 1 < annots2->getNumAnnotations() )
		next_annot2 = annots2->getAnnotation( annotIndex2 + 1 );

	if( _overlap( annot1, annot2 ) > 0.00001f &&
		next_annot2 != NULL && _overlap( annot1, next_annot2 ) >= 0.00001f &&
		( next_annot1 == NULL || _overlap( next_annot1, next_annot2 ) < 0.00001f ) )
	{
		return true;
	}

	return false;
}

float AnnotationMatchMaker::_overlap( AnimationAnnotation* annot1, AnimationAnnotation* annot2 ) const
{
	if( annot2->getStartTime() < annot1->getStartTime() )
		std::swap( annot1, annot2 );

	float l_int = 0; // length of the intersection

	if( annot2->getStartTime() < annot1->getEndTime() )
	{
		if( annot2->getEndTime() <= annot1->getEndTime() )
			l_int = annot2->getEndTime() - annot2->getStartTime();
		else
			l_int = annot1->getEndTime() - annot2->getStartTime();
	}

	float l_seg = std::min<float>( annot1->getEndTime() - annot1->getStartTime(),
		annot2->getEndTime() - annot2->getStartTime() ); // length of the shorter segment

	return l_int/l_seg;
}

void AnnotationMatchMaker::_getSplitTimes( AnimationAnnotationContainer* annots1, unsigned int annotIndex1,
										  AnimationAnnotationContainer* annots2, unsigned int annotIndex2,
										  float& lEndTime, float& rStartTime )
{
	// compute split times for the first annotation depending on the second annotation

	AnimationAnnotation* annot1 = annots1->getAnnotation(annotIndex1);
	AnimationAnnotation* annot2 = annots2->getAnnotation(annotIndex2);
	AnimationAnnotation* next_annot2 = NULL;
	if( annotIndex2 + 1 < annots2->getNumAnnotations() )
		next_annot2 = annots2->getAnnotation( annotIndex2 + 1 );
	AnimationAnnotation* next2_annot2 = NULL;
	if( annotIndex2 + 2 < annots2->getNumAnnotations() )
		next2_annot2 = annots2->getAnnotation( annotIndex2 + 2 );

	float p = 0;
	if( next2_annot2 != NULL &&
		next2_annot2->getStartTime() < annot1->getEndTime() )
	{
		p = next_annot2->getEndTime();
	}
	else
	{
		p = annot1->getEndTime();
	}

	lEndTime = annot1->getStartTime() + ( p - annot1->getStartTime() ) / ( next_annot2->getEndTime() - annot2->getStartTime() ) * ( annot2->getEndTime() - annot2->getStartTime() );
	rStartTime = p - ( p - annot1->getStartTime() ) / ( next_annot2->getEndTime() - annot2->getStartTime() ) * ( next_annot2->getEndTime() - next_annot2->getStartTime() );
}

}
