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

#include "MatchGraphView.h"
#include "AnimationStudioApp.h"

#define zhMGView_HSpace 20
#define zhMGView_VSpace 100
#define zhMGView_NodeHSize 80
#define zhMGView_NodeVSize 30
#define zhMGView_FontSize 10

MatchGraphView::MatchGraphView( wxWindow* parent, wxWindowID id, MatchGraph* matches )
: wxWindow( parent, id, wxDefaultPosition, wxDefaultSize ), mMatches(matches), mSelMatch(NULL), mMaxCost(0), mZoom(100)
{
	zhAssert( matches != NULL && matches->getNumNodes() != 0 );

	Character* ch = gApp->getCurrentCharacter();
	Model* mdl = ch->getModelController()->getModel();

	// create temp. anim. set for search results
	unsigned long tas_id = 0;
	unsigned int tas_i = 0;
	std::string tas_name = "AS_Test";
	while( zhAnimationSystem->getAnimationManager()->hasResource(tas_id) ) ++tas_id;
	while( zhAnimationSystem->getAnimationManager()->hasResource(tas_name) ) tas_name = "AS_Test" + toString<unsigned int>(tas_i++);
	mTempAnimSet = AnimationSetPtr::DynamicCast<zh::Resource>( zhAnimationSystem->getAnimationManager()->createResource( tas_id, tas_name ) );

	// create temp. animations
	MatchGraph::NodeConstIterator node_i = mMatches->getNodeConstIterator();
	while( !node_i.end() )
	{
		MatchGraph::Node* node = node_i.next();
		const AnimationSegment& anim_seg = node->getAnimationSegment();
		zh::Animation* raw_anim = anim_seg.getAnimation();
	
		zh::Animation* anim = mTempAnimSet->createAnimation( node->getHandle(),
			"Match" + toString<unsigned short>( node->getHandle() ) );		
		zh::Animation::BoneTrackConstIterator bti = raw_anim->getBoneTrackConstIterator();
		while( !bti.end() )
		{
			BoneAnimationTrack* rbat = bti.next();
			BoneAnimationTrack* bat = anim->createBoneTrack( rbat->getBoneId() );
			
			// create initial key-frame
			zh::TransformKeyFrame* tkf = static_cast<zh::TransformKeyFrame*>( bat->createKeyFrame(0) );
			rbat->getInterpolatedKeyFrame( anim_seg.getStartTime(), tkf );

			// create final key-frame
			tkf = static_cast<zh::TransformKeyFrame*>( bat->createKeyFrame( anim_seg.getEndTime() - anim_seg.getStartTime() ) );
			rbat->getInterpolatedKeyFrame( anim_seg.getEndTime(), tkf );

			// copy intervening key-frames
			for( unsigned int kfi = 0; kfi < rbat->getNumKeyFrames(); ++kfi )
			{
				zh::TransformKeyFrame* rtkf = static_cast<zh::TransformKeyFrame*>( rbat->getKeyFrame(kfi) );
				
				if( rtkf->getTime() <= anim_seg.getStartTime() )
					continue;
				if( rtkf->getTime() >= anim_seg.getEndTime() )
					break;

				tkf = static_cast<zh::TransformKeyFrame*>( bat->createKeyFrame( rtkf->getTime() - anim_seg.getStartTime() ) );
				tkf->setTranslation( rtkf->getTranslation() );
				tkf->setRotation( rtkf->getRotation() );
				tkf->setScale( rtkf->getScale() );
			}
		}
	}

	// add corresponding nodes to the internal animation tree
	gApp->stopAnimation();
	ch->getAnimationController()->removeAnimationTree(zhIntAnimTree_Name);
	AnimationTreePtr atree_int = AnimationTreePtr::DynamicCast<zh::Resource>( zhAnimationSystem->getAnimationTreeManager()->getResource(zhIntAnimTree_Name) );
	gApp->_addAnimsToAnimTree( mTempAnimSet, atree_int, atree_int->getRoot() );
	atree_int->setApplyMover(false);
	ch->getAnimationController()->addAnimationTree(atree_int);
	gApp->selectAnimationTree(zhIntAnimTree_Name);

	_initView();
}

unsigned int MatchGraphView::getZoom() const
{
	return mZoom;
}

