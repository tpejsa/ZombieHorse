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

#ifndef __zh_h__
#define __zh_h__

#include "zhPrereq.h"
#include "zhIterators.h"
#include "zhError.h"
#include "zhString.h"
#include "zhFileSystem.h"
#include "zhTimer.h"
#include "zhLogger.h"
#include "zhAllocObj.h"
#include "zhMemoryPool.h"
#include "zhFunctor.h"
#include "zhEvent.h"
#include "zhObjectFactory.h"
#include "zhSmartPtr.h"
#include "zhResourceManager.h"
#include "zhMath.h"
#include "zhAnimationSystem.h"
#include "zhAnimationManager.h"
#include "zhAnimationSet.h"
#include "zhAnimation.h"
#include "zhAnimationSpace.h"
#include "zhAnimationTrack.h"
#include "zhBoneAnimationTrack.h"
#include "zhAnimationSpace.h"
#include "zhZHALoader.h"
#include "zhZHASerializer.h"
#include "zhAnimationTree.h"
#include "zhAnimationNode.h"
#include "zhAnimationSampler.h"
#include "zhAnimationBlender.h"
#include "zhAnimationTransitionBlender.h"
#include "zhAnimationDistanceGrid.h"
#include "zhMatchGraph.h"
#include "zhMatchWeb.h"
#include "zhAnimationIndexManager.h"
#include "zhAnimationIndex.h"
#include "zhAnimationSpaceBuilder.h"
#include "zhAnimationSearchSystem.h"
#include "zhDenseSamplingParamBuilder.h"
#include "zhPlantConstrDetector.h"
#include "zhAnnotationMatchMaker.h"

#include "rapidxml.hpp"
#include "rapidxml_print.hpp"

#endif // __zh_h__
