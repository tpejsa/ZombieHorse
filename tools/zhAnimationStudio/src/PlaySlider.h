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

#ifndef __PlaySlider_h__
#define __PlaySlider_h__

#include "AnimationStudio.h"

class PlaySlider : public wxWindow
{

public:

	/**
	* Constructor.
	*/
	PlaySlider( wxWindow *parent, wxWindowID id );

	/**
	* true if Select Mode is active, false otherwise.
	*/
	//bool getSelectMode() const;

	/**
	* Toggle Select Mode.
	*/
	//bool setSelectMode( bool selMode = true ) const;

	/**
	* true if an animation segment is selected,
	* false otherwise.
	*/
	//bool hasSelection() const;

	/**
	* Gets the currently selected animation segment.
	*/
	//AnimationSegment getSelection() const;

	/**
	* Cancel current selection.
	*/
	//void deselect();
	
	void OnPaint( wxPaintEvent& evt );

	DECLARE_EVENT_TABLE()

private:

};

#endif // __PlaySlider_h__
