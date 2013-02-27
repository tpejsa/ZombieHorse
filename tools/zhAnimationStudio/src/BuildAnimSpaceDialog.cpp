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

#include "BuildAnimSpaceDialog.h"

BuildAnimSpaceDialog::BuildAnimSpaceDialog( wxWindow* parent, wxWindowID id )
								   : wxDialog( parent, id, "Build Animation Space", wxDefaultPosition, wxSize( 360, 240 ), wxDEFAULT_DIALOG_STYLE )
{
	new wxStaticText( this, wxID_ANY, "Animation space name:",
		wxPoint( 20, 20 ), wxSize( 120, 20 ) );
	mTxtName = new wxTextCtrl( this, wxID_ANY, "NewAnimSpace",
		wxPoint( 140, 20 ), wxSize( 120, 20 ) );

	new wxStaticBox( this, wxID_ANY, "Advanced settings",
		wxPoint( 10, 50 ), wxSize( 330, 100 ) );

	mChkBuildBlendCurves = new wxCheckBox( this, wxID_ANY, "Build blend curves",
		wxPoint( 20, 80 ), wxSize( 120, 20 ) );
	mChkBuildBlendCurves->Set3StateValue( wxCHK_CHECKED );

	mChkMatchAnnots = new wxCheckBox( this, wxID_ANY, "Match annotations",
		wxPoint( 180, 80 ), wxSize( 120, 20 ) );
	mChkMatchAnnots->Set3StateValue( wxCHK_CHECKED );

	new wxStaticText( this, wxID_ANY, "Blend curve knot spacing:",
		wxPoint( 20, 120 ), wxSize( 140, 20 ) );
	mTxtKnotSpacing = new wxTextCtrl( this, wxID_ANY, "3",
		wxPoint( 160, 120 ), wxSize( 40, 20 ) );

	// create OK button
	new wxButton( this, wxID_OK, _("&OK"),  wxPoint( 360/2 - 60, 240 - 60 - 10 ), wxSize( 75, 30 ) );

	// create Cancel button
	new wxButton( this, wxID_CANCEL, _("&Cancel"),  wxPoint( 360/2 + 40, 240 - 60 - 10 ), wxSize( 75, 30 ) );
}

BEGIN_EVENT_TABLE( BuildAnimSpaceDialog, wxDialog )
END_EVENT_TABLE()
