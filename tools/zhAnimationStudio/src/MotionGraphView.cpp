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

#include "MotionGraphView.h"
#include "AnimationStudioApp.h"

#define zhMGView_HSpace 20
#define zhMGView_VSpace 75
#define zhMGView_FontSize 10
#define zhMGView_HLabelSize 100
#define zhMGView_AnimLineLen 800
#define zhMGView_AnimLineLenCoef 200

MotionGraphView::MotionGraphView( wxWindow* parent, wxWindowID id, MGViewStyle mgStyle )
: wxWindow( parent, id, wxDefaultPosition, wxDefaultSize ),
mMGStyle(mgStyle), mZoom(100), mUseTimeProp(false), mShowTransInt(false)
{
	_initView();
}

unsigned int MotionGraphView::getZoom() const
{
	return mZoom;
}

void MotionGraphView::setZoom( unsigned int zoom )
{
	mZoom = zoom;
}

bool MotionGraphView::getUseTimeProp() const
{
	return mUseTimeProp;
}

void MotionGraphView::setUseTimeProp( bool use)
{
	mUseTimeProp = use;
	_updateSize();
	redraw();
}

bool MotionGraphView::getShowTransitionInterval() const
{
	return mShowTransInt;
}

void MotionGraphView::setShowTransitionInterval( bool show )
{
	mShowTransInt = show;
	redraw();
}

void MotionGraphView::redraw()
{
	wxClientDC dc(this);
	render(dc);
}
	
