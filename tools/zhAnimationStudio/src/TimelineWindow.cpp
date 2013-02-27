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

#include "TimelineWindow.h"
#include "AnimationStudioApp.h"

TimelineWindow::TimelineWindow( wxWindow* parent, wxWindowID id )
: wxWindow( parent, id, wxDefaultPosition, wxDefaultSize, wxVSCROLL|wxHSCROLL ),
mPlaySl(NULL), mAnnotGrid(NULL)
{
}

/*void TimelineWindow::OnTool_Play( wxCommandEvent& evt )
{
}

void TimelineWindow::OnTool_Stop( wxCommandEvent& evt )
{
}

void TimelineWindow::OnTool_Select( wxCommandEvent& evt )
{
}

void TimelineWindow::OnTool_Deselect( wxCommandEvent& evt )
{
}

void TimelineWindow::OnComboBox_Params( wxCommandEvent& evt )
{
}

void TimelineWindow::OnTextEnter_ParamValue( wxCommandEvent& evt )
{
}

void TimelineWindow::OnComboBox_TargetAnims( wxCommandEvent& evt )
{
}

void TimelineWindow::OnComboBox_TargetParams( wxCommandEvent& evt )
{
}

void TimelineWindow::OnTextEnter_TargetParamValue( wxCommandEvent& evt )
{
}

void TimelineWindow::OnTool_ZoomIn( wxCommandEvent& evt )
{
}

void TimelineWindow::OnTool_ZoomOut( wxCommandEvent& evt )
{
}

void TimelineWindow::OnComboBox_Annots( wxCommandEvent& evt )
{
}*/

/*void TimelineWindow::OnPaint( wxPaintEvent& evt )
{
	wxPaintDC dc(this);
}*/

BEGIN_EVENT_TABLE( TimelineWindow, wxWindow )
	//EVT_PAINT( TimelineWindow::OnPaint )
END_EVENT_TABLE()
