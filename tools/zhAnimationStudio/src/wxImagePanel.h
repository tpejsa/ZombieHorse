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

#ifndef __wxImagePanel_h__
#define __wxImagePanel_h__

#include "AnimationStudio.h"

/**
* @brief Image panel.
*/
class wxImagePanel : public wxPanel
{

public:
	
	/**
	* Constructor.
	*
	* @param parent Parent window.
	* @param path Image path.
	*/
	wxImagePanel( wxWindow* parent, const wxString& path );

	/**
	* Constructor.
	*
	* @param parent Parent window.
	* @param data Pointer to the array of color data.
	* @param width Image width.
	* @param height Image height.
	* @param scale Image scale.
	*/
	wxImagePanel( wxWindow* parent, unsigned char* data, int width, int height );

	/**
	* Gets the image bitmap.
	*/
	wxBitmap& getBitmap() { return mImg; }

	/**
	* Gets the image scale.
	*/
	double getScale() const { return mScale; }

	/**
	* Sets the image scale.
	*/
	void setScale( double scale ) { mScale = scale; this->SetSize( mImg.GetWidth() * mScale, mImg.GetHeight() * mScale ); }
    
	/**
	* Event handler for paint events.
	*/
    void OnPaint(wxPaintEvent & evt);

	DECLARE_EVENT_TABLE()

private:

	wxBitmap mImg;
	double mScale;

};

#endif // __wxImagePanel_h__
