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

#include "zhFaceController.h"

namespace zh
{

bool FaceController::isLeaf() const
{
	return true;
}

bool FaceController::getPlaying() const
{
	return false;
}

void FaceController::setPlaying( bool playing )
{
}

bool FaceController::getPaused() const
{
	return false;
}

void FaceController::setPaused( bool paused )
{
}

float FaceController::getPlayTime() const
{
	return 0;
}

void FaceController::setPlayTime( float time )
{
}

float FaceController::getNormalizedPlayTime() const
{
	return 0;
}

void FaceController::setNormalizedPlayTime( float time )
{
}

float FaceController::getPlayRate() const
{
	return 0;
}

void FaceController::setPlayRate( float rate )
{
}

float FaceController::getPlayLength() const
{
	return 0;
}

void FaceController::_update( float dt )
{
}

void FaceController::_applyNode( float weight, const std::set<unsigned short>& boneMask ) const
{
}

const Vector& FaceController::getMorphTargetWeights() const
{
	return Vector();
}

void FaceController::setMorphTargetWeights( const Vector& weights )
{
}

size_t FaceController::_calcMemoryUsage() const
{
	return 0;
}

void FaceController::_unload()
{
}

void FaceController::_clone( AnimationNode* clonePtr, bool shareData ) const
{
}

}
