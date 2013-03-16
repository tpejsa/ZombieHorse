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

#ifndef _ParamSkeletonEditor_h__
#define _ParamSkeletonEditor_h__

#include "AnimationStudio.h"

/**
* @brief Class representing an editor for various parts
* of a skeleton.
*/
class ParamSkeletonEditor
{
	
public:

	/**
	* Constructor.
	*/
	ParamSkeletonEditor();

	/**
	* Destructor.
	*/
	~ParamSkeletonEditor();

	/**
	* Initialize the editor with a new skeleton.
	*
	* @param skel Pointer to the target skeleton.
	*/
	void init( zh::Skeleton* skel );

	/**
	* Set the scale of the entire skeleton.
	*
	* @param scale Scaling factor.
	*/
	void scaleBody( float scale );

	/**
	* Set the scale of the trunk.
	*
	* @param scale Scaling factor.
	*/
	void scaleTrunk( float scale );

	/**
	* Set the scale of the arms.
	*
	* @param scale Scaling factor.
	*/
	void scaleArms( float scale );

	/**
	* Set the scale of the legs.
	*
	* @param scale Scaling factor.
	*/
	void scaleLegs( float scale );

	/**
	* Set the scale of the specified chain of bone segments.
	*
	* @param start Start bone.
	* @param end End bone.
	* @param scale Scaling factor.
	*/
	void scaleChain( BoneTag start, BoneTag end, float scale );

	/**
	* Reset the skeleton to the initial state.
	*/
	void resetToInitialState();

private:

	zh::Skeleton* mSkel;
	zh::Vector3 mInitScale;
	std::map<zh::Bone*, zh::Vector3> mInitBonePos;
};

#endif // _ParamSkeletonEditor_h__
