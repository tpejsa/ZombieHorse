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

#ifndef __zhAnimationMixer_h__
#define __zhAnimationMixer_h__

#include "zhPrereq.h"
#include "zhIterators.h"
#include "zhObjectFactory.h"
#include "zhAnimationTree.h"
#include "zhAnimationNode.h"

namespace zh
{

/**
* @brief Class representing an animation sampling node.
*/
class zhDeclSpec AnimationMixer : public AnimationNode
{

public:

	zhDeclare_AnimationNode( AnimationMixer, zhAnimationMixer_ClassId, zhAnimationMixer_ClassName )

	/**
	* Constructor.
	*/
	AnimationMixer();

	/**
	* Destructor.
	*/
	~AnimationMixer();

	/**
	* Returns true if the current animation node is a leaf
	* (and therefore accepts no child nodes) or false otherwise.
	*/
	bool isLeaf() const;

	/**
	* Calculates the AnimationNode memory usage.
	*/
	size_t _calcMemoryUsage() const;

	/**
	* Unloads the AnimationNode, freeing up the memory it occupies.
	*/
	void _unload();

	/**
	* Creates a deep copy of the AnimationNode.
	*
	* @param clonePtr Pointer to the copy.
	* @param shareData Specifies if the clone
	* should share data with the original.
	*/
	void _clone( AnimationNode* clonePtr, bool shareData = false ) const;

};

}

#endif // __zhAnimationMixer_h__
