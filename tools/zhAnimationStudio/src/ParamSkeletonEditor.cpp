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

#include "ParamSkeletonEditor.h"

ParamSkeletonEditor::ParamSkeletonEditor() : mSkel(NULL)
{
}

ParamSkeletonEditor::~ParamSkeletonEditor()
{
}

void ParamSkeletonEditor::init( zh::Skeleton* skel )
{
	zhAssert( skel != NULL );

	resetToInitialState();
	mSkel = skel;

	// Save initial skeleton state
	mInitScale = mSkel->getRoot()->getInitialScale();
	mInitBonePos.clear();
	zh::Skeleton::BoneConstIterator bone_i = mSkel->getBoneConstIterator();
	while( bone_i.hasMore() )
	{
		zh::Bone* bone = bone_i.next();
		mInitBonePos[bone] = bone->getInitialPosition();
	}
}

void ParamSkeletonEditor::scaleBody( float scale )
{
	mSkel->getRoot()->setInitialScale( zh::Vector3(scale,scale,scale) );
}

void ParamSkeletonEditor::scaleTrunk( float scale )
{
	scaleChain( BT_LowerBack, BT_Chest, scale );
}

void ParamSkeletonEditor::scaleArms( float scale )
{
	scaleChain( BT_LElbow, BT_LWrist, scale );
	scaleChain( BT_RElbow, BT_RWrist, scale );
}

void ParamSkeletonEditor::scaleLegs( float scale )
{
	scaleChain( BT_LKnee, BT_LAnkle, scale );
	scaleChain( BT_RKnee, BT_RAnkle, scale );
}

void ParamSkeletonEditor::scaleChain( BoneTag start, BoneTag end, float scale )
{
	if( !mSkel->hasBoneWithTag(start) || !mSkel->hasBoneWithTag(end) )
		return;

	zh::Bone* start_bone = mSkel->getBoneByTag(start);
	zh::Bone* end_bone = mSkel->getBoneByTag(end);
	std::vector<zh::Bone*> chain;
	if( !start_bone->findChain( end_bone, chain ) )
		return;

	for( std::vector<zh::Bone*>::const_iterator bone_i = chain.begin();
		bone_i != chain.end(); ++bone_i )
	{
		zh::Bone* bone = *bone_i;
		bone->setInitialPosition( mInitBonePos.find(bone)->second*scale );
		bone->resetToInitialPose();
	}
}

void ParamSkeletonEditor::resetToInitialState()
{
	if( mSkel == NULL ) return;

	zh::Skeleton::BoneIterator bone_i = mSkel->getBoneIterator();
	while( bone_i.hasMore() )
	{
		zh::Bone* bone = bone_i.next();
		bone->setInitialPosition( mInitBonePos.find(bone)->second );
		bone->resetToInitialPose();
	}
}
