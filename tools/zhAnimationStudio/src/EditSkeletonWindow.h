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

#ifndef _EditSkeletonWindow_h__
#define _EditSkeletonWindow_h__

#include "AnimationStudio.h"

enum
{
	ID_slHeight,
	ID_slTrunkSize,
	ID_slArmLength,
	ID_slLegLength
};

class EditSkeletonWindow : public wxWindow
{

public:

	EditSkeletonWindow( wxWindow *parent, wxWindowID id );

	/**
	* Updates the edit skeleton window contents.
	*/
	void refresh();

	void OnSlider_Height( wxScrollEvent& evt );
	void OnSlider_TrunkSize( wxScrollEvent& evt );
	void OnSlider_ArmLength( wxScrollEvent& evt );
	void OnSlider_LegLength( wxScrollEvent& evt );

	DECLARE_EVENT_TABLE()

private:

	float _computeScaleValue( int sliderValue ) const;

	std::map<std::string, zh::Vector3> mBonePosStored;
	wxSlider *mSlHeight, *mSlTrunkSize, *mSlArmLength, *mSlLegLength;
};

#endif // _EditSkeletonWindow_h__
