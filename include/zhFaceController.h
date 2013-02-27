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

#ifndef __zhFaceController_h__
#define __zhFaceController_h__

#include "zhPrereq.h"
#include "zhIterators.h"
#include "zhObjectFactory.h"
#include "zhVector.h"
#include "zhAnimationTree.h"
#include "zhAnimationNode.h"

namespace zh
{

/**
* @brief Facial pose controller class.
*/
class zhDeclSpec FaceController : public AnimationNode
{

public:

	zhDeclare_AnimationNode( FaceController, zhFaceController_ClassId, zhFaceController_ClassName )

	/**
	* Constructor.
	*/
	FaceController()
	{
	}

	/**
	* Destructor.
	*/
	~FaceController()
	{
	}

	/**
	* Returns true if the current animation node is a leaf
	* (and therefore accepts no child nodes) or false otherwise.
	*/
	bool isLeaf() const;

	/**
	* Gets the play status of this animation node and its children.
	*
	* @return true if this animation node is playing, false otherwise.
	*/
	bool getPlaying() const;

	/**
	* Sets the play status of this animation node and its children.
	*
	* @param play If true, this animation node will be playing.
	*/
	void setPlaying( bool playing = true );

	/**
	* Gets the pause status of this animation node and its children.
	*
	* @return true if this animation node is paused, false otherwise.
	*/
	bool getPaused() const;

	/**
	* Sets the pause status of this animation node and its children.
	*
	* @param pause If true, this animation node will be paused.
	*/
	void setPaused( bool paused = true );

	/**
	* Gets the current play time of this animation node.
	*/
	float getPlayTime() const;

	/**
	* Sets the current play time of this animation node.
	*/
	void setPlayTime( float time );

	/**
	* Gets the current play time of this animation node,
	* normalized to range 0 to 1.
	*/
	float getNormalizedPlayTime() const;

	/**
	* Sets the current play time of this animation node,
	* normalized to range 0 to 1.
	*/
	void setNormalizedPlayTime( float time );

	/**
	* Gets the playback rate of this animation node.
	*/
	float getPlayRate() const;

	/**
	* Sets the playback rate of this animation node.
	*/
	void setPlayRate( float rate );

	/**
	* Gets the total playback length of this animation node.
	*/
	float getPlayLength() const;

	/**
	* Gets the currently specified morph target weights.
	*/
	virtual const Vector& getMorphTargetWeights() const;

	/**
	* Sets the morph target weights.
	*/
	virtual void setMorphTargetWeights( const Vector& weights );

	// TODO: implement weights smoothing (to ensure smooth facial movement?)

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

protected:

	/**
	* Updates this animation node's playback state. Any subclass
	* that wishes to implement its own update method should do so
	* by overriding this method.
	*
	* @param dt Elapsed time.
	*/
	void _update( float dt );

	/**
	* Applies this animation node to the current model.
	* Any subclass that wishes to implement its own apply method 
	* should do so by overriding this method.
	*
	* @param weight Blend weight.
	* @param boneMask Bone mask. Animation is not applied to masked bones.
	*/
	void _applyNode( float weight = 1.f, const std::set<unsigned short>& boneMask = Animation::EmptyBoneMask ) const;

};

}

#endif // __zhFaceController_h__
