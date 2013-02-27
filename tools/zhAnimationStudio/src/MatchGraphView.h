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

#ifndef __MatchGraphView_h__
#define __MatchGraphView_h__

#include "AnimationStudio.h"

/**
* Match graph view custom control.
*/
class MatchGraphView : public wxWindow
{

public:

	MatchGraphView( wxWindow* parent, wxWindowID id, MatchGraph* matches );

	/**
	* Gets the zoom level (in percentages).
	*/
	unsigned int getZoom() const;

	/**
	* Sets the zoom level (in percentages).
	*/
	void setZoom( unsigned int zoom = 100 );

	/**
	* Repaints the match graph view.
	*/
	void redraw();
	
	/**
	* Renders the match graph view.
	*
	* @param dc Device context.
	*/
	void render( wxDC& dc );

	void OnLeftDown( wxMouseEvent& evt );
	void OnLeftUp( wxMouseEvent& evt );
	void OnKeyDown( wxKeyEvent& evt );
	void OnPaint( wxPaintEvent& evt );
	void OnClose( wxCloseEvent& evt );

	DECLARE_EVENT_TABLE()

private:

	void _initView();
	// computes offset from left edge of the graph for each node,
	// returns size of the subtree (num. leaves)
	unsigned int _computeNodeProps( MatchGraph::Node* node, unsigned int lvl = 0, float cost = 0, unsigned int lOff = 0 );

	MatchGraph* mMatches;
	MatchGraph::Node* mSelMatch;
	AnimationSetPtr mTempAnimSet;

	std::map<MatchGraph::Node*, float> mNodeCosts; // match graph costs (relative to root node)
	float mMaxCost;
	std::vector< std::vector< std::pair<MatchGraph::Node*, unsigned int> > > mNodeLOffs; // match graph node left offsets;
	// for each level of the graph, a map of nodes and their position relative to the left edge of the graph
	std::map<MatchGraph::Node*, wxRect> mNodeRect; // bounding boxes for selection
	unsigned int mZoom;

};

#endif // __MatchGraphView_h__
