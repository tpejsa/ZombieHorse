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

#include "NewResourceDialog.h"
#include "AnimationStudioApp.h"

NewResourceDialog::NewResourceDialog( wxWindow* parent, wxWindowID id, ResourceType resType )
								   : wxDialog( parent, id, "Create New Resource", wxDefaultPosition, wxSize( 240, 500 ), wxDEFAULT_DIALOG_STYLE )
{
	std::string name_str = "";
	wxArrayString choices;
	zh::ResourceManager* res_mgr = NULL;

	if( resType == ResourceType_AnimSet )
	{
		res_mgr = zhAnimationSystem->getAnimationManager();
		name_str = "AS_" + zhAnimationSystem->getOutputSkeleton()->getName() + toString<unsigned int>( res_mgr->getNumResources() );
	}
	else // if( resType == ResourceType_AnimIndex )
	{
		res_mgr = zhAnimationSearchSystem->getAnimationIndexManager();
		name_str = "AI_" + zhAnimationSystem->getOutputSkeleton()->getName() + toString<unsigned int>( res_mgr->getNumResources() );
	}

	// get res. directories for this res. type
	for( unsigned int rdi = 0; rdi < res_mgr->getNumResourceDirectories(); ++rdi )
		choices.push_back( res_mgr->getResourceDirectory(rdi) );

	new wxStaticText( this, wxID_ANY, "Name:",
		wxPoint( 20, 20 ), wxSize( 60, 20 ) );
	mTxtName = new wxTextCtrl( this, wxID_ANY, "",
		wxPoint( 80, 20 ), wxSize( 120, 20 ) );
	mTxtName->SetValue(name_str);

	new wxStaticText( this, wxID_ANY, "Location:",
		wxPoint( 20, 60 ), wxSize( 100, 20 ) );
	mLbDir = new wxListBox( this, wxID_ANY,
		wxPoint( 20, 80 ), wxSize( 200, 320 ),
		choices, wxLB_SINGLE|wxLB_HSCROLL|wxLB_NEEDED_SB );

	// create OK button
	new wxButton( this, wxID_OK, _("&OK"),  wxPoint( 240/2 - 60, 500 - 60 - 10 ), wxSize( 75, 30 ) );

	// create Cancel button
	new wxButton( this, wxID_CANCEL, _("&Cancel"),  wxPoint( 240/2 + 40, 500 - 60 - 10 ), wxSize( 75, 30 ) );
}

BEGIN_EVENT_TABLE( NewResourceDialog, wxDialog )
END_EVENT_TABLE()
