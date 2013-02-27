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

#include "MatchGraphViewDialog.h"
#include "AnimationStudioApp.h"

MatchGraphViewDialog::MatchGraphViewDialog( wxWindow* parent, wxWindowID id, MatchGraph* matches )
: wxDialog( parent, id, "View Matches", wxDefaultPosition, wxSize( 820, 690 ), wxDEFAULT_DIALOG_STYLE ),
mWndMatchGraphView(NULL)
{
	// TODO: zoom controls?
	// TODO: sorting/filtering controls?

	// create view
	mWndMatchGraphViewScroll = new wxScrolledWindow( this, ID_wndMatchGraphViewScroll, wxPoint( 10, 10 ), wxSize( 800, 600 ),
		wxHSCROLL|wxVSCROLL|wxBORDER_SIMPLE	);
	mWndMatchGraphView = new MatchGraphView( mWndMatchGraphViewScroll, ID_wndMatchGraphView, matches );
	mWndMatchGraphViewScroll->SetScrollbars( 1, 1, mWndMatchGraphView->GetSize().GetWidth(), mWndMatchGraphView->GetSize().GetHeight() );

	// create OK button
	new wxButton( this, wxID_OK, _("&OK"),  wxPoint( 820/2 - 60, 690 - 60 - 10 ), wxSize( 75, 30 ) );
}

int MatchGraphViewDialog::ShowModal()
{
	int res = wxDialog::ShowModal();
	mWndMatchGraphView->Close();

	return res;
}

BEGIN_EVENT_TABLE( MatchGraphViewDialog, wxDialog )
END_EVENT_TABLE()
