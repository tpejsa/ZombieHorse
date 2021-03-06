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

#ifndef __AnimationStudio_h__
#define __AnimationStudio_h__

#include "zh.h"

#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/grid.h>
#include <wx/dir.h>
#include <wx/propgrid/propgrid.h>
#include <wx/dnd.h>

#include "ogre.h"

#define zhGround_Size 1000
#define zhGround_TileSize 15.f

#define zhSkeleton_BoneSize 0.2f
#define zhSkeleton_BoneColor Ogre::ColourValue(0,0.6f,0.2f)
#define zhSkeleton_MarkerSize 12.f
#define zhSkeleton_MarkerColor Ogre::ColourValue(0.8f,0.2f,0.6f)
#define zhSkeleton_TraceColor Ogre::ColourValue(0.8f,0.2f,0.2f)
#define zhSkeleton_EnvObjColor Ogre::ColourValue(0.2f,0.2f,0.8f)

#define zhCamera_Pos Ogre::Vector3( 50.f, 50.f, 50.f )
#define zhCamera_Focus Ogre::Vector3( 0.f, 12.f, 0.f )
#define zhCamera_TransSpeed 50.f
#define zhCamera_RotSpeed 1

#define zhOgreVector3(v) Ogre::Vector3( (v).x, (v).y, (v).z )
#define zhOgreQuat(q) Ogre::Quaternion( (q).w, (q).x, (q).y, (q).z )
#define zhVector3(v) zh::Vector3( (v).x, (v).y, (v).z )
#define zhQuat(q) zh::Quat( (q).w, (q).x, (q).y, (q).z )

using namespace std;
using namespace Ogre;
using namespace zh;

#endif // __AnimationStudio_h__