void MotionGraphView::render( wxDC& dc )
{
	Character* ch = gApp->getCurrentCharacter();

	dc.SetBackground( *wxWHITE_BRUSH );
	dc.Clear();

	if( mMGStyle == MGView_Raw )
	{
		int len = 0;

		// draw anim. clips
		for( std::map<zh::Animation*, unsigned int>::const_iterator anim_i = mAnimPos.begin();
			anim_i != mAnimPos.end(); ++anim_i )
		{
			zh::Animation* anim = anim_i->first;
			unsigned int pos = anim_i->second;

			// draw animation clip label
			if( mSelAnims.size() > 0 && mSelAnims.count(anim) > 0 )
				dc.SetTextForeground(*wxRED);
			else
				dc.SetTextForeground(*wxBLACK);
			dc.SetFont( wxFont( zhMGView_FontSize, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL ) );
			dc.DrawText( anim->getName(), zhMGView_HSpace, ( pos + 1 ) * zhMGView_VSpace - 5 );
			// draw animation clip line
			if( mSelAnims.size() > 0 && mSelAnims.count(anim) > 0 )
				dc.SetPen( wxPen( *wxRED, 2, wxSOLID ) );
			else
				dc.SetPen( wxPen( *wxBLACK, 2, wxSOLID ) );
			len = mUseTimeProp ? anim->getLength() * zhMGView_AnimLineLenCoef :
				zhMGView_AnimLineLen;
			dc.DrawLine( zhMGView_HSpace + zhMGView_HLabelSize, ( pos + 1 ) * zhMGView_VSpace,
				zhMGView_HSpace + zhMGView_HLabelSize + len, ( pos + 1 ) * zhMGView_VSpace );
		}

		// draw transitions
		for( std::map<zh::Animation*, unsigned int>::const_iterator anim_i = mAnimPos.begin();
			anim_i != mAnimPos.end(); ++anim_i )
		{
			zh::Animation* anim = anim_i->first;
			unsigned int pos = anim_i->second;

			if( mSelAnims.size() > 0 && mSelAnims.count(anim) <= 0 )
				// this anim. is not selected, so don't draw its transitions
				continue;

			TransitionAnnotationContainer* annots = anim->getTransitionAnnotations();
			for( unsigned int annot_i = 0; annot_i < annots->getNumAnnotations(); ++annot_i )
			{
				TransitionAnnotation* annot = static_cast<TransitionAnnotation*>( annots->getAnnotation(annot_i) );
				float stime = annot->getStartTime();
				float etime = annot->getEndTime();
				float tlen = annot->getEndTime() - annot->getStartTime();

				AnimationSetPtr tanim_set = AnimationSetPtr::DynamicCast<zh::Resource>(
					zhAnimationSystem->getAnimationManager()->getResource( annot->getTargetSetId() ) );
				zhAssert( gApp->hasRawAnimationSet( ch->getId(), tanim_set->getName() ) );
				zh::Animation* tanim = tanim_set->getAnimation( annot->getTargetId() );
				unsigned int tpos = mAnimPos.find(tanim)->second;
				float ttime = annot->getTargetTime();

				if( mSelAnims.size() > 1 && mSelAnims.count(tanim) <= 0 )
					// if more than 1 anim. is selected, then only show transitions between selected anims
					continue;

				int stpos = mUseTimeProp ? stime * zhMGView_AnimLineLenCoef :
					( stime / anim->getLength() ) * zhMGView_AnimLineLen;
				int etpos = mUseTimeProp ? etime * zhMGView_AnimLineLenCoef :
					( etime / anim->getLength() ) * zhMGView_AnimLineLen;
				int tstpos = mUseTimeProp ? ttime * zhMGView_AnimLineLenCoef :
					( ttime / tanim->getLength() ) * zhMGView_AnimLineLen;
				int tetpos = mUseTimeProp ? ( ttime + tlen ) * zhMGView_AnimLineLenCoef :
					( ( ttime + tlen ) / tanim->getLength() ) * zhMGView_AnimLineLen;

				// draw trans. line
				int x1 = zhMGView_HSpace + zhMGView_HLabelSize + stpos,
					y1 = ( pos + 1 ) * zhMGView_VSpace;
				int x2 = zhMGView_HSpace + zhMGView_HLabelSize + tetpos,
					y2 = ( tpos + 1 ) * zhMGView_VSpace;
				dc.SetPen( wxPen( *wxLIGHT_GREY, 1, wxSOLID ) );
				if( anim != tanim )
				{
					dc.DrawLine( x1, y1, x2, y2 );
				}
				else
				{
					float t = ((float)( x2 - x1 )) / len;
					int xc = ( x1 + x2 ) / 2,
						yc = ( 1.f - t ) * ( y1 + 5 ) + t * ( y1 + zhMGView_VSpace - 5 );
					dc.DrawSpline( x1, y1, xc, yc, x2, y2 );
				}

				// draw trans. proj. lines
				if( mShowTransInt )
				{
					dc.SetPen( wxPen( *wxBLUE, 1, wxSOLID ) );
					dc.DrawLine( zhMGView_HSpace + zhMGView_HLabelSize + stpos, ( pos + 1 ) * zhMGView_VSpace - 1,
						zhMGView_HSpace + zhMGView_HLabelSize + etpos, ( pos + 1 ) * zhMGView_VSpace - 1 );
					dc.DrawLine( zhMGView_HSpace + zhMGView_HLabelSize + tstpos, ( tpos + 1 ) * zhMGView_VSpace - 1,
						zhMGView_HSpace + zhMGView_HLabelSize + tetpos, ( tpos + 1 ) * zhMGView_VSpace - 1 );
				}

				// draw trans. points
				if( mSelAnims.size() > 0 && mSelAnims.count(anim) > 0 )
				{
					dc.SetPen( wxPen( *wxRED, 0, wxSOLID ) );
					dc.SetBrush( wxBrush( *wxRED ) );
				}
				else
				{
					dc.SetPen( wxPen( *wxBLACK, 0, wxSOLID ) );
					dc.SetBrush( wxBrush( *wxBLACK ) );
				}
				dc.DrawRectangle( zhMGView_HSpace + zhMGView_HLabelSize + stpos - 2, ( pos + 1 ) * zhMGView_VSpace - 2,
					4, 4 );
				dc.DrawRectangle( zhMGView_HSpace + zhMGView_HLabelSize + tetpos - 2, ( tpos + 1 ) * zhMGView_VSpace - 2,
					4, 4 );
			}
		}
	}
}

void MotionGraphView::OnLeftDown( wxMouseEvent& evt )
{
}

