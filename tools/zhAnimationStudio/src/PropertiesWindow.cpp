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

#include "PropertiesWindow.h"
#include "AnimationStudioApp.h"
#include "AnimationStudioFrame.h"

PropertiesWindow::PropertiesWindow( wxWindow* parent, wxWindowID id )
: wxWindow( parent, id, wxDefaultPosition, wxDefaultSize ), mPgProps(NULL)
{
	// subclass wxPropertyGrid for each object class that needs to expose properties
	// - e.g. AnimationSamplerPropGrid
	// instantiate it and use SetPropertyGrid() to display object properties
	// property validation and obj. update are to be implemented through wxEVT_PG_CHANGED handler
	// in your wxPropertyGrid subclass
}

wxPropertyGrid* PropertiesWindow::GetPropertyGrid() const
{
	return mPgProps;
}

void PropertiesWindow::SetPropertyGrid( wxPropertyGrid* grid )
{
	mPgProps = grid;

	if ( grid == NULL )
	{
		this->DestroyChildren();
		return;
	}

	mPgProps->SetParent(this);
	mPgProps->SetPosition( wxDefaultPosition );
	mPgProps->SetSize( GetSize() );
}

bool PropertiesWindow::Show( bool show )
{
	AnimationStudioFrame* frm_app = gApp->getAppFrame();
	if( frm_app != NULL )
	{
		frm_app->GetMenuBar()->Check( ID_mnViewProperties, show );
	}

	return wxWindow::Show(show);
}

void PropertiesWindow::OnSize( wxSizeEvent& evt )
{
	if( mPgProps != NULL )
		mPgProps->SetSize( GetSize() );
}

BEGIN_EVENT_TABLE( PropertiesWindow, wxWindow )
	EVT_SIZE( PropertiesWindow::OnSize )
END_EVENT_TABLE()
