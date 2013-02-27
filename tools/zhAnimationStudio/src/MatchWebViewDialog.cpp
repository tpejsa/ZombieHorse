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

#include "MatchWebViewDialog.h"
#ifdef _DEBUG
#include "zhAnimationDistanceGrid.h"
#endif

MatchWebViewDialog::MatchWebViewDialog( MatchWeb* matchWeb, wxWindow* parent, wxWindowID id )
: wxDialog( parent, id, "View Match Web", wxDefaultPosition, wxSize( 620, 780 ), wxDEFAULT_DIALOG_STYLE ),
mMatchWeb(matchWeb)
{
	zhAssert( matchWeb != NULL );

	int width, height;
	double scale = 8;

	// view controls
	mChkDistGrid = new wxCheckBox( this, ID_chkDistGrid, "Anim. Distance Grid", wxPoint( 20, 10 ), wxSize( 140, 20 ) );
	mChkDistGrid->SetValue(true);
	mChkLocalMinima = new wxCheckBox( this, ID_chkLocalMinima, "Local Minima", wxPoint( 160, 10 ), wxSize( 100, 20 ) );
	mChkLocalMinima->SetValue(false);
	mChkBlendRegion = new wxCheckBox( this, ID_chkBlendRegion, "Blend Region", wxPoint( 260, 10 ), wxSize( 100, 20 ) );
	mChkBlendRegion->SetValue(false);
	
	// TODO: controls for setting local minima thresholds
	// TODO: scale/zoom controls

	// compute labels
	std::string seg1label = mMatchWeb->getAnimation1().getAnimation()->getName();
	seg1label += ( std::string("[") + toString<float>( mMatchWeb->getAnimation1().getStartTime() ) + "-" + toString<float>( mMatchWeb->getAnimation1().getEndTime() ) + "]" );
	std::string seg2label = mMatchWeb->getAnimation2().getAnimation()->getName();
	seg2label += ( std::string("[") + toString<float>( mMatchWeb->getAnimation2().getStartTime() ) + "-" + toString<float>( mMatchWeb->getAnimation2().getEndTime() ) + "]" );

	// create labels
	new wxStaticText( this, wxID_ANY, seg2label, wxPoint( 20, 40 ), wxSize( 200, 20 ) );
	new wxStaticText( this, wxID_ANY, seg1label, wxPoint( 20, 780 - 60 - 50 ), wxSize( 200, 20 ) );

	// compute image size
	width = mMatchWeb->getNumSamples1();
	height = mMatchWeb->getNumSamples2();

	// create image panel
	unsigned char* img_data = new unsigned char[3*width*height];
	memset( img_data, 255, 3*width*height*sizeof(unsigned char) );
	mWndMatchWebScroll = new wxScrolledWindow( this, ID_wndMatchWebScroll, wxPoint( 10, 60 ), wxSize( 600, 600 ) );
	mImgMatchWeb = new wxImagePanel( mWndMatchWebScroll, img_data, width, height );
	mImgMatchWeb->setScale(scale);
	mWndMatchWebScroll->SetScrollbars( 4, 4, width*scale/4, height*scale/4 );

	// create OK button
	new wxButton( this, wxID_OK, _("&OK"),  wxPoint( 620/2 - 60, 780 - 60 - 10 ), wxSize( 75, 30 ) );

	AnimationDistanceGrid* grid = mMatchWeb->getDistanceGrid();
	if( grid != NULL ) grid->findLocalMinima( 0.05f, true, true, 0.15f );

	redraw();
}

void MatchWebViewDialog::redraw()
{
	wxMemoryDC dc;
	dc.SelectObject( mImgMatchWeb->getBitmap() );
	dc.SetBackground( wxBrush(*wxWHITE) );
	dc.Clear();

	int width = mImgMatchWeb->getBitmap().GetWidth();
	int height = mImgMatchWeb->getBitmap().GetHeight();

	// draw distance grid
	AnimationDistanceGrid* grid = mMatchWeb->getDistanceGrid();
	if( grid != NULL && mChkDistGrid->IsChecked() )
	{
		for( int pi = 0; pi < height; ++pi )
		{
			for( int pj = 0; pj < width; ++pj )
			{
				unsigned char dist = 255 * grid->getNormDistance( make_pair( pj, height - pi - 1 ) );
				dc.SetPen( wxPen( wxColour( dist, dist, dist ) ) );
				dc.DrawPoint( pj, pi );
			}
		}
	}

	// draw blend region
	if( grid != NULL && mChkBlendRegion->IsChecked() )
	{
		dc.SetPen( wxPen( wxColour( 200, 200, 0 ) ) );

		for( int pi = 0;  pi < height; ++pi )
		{
			for( int pj = 0; pj < width; ++pj )
			{
				if( grid->isBlendRegion( make_pair( pj, height - pi - 1 ) ) )
					dc.DrawPoint( pj, pi );
			}
		}
	}

	// draw match web paths
	dc.SetPen( wxPen( wxColour( 255, 0, 255 ) ) );
	for( unsigned int path_i = 0; path_i < mMatchWeb->getNumPaths(); ++path_i )
	{
		const MatchWeb::Path& path = mMatchWeb->getPath(path_i);
		
		for( unsigned int pi = 0; pi < path.getNumPoints(); ++pi )
		{
			const AnimationDistanceGrid::Point& pt = path.getPoint(pi);
			dc.DrawPoint( pt.getIndex().first, height - pt.getIndex().second - 1 );
		}
	}

	// draw local minima
	if( grid != NULL && mChkLocalMinima->IsChecked() )
	{
		dc.SetPen( wxPen( wxColour( 0, 255, 0 ) ) );

		for( unsigned int pti1 = 0; pti1 < grid->getNumSamples1(); ++pti1 )
		{
			for( unsigned int pti2 = 0; pti2 < grid->getNumSamples2(); ++pti2 )
			{
				if( grid->isLocalMinimum( AnimationDistanceGrid::Index( pti1, pti2 ) ) )
					dc.DrawPoint( pti1, height - pti2 - 1 );
			}
		}

		/*for( unsigned int mini = 0; mini < grid->getNumLocalMinima(); ++mini )
		{
			AnimationDistanceGrid::Index min = grid->getLocalMinimum( mini );
			dc.DrawPoint( min.first, height - min.second - 1 );
		}*/
	}

	//
	//mImgMatchWeb->getBitmap().SaveFile( "C:\\Users\\RatTyrant\\Documents\\My Education\\Projekt_Viz\\pic3c.bmp", wxBITMAP_TYPE_BMP );
	//

	this->Refresh();
}

void MatchWebViewDialog::OnCheckBox_DistGrid( wxCommandEvent& evt )
{
	redraw();
}

void MatchWebViewDialog::OnCheckBox_LocalMinima( wxCommandEvent& evt )
{
	redraw();
}

void MatchWebViewDialog::OnCheckBox_BlendRegion( wxCommandEvent& evt )
{
	redraw();
}

BEGIN_EVENT_TABLE( MatchWebViewDialog, wxDialog )
	EVT_CHECKBOX( ID_chkDistGrid, OnCheckBox_DistGrid )
	EVT_CHECKBOX( ID_chkLocalMinima, OnCheckBox_LocalMinima )
	EVT_CHECKBOX( ID_chkBlendRegion, OnCheckBox_BlendRegion )
END_EVENT_TABLE()
