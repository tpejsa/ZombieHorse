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

#include "zhAnimationAdaptor.h"
#include "zhAnimationNode.h"

namespace zh
{

AnimationAdaptor::AnimationAdaptor( Skeleton* origSkel, AnimationNode* animNode ) :
mOrigSkel(origSkel), mAnimNode(animNode)
{
	zhAssert( animNode != NULL );
}

AnimationAdaptor::~AnimationAdaptor()
{
}

Skeleton* AnimationAdaptor::getOriginalSkeleton() const
{
	return mOrigSkel;
}

AnimationNode* AnimationAdaptor::getAnimationNode() const
{
	return mAnimNode;
}

void AnimationAdaptor::adapt( Skeleton* targetSkel )
{
	// Get all end-effector bones and environment objects
	// Compute IK goals - end-effector target positions, and weights from distances
	// Get IK solvers from targetSkel (in correct order!)
	// Specify IK goals for the solvers
	// Execute each IK solver (in correct order!)
}

}
