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

#ifndef __AnimationNodePropGrid_h__
#define __AnimationNodePropGrid_h__

#include "AnimationStudio.h"

/**
*
* @brief wxPropertyGrid implementation for an animation node property grid.
*
* To implement a property grid class for an AnimationNode subclass:
*
* - subclass AnimationNodePropGrid
* - in the constructor, call AnimationNodePropGrid constructor
* - implement handler for EVT_PG_CHANGED event and
* call AnimationNodePropGrid::OnPGChanged(evt) at the start of the handler
*
* Note: see one of the existing subclasses (e.g. AnimationTransitionBlenderPropGrid) for an example.
*
* To plug an animation node into the Properties Window:
*
* PropertiesWindow* wnd_props = gApp->getAppFrame()->getPropertiesWindow();
* wnd_props->SetPropertyGrid( new AnimationNodePropGrid( wnd_props, node ) ); // node is of type AnimationNode*, retrieved from AnimationTreeInstance
*
* Note: the code is analogous for subclasses of AnimationNode,
* just instantiate appropriate AnimationNodePropGrid subclass.
*
*/
class AnimationNodePropGrid : public wxPropertyGrid
{

public:

	AnimationNodePropGrid( wxWindow* parent, AnimationNode* animNode );

	/**
	* Gets the current animation node.
	*/
	virtual AnimationNode* getAnimationNode() const;

	void OnPGChanged( wxPropertyGridEvent& evt );

	DECLARE_EVENT_TABLE()

protected:

	virtual void _refreshPlaybackProps();

	AnimationNode* mAnimNode;
	AnimationNode* mOrigNode; // original node, from animation tree resource

};

#endif // __AnimationNodePropGrid_h__
