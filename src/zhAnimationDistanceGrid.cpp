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

#include "zhAnimationDistanceGrid.h"
#include "zhAnimation.h"

namespace zh
{

AnimationDistanceGrid::AnimationDistanceGrid( Skeleton* skel, const AnimationSegment& anim1, const AnimationSegment& anim2, unsigned int sampleRate )
: mSkel(skel), mAnim1(anim1), mAnim2(anim2), mSampleRate(sampleRate), mWndLen(0), mMinDist(0)
{
	zhAssert( skel != NULL );
	zhAssert( sampleRate > 0 );

	// init. bone weights
	/*Skeleton::BoneConstIterator bone_i = skel->getSkeleton()->getBoneConstIterator();
	while( !bone_i.end() )
	{
		Bone* bone = bone_i.next();
		mBoneWeights[ bone->getId() ] = 1.f / skel->getSkeleton()->getNumBones();
	}*/
	_computeBoneWeights( mSkel->getRoot() );
	// normalize weights
	float total_weight = 0;
	for( std::map<unsigned short, float>::const_iterator jwi = mBoneWeights.begin();
		jwi != mBoneWeights.end(); ++jwi )
		total_weight += jwi->second;
	for( std::map<unsigned short, float>::iterator jwi = mBoneWeights.begin();
		jwi != mBoneWeights.end(); ++jwi )
		jwi->second = jwi->second / total_weight;

	mNumSamples1 = (unsigned int)( anim1.getLength() * mSampleRate ) + 1;
	mNumSamples2 = (unsigned int)( anim2.getLength() * mSampleRate ) + 1;

	mGrid.resize( mNumSamples1 * mNumSamples2 );
}

AnimationDistanceGrid::~AnimationDistanceGrid()
{
}

Skeleton* AnimationDistanceGrid::getSkeleton() const
{
	return mSkel;
}

float AnimationDistanceGrid::getBoneWeight( unsigned short boneId ) const
{
	std::map<unsigned short, float>::const_iterator jwi = mBoneWeights.find(boneId);
	if( jwi != mBoneWeights.end() )
		return jwi->second;
	
	return 0;
}

void AnimationDistanceGrid::setBoneWeight( unsigned short boneId, float weight )
{
	zhAssert( mSkel->hasBone(boneId) );

	mBoneWeights[boneId] = weight;
}

const AnimationSegment& AnimationDistanceGrid::getAnimation1() const
{
	return mAnim1;
}

unsigned int AnimationDistanceGrid::getNumSamples1() const
{
	return mNumSamples1;
}

const AnimationSegment& AnimationDistanceGrid::getAnimation2() const
{
	return mAnim2;
}

unsigned int AnimationDistanceGrid::getNumSamples2() const
{
	return mNumSamples2;
}

AnimationDistanceGrid::Index AnimationDistanceGrid::getFramesAtTimes( float time1, float time2 ) const
{
	if( time1 < 0 )
		time1 = 0;
	else if( time1 > mAnim1.getLength() )
		time1 = mAnim1.getLength();

	if( time2 < 0 )
		time2 = 0;
	else if( time2 > mAnim2.getLength() )
		time2 = mAnim2.getLength();

	unsigned int si1 = unsigned int( time1 * mSampleRate );
	si1 = si1 < mNumSamples1 ? si1 : mNumSamples1 - 1;

	unsigned int si2 = unsigned int( time2 * mSampleRate );
	si2 = si2 < mNumSamples2 ? si2 : mNumSamples2 - 1;

	return make_pair( si1, si2 );
}

void AnimationDistanceGrid::getTimesAtFrames( const Index& index, float& time1, float& time2 ) const
{
	zhAssert( index.first < mNumSamples1 && index.second < mNumSamples2 );

	time1 = index.first / ((float)mSampleRate);
	time2 = index.second / ((float)mSampleRate);
}

unsigned int AnimationDistanceGrid::getSampleRate() const
{
	return mSampleRate;
}

void AnimationDistanceGrid::build( float wndLength )
{
	zhAssert( wndLength >= 0 );

	zhLog( "AnimationDistanceGrid", "build", "Building distance grid for animation segments %u, %s [%f - %f] and %u, %s [%f - %f].",
		mAnim1.getAnimation()->getId(), mAnim1.getAnimation()->getName().c_str(),
		mAnim1.getStartTime(), mAnim1.getEndTime(),
		mAnim2.getAnimation()->getId(), mAnim2.getAnimation()->getName().c_str(),
		mAnim2.getStartTime(), mAnim2.getEndTime() );

	mMaxDist = 0;
	mMinDist = FLT_MAX;
	mWndLen = wndLength;

	float dt = 1.f / mSampleRate; // offset between samples (poses)
	std::vector<Vector3> pos1( mNumSamples1 * mSkel->getNumBones() ); // marker positions 1
	std::vector<Vector3> pos2( mNumSamples2 * mSkel->getNumBones() ); // marker positions 2
	std::vector<Vector3> avg_pos1( mNumSamples1 ); // weighted averages of marker positions 1
	std::vector<Vector3> avg_pos2( mNumSamples2 ); // weighted averages of marker positions 2
	std::vector<float> avg_poslen1( mNumSamples1 ); // weighted averages of squared lengths of marker positions 1
	std::vector<float> avg_poslen2( mNumSamples2 ); // weighted averages of squared lengths of marker positions 2

	// compute marker positions for animation 1
	for( unsigned int si = 0; si < mNumSamples1; ++si )
	{
		float t = mAnim1.getStartTime() + si * dt;

		mSkel->resetToInitialPose();
		mAnim1.getAnimation()->apply( mSkel, t, 1, 1, Animation::EmptyBoneMask );

		Skeleton::BoneConstIterator bone_i = mSkel->getBoneConstIterator();
		unsigned bone_i0 = 0;
		Vector3 wpos;
		while( !bone_i.end() )
		{
			Bone* bone = bone_i.next();

			wpos = bone->getWorldPosition();
			pos1[ (bone_i0++) + si * mSkel->getNumBones() ] = wpos;
			avg_pos1[si] += wpos * mBoneWeights[ bone->getId() ];
			avg_poslen1[si] += wpos.lengthSq() * mBoneWeights[ bone->getId() ];
		}
	}

	// compute marker positions for animation 2
	for( unsigned int si = 0; si < mNumSamples2; ++si )
	{
		float t = mAnim2.getStartTime() + si * dt;

		mSkel->resetToInitialPose();
		mAnim2.getAnimation()->apply( mSkel, t, 1, 1, Animation::EmptyBoneMask );

		Skeleton::BoneConstIterator bone_i = mSkel->getBoneConstIterator();
		unsigned bone_i0 = 0;
		Vector3 wpos;
		while( !bone_i.end() )
		{
			Bone* bone = bone_i.next();

			wpos = bone->getWorldPosition();
			pos2[ (bone_i0++) + si * mSkel->getNumBones() ] = wpos;
			avg_pos2[si] += wpos * mBoneWeights[ bone->getId() ];
			avg_poslen2[si] += wpos.lengthSq() * mBoneWeights[ bone->getId() ];
		}
	}
	
	// reset skeleton to initial pose and don't touch it anymore
	mSkel->resetToInitialPose();

	std::vector<float> avg_xxzz( mNumSamples1 * mNumSamples2, 0 ); // weighted averages of combined marker positions (1)
	std::vector<float> avg_xzzx( mNumSamples1 * mNumSamples2, 0 ); // weighted averages of marker positions (2)
	std::vector<float> avg_yy( mNumSamples1 * mNumSamples2, 0 ); // weighted averages of marker positions (3)

	// compute weighted averages of combined marker positions
	for( unsigned int si2 = 0; si2 < mNumSamples2; ++si2 )
	{
		for( unsigned int si1 = 0; si1 < mNumSamples1; ++si1 )
		{
			Skeleton::BoneConstIterator bone_i = mSkel->getBoneConstIterator();
			unsigned bone_i0 = 0;
			while( !bone_i.end() )
			{
				Bone* bone = bone_i.next();

				unsigned int pti = si1 + si2 * mNumSamples1,
					pti1 = bone_i0 + si1 * mSkel->getNumBones(),
					pti2 = bone_i0 + si2 * mSkel->getNumBones();

				avg_xxzz[pti] +=
					( pos1[pti1].x * pos2[pti2].x + pos1[pti1].z * pos2[pti2].z ) *
					mBoneWeights[ bone->getId() ];
				avg_xzzx[pti] +=
					( pos1[pti1].x * pos2[pti2].z -	pos1[pti1].z * pos2[pti2].x ) *
					mBoneWeights[ bone->getId() ];
				avg_yy[pti] +=
					( pos1[pti1].y * pos2[pti2].y ) *
					mBoneWeights[ bone->getId() ];
				++bone_i0;
			}
		}
	}

	unsigned int num_samples = unsigned int( wndLength / dt + 0.5f );
	num_samples = num_samples % 2 != 0 ? num_samples : num_samples + 1;
	num_samples = num_samples <= mNumSamples1 ? num_samples : mNumSamples1;
	num_samples = num_samples <= mNumSamples2 ? num_samples : mNumSamples2;

	float w_max = 1.f / ( num_samples/2 + 1 ); // maximum frame weight value
	float w_min = w_max * w_max; // minimum frame weight value
	std::vector<float> weights( num_samples, 0 ); // frame weight values

	// compute frame weight values
	for( unsigned int wsi = 0; wsi < num_samples; ++wsi )
	{
		weights[wsi] = w_min + ( w_max - w_min ) / ((float)(num_samples/2)) * 
			( wsi <= num_samples/2 ? wsi : ( num_samples - 1 - wsi ) );
	}

	num_samples = ( num_samples - 1 ) / 2; // number of samples in one half of anim. window

	// compute distances and align. transf. between frame pairs
	for( unsigned int si2 = 0; si2 < mNumSamples2; ++si2 )
	{
		for( unsigned int si1 = 0; si1 < mNumSamples1; ++si1 )
		{
			unsigned int si0_1 = 0;
			unsigned int si0_2 = 0;
			float A, B, C, D, E, F;
			A = B = C = D = E = F = 0;
			float dist = 0;

			// compute sumation terms in align. transf. formulas
			for( int wsi = - (int)num_samples; wsi <= (int)num_samples; ++wsi )
			{
				int si0_1 = (int)si1 + wsi;
				if( si0_1 < 0 ) si0_1 = 0;
				else if( si0_1 >= (int)mNumSamples1 ) si0_1 = mNumSamples1 - 1;

				int si0_2 = (int)si2 + wsi;
				if( si0_2 < 0 ) si0_2 = 0;
				else if( si0_2 >= (int)mNumSamples2 ) si0_2 = mNumSamples2 - 1;

				unsigned int pti = si0_1 + si0_2 * mNumSamples1;

				A += weights[ wsi + num_samples ] * avg_xzzx[pti];
				B += weights[ wsi + num_samples ] * avg_pos1[si0_1].x;
				C += weights[ wsi + num_samples ] * avg_pos2[si0_2].z;
				D += weights[ wsi + num_samples ] * avg_pos1[si0_1].z;
				E += weights[ wsi + num_samples ] * avg_pos2[si0_2].x;
				F += weights[ wsi + num_samples ] * avg_xxzz[pti];
			}

			// compute distance and align. transf. for current frame pair
			float orient_y = atan2( A - ( B * C - D * E ), F - ( B * E + D * C ) );
			float pos_x = B - E * cos(orient_y) - C * sin(orient_y),
				pos_z = D + E * sin(orient_y) - C * cos(orient_y);

			// compute distance
			dist = pos_x * pos_x + pos_z * pos_z;
			for( int wsi = - (int)num_samples; wsi <= (int)num_samples; ++wsi )
			{
				int si0_1 = (int)si1 + wsi;
				if( si0_1 < 0 ) si0_1 = 0;
				else if( si0_1 >= (int)mNumSamples1 ) si0_1 = (int)mNumSamples1 - 1;

				int si0_2 = (int)si2 + wsi;
				if( si0_2 < 0 ) si0_2 = 0;
				else if( si0_2 >= (int)mNumSamples2 ) si0_2 = (int)mNumSamples2 - 1;

				unsigned int pti = si0_1 + si0_2 * mNumSamples1;

				float G, H, I;
				G = - avg_xxzz[pti] +
					pos_x * avg_pos2[si0_2].x + pos_z * avg_pos2[si0_2].z;
				H = - avg_xzzx[pti] +
					pos_x * avg_pos2[si0_2].z - pos_z * avg_pos2[si0_2].x;
				I = - avg_yy[pti] -
					pos_x * avg_pos1[si0_1].x - pos_z * avg_pos1[si0_1].z;

				dist += weights[ wsi + num_samples ] * (
					avg_poslen1[si0_1] + avg_poslen2[si0_2] +
					2 * ( G * cos(orient_y) + H * sin(orient_y) + I )
					);
			}

			dist = dist >= 0 ? dist : 0;
			if( dist < mMinDist ) mMinDist = dist;
			if( dist > mMaxDist ) mMaxDist = dist;

			mGrid[ si1 + mNumSamples1 * si2 ] = Point( Index( si1, si2 ), dist, Skeleton::Situation( pos_x, pos_z, orient_y ) );
		}
	}

	zhLog( "AnimationDistanceGrid", "build", "Finished building distance grid for animation segments %u, %s [%f - %f] and %u, %s [%f - %f].",
		mAnim1.getAnimation()->getId(), mAnim1.getAnimation()->getName().c_str(),
		mAnim1.getStartTime(), mAnim1.getEndTime(),
		mAnim2.getAnimation()->getId(), mAnim2.getAnimation()->getName().c_str(),
		mAnim2.getStartTime(), mAnim2.getEndTime() );
}

const AnimationDistanceGrid::Point& AnimationDistanceGrid::getPoint( const Index& index ) const
{
	zhAssert( index.first < mNumSamples1 && index.second < mNumSamples2 );
	
	return mGrid[ index.first + mNumSamples1 * index.second ];
}

void AnimationDistanceGrid::setPoint( const Point& pt )
{
	zhAssert( pt.getIndex().first < mNumSamples1 && pt.getIndex().second < mNumSamples2 );

	mGrid[ pt.getIndex().first + mNumSamples1 * pt.getIndex().second ] = pt;
}

float AnimationDistanceGrid::getDistance( const Index& index ) const
{
	if( index.first >= mNumSamples1 || index.second >= mNumSamples2 )
		return mMaxDist;
	
	return mGrid[ index.first + mNumSamples1 * index.second ].getDistance();
}

float AnimationDistanceGrid::getNormDistance( const Index& index ) const
{
	if( zhEqualf( mMinDist, mMaxDist ) )
		return 1;

	return ( getDistance(index) - mMinDist ) / ( mMaxDist - mMinDist );
}

void AnimationDistanceGrid::setDistance( const Index& index, float dist )
{
	zhAssert( index.first < mNumSamples1 && index.second < mNumSamples2 );

	Point pt = mGrid[ index.first + mNumSamples1 * index.second ];
	mGrid[ index.first + mNumSamples1 * index.second ] = Point( index, dist, pt.getAlignTransf() );
}

const Skeleton::Situation& AnimationDistanceGrid::getAlignTransf( const Index& index ) const
{
	if( index.first >= mNumSamples1 || index.second >= mNumSamples2 )
		return Skeleton::Situation::Identity;
	
	return mGrid[ index.first + mNumSamples1 * index.second ].getAlignTransf();
}

void AnimationDistanceGrid::setAlignTransf( const Index& index, const Skeleton::Situation& transf )
{
	zhAssert( index.first < mNumSamples1 && index.second < mNumSamples2 );

	Point pt = mGrid[ index.first + mNumSamples1 * index.second ];
	mGrid[ index.first + mNumSamples1 * index.second ] = Point( index, pt.getDistance(), transf );
}

float AnimationDistanceGrid::getAnimationWindowLength() const
{
	return mWndLen;
}

unsigned int AnimationDistanceGrid::findLocalMinima( float minDist, bool only2d,
													bool computeBlendRegion, float maxDistDiff )
{
	float d, ld, tld, td, trd, rd, brd, bd, bld;
	minDist = mMinDist + minDist * ( mMaxDist - mMinDist );

	for( unsigned int si2 = 0; si2 < mNumSamples2; ++si2 )
	{
		for( unsigned int si1 = 0; si1 < mNumSamples1; ++si1 )
		{
			d = getDistance( make_pair( si1, si2 ) );
			ld  = getDistance( make_pair( si1 - 1, si2 ) );
			tld  = getDistance( make_pair( si1 - 1, si2 + 1 ) );
			td  = getDistance( make_pair( si1, si2 + 1 ) );
			trd  = getDistance( make_pair( si1 + 1, si2 + 1 ) );
			rd  = getDistance( make_pair( si1 + 1, si2 ) );
			brd  = getDistance( make_pair( si1 + 1, si2 - 1 ) );
			bd  = getDistance( make_pair( si1, si2 - 1 ) );
			bld  = getDistance( make_pair( si1 - 1, si2 - 1 ) );

			if( only2d && d < minDist &&
				d < ld && d < tld && d < td && d < trd &&
				d < rd && d < brd && d < bd && d < bld )
			{
				mMinima.push_back( make_pair( si1, si2 ) );
			}
			else if( !only2d )
			{
				bool min_added = false;

				if( d < minDist && d < ld && d < rd )
				{
					mMinima.push_back( make_pair( si1, si2 ) );
					min_added = true;

					if( computeBlendRegion )
					{
						mBlendRegion.insert( make_pair( si1, si2 ) );

						// extend blend region to the left
						for( unsigned int bri = si1 - 1; bri >= 0 && bri < mNumSamples1; --bri )
						{
							float cur_d = getDistance( make_pair( bri, si2 ) );

							if( ( cur_d - d ) / cur_d < maxDistDiff || cur_d < minDist )
								mBlendRegion.insert( make_pair( bri, si2 ) );
							else
								break;
						}

						// extend blend region to the right
						for( unsigned int bri = si1 + 1; bri < mNumSamples1; ++bri )
						{
							float cur_d = getDistance( make_pair( bri, si2 ) );

							if( ( cur_d - d ) / cur_d < maxDistDiff || cur_d < minDist )
								mBlendRegion.insert( make_pair( bri, si2 ) );
							else
								break;
						}
					}
				}
				
				if( d < minDist && d < td && d < bd )
				{
					if( !min_added )
					{
						mMinima.push_back( make_pair( si1, si2 ) );
					}

					if( computeBlendRegion )
					{
						mBlendRegion.insert( make_pair( si1, si2 ) );

						// extend blend region downwards
						for( unsigned int bri = si2 - 1; bri >= 0 && bri < mNumSamples2; --bri )
						{
							float cur_d = getDistance( make_pair( si1, bri ) );

							if( ( cur_d - d ) / cur_d < maxDistDiff || cur_d < minDist )
								mBlendRegion.insert( make_pair( si1, bri ) );
							else
								break;
						}

						// extend blend region upwards
						for( unsigned int bri = si2 + 1; bri < mNumSamples2; ++bri )
						{
							float cur_d = getDistance( make_pair( si1, bri ) );

							if( ( cur_d - d ) / cur_d < maxDistDiff || cur_d < minDist )
								mBlendRegion.insert( make_pair( si1, bri ) );
							else
								break;
						}
					}
				}
			}
		}
	}

	return mMinima.size();
}

const AnimationDistanceGrid::Index& AnimationDistanceGrid::getLocalMinimum( unsigned int minIndex ) const
{
	zhAssert( minIndex < getNumLocalMinima() );

	return mMinima[minIndex];
}

unsigned int AnimationDistanceGrid::getNumLocalMinima() const
{
	return mMinima.size();
}

bool AnimationDistanceGrid::isLocalMinimum( const Index& index, float minDist, bool only2d ) const
{
	unsigned int index1 = index.first,
		index2 = index.second;

	if( index1 >= mNumSamples1 || index2 >= mNumSamples2 )
		return false;

	float d, ld, tld, td, trd, rd, brd, bd, bld;
	minDist = mMinDist + minDist * ( mMaxDist - mMinDist );

	d = getDistance( index );
	ld  = getDistance( make_pair( index1 - 1, index2 ) );
	tld  = getDistance( make_pair( index1 - 1, index2 + 1 ) );
	td  = getDistance( make_pair( index1, index2 + 1 ) );
	trd  = getDistance( make_pair( index1 + 1, index2 + 1 ) );
	rd  = getDistance( make_pair( index1 + 1, index2 ) );
	brd  = getDistance( make_pair( index1 + 1, index2 - 1 ) );
	bd  = getDistance( make_pair( index1, index2 - 1 ) );
	bld  = getDistance( make_pair( index1 - 1, index2 - 1 ) );

	if( only2d && d < minDist &&
		d < ld && d < tld && d < td && d < trd &&
		d < rd && d < brd && d < bd && d < bld )
	{
		return true;
	}
	else if( !only2d )
	{
		if( d < minDist && d < ld && d < rd )
		{
			return true;
		}
		else if( d < minDist && d < td && d < bd )
		{
			return true;
		}
	}

	return false;
}

bool AnimationDistanceGrid::isBlendRegion( const Index& index ) const
{
	return mBlendRegion.count(index) > 0;
}

float AnimationDistanceGrid::computeOptimalPath( const Index& srcIndex, const Index& dstIndex,
											   std::vector<Point>& path ) const
{
	if( srcIndex.first > dstIndex.first || srcIndex.second > dstIndex.second )
	{
		return FLT_MAX;
	}

	float d = 0;
	Index si = srcIndex;
	unsigned int k = (unsigned int)( zhDTW_KernelSize * 2.f/3.f + 0.5f ); // how many points of the path segment we will keep

	while( dstIndex.first - si.first > zhDTW_KernelSize &&
		dstIndex.second - si.second > zhDTW_KernelSize )
	{
		unsigned int k1 = zhDTW_KernelSize,
			k2 = (unsigned int)( zhDTW_KernelSize * ((float)( dstIndex.second - si.second ))/((float)( dstIndex.first - si.first )) + 0.5f ),
			pti = path.size(); // index of the first point of the path segment

		// append new path segment
		d += _computeOptimalPathSegment( si, Index( si.first + k1, si.second + k2 ), path );

		if( path.size() == 0 )
		{
			// destination point unreachable
			return FLT_MAX;
		}
		
		// keep only the first k points of the path segment
		si = path[ pti + k ].getIndex();
		path.resize( pti + k );
	}

	d += _computeOptimalPathSegment( si, dstIndex, path );

	return d;
}

void AnimationDistanceGrid::_computeBoneWeights( Bone* bone )
{
	zhAssert( bone != NULL );

	mBoneWeights[ bone->getId() ] = bone->getParent() != NULL ? bone->getPosition().length() : 0;

	Bone::ChildConstIterator child_i = bone->getChildConstIterator();
	while( !child_i.end() )
	{
		Bone* child = child_i.next();

		_computeBoneWeights(child);
		mBoneWeights[ bone->getId() ] += mBoneWeights[ child->getId() ];
	}
}

float AnimationDistanceGrid::_computeOptimalPathSegment( const Index& srcIndex, const Index& dstIndex,
														std::vector<Point>& path ) const
{
	std::map<Index,_DTWAnnot> dtw_annots;

	// compute cost values and optimal predecessors
	for( unsigned int i = 0, i1 = srcIndex.first, i2 = srcIndex.second;
		i1 <= dstIndex.first || i2 <= dstIndex.second;
		++i, ++i1, ++i2 )
	{
		unsigned int jext = ( i + 1 ) * zhBlend_DegeneracyLimit - i;

		for( unsigned int j = 0; j < jext; ++j )
		{
			unsigned int j1 = i1 + j,
				j2 = i2 + j;

			if( j1 <= dstIndex.first && i2 <= dstIndex.second )
				dtw_annots[ Index( j1, i2 ) ] = _DTW( Index( j1, i2 ), dtw_annots );
				//path.push_back( Index( j1, i2 ) );

			if( j2 > 0 &&
				i1 <= dstIndex.first && j2 <= dstIndex.second )
				dtw_annots[ Index( i1, j2 ) ] = _DTW( Index( i1, j2 ), dtw_annots );
				//path.push_back( Index( i1, j2 ) );
		}
	}

	if( dtw_annots.count(dstIndex) <= 0 )
	{
		// destination point unreachable
		path.clear();
		return FLT_MAX;
	}

	// compute path by following the chain of predecessors back to source
	Point pt = getPoint(dstIndex);
	path.push_back(pt);
	unsigned int pti = path.size() - 1; // index of the first point in the path segment
	float d = pt.getDistance();
	while( pt.getIndex().first > srcIndex.first &&
		pt.getIndex().second > srcIndex.second )
	{
		Index prev = dtw_annots.find( pt.getIndex() )->second.prev;

		for( int i1 = (int)pt.getIndex().first - 1; i1 >= (int)prev.first; --i1 )
		{
			for( int i2 = (int)pt.getIndex().second - 1; i2 >= (int)prev.second; --i2 )
			{
				Point new_pt = getPoint( Index( i1, i2 ) );
				path.insert( path.begin() + pti, new_pt );
				d += new_pt.getDistance();
			}
		}

		pt = getPoint(prev);
	}

	// clamp path to source point if necessary
	if( srcIndex != pt.getIndex() )
	{
		for( unsigned int i1 = srcIndex.first; i1 < pt.getIndex().first; ++i1 )
		{
			Point new_pt = getPoint( Index( srcIndex.first + pt.getIndex().first - i1 - 1 , pt.getIndex().second ) );
			path.insert( path.begin() + pti, new_pt );
			d += new_pt.getDistance();
		}

		for( unsigned int i2 = srcIndex.second; i2 < pt.getIndex().second; ++i2 )
		{
			Point new_pt = getPoint( Index( pt.getIndex().first, srcIndex.second + pt.getIndex().second - i2 - 1 ) );
			path.insert( path.begin() + pti, new_pt );
			d += new_pt.getDistance();
		}
	}

	return d;
}

AnimationDistanceGrid::_DTWAnnot AnimationDistanceGrid::_DTW( const Index& ptIndex, const std::map<Index,_DTWAnnot>& dtwAnnots ) const
{
	float dmin_h = FLT_MAX,
		dmin_v = FLT_MAX;
	Index imin_h, imin_v;

	// search for a horizontal predecessor candidate
	for( unsigned int r = 1; r <= zhBlend_DegeneracyLimit; ++r )
	{
		unsigned int i1, i2;
		i1 = ptIndex.first - r;
		i2 = ptIndex.second - 1;

		std::map<Index,_DTWAnnot>::const_iterator ani = dtwAnnots.find( Index( i1, i2 ) );
		if( ani == dtwAnnots.end() )
			// must remain in the search area
			continue;

		float d = ani->second.cost;

		for( unsigned int k = 0; k < r; ++k )
		{
			d += getDistance( Index( ptIndex.first - k, ptIndex.second ) );
		}

		if( d < dmin_h )
		{
			dmin_h = d;
			imin_h = Index( i1, i2 );
		}
	}

	// search for a vertical predecessor candidate
	for( unsigned int r = 1; r <= zhBlend_DegeneracyLimit; ++r )
	{
		unsigned int i1, i2;
		i1 = ptIndex.first - 1;
		i2 = ptIndex.second - r;

		std::map<Index,_DTWAnnot>::const_iterator ani = dtwAnnots.find( Index( i1, i2 ) );
		if( ani == dtwAnnots.end() )
			// must remain in the search area
			continue;

		float d = ani->second.cost;

		for( unsigned int k = 0; k < r; ++k )
		{
			d += getDistance( Index( ptIndex.first, ptIndex.second - k ) );
		}

		if( d < dmin_v )
		{
			dmin_v = d;
			imin_v = Index( i1, i2 );
		}
	}

	// choose the predecessor and return the annotation
	if( dmin_v >= FLT_MAX && dmin_h >= FLT_MAX )
	{
		return _DTWAnnot( 0, Index( 0, 0 ) );
	}
	else if( dmin_v < dmin_h )
	{
		return _DTWAnnot( dmin_v, imin_v );
	}
	else
	{
		return _DTWAnnot( dmin_h, imin_h );
	}
}

}
