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

#include "EditSkeletonWindow.h"
#include "AnimationStudioApp.h"
#include "AnimationStudioFrame.h"
#include "OgreWindow.h"

EditSkeletonWindow::EditSkeletonWindow( wxWindow *parent, wxWindowID id )
: wxWindow( parent, id, wxDefaultPosition, wxDefaultSize )
{
	mSlHeight = new wxSlider( this, ID_slHeight, 50, 1, 100,
		wxPoint(5,20), wxSize(162,25) );
	mSlTrunkSize = new wxSlider( this, ID_slTrunkSize, 50, 1, 100,
		wxPoint(5,70), wxSize(162,25) );
	mSlArmLength = new wxSlider( this, ID_slArmLength, 50, 1, 100,
		wxPoint(5,120), wxSize(162,25) );
	mSlLegLength = new wxSlider( this, ID_slLegLength, 50, 1, 100,
		wxPoint(5,170), wxSize(162,25) );

	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add( new wxStaticText( this, -1, "Body Size", wxPoint(5,5), wxSize(162,16) ),
		1, wxEXPAND | wxALL, 5 );
	sizer->Add(mSlHeight);
	sizer->Add( new wxStaticText( this, -1, "Trunk Size", wxPoint(5,55), wxSize(162,16) ),
		1, wxEXPAND | wxALL, 5 );
	sizer->Add(mSlTrunkSize);
	sizer->Add( new wxStaticText( this, -1, "Arm Length", wxPoint(5,105), wxSize(162,16) ),
		1, wxEXPAND | wxALL, 5 );
	sizer->Add(mSlArmLength);
	sizer->Add( new wxStaticText( this, -1, "Leg Length", wxPoint(5,155), wxSize(162,16) ),
		1, wxEXPAND | wxALL, 5 );
	sizer->Add(mSlLegLength);
	SetSizer(sizer);
	sizer->SetSizeHints(this);
}

void EditSkeletonWindow::refresh()
{
}

void EditSkeletonWindow::OnSlider_Height( wxScrollEvent& evt )
{
	float scale = _computeScaleValue(evt.GetInt());
	gApp->getParamSkeletonEditor()->scaleBody(scale);
}

void EditSkeletonWindow::OnSlider_TrunkSize( wxScrollEvent& evt )
{
	float scale = _computeScaleValue(evt.GetInt());
	gApp->getParamSkeletonEditor()->scaleTrunk(scale);
}

void EditSkeletonWindow::OnSlider_ArmLength( wxScrollEvent& evt )
{
	float scale = _computeScaleValue(evt.GetInt());
	gApp->getParamSkeletonEditor()->scaleArms(scale);
}

void EditSkeletonWindow::OnSlider_LegLength( wxScrollEvent& evt )
{
	float scale = _computeScaleValue(evt.GetInt());
	gApp->getParamSkeletonEditor()->scaleLegs(scale);
}

float EditSkeletonWindow::_computeScaleValue( int sliderValue ) const
{
	float scale = 1;
	float t = ((float)sliderValue)/100;
	if( t < 0.5f )
	{
		t = 2*t;
		scale = 0.1f + 0.9f*t;
	}
	else
	{
		t = 2*(t-0.5f);
		scale = 1.f + 2.f*t;
	}

	return scale;
}

BEGIN_EVENT_TABLE( EditSkeletonWindow, wxWindow )
	EVT_COMMAND_SCROLL( ID_slHeight, EditSkeletonWindow::OnSlider_Height )
	EVT_COMMAND_SCROLL( ID_slTrunkSize, EditSkeletonWindow::OnSlider_TrunkSize )
	EVT_COMMAND_SCROLL( ID_slArmLength, EditSkeletonWindow::OnSlider_ArmLength )
	EVT_COMMAND_SCROLL( ID_slLegLength, EditSkeletonWindow::OnSlider_LegLength )
END_EVENT_TABLE()
