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

#include "MotionVisualizationWindow.h"
#include "AnimationStudioApp.h"
#include "AnimationStudioFrame.h"

MotionVisualizationWindow::MotionVisualizationWindow( wxWindow* parent, wxWindowID id )
: wxWindow( parent, id, wxDefaultPosition, wxDefaultSize )
{
	new wxStaticText( this, -1, "Joint Markers:", wxPoint(10,5), wxSize(80,20) );
	mClbJointMarkers = new wxCheckListBox( this, ID_clbJointMarkers,
		wxPoint(10,25), wxSize( GetSize().GetX()-10, 300 ), 0, 0, wxLB_SINGLE );
	new wxStaticText( this, -1, "Joint Traces:", wxPoint(10,335), wxSize(80,20) );
	mClbJointTraces = new wxCheckListBox( this, ID_clbJointTraces,
		wxPoint(10,355), wxSize( GetSize().GetX()-10, 300 ), 0, 0, wxLB_SINGLE );

	refresh();
}

void MotionVisualizationWindow::refresh()
{
	zh::Skeleton* skel = zhAnimationSystem->getOutputSkeleton();
	mClbJointMarkers->Clear();
	mClbJointTraces->Clear();
	if( skel == NULL )
	{
		// No skeleton currently selected
		return;
	}

	// Add joint markers to the checklist
	zh::Skeleton::BoneConstIterator bone_i = skel->getBoneConstIterator();
	while( bone_i.hasMore() )
	{
		zh::Bone* bone = bone_i.next();
		mClbJointMarkers->Append( bone->getName() );
		mClbJointMarkers->Check(
			mClbJointMarkers->GetCount()-1,
			gApp->hasJointMarker( bone->getName() )
			);
	}

	// Add joint traces to the checklist
	bone_i = skel->getBoneConstIterator();
	while( bone_i.hasMore() )
	{
		zh::Bone* bone = bone_i.next();
		mClbJointTraces->Append( bone->getName() );
		mClbJointTraces->Check(
			mClbJointTraces->GetCount()-1,
			gApp->hasJointTrace( bone->getName() )
			);
	}
}

void MotionVisualizationWindow::OnCheckListBox_JointMarkers( wxCommandEvent& evt )
{
	if( evt.IsSelection() )
		return;

	for( unsigned int bone_i = 0; bone_i < mClbJointMarkers->GetCount(); ++bone_i )
	{
		std::string bone_name = mClbJointMarkers->GetString(bone_i).To8BitData();
		bool checked = mClbJointMarkers->IsChecked(bone_i);
		bool has_marker = gApp->hasJointMarker(bone_name);
		
		if( checked != has_marker )
			gApp->displayJointMarker( bone_name, checked );
	}
}

void MotionVisualizationWindow::OnCheckListBox_JointTraces( wxCommandEvent& evt )
{
	if( evt.IsSelection() )
		return;

	for( unsigned int bone_i = 0; bone_i < mClbJointTraces->GetCount(); ++bone_i )
	{
		std::string bone_name = mClbJointTraces->GetString(bone_i).To8BitData();
		bool checked = mClbJointTraces->IsChecked(bone_i);
		bool has_trace = gApp->hasJointTrace(bone_name);
		
		if( checked != has_trace )
			gApp->traceJointPath( bone_name, checked );
	}
}

void MotionVisualizationWindow::OnSize( wxSizeEvent& evt )
{
	mClbJointMarkers->SetSize( GetSize().GetX()-10, 300 );
	mClbJointTraces->SetSize( GetSize().GetX()-10, 300 );
}

BEGIN_EVENT_TABLE( MotionVisualizationWindow, wxWindow )
	EVT_CHECKLISTBOX( ID_clbJointMarkers, MotionVisualizationWindow::OnCheckListBox_JointMarkers )
	EVT_CHECKLISTBOX( ID_clbJointTraces, MotionVisualizationWindow::OnCheckListBox_JointTraces )
	EVT_SIZE( MotionVisualizationWindow::OnSize )
END_EVENT_TABLE()
