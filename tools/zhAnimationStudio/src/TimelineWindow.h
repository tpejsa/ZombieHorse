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

#ifndef __TimelineWindow_h__
#define __TimelineWindow_h__

#include "AnimationStudio.h"
#include "PlaySlider.h"
#include "AnimAnnotGrid.h"

/*enum
{
	ID_tbPlayer,
	ID_btnPlay,
	ID_btnStop,
	ID_stxPlayTime,
	ID_stxPlayLength,
	ID_btnSelect,
	ID_btnDeselect,
	ID_tbTransitions,
	ID_cbParams,
	ID_txtParamValue,
	ID_cbTargetAnims,
	ID_cbTargetParams,
	ID_txtTargetParamValue,
	ID_tbAnnots,
	ID_btnZoomIn,
	ID_btnZoomOut,
	ID_cbAnnots,
	ID_slPlaySlider,
	ID_grdAnnots
};*/

class TimelineWindow : public wxWindow
{

public:

	/**
	* Constructor.
	*/
	TimelineWindow( wxWindow *parent, wxWindowID id );

	/**
	* Gets the animation playback slider.
	*/
	PlaySlider* getPlaySlider() const { return mPlaySl; }

	/**
	* Gets the animation annotation grid.
	*/
	AnimAnnotGrid* getAnnotGrid() const { return mAnnotGrid; }

	/*void OnTool_Play( wxCommandEvent& evt );
	void OnTool_Stop( wxCommandEvent& evt );
	void OnTool_Select( wxCommandEvent& evt );
	void OnTool_Deselect( wxCommandEvent& evt );
	void OnComboBox_Params( wxCommandEvent& evt );
	void OnTextEnter_ParamValue( wxCommandEvent& evt );
	void OnComboBox_TargetAnims( wxCommandEvent& evt );
	void OnComboBox_TargetParams( wxCommandEvent& evt );
	void OnTextEnter_TargetParamValue( wxCommandEvent& evt );
	void OnTool_ZoomIn( wxCommandEvent& evt );
	void OnTool_ZoomOut( wxCommandEvent& evt );
	void OnComboBox_Annots( wxCommandEvent& evt );*/

	DECLARE_EVENT_TABLE()

private:

	PlaySlider* mPlaySl;
	AnimAnnotGrid* mAnnotGrid;

};

#endif // __TimelineWindow_h__
