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

#include "wxImagePanel.h"

wxImagePanel::wxImagePanel( wxWindow* parent, const wxString& path ) :
wxPanel( parent ), mScale(1)
{
	wxImage img;
	if( !img.LoadFile(path) )
	{
		wxMessageBox( "Failed to load image file " + path + ".", "Error" );
	}

	mImg = wxBitmap(img);

	this->SetSize( mImg.GetWidth(), mImg.GetHeight() );
}

wxImagePanel::wxImagePanel( wxWindow* parent, unsigned char* data, int width, int height ) :
wxPanel( parent )
{
	zhAssert( data != NULL );
	zhAssert( width > 0 && height > 0 );

	wxImage img( width, height, data, false );
	mImg = wxBitmap(img);

	this->SetSize( mImg.GetWidth() * mScale, mImg.GetHeight() * mScale );
}

void wxImagePanel::OnPaint( wxPaintEvent& evt )
{
    wxPaintDC dc(this);
	dc.SetUserScale( mScale, mScale );
	dc.DrawBitmap( mImg, 0, 0 );
}

BEGIN_EVENT_TABLE( wxImagePanel, wxPanel )
	EVT_PAINT( wxImagePanel::OnPaint )
END_EVENT_TABLE()
