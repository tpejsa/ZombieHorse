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

#include "BuildMotionGraphDialog.h"

BuildMotionGraphDialog::BuildMotionGraphDialog( wxWindow* parent, wxWindowID id )
								   : wxDialog( parent, id, "Build Animation Index", wxDefaultPosition, wxSize( 360, 250 ), wxDEFAULT_DIALOG_STYLE )
{
	new wxStaticText( this, wxID_ANY, "Label filter:",
		wxPoint( 20, 20 ), wxSize( 100, 20 ) );
	mTxtLabelFilter = new wxTextCtrl( this, wxID_ANY, "",
		wxPoint( 120, 20 ), wxSize( 180, 20 ) );

	new wxStaticBox( this, wxID_ANY, "Advanced settings",
		wxPoint( 10, 50 ), wxSize( 330, 60 ) );
	
	new wxStaticText( this, wxID_ANY, "Window length:",
		wxPoint( 20, 80 ), wxSize( 100, 20 ) );
	mTxtWndLength = new wxTextCtrl( this, wxID_ANY, "0.35",
		wxPoint( 120, 80 ), wxSize( 40, 20 ) );

	new wxStaticText( this, wxID_ANY, "Min. distance:",
		wxPoint( 180, 80 ), wxSize( 100, 20 ) );
	mTxtMinDist = new wxTextCtrl( this, wxID_ANY, "0.01",
		wxPoint( 280, 80 ), wxSize( 40, 20 ) );

	// create OK button
	new wxButton( this, wxID_OK, _("&OK"),  wxPoint( 360/2 - 60, 300 - 120 ), wxSize( 75, 30 ) );

	// create Cancel button
	new wxButton( this, wxID_CANCEL, _("&Cancel"),  wxPoint( 360/2 + 40, 300 - 120 ), wxSize( 75, 30 ) );
}

BEGIN_EVENT_TABLE( BuildMotionGraphDialog, wxDialog )
END_EVENT_TABLE()
