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

#include "zhBoneTransformController.h"

namespace zh
{

const Vector3& BoneTransformController::getTranslation() const
{
	return Vector3();
}

void BoneTransformController::setTranslation( const Vector3& trans )
{
}

const Quat& BoneTransformController::getRotation() const
{
	return Quat();
}

void BoneTransformController::setRotation( const Quat& rot )
{
}

const Vector3& BoneTransformController::getScale() const
{
	return Vector3();
}

void BoneTransformController::setScale( const Vector3& scal )
{
}

BoneTransformMode BoneTransformController::getTransformMode() const
{
	return BoneTransformMode_RelToInitial;
}

void BoneTransformController::setTransformMode( BoneTransformMode mode )
{
}

/*bool BoneTransformController::getRotationSmoothed() const
{
	return false;
}

void BoneTransformController::setRotationSmoothed( bool smoothing, ValueSmootherClass smoother ) const
{
}

ValueSmoother<Quat>* BoneTransformController::getRotationSmoother() const
{
	return NULL;
}*/

size_t BoneTransformController::_calcMemoryUsage() const
{
	return 0;
}

void BoneTransformController::_unload()
{
}

void BoneTransformController::_clone( BoneController* clonePtr, bool shareData ) const
{
}

void BoneTransformController::_update( float dt )
{
}

void BoneTransformController::_applyController( float weight ) const
{
}

}
