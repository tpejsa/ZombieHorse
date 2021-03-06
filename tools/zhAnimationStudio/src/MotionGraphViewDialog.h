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

#ifndef __MotionGraphViewDialog_h__
#define __MotionGraphViewDialog_h__

#include "AnimationStudio.h"
#include "MotionGraphView.h"

enum
{
	ID_chkUseTimeProp,
	ID_chkShowTransInt,
	ID_wndMotionGraphViewScroll,
	ID_wndMotionGraphView
};

/**
* Motion graph view dialog.
*/
class MotionGraphViewDialog : public wxDialog
{

public:

	MotionGraphViewDialog( wxWindow* parent, wxWindowID id, MGViewStyle mgStyle );

	void OnCheckBox_UseTimeProp( wxCommandEvent& evt );
	void OnCheckBox_ShowTransInt( wxCommandEvent& evt );

	DECLARE_EVENT_TABLE()

private:

	wxCheckBox* mChkUseTimeProp, *mChkShowTransInt;
	wxScrolledWindow* mWndMotionGraphViewScroll;
	MotionGraphView* mWndMotionGraphView;

};

#endif // __MotionGraphViewDialog_h__
