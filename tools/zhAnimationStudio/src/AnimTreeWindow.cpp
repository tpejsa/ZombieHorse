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

#include "AnimTreeWindow.h"
#include "AnimationStudioApp.h"
#include "AnimationStudioFrame.h"
#include "connex.h"
#include "connexspace.h"
#include "AnimTreeConnexSpace.h"
#include <wx/treectrl.h>

AnimTreeWindow::AnimTreeWindow( wxWindow* parent, wxWindowID id )
: wxWindow( parent, id, wxDefaultPosition, wxDefaultSize ), mActive(false)
{
	mWndAnimTreeConnexSpaceScroll = new wxScrolledWindow( this, ID_wndAnimTreeConnexSpaceScroll, wxPoint( -1,-1 ), parent->GetSize(), wxHSCROLL|wxVSCROLL|wxBORDER_SIMPLE	);
}

void AnimTreeWindow::initEditor()
{
	if( isEditorActive() )
		deinitEditor();
	
	connex_space = new AnimTreeConnexSpace( mWndAnimTreeConnexSpaceScroll, ID_connexWindow, wxPoint(-1,-1), mWndAnimTreeConnexSpaceScroll->GetSize(), 0 );
	mWndAnimTreeConnexSpaceScroll->SetScrollbars( 1, 1, connex_space->GetSize().x, connex_space->GetSize().y );
	connex_space->SetBackgroundColour( wxColor(128,128,128,255) );

	AnimationTreeInstance* anim_tree = gApp->getCurrentAnimationTree();
	AnimationTreePtr anim_tree_ptr = anim_tree->getAnimationTree();
	zhAssert( anim_tree != NULL );
	mActive = true;

	//show anim. tree in editor
	AnimationTree::NodeIterator it = anim_tree->getNodeIterator();
	while( !it.end() )
	{
		AnimationNode* node = it.next();
		connex_space->createAnimationNode( node->getClassName(), node->getId(), node->getName() );
	}
	connex_space->mNodeGrid.clear();
	connex_space->mJCGrid.clear();
	connex_space->enumNodes( 0, anim_tree->getRoot() );
	
	if ( anim_tree->getNumBoneControllers() > 0 )
	{
		AnimationTree::BoneControllerIterator jc_it = anim_tree->getBoneControllerIterator();
		while( !jc_it.end() )
		{
			BoneController* jc = jc_it.next();
			connex_space->createAnimationNode( jc->getClassName(), jc->getId(), jc->getName() );
		}
		connex_space->enumJC( 0, anim_tree->getFirst() );
	}
	
	connex_space->autoArrangeNodes( anim_tree );
	mWndAnimTreeConnexSpaceScroll->SetScrollbars( 1, 1, connex_space->GetSize().x, connex_space->GetSize().y );
}

void AnimTreeWindow::deinitEditor()
{
	// TODO: clear editor
	// serialize anim. tree

	connex_space->RemoveAllNodes();
	mActive = false;
}

bool AnimTreeWindow::isEditorActive() const
{
	return mActive;
}

bool AnimTreeWindow::Show( bool show )
{
	AnimationStudioFrame* frm_app = gApp->getAppFrame();
	if( frm_app != NULL )
	{
		frm_app->GetMenuBar()->Check( ID_mnViewProperties, show );
	}

	return wxWindow::Show(show);
}

void AnimTreeWindow::OnSize( wxSizeEvent& evt )
{
	mWndAnimTreeConnexSpaceScroll->SetSize ( GetSize() );
	if ( mActive == true )
	{
		mWndAnimTreeConnexSpaceScroll->SetScrollbars( 1, 1, connex_space->GetSize().x, connex_space->GetSize().y );
		connex_space->SetPosition(wxPoint(0,0));
	}
}

void AnimTreeWindow::OnMouseEvent( wxMouseEvent& evt )
{
	//wxConnexSpace::OnMouseEvent( evt );
	if ( mActive == true )
	{
		if ( evt.Dragging() )
			mWndAnimTreeConnexSpaceScroll->SetScrollbars( 1, 1, connex_space->GetSize().x, connex_space->GetSize().y );
	}
}

BEGIN_EVENT_TABLE( AnimTreeWindow, wxWindow )
	EVT_SIZE( AnimTreeWindow::OnSize )
	EVT_MOUSE_EVENTS( AnimTreeWindow::OnMouseEvent )
END_EVENT_TABLE()

