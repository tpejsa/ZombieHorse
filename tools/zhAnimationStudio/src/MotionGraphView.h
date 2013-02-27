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

#ifndef __MotionGraphView_h__
#define __MotionGraphView_h__

#include "AnimationStudio.h"

enum MGViewStyle
{
	MGView_Raw,
	MGView_Structured
};

/**
* Motion graph view custom control.
*/
class MotionGraphView : public wxWindow
{

public:

	MotionGraphView( wxWindow* parent, wxWindowID id, MGViewStyle mgStyle = MGView_Raw );

	/**
	* Gets the motion graph view style.
	*/
	MGViewStyle getMGStyle() { return mMGStyle; }

	/**
	* Gets the zoom level (in percentages).
	*/
	unsigned int getZoom() const;

	/**
	* Sets the zoom level (in percentages).
	*/
	void setZoom( unsigned int zoom = 100 );

	/**
	* If true, length of animation lines in the graph is proportionate
	* to temporal length of corresponding animation clips, otherwise
	* all motion lines are of equal length.
	* 
	* @remark This option applies only to MGView_Raw style.
	*/
	bool getUseTimeProp() const;

	/**
	* If set to true, length of animation lines in the graph is proportionate
	* to temporal length of corresponding animation clips, otherwise
	* all motion lines are of equal length.
	* 
	* @remark This option applies only to MGView_Raw style.
	*/
	void setUseTimeProp( bool use = true);

	/**
	* If true, transition interval is indicated along animation lines
	* in the graph.
	* 
	* @remark This option applies only to MGView_Raw style.
	*/
	bool getShowTransitionInterval() const;

	/**
	* If set to  true, transition interval is indicated along animation lines
	* in the graph.
	* 
	* @remark This option applies only to MGView_Raw style.
	*/
	void setShowTransitionInterval( bool show = true );

	/**
	* Repaints the motion graph view.
	*/
	void redraw();
	
	/**
	* Renders the motion graph view.
	*
	* @param dc Device context.
	*/
	void render( wxDC& dc );

	void OnLeftDown( wxMouseEvent& evt );
	void OnLeftUp( wxMouseEvent& evt );
	void OnPaint( wxPaintEvent& evt );

	DECLARE_EVENT_TABLE()

private:

	void _initView();
	void _updateSize();

	MGViewStyle mMGStyle;
	unsigned int mZoom;
	bool mUseTimeProp;
	bool mShowTransInt;
	std::map<zh::Animation*, unsigned int> mAnimPos;
	std::map<zh::Animation*, wxRect> mAnimRect;
	std::set<zh::Animation*> mSelAnims;

	float mMaxLen;

};

#endif // __MotionGraphView_h__
