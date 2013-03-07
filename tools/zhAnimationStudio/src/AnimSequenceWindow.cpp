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

#include "AnimSequenceWindow.h"

AnimSequenceWindow::AnimSequenceWindow( wxWindow* parent, wxWindowID id )
: wxWindow( parent, id, wxDefaultPosition, wxDefaultSize )
{
	mLbAnimSequence = new wxListBox( this, ID_lbAnimSequence,
		wxDefaultPosition, GetSize() );
	SetDropTarget(this);

	refresh();
}

void AnimSequenceWindow::getAnimSequence( std::vector<zh::Animation*>& anims ) const
{
	anims.clear();

	for( unsigned int anim_i = 0; anim_i < mLbAnimSequence->GetCount(); ++anim_i )
	{
		std::string anim_name = mLbAnimSequence->GetString(anim_i).To8BitData();
		if( zhAnimationSystem->hasAnimation(anim_name) )
			anims.push_back( zhAnimationSystem->getAnimation(anim_name) );
	}
}

void AnimSequenceWindow::clearAnimSequence()
{
	mLbAnimSequence->Clear();
}

void AnimSequenceWindow::refresh()
{
	for( unsigned int anim_i = 0; anim_i < mLbAnimSequence->GetCount(); ++anim_i )
	{
		std::string anim_name = mLbAnimSequence->GetString(anim_i).To8BitData();
		if( !zhAnimationSystem->hasAnimation(anim_name) )
			mLbAnimSequence->Delete(anim_i--);
	}
}

bool AnimSequenceWindow::OnDropText( wxCoord x, wxCoord y, const wxString& data )
{
	if( zhAnimationSystem->hasAnimation( data.To8BitData() ) )
	{
		mLbAnimSequence->Append(data);
		return true;
	}

	return false;
}

void AnimSequenceWindow::OnSize( wxSizeEvent& evt )
{
	mLbAnimSequence->SetSize( GetSize() );
}

BEGIN_EVENT_TABLE( AnimSequenceWindow, wxWindow )
	EVT_SIZE( AnimSequenceWindow::OnSize )
END_EVENT_TABLE()