void MatchGraphView::setZoom( unsigned int zoom )
{
	mZoom = zoom;
}

void MatchGraphView::redraw()
{
	wxClientDC dc(this);
	render(dc);
}
	
void MatchGraphView::render( wxDC& dc )
{
	Character* ch = gApp->getCurrentCharacter();

	// clear background
	dc.SetBackground( *wxWHITE_BRUSH );
	dc.Clear();

	dc.SetPen( wxPen( *wxBLACK, 1 ) );
	dc.SetBrush(*wxWHITE_BRUSH);
	dc.SetTextForeground(*wxBLACK);
	dc.SetFont( wxFont( zhMGView_FontSize, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL ) );

	std::string label = "";
	for( unsigned int lvl = 0; lvl < mNodeLOffs.size(); ++lvl )
	{
		for( unsigned int node_i = 0; node_i < mNodeLOffs[lvl].size(); ++node_i )
		{
			MatchGraph::Node* node = mNodeLOffs[lvl][node_i].first;
			unsigned int loff = mNodeLOffs[lvl][node_i].second;

			// find parent
			MatchGraph::Node* prev = NULL;
			unsigned int prev_loff = 0;
			unsigned int prev_lvl = lvl - 1;
			if( lvl >= 1 )
			{
				for( unsigned int prev_i = 0; prev_i < mNodeLOffs[prev_lvl].size(); ++prev_i )
				{
					prev = mNodeLOffs[prev_lvl][prev_i].first;
					prev_loff = mNodeLOffs[prev_lvl][prev_i].second;

					if( prev->getNext( node->getHandle() ) != NULL )
						break;
				}
			}

			unsigned int x = zhMGView_HSpace * ( loff + 1 ) + zhMGView_NodeHSize * loff;
			unsigned int y = zhMGView_VSpace * ( lvl + 1 ) + zhMGView_NodeVSize * lvl - zhMGView_VSpace/2;
			std::string label = node->getHandle() == 0 ? "Q" : "R" + toString<unsigned short>( node->getHandle() );

			// set node color (derived from *cost*)
			float cost = mNodeCosts[node];
			float ct = cost/mMaxCost;
			if( node == mSelMatch )
			{
				// give node thick outline and green color if selected
				dc.SetPen( wxPen( *wxBLACK, 3 ) );
				dc.SetBrush( wxBrush( wxColour( 127, 255, 127 ) ) );
			}
			else
			{
				// give node normal color
				dc.SetBrush( wxBrush( wxColour( 127 + 128*( 1.f - ct ), 127, 127 + 128*ct ) ) );
			}

			// draw node
			wxRect node_bb( x, y, zhMGView_NodeHSize, zhMGView_NodeVSize );
			dc.DrawRoundedRectangle( node_bb, 7 );
			dc.DrawText( label, x + 5, y + 5 );

			// add node bounding box
			mNodeRect[node] = node_bb;

			if( node == mSelMatch )
				dc.SetPen( wxPen( *wxBLACK, 1 ) );

			if( lvl >= 1 )
			{
				unsigned int prev_x = zhMGView_HSpace * ( prev_loff + 1 ) + zhMGView_NodeHSize * prev_loff;
				unsigned int prev_y = zhMGView_VSpace * ( prev_lvl + 1 ) + zhMGView_NodeVSize * prev_lvl - zhMGView_VSpace/2;

				// draw link
				unsigned int lx1 = prev_x + zhMGView_NodeHSize/2,
					ly1 = prev_y + zhMGView_NodeVSize,
					lx2 = x + zhMGView_NodeHSize/2,
					ly2 = y;
				dc.DrawLine( lx1, ly1, lx2, ly2 );
				label = toString<float>( mMatches->getEdge( prev->getHandle(), node->getHandle() )->getCost() );
				//if( node == mSelMatch )
				dc.DrawText( label, ( lx1 + lx2 )/2 + 5, ( ly1 + ly2 )/2 );
			}
		}
	}
}

void MatchGraphView::OnLeftDown( wxMouseEvent& evt )
{
}

