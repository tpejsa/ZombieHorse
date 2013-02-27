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

#include "MotionGraphViewDialog.h"
#include "AnimationStudioApp.h"

MotionGraphViewDialog::MotionGraphViewDialog( wxWindow* parent, wxWindowID id, MGViewStyle mgStyle )
: wxDialog( parent, id, "View Motion Graph", wxDefaultPosition, wxSize( 820, 720 ), wxDEFAULT_DIALOG_STYLE ),
mWndMotionGraphView(NULL)
{
	Character* ch = gApp->getCurrentCharacter();

	// view controls
	mChkUseTimeProp = new wxCheckBox( this, ID_chkUseTimeProp, "Use Time Proportions", wxPoint( 20, 10 ), wxSize( 120, 20 ) );
	mChkShowTransInt = new wxCheckBox( this, ID_chkShowTransInt, "Show Transition Interval", wxPoint( 160, 10 ), wxSize( 120, 20 ) );

	// TODO: zoom controls?
	// TODO: sorting/filtering controls?

	// create view
	mWndMotionGraphViewScroll = new wxScrolledWindow( this, ID_wndMotionGraphViewScroll, wxPoint( 10, 40 ), wxSize( 800, 600 ),
		wxHSCROLL|wxVSCROLL|wxBORDER_SIMPLE	);
	mWndMotionGraphView = new MotionGraphView( mWndMotionGraphViewScroll, ID_wndMotionGraphView, mgStyle );
	mWndMotionGraphViewScroll->SetScrollbars( 1, 1, mWndMotionGraphView->GetSize().GetWidth(), mWndMotionGraphView->GetSize().GetHeight() );

	// TODO: solve sizing issues

	// create OK button
	new wxButton( this, wxID_OK, _("&OK"),  wxPoint( 820/2 - 60, 720 - 60 - 10 ), wxSize( 75, 30 ) );
}

void MotionGraphViewDialog::OnCheckBox_UseTimeProp( wxCommandEvent& evt )
{
	mWndMotionGraphViewScroll->Scroll( 0, 0 );
	mWndMotionGraphView->setUseTimeProp( evt.IsChecked() );
	mWndMotionGraphViewScroll->SetScrollbars( 1, 1, mWndMotionGraphView->GetSize().GetWidth(), mWndMotionGraphView->GetSize().GetHeight() );
}

void MotionGraphViewDialog::OnCheckBox_ShowTransInt( wxCommandEvent& evt )
{
	mWndMotionGraphView->setShowTransitionInterval( evt.IsChecked() );
}

BEGIN_EVENT_TABLE( MotionGraphViewDialog, wxDialog )
	EVT_CHECKBOX( ID_chkUseTimeProp, MotionGraphViewDialog::OnCheckBox_UseTimeProp )
	EVT_CHECKBOX( ID_chkShowTransInt, MotionGraphViewDialog::OnCheckBox_ShowTransInt )
END_EVENT_TABLE()
