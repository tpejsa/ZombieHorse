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

#ifndef __PropertiesWindow_h__
#define __PropertiesWindow_h__

#include "AnimationStudio.h"

enum
{
};

/**
* Properties viewing and editing window.
*/
class PropertiesWindow : public wxWindow
{
	// more info on property grids in wxWidgets at http://docs.wxwidgets.org/trunk/overview_propgrid.html

public:

	/**
	* Constructor.
	*/
	PropertiesWindow( wxWindow *parent, wxWindowID id );

	/**
	* Gets the current property grid.
	*/
	wxPropertyGrid* GetPropertyGrid() const;

	/**
	* Sets the current property grid.
	*/
	void SetPropertyGrid( wxPropertyGrid* grid );

	bool Show( bool show = true );

	void OnSize( wxSizeEvent& evt );

	DECLARE_EVENT_TABLE()

private:

	wxPropertyGrid* mPgProps;

};

#endif // __PropertiesWindow_h__
