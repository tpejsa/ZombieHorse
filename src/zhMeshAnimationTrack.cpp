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

#include "zhMeshAnimationTrack.h"
#include "zhModel.h"

namespace zh
{

MorphKeyFrame::MorphKeyFrame( float time, unsigned int index )
: KeyFrame( time, index )
{
}

MorphKeyFrame::~MorphKeyFrame()
{
}

const Vector& MorphKeyFrame::getMorphTargetWeights() const
{
	return mMTWeights;
}

void MorphKeyFrame::setMorphTargetWeights( const Vector& weights )
{
	mMTWeights = weights;
}

MeshAnimationTrack::MeshAnimationTrack( unsigned short meshId, Animation* anim )
: AnimationTrack(anim), mMeshId(meshId)
{
}

MeshAnimationTrack::~MeshAnimationTrack()
{
}

unsigned short MeshAnimationTrack::getMeshId() const
{
	return mMeshId;
}

void MeshAnimationTrack::getInterpolatedKeyFrame( float time, KeyFrame* kf ) const
{
	zhAssert( kf != NULL );

	MorphKeyFrame* mkf = static_cast<MorphKeyFrame*>(kf);
	KeyFrame *kf1, *kf2;
	MorphKeyFrame *mkf1, *mkf2;

	// get nearest 2 key-frames
	float t = getKeyFramesAtTime( time, &kf1, &kf2 );
	mkf1 = static_cast<MorphKeyFrame*>(kf1);
	mkf2 = static_cast<MorphKeyFrame*>(kf2);

	// interpolate between them
	if( zhEqualf( t, 0 ) )
	{
		mkf->setMorphTargetWeights( mkf1->getMorphTargetWeights() );
	}
	else if( zhEqualf( t, 1 ) )
	{
		mkf->setMorphTargetWeights( mkf2->getMorphTargetWeights() );
	}
	else
	{
		const Vector& mtw1 = mkf1->getMorphTargetWeights();
		const Vector& mtw2 = mkf2->getMorphTargetWeights();

		mkf->setMorphTargetWeights( mtw1 + ( mtw2 - mtw1 ) * t );
	}
}

void MeshAnimationTrack::apply( Model* model, float time, float weight, float scale ) const
{
	MorphKeyFrame mkf( time, 0 );
	Mesh* mesh = model->getMesh(mMeshId);

	getInterpolatedKeyFrame( time, &mkf );

	// apply morph target weights
	mesh->setMorphTargetWeights( mkf.getMorphTargetWeights() * weight );
}

KeyFrame* MeshAnimationTrack::_createKeyFrame( float time )
{
	return new MorphKeyFrame( time, 0 );
}

}