void MotionGraphView::OnLeftUp( wxMouseEvent& evt )
{
	// if clicked on anim. label, select that anim.
	for( std::map<zh::Animation*, wxRect>::const_iterator rect_i = mAnimRect.begin();
		rect_i != mAnimRect.end(); ++rect_i )
	{
		zh::Animation* anim = rect_i->first;

		if( rect_i->second.Contains( evt.GetPosition() ) )
		{
			if( mSelAnims.count(anim) > 0 )
				mSelAnims.erase(anim);
			else
				mSelAnims.insert(anim);
			redraw();
			return;
		}
	}

	mSelAnims.clear();
	redraw();
}

void MotionGraphView::OnPaint( wxPaintEvent& evt )
{
	wxPaintDC dc(this);
	render(dc);
}

void MotionGraphView::_initView()
{
	Character* ch = gApp->getCurrentCharacter();
	mMaxLen = 0;

	// add anim. clips to render list in default order
	if( mMGStyle == MGView_Raw )
	{
		unsigned int free_pos = 0; // next free pos.

		AnimationStudioApp::AnimationSetConstIterator rasi = gApp->getRawAnimationSetConstIterator( ch->getId() );
		while( !rasi.end() )
		{
			AnimationSetPtr anim_set = rasi.next();
			
			AnimationSet::AnimationConstIterator anim_i = anim_set->getAnimationConstIterator();
			while( !anim_i.end() )
			{
				zh::Animation* anim = anim_i.next();
				unsigned int pos = 0;
				if( mAnimPos.count(anim) <= 0 )
				{
					pos = free_pos++;
					mAnimPos[anim] = pos;
				}
				else
				{
					continue;
				}

				// add label rect.
				mAnimRect[anim] = wxRect( zhMGView_HSpace, ( pos + 1 ) * zhMGView_VSpace - 5,
					zhMGView_HLabelSize, zhMGView_FontSize );

				// update max anim. length
				if( anim->getLength() > mMaxLen )
					mMaxLen = anim->getLength();

				// add transition target clips
				TransitionAnnotationContainer* annots = anim->getTransitionAnnotations();
				for( unsigned int annot_i = 0; annot_i < annots->getNumAnnotations(); ++annot_i )
				{
					TransitionAnnotation* annot = static_cast<TransitionAnnotation*>( annots->getAnnotation(annot_i) );

					AnimationSetPtr tanim_set = AnimationSetPtr::DynamicCast<zh::Resource>(
						zhAnimationSystem->getAnimationManager()->getResource( annot->getTargetSetId() ) );
					zhAssert( gApp->hasRawAnimationSet( ch->getId(), tanim_set->getName() ) );
					zh::Animation* tanim = tanim_set->getAnimation( annot->getTargetId() );

					if( mAnimPos.count(tanim) <= 0 )
					{
						pos = free_pos++;
						mAnimPos[tanim] = pos;
					}
					else
					{
						continue;
					}

					// add label rect.
					mAnimRect[tanim] = wxRect( zhMGView_HSpace, ( pos + 1 ) * zhMGView_VSpace - 5,
						zhMGView_HLabelSize, zhMGView_FontSize );

					// update max anim. length
					if( tanim->getLength() > mMaxLen )
						mMaxLen = tanim->getLength();
				}
			}
		}
	}

	_updateSize();
}

void MotionGraphView::_updateSize()
{
	if( mUseTimeProp )
		SetSize( 2 * zhMGView_HSpace + zhMGView_HLabelSize + mMaxLen * zhMGView_AnimLineLenCoef, ( mAnimPos.size() + 1 ) * zhMGView_VSpace );
	else
		SetSize( 2 * zhMGView_HSpace + zhMGView_HLabelSize + zhMGView_AnimLineLen, ( mAnimPos.size() + 1 ) * zhMGView_VSpace );
}

BEGIN_EVENT_TABLE( MotionGraphView, wxWindow )
	EVT_LEFT_DOWN(MotionGraphView::OnLeftDown)
	EVT_LEFT_UP(MotionGraphView::OnLeftUp)
	EVT_PAINT(MotionGraphView::OnPaint)
END_EVENT_TABLE()