void MatchGraphView::OnLeftUp( wxMouseEvent& evt )
{
	// if clicked on node label, select that node
	for( std::map<MatchGraph::Node*, wxRect>::const_iterator rect_i = mNodeRect.begin();
		rect_i != mNodeRect.end(); ++rect_i )
	{
		MatchGraph::Node* node = rect_i->first;

		if( rect_i->second.Contains( evt.GetPosition() ) )
		{
			mSelMatch = rect_i->first;

			// play selected animation
			gApp->selectAnimation( mTempAnimSet->getName(),
				"Match" + toString<unsigned short>( mSelMatch->getHandle() ) );

			redraw();
			return;
		}
	}

	if( mSelMatch != NULL )
	{
		// clicked on nothing, deselect
		mSelMatch = NULL;

		// stop anim. playback
		gApp->stopAnimation();

		redraw();
	}
}

void MatchGraphView::OnKeyDown( wxKeyEvent& evt )
{
	if( evt.GetKeyCode() == WXK_DELETE && mSelMatch != NULL )
	{
		// delete selected node
		mMatches->deleteNode( mSelMatch->getHandle() );
		mSelMatch = NULL;
		_initView();
		redraw();
	}
}

void MatchGraphView::OnPaint( wxPaintEvent& evt )
{
	wxPaintDC dc(this);
	render(dc);
}

void MatchGraphView::OnClose( wxCloseEvent& evt )
{
	Character* ch = gApp->getCurrentCharacter();
	Model* mdl = ch->getModelController()->getModel();

	// remove nodes from internal anim. tree
	gApp->stopAnimation();
	ch->getAnimationController()->removeAnimationTree(zhIntAnimTree_Name);
	AnimationTreePtr atree_int = AnimationTreePtr::DynamicCast<zh::Resource>( zhAnimationSystem->getAnimationTreeManager()->getResource(zhIntAnimTree_Name) );
	gApp->_removeAnimsFromAnimTree( mTempAnimSet, atree_int );
	ch->getAnimationController()->addAnimationTree(atree_int);
	gApp->selectAnimationTree(zhIntAnimTree_Name);

	// delete temp. anim. set
	zhAnimationSystem->getAnimationManager()->deleteResource( mTempAnimSet->getId() );
}

void MatchGraphView::_initView()
{
	mNodeCosts.clear();
	mMaxCost = 0;
	mNodeLOffs.clear();
	unsigned int num_leaves = _computeNodeProps( mMatches->getRoot() );

	// set view size
	SetSize( zhMGView_HSpace * ( num_leaves + 2 ) + zhMGView_NodeHSize * num_leaves,
		zhMGView_VSpace * ( mNodeLOffs.size() + 2 ) + zhMGView_NodeVSize * mNodeLOffs.size() );
}

unsigned int MatchGraphView::_computeNodeProps( MatchGraph::Node* node, unsigned int lvl, float cost, unsigned int lOff )
{
	if( mNodeCosts.count(node) > 0 )
		// stop on loop
		return 1;

	// add cost for this node
	mNodeCosts[node] = cost;
	if( cost > mMaxCost )
		mMaxCost = cost;

	// add left offset for this node
	if( lvl >= mNodeLOffs.size() )
		mNodeLOffs.push_back( std::vector< std::pair<MatchGraph::Node*, unsigned int> >() );
	mNodeLOffs[lvl].push_back( make_pair( node, lOff ) );

	// compute left offset for each child
	unsigned int num_leaves = 0;
	MatchGraph::NodeConstIterator next_i = node->getNextConstIterator();
	while( !next_i.end() )
	{
		MatchGraph::Node* next = next_i.next();
		num_leaves += _computeNodeProps( next, lvl+1,
			cost + mMatches->getEdge( node->getHandle(), next->getHandle() )->getCost(),
			lOff + num_leaves );
	}

	if( num_leaves <= 0 )
		// this node is a leaf
		num_leaves = 1;

	return num_leaves;
}

BEGIN_EVENT_TABLE( MatchGraphView, wxWindow )
	EVT_LEFT_DOWN(MatchGraphView::OnLeftDown)
	EVT_LEFT_UP(MatchGraphView::OnLeftUp)
	EVT_KEY_DOWN(MatchGraphView::OnKeyDown)
	EVT_PAINT(MatchGraphView::OnPaint)
	EVT_CLOSE(MatchGraphView::OnClose)
END_EVENT_TABLE()
