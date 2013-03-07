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

#include "AnimationStudioFrame.h"
#include "AnimationStudioApp.h"
#include "TimelineWindow.h"
#include "ProjectViewWindow.h"
#include "AnimSequenceWindow.h"
#include "MotionVisualizationWindow.h"
#include "OgreWindow.h"
/*#include "NewResourceDialog.h"
#include "BuildMotionGraphDialog.h"
#include "MotionGraphViewDialog.h"
#include "BuildIndexDialog.h"
#include "MatchWebViewDialog.h"
#include "MatchGraphViewDialog.h"
#include "BuildAnimSpaceDialog.h"
#include "DefineAnimParamDialog.h"*/

AnimationStudioFrame::AnimationStudioFrame( wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size )
: wxFrame( NULL, id, title, pos, size, wxDEFAULT_FRAME_STYLE, "frmAnimationStudio" ),
mbMain(NULL), mTbAnnots(NULL), mTbPlayer(NULL),
mSelectMode(false), mSelectStart(0), mSelectEnd(-1000),
mWndOgre(NULL), mWndProjectView(NULL), mWndMotionVis(NULL)//, mWndTimeline(NULL)
{
	// init AUI manager
	mAuiMgr.SetManagedWindow(this);

	// load bitmaps
	mBitmaps["ID_bmpPlay"] = new wxBitmap( "ID_bmpPlay", wxBITMAP_TYPE_BMP_RESOURCE );
	mBitmaps["ID_bmpPlayDisabled"] = new wxBitmap( "ID_bmpPlayDisabled", wxBITMAP_TYPE_BMP_RESOURCE );
	mBitmaps["ID_bmpPlaySequence"] = new wxBitmap( "ID_bmpPlaySequence", wxBITMAP_TYPE_BMP_RESOURCE );
	mBitmaps["ID_bmpPlaySequenceDisabled"] = new wxBitmap( "ID_bmpPlaySequenceDisabled", wxBITMAP_TYPE_BMP_RESOURCE );
	mBitmaps["ID_bmpPause"] = new wxBitmap( "ID_bmpPause", wxBITMAP_TYPE_BMP_RESOURCE );
	mBitmaps["ID_bmpStop"] = new wxBitmap( "ID_bmpStop", wxBITMAP_TYPE_BMP_RESOURCE );
	mBitmaps["ID_bmpStopDisabled"] = new wxBitmap( "ID_bmpStopDisabled", wxBITMAP_TYPE_BMP_RESOURCE );
	mBitmaps["ID_bmpAnnotStart"] = new wxBitmap( "ID_bmpAnnotStart", wxBITMAP_TYPE_BMP_RESOURCE );
	mBitmaps["ID_bmpAnnotStartDisabled"] = new wxBitmap( "ID_bmpAnnotStartDisabled", wxBITMAP_TYPE_BMP_RESOURCE );
	mBitmaps["ID_bmpAnnotEnd"] = new wxBitmap( "ID_bmpAnnotEnd", wxBITMAP_TYPE_BMP_RESOURCE );
	mBitmaps["ID_bmpClearAnnots"] = new wxBitmap( "ID_bmpClearAnnots", wxBITMAP_TYPE_BMP_RESOURCE );
	mBitmaps["ID_bmpClearAnnotsDisabled"] = new wxBitmap( "ID_bmpClearAnnotsDisabled", wxBITMAP_TYPE_BMP_RESOURCE );
	mBitmaps["ID_bmpDoTransition"] = new wxBitmap( "ID_bmpDoTransition", wxBITMAP_TYPE_BMP_RESOURCE );
	mBitmaps["ID_bmpDoTransitionDisabled"] = new wxBitmap( "ID_bmpDoTransitionDisabled", wxBITMAP_TYPE_BMP_RESOURCE );
	mBitmaps["ID_bmpSelect"] = new wxBitmap( "ID_bmpSelect", wxBITMAP_TYPE_BMP_RESOURCE );
	mBitmaps["ID_bmpSelectDisabled"] = new wxBitmap( "ID_bmpSelectDisabled", wxBITMAP_TYPE_BMP_RESOURCE );
	mBitmaps["ID_bmpDeselect"] = new wxBitmap( "ID_bmpDeselect", wxBITMAP_TYPE_BMP_RESOURCE );
	mBitmaps["ID_bmpDeselectDisabled"] = new wxBitmap( "ID_bmpDeselectDisabled", wxBITMAP_TYPE_BMP_RESOURCE );

	// set icon
	this->SetIcon( wxICON( ID_icoAnimationStudio ) );

	// create application windows
	mWndOgre = new OgreWindow( this, ID_wndOgre );
	//mWndTimeline = new TimelineWindow( this, ID_wndTimeline );	
	mWndProjectView = new ProjectViewWindow( this, ID_wndProjectView );
	mWndAnimSequence = new AnimSequenceWindow( this, ID_wndAnimSequence );
	mWndMotionVis = new MotionVisualizationWindow( this, ID_wndMotionVis );

	//
	// Create main menu:
	//

	mbMain = new wxMenuBar();
	wxMenu* menu = NULL;
	wxMenu* submenu = NULL;
	
	menu = new wxMenu();
	menu->AppendCheckItem( ID_mnViewShowSkel, wxT("Show Skeleton") );
	menu->Check( ID_mnViewShowSkel, true );
	menu->AppendCheckItem( ID_mnViewShowGround, wxT("Show Ground") );
	menu->Check( ID_mnViewShowGround, true );
	menu->AppendCheckItem( ID_mnViewShowSkybox, wxT("Show Skybox") );
	mbMain->Append( menu, wxT( "&View" ) );
	menu->Check( ID_mnViewShowSkybox, true );
	
	menu = new wxMenu();
	menu->Append( ID_mnToolsDefineEndEffectors, wxT( "Define End-effectors" ) );
	menu->Append( ID_mnToolsDetectPlantConstr, wxT( "Detect Plant Constr." ) );
	menu->Append( ID_mnToolsFootskateCleanup, wxT( "Footskate Cleanup" ) );
	menu->Append( ID_mnToolsMirror, wxT( "Mirror Animation" ) );
	/*menu->AppendSeparator();
	submenu = new wxMenu();
	submenu->Append( ID_mnToolsBuildAnimIndex, wxT( "Build" ) );
	submenu->Append( ID_mnToolsViewMatchWeb, wxT( "View Match Web" ) );
	submenu->Append( ID_mnToolsSearchAnimIndex, wxT( "Search" ) );
	menu->AppendSubMenu( submenu, "Animation Index" );
	submenu = new wxMenu();
	submenu->Append( ID_mnToolsBuildMotionGraph, wxT( "Build" ) );
	submenu->Append( ID_mnToolsViewMotionGraph, wxT( "View" ) );
	menu->AppendSubMenu( submenu, "Motion Graph" );
	menu->AppendSeparator();
	menu->Append( ID_mnToolsCreateAnimFromSeg, wxT( "Create Animation From Segment" ) );
	menu->Append( ID_mnToolsBuildAnimSpace, wxT( "Build Anim. Space" ) );
	submenu = new wxMenu();
	submenu->Append( ID_mnToolsBuildTransitions, wxT( "Build Transitions" ) );
	submenu->Append( ID_mnToolsViewMotionGraph2, wxT( "View Motion Graph" ) );
	submenu->AppendSeparator();
	submenu->Append( ID_mnToolsDefineParam, wxT( "Define Parametrization" ) );
	submenu->Append( ID_mnToolsMatchAnnots, wxT( "Match Annotations" ) );
	submenu->Append( ID_mnToolsBuildBlendCurves, wxT( "Build Blend Curves" ) );
	menu->AppendSubMenu( submenu, "Current Animation" );
	mbMain->Append( menu, wxT( "&Tools" ) );*/

	menu = new wxMenu();
	menu->Append( ID_mnHelpAbout, wxT( "About" ) );
	mbMain->Append( menu, wxT( "&Help" ) );
	
	this->SetMenuBar(mbMain);

	// create annotations toolbar
	/*mTbAnnots = new wxToolBar( this, ID_tbAnnots, wxDefaultPosition, wxDefaultSize,
		wxNO_BORDER | wxTB_HORIZONTAL | wxTB_FLAT, "tbAnnots" );
	mTbAnnots->AddCheckTool( ID_btnAnnotStart, "btnAnnotStart",
		*mBitmaps["ID_bmpAnnotStart"], *mBitmaps["ID_bmpAnnotStartDisabled"],
		"Start/End Annotation" );
	mTbAnnots->AddControl( new wxComboBox( mTbAnnots, ID_cbAnnots, wxEmptyString,
		wxDefaultPosition, wxSize( 120, 24 ) )
		);
	mTbAnnots->AddTool( ID_btnClearAnnots, "btnClearAnnots",
		*mBitmaps["ID_bmpClearAnnots"], *mBitmaps["ID_bmpClearAnnotsDisabled"],
		wxITEM_NORMAL, "Clear Annotations" );
	mTbAnnots->Realize();*/

	// create player toolbar
	mTbPlayer = new wxToolBar( this, ID_tbPlayer, wxDefaultPosition, wxDefaultSize,
		wxNO_BORDER | wxTB_HORIZONTAL | wxTB_FLAT, "tbPlayer" );
	mTbPlayer->AddCheckTool( ID_btnPlay, "btnPlay",
		*mBitmaps["ID_bmpPlay"], *mBitmaps["ID_bmpPlayDisabled"],
		"Play Current Animation" );
	mTbPlayer->AddTool( ID_btnPlaySequence, "Play Sequence",
		*mBitmaps["ID_bmpPlaySequence"], *mBitmaps["ID_bmpPlaySequenceDisabled"],
		wxITEM_NORMAL, "Play Animation Sequence" );
	mTbPlayer->AddTool( ID_btnStop, "Stop",
		*mBitmaps["ID_bmpStop"], *mBitmaps["ID_bmpStopDisabled"],
		wxITEM_NORMAL, "Stop Animation" );
	/*mTbPlayer->AddControl( new wxTextCtrl( mTbPlayer, ID_txtParams,
		wxEmptyString, wxDefaultPosition, wxSize( 80, 24 ) )
		);*/
	mTbPlayer->AddSeparator();
	mTbPlayer->AddControl( new wxStaticText( mTbPlayer, ID_stxPlayTime, "00.00" ) );
	mTbPlayer->AddControl( new wxStaticText( mTbPlayer, -1, "/" ) );
	mTbPlayer->AddControl( new wxStaticText( mTbPlayer, ID_stxPlayLength, "00.00" ) );
	mTbPlayer->AddControl( new wxStaticText( mTbPlayer, -1, "s" ) );
	mTbPlayer->AddControl( new wxSlider( mTbPlayer, ID_slPlaySlider, 0, 0, 1000, wxDefaultPosition, wxSize( 480, 24 ) ) );
	mTbPlayer->AddSeparator();
	mTbPlayer->AddControl( new wxTextCtrl( mTbPlayer, ID_txtFrameRate,
		"0", wxDefaultPosition, wxSize( 40, 24 ) )
		);
	mTbPlayer->AddControl( new wxStaticText( mTbPlayer, -1, "/" ) );
	mTbPlayer->AddControl( new wxStaticText( mTbPlayer, ID_stxOrigFrameRate, "0" ) );
	mTbPlayer->AddControl( new wxStaticText( mTbPlayer, -1, "Hz" ) );
	mTbPlayer->AddControl( new wxSlider( mTbPlayer, ID_slPlaySlider, 0, 0, 1000, wxDefaultPosition, wxSize( 480, 24 ) ) );
	mTbPlayer->AddSeparator();
	mTbPlayer->AddCheckTool( ID_btnSelect, "btnSelect",
		*mBitmaps["ID_bmpSelect"], *mBitmaps["ID_bmpSelectDisabled"],
		"Select Animation Segment" );
	mTbPlayer->AddTool( ID_btnDeselect, "Deselect",
		*mBitmaps["ID_bmpDeselect"], *mBitmaps["ID_bmpDeselectDisabled"],
		wxITEM_NORMAL, "Deselect Animation Segment" );
	mTbPlayer->Realize();

	// TODO: also add toolbar items for: see current anim. framerate, set desired framerate

	// Add OGRE window to AUI manager
	wxAuiPaneInfo wndOgre_pi;
	wndOgre_pi.CentrePane();
	wndOgre_pi.Dock();
	wndOgre_pi.Floatable(false);
	wndOgre_pi.CloseButton(false);
	mAuiMgr.AddPane( mWndOgre, wndOgre_pi );

	// Add timeline window to AUI manager
	/*mWndTimeline->SetSize( 272, size.GetHeight() );
	wxAuiPaneInfo wndTimeline_pi;
	wndTimeline_pi.Top();
	wndTimeline_pi.Floatable();
	wndTimeline_pi.CloseButton();
	wndTimeline_pi.Caption( "Timeline" );
	wndTimeline_pi.Show(true);
	mAuiMgr.AddPane( mWndTimeline, wndTimeline_pi );*/

	// Add project view window to AUI manager
	mWndProjectView->SetSize( 272, size.GetHeight() );
	wxAuiPaneInfo wndProjectView_pi;
	wndProjectView_pi.Left();
	wndProjectView_pi.Floatable();
	wndProjectView_pi.CloseButton(false);
	wndProjectView_pi.Caption( "Project View" );
	mAuiMgr.AddPane( mWndProjectView, wndProjectView_pi );

	// Add animation sequence window to AUI manager
	mWndAnimSequence->SetSize( 272, size.GetHeight() );
	wxAuiPaneInfo wndAnimSeq_pi;
	wndAnimSeq_pi.Left();
	wndAnimSeq_pi.Floatable();
	wndAnimSeq_pi.CloseButton(false);
	wndAnimSeq_pi.Caption( "Animation Sequence" );
	mAuiMgr.AddPane( mWndAnimSequence, wndAnimSeq_pi );

	// Add motion visualization window to AUI manager
	mWndMotionVis->SetSize( 272, size.GetHeight() );
	wxAuiPaneInfo wndMotionVis_pi;
	wndMotionVis_pi.Right();
	wndMotionVis_pi.Floatable();
	wndMotionVis_pi.CloseButton(false);
	wndMotionVis_pi.Caption( "Motion Visualization" );
	mAuiMgr.AddPane( mWndMotionVis, wndMotionVis_pi );

	// add tbAnnots to AUI manager
	/*wxAuiPaneInfo tbAnnots_pi;
	tbAnnots_pi.ToolbarPane();
	tbAnnots_pi.Top();
	tbAnnots_pi.Floatable();
	tbAnnots_pi.CloseButton();
	mAuiMgr.AddPane( mTbAnnots, tbAnnots_pi );*/

	// add tbPlayer to AUI manager
	wxAuiPaneInfo tbPlayer_pi;
	tbPlayer_pi.ToolbarPane();
	tbPlayer_pi.Top();
	tbPlayer_pi.Floatable();
	tbPlayer_pi.CloseButton(false);
	mAuiMgr.AddPane( mTbPlayer, tbPlayer_pi );

	// update AUI manager
	mAuiMgr.Update();

	Maximize();
}

AnimationStudioFrame::~AnimationStudioFrame()
{
	mAuiMgr.UnInit();
	// TODO: delete bitmaps?
}

OgreWindow* AnimationStudioFrame::getOgreWindow() const
{
	return mWndOgre;
}

ProjectViewWindow* AnimationStudioFrame::getProjectViewWindow() const
{
	return mWndProjectView;
}

MotionVisualizationWindow* AnimationStudioFrame::getMotionVisualizationWindow() const
{
	return mWndMotionVis;
}

bool AnimationStudioFrame::hasSelection() const
{
	return gApp->getCurrentAnimation() != NULL &&
		!mSelectMode && mSelectStart <= mSelectEnd;
}

AnimationSegment AnimationStudioFrame::getSelection() const
{
	zhAssert( hasSelection() );

	zh::Animation* anim = gApp->getCurrentAnimation();

	return AnimationSegment( anim, mSelectStart * anim->getLength(), mSelectEnd * anim->getLength() );
}

void AnimationStudioFrame::refresh()
{
	mWndProjectView->refresh();
	mWndMotionVis->refresh();
}

void AnimationStudioFrame::OnMenu_ViewShowSkel( wxCommandEvent& evt )
{
	gApp->getAppFrame()->getOgreWindow()->showSkeleton( mbMain->IsChecked( ID_mnViewShowSkel ) );
}

void AnimationStudioFrame::OnMenu_ViewShowGround( wxCommandEvent& evt )
{
	gApp->getAppFrame()->getOgreWindow()->showGround( mbMain->IsChecked( ID_mnViewShowGround ) );
}

void AnimationStudioFrame::OnMenu_ViewShowSkybox( wxCommandEvent& evt )
{
	gApp->getAppFrame()->getOgreWindow()->showSkybox( mbMain->IsChecked( ID_mnViewShowSkybox ) );
}

void AnimationStudioFrame::OnMenu_ToolsDefineEndEffectors( wxCommandEvent& evt )
{
	// TODO: pop multichoice dialog with list of all bones, designate checked ones as end-effectors
}

void AnimationStudioFrame::OnMenu_ToolsDetectPlantConstr( wxCommandEvent& evt )
{
	// TODO: choose correct skeleton for the currently selected motion!
	zh::Skeleton* skel = zhAnimationSystem->getOutputSkeleton();
	zh::Animation* anim = gApp->getCurrentAnimation();

	if( skel == NULL || anim == NULL )
	{
		wxMessageBox( "Cannot detect plant constraints. Character model or animation not loaded.",
			"Error", wxOK | wxICON_EXCLAMATION );
		return;
	}

	// TODO: pop settings dialog
	float max_poschange = 1.5f;
	float min_constrlength = 0.1f;
	//

	// delete existing plant constraints
	anim->getPlantConstraintAnnotations()->deleteAllAnnotations();

	// Detect plant constraints
	// TODO: update this
	/*const std::set<std::string>& ee_names = gApp->getEndEffectorSet( skel->getName() );
	for( std::set<std::string>::const_iterator eei = ee_names.begin(); eei != ee_names.end(); ++eei )
	{
		PlantConstrDetector* pcd = new PlantConstrDetector( skel, anim );
		pcd->setMaxPosChange(max_poschange);
		pcd->setMinConstrLength(min_constrlength);
		pcd->detect(*eei);
		delete pcd;
	}*/

	// TODO: refresh appropriate displays
}

void AnimationStudioFrame::OnMenu_ToolsFootskateCleanup( wxCommandEvent& evt )
{
	// TODO: choose correct skeleton for the currently selected motion!
	zh::Skeleton* skel = zhAnimationSystem->getOutputSkeleton();
	zh::Animation* anim = gApp->getCurrentAnimation();

	if( skel == NULL || anim == NULL )
	{
		wxMessageBox( "Cannot clean up footskate errors. Character model or animation not loaded.",
			"Error", wxOK | wxICON_EXCLAMATION );
		return;
	}

	// TODO: pop settings dialog
	// TODO: clean up end-effectors
	// TODO: refresh appropriate displays
}

void AnimationStudioFrame::OnMenu_ToolsMirror( wxCommandEvent& evt )
{
	// TODO: create mirrored version of the current motion
}

void AnimationStudioFrame::OnMenu_ToolsBuildAnimIndex( wxCommandEvent& evt )
{
	/*Character* ch = gApp->getCurrentCharacter();
	Model* mdl = NULL;

	if( gApp->getCurrentCharacter() != NULL )
		mdl = gApp->getCurrentCharacter()->getModelController()->getModel();

	if( mdl == NULL )
	{
		wxMessageBox( "Cannot view match web. Character model not loaded.",
			"Error", wxOK|wxICON_EXCLAMATION );
		return;
	}

	NewResourceDialog newres_dlg( this, wxID_ANY, NewResourceDialog::ResourceType_AnimIndex );
	if( newres_dlg.ShowModal() != wxID_OK )
	{
		// user cancelled
		return;
	}

	std::string name = newres_dlg.getName();
	std::string dir = newres_dlg.getDirectory();

	if( zhAnimationSearchSystem->getAnimationIndexManager()->hasResource(name) )
	{
		// duplicate resource name
		wxMessageBox( "Animation index by the name " + name + " already exists!",
			"Duplicate Name", wxOK|wxICON_ERROR );
		return;
	}

	if( dir == "" )
	{
		// no resource dir.
		wxMessageBox( "Valid resource location not specified.",
			"Invalid Location", wxOK|wxICON_ERROR );
		return;
	}

	std::string path = dir + name + ".vai";

	// prompt user to build anim. index
	BuildIndexDialog dlg( this, wxID_ANY );
	if( dlg.ShowModal() != wxID_OK )
	{
		// user cancelled
		return;
	}

	std::string labelfilter = dlg.getLabelFilter();
	unsigned int resample_fact = dlg.getResampleFactor();
	float wnd_length = dlg.getWindowLength(),
		min_dist = dlg.getMinDistance(),
		max_distdiff = dlg.getMaxDistanceDiff(),
		min_chainlength = dlg.getMinChainLength(),
		max_bridgelength = dlg.getMaxBridgeLength();

	// get raw animations on this character
	std::vector<AnimationSetPtr> raw_anims;
	AnimationStudioApp::AnimationSetConstIterator rasi = gApp->getRawAnimationSetConstIterator( ch->getId() );
	while( !rasi.end() )
	{
		AnimationSetPtr anim_set = rasi.next();
		raw_anims.push_back(anim_set);
	}

	// build anim. index
	zhAnimationSearchSystem->setResampleFactor(resample_fact);
	zhAnimationSearchSystem->setWindowLength(wnd_length);
	zhAnimationSearchSystem->setMinDistance(min_dist);
	zhAnimationSearchSystem->setMaxDistanceDiff(max_distdiff);
	zhAnimationSearchSystem->setMinChainLength(min_chainlength);
	zhAnimationSearchSystem->setMaxBridgeLength(max_bridgelength);
	unsigned long id = 0;
	while( zhAnimationSearchSystem->getAnimationIndexManager()->hasResource(id) ) ++id;
	wxStopWatch sw;
	AnimationIndexPtr anim_index = zhAnimationSearchSystem->buildIndex( id, name, mdl, raw_anims, labelfilter );
	float build_time = sw.Time() / 1000.f;

	// inform user
	wxMessageBox( "Finished building animation index. Time taken: " + toString<float>(build_time) + "s",
		"Build Complete", wxOK|wxICON_INFORMATION );

	// write index to file
	zhAnimationSearchSystem->getAnimationIndexManager()->serializeResource( anim_index->getId(), path );

	// add index to project
	gApp->addAnimationIndex( gApp->getCurrentCharacter()->getId(), anim_index );

	// refresh controls
	refresh();*/
}

void AnimationStudioFrame::OnMenu_ToolsViewMatchWeb( wxCommandEvent& evt )
{
	/*Character* ch = gApp->getCurrentCharacter();
	Model* mdl = NULL;

	if( gApp->getCurrentCharacter() != NULL )
		mdl = gApp->getCurrentCharacter()->getModelController()->getModel();

	if( mdl == NULL )
	{
		wxMessageBox( "Cannot view match web. Character model not loaded.",
			"Error", wxOK|wxICON_EXCLAMATION );
		return;
	}

	// get the list of all animation indexes
	wxArrayString animindexes_str;
	AnimationStudioApp::AnimationIndexConstIterator aii = gApp->getAnimationIndexConstIterator( ch->getId() );
	while( !aii.end() )
	{
		animindexes_str.Add( aii.next()->getName() );
	}

	// prompt the user to choose an animation index
	wxSingleChoiceDialog dlg( this, "Select the animation index:", "Select Animation Index", animindexes_str );
	if( dlg.ShowModal() != wxID_OK )
	{
		// user cancelled
		return;
	}

	// get the selection
	AnimationIndexPtr anim_index = gApp->getAnimationIndex( ch->getId(), dlg.GetStringSelection().c_str() );

	// get the list of all indexed animations
	wxArrayString anims_str;
	for( unsigned int segi = 0; segi < anim_index->getNumAnimationSegments(); ++segi )
	{
		const AnimationSegment& anim_seg = anim_index->getAnimationSegment(segi);
		anims_str.Add( anim_seg.getAnimation()->getAnimationSet()->getName() + ":" + anim_seg.getAnimation()->getName() +
			"[" + toString<float>( anim_seg.getStartTime() ) + "-" + toString<float>( anim_seg.getEndTime() ) + "]" );
	}

	// prompt the user to choose an animation
	wxSingleChoiceDialog dlg1( this, "Select the first animation:", "Select Animation", anims_str );
	if( dlg1.ShowModal() != wxID_OK )
	{
		// user cancelled
		return;
	}

	// get the selection
	unsigned int seg1i = dlg1.GetSelection();
	
	// prompt the user to choose an animation
	wxSingleChoiceDialog dlg2( this, "Select the second animation:", "Select Animation", anims_str );
	if( dlg2.ShowModal() != wxID_OK )
	{
		// user cancelled
		return;
	}

	// get the selection
	unsigned int seg2i = dlg2.GetSelection();

	// display match web
	MatchWeb* mw = anim_index->getMatchWeb( MatchWeb::Index( seg1i, seg2i ) );
	if( mw != NULL )
	{
		MatchWebViewDialog mwv_dlg( mw, this, wxID_ANY );
		mwv_dlg.ShowModal();
	}*/
}

void AnimationStudioFrame::OnMenu_ToolsSearchAnimIndex( wxCommandEvent& evt )
{
	/*Character* ch = gApp->getCurrentCharacter();
	Model* mdl = NULL;

	if( gApp->getCurrentCharacter() != NULL )
		mdl = gApp->getCurrentCharacter()->getModelController()->getModel();

	if( mdl == NULL )
	{
		wxMessageBox( "Cannot search animation index. Character model not loaded.",
			"Error", wxOK|wxICON_EXCLAMATION );
		return;
	}

	if( !gApp->getAppFrame()->hasSelection() )
	{
		wxMessageBox( "Select the query segment of an indexed animation.", "Select Query Animation",
			wxOK|wxICON_INFORMATION, this );
		return;
	}

	AnimationSegment qanim = gApp->getAppFrame()->getSelection();

	// search for similar anim. segments
	MatchGraph* mg = zhAnimationSearchSystem->search(qanim);
	if( mg == NULL )
	{
		wxMessageBox( "Selected animation is not indexed.", "Error",
			wxOK|wxICON_ERROR, this );
		return;
	}

	// display, test and accept/reject results
	MatchGraphViewDialog mgdlg( this, wxID_ANY, mg );
	if( mgdlg.ShowModal() != wxID_OK )
	{
		// user cancelled
		return;
	}

	// prompt user to build anim. space
	BuildAnimSpaceDialog dlg( this, wxID_ANY );
	if( dlg.ShowModal() != wxID_OK )
	{
		// user cancelled
		return;
	}

	std::string name = dlg.getName();
	bool build_regcurves = dlg.getBuildBlendCurves();
	bool match_annots = dlg.getMatchAnnots();
	unsigned int knot_spacing = dlg.getKnotSpacing();

	// choose anim. set for anim. space
	AnimationSetPtr anim_set = NULL;
	if( gApp->getNumAnimationSets( ch->getId() ) > 0 )
	{
		wxArrayString choices;
		AnimationStudioApp::AnimationSetConstIterator asi = gApp->getAnimationSetConstIterator( ch->getId() );
		while( !asi.end() )
		{
			anim_set = asi.next();
			choices.push_back( anim_set->getName() );
		}

		wxSingleChoiceDialog dlg( this, "Select animation set for the animation space:", "Select Animation Set", choices );
		if( dlg.ShowModal() != wxID_OK )
		{
			// user cancelled
			return;
		}

		anim_set = gApp->getAnimationSet( ch->getId(), dlg.GetStringSelection().c_str() );
	}
	else
	{
		anim_set = gApp->createAnimationSet( ch->getId() );
	}

	if( anim_set == NULL )
	{
		// valid anim. set not created or selected
		return;
	}

	unsigned short id = 0;
	while( anim_set->hasAnimationSpace(id) )
		++id;
	if( anim_set->hasAnimationSpace(name) )
	{
		if( wxMessageBox( "Animation space by the name " + name + " already exists in the animation set. Overwrite?",
			"Duplicate Name", wxOK|wxCANCEL|wxICON_QUESTION ) == wxCANCEL )
			return;

		anim_set->deleteAnimationSpace(name);
	}

	// build anim. space
	zhAnimationSearchSystem->setBuildBlendCurves(build_regcurves);
	zhAnimationSearchSystem->setMatchAnnots(match_annots);
	zhAnimationSearchSystem->setKnotSpacing(knot_spacing);
	AnimationSpace* anim_space = zhAnimationSearchSystem->buildAnimationSpace( id, name,
		mdl, anim_set, mg );

	// add corresponding nodes to the internal animation tree
	gApp->stopAnimation();
	ch->getAnimationController()->removeAnimationTree(zhIntAnimTree_Name);
	AnimationTreePtr atree_int = AnimationTreePtr::DynamicCast<zh::Resource>( zhAnimationSystem->getAnimationTreeManager()->getResource(zhIntAnimTree_Name) );
	gApp->_addAnimsToAnimTree( anim_set, atree_int, atree_int->getRoot() );
	ch->getAnimationController()->addAnimationTree(atree_int);
	gApp->selectAnimationTree(zhIntAnimTree_Name);
	
	// TODO: anim. set modified

	// refresh controls
	refresh();*/
}

void AnimationStudioFrame::OnMenu_ToolsBuildMotionGraph( wxCommandEvent& evt )
{
	/*Character* ch = gApp->getCurrentCharacter();
	Model* mdl = NULL;

	if( gApp->getCurrentCharacter() != NULL )
		mdl = gApp->getCurrentCharacter()->getModelController()->getModel();

	if( mdl == NULL )
	{
		wxMessageBox( "Cannot build motion graph. Character model not loaded.",
			"Error", wxOK|wxICON_EXCLAMATION );
		return;
	}

	// get raw anims list
	std::vector<AnimationSetPtr> ranims;
	AnimationStudioApp::AnimationSetConstIterator rasi = gApp->getRawAnimationSetConstIterator( ch->getId() );
	while( !rasi.end() )
		ranims.push_back( rasi.next() );

	// prompt user to specify motion graph build settings
	BuildMotionGraphDialog dlg( this, wxID_ANY );
	if( dlg.ShowModal() != wxID_OK )
	{
		// user cancelled
		return;
	}

	std::string labelfilter = dlg.getLabelFilter();
	float wnd_length = dlg.getWindowLength(),
		min_dist = dlg.getMinDistance();

	// build transitions
	zhAnimationSearchSystem->setWindowLength(wnd_length);
	zhAnimationSearchSystem->setMinDistance(min_dist);
	wxStopWatch sw;
	zhAnimationSearchSystem->buildTransitions( mdl, ranims, labelfilter );
	float build_time = sw.Time() / 1000.f;

	// inform user
	wxMessageBox( "Finished building animation transitions. Time taken: " + toString<float>(build_time) + "s",
		"Build Complete", wxOK|wxICON_INFORMATION );*/
}

void AnimationStudioFrame::OnMenu_ToolsViewMotionGraph( wxCommandEvent& evt )
{
	/*if( gApp->getCurrentCharacter() == NULL )
	{
		wxMessageBox( "Cannot view motion graph. Character model not loaded.",
			"Error", wxOK|wxICON_EXCLAMATION );
		return;
	}

	MotionGraphViewDialog dlg( this, wxID_ANY, MGView_Raw );
	dlg.ShowModal();*/
}

void AnimationStudioFrame::OnMenu_ToolsCreateAnimFromSeg( wxCommandEvent& evt )
{
	if( !hasSelection() )
	{
		wxMessageBox( "You must select the animation segment first.", "Select Animation Segment",
			wxOK|wxICON_INFORMATION, this );
		return;
	}

	AnimationSegment anim_seg = getSelection();
	zhAnimationSystem->createAnimationFromSegment( anim_seg.getAnimation()->getName()
		+ "[" + toString<float>(anim_seg.getStartTime()) + "-" + 
		toString<float>(anim_seg.getEndTime()) + "]", anim_seg.getAnimation()->getName(),
		anim_seg.getStartTime(), anim_seg.getLength() );

	// Refresh controls
	refresh();
}

void AnimationStudioFrame::OnMenu_ToolsBuildAnimSpace( wxCommandEvent& evt )
{
	/*// TODO: implement this

	// refresh controls
	refresh();*/
}

void AnimationStudioFrame::OnMenu_ToolsBuildTransitions( wxCommandEvent& evt )
{
	/*Model* mdl = NULL;
	AnimationSpace *src_panim = NULL, *trg_panim = NULL;
	zh::Animation *src_anim = NULL, *trg_anim = NULL;

	if( gApp->getCurrentCharacter() != NULL )
		mdl = gApp->getCurrentCharacter()->getModelController()->getModel();

	if( mdl == NULL )
	{
		wxMessageBox( "Cannot build animation transitions. Character model not loaded.",
			"Error", wxOK|wxICON_EXCLAMATION );
		return;
	}

	if( gApp->getCurrentAnimationNode() != NULL )
	{
		if( gApp->getCurrentAnimationNode()->getClassId() == AnimationBlender::ClassId() )
			src_panim = static_cast<AnimationBlender*>( gApp->getCurrentAnimationNode() )->getAnimationSpace();
		else if( gApp->getCurrentAnimationNode()->getClassId() == AnimationSampler::ClassId() )
			src_anim = static_cast<AnimationSampler*>( gApp->getCurrentAnimationNode() )->getAnimation();
	}

	if( src_panim == NULL && src_anim == NULL )
	{
		wxMessageBox( "Cannot build animation transitions. Source animation not loaded.",
			"Error", wxOK|wxICON_EXCLAMATION );
		return;
	}

	AnimationSetPtr anim_set = src_panim != NULL ? src_panim->getAnimationSet() : src_anim->getAnimationSet();
	
	// get the list of all animations
	wxArrayString anims_str;
	AnimationSet::AnimationSpaceConstIterator panim_i = anim_set->getAnimationSpaceConstIterator();
	while( !panim_i.end() )
	{
		zh::AnimationSpace* panim = panim_i.next();

		if( !panim->hasParametrization() )
			continue;

		anims_str.Add( panim->getName() );
	}
	AnimationSet::AnimationConstIterator anim_i = anim_set->getAnimationConstIterator();
	while( !anim_i.end() )
	{
		zh::Animation* anim = anim_i.next();
		anims_str.Add( anim->getName() );
	}

	// prompt the user to select target param. animation
	wxSingleChoiceDialog dlg( this, "Select the target animation:", "Select Target Animation", anims_str );
	if( dlg.ShowModal() != wxID_OK )
	{
		// user cancelled
		return;
	}

	// TODO: show dialog for configuring transition build settings (e.g. motion distance threshold)
	zhAnimationSearchSystem->setWindowLength(0.25f);
	zhAnimationSearchSystem->setMinDistance(0.05f);
	//
	
	// find transitions
	std::string anim_name = dlg.GetStringSelection().c_str();
	if( anim_set->hasAnimationSpace(anim_name) )
	{
		trg_panim = anim_set->getAnimationSpace(anim_name);

		if( src_panim != NULL )
			zhAnimationSearchSystem->buildTransitions( mdl, src_panim, trg_panim );
		else
			zhAnimationSearchSystem->buildTransitions( mdl, src_anim, trg_panim );
	}
	else
	{
		trg_anim = anim_set->getAnimation(anim_name);

		if( src_panim != NULL )
			zhAnimationSearchSystem->buildTransitions( mdl, src_panim, trg_anim );
		else
			zhAnimationSearchSystem->buildTransitions( mdl, src_anim, trg_anim );
	}*/
}

void AnimationStudioFrame::OnMenu_ToolsViewMotionGraph2( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnMenu_ToolsDefineParam( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnMenu_ToolsMatchAnnots( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnMenu_ToolsBuildBlendCurves( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnMenu_HelpAbout( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnTool_AnnotStart( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnComboBox_Annots( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnTool_ClearAnnots( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnTool_Play( wxCommandEvent& evt )
{
	zh::Animation* anim = gApp->getCurrentAnimation();
	if( anim == NULL )
	{
		wxMessageBox( "No animation currently selected.",
			"Error", wxOK | wxICON_EXCLAMATION );
		return;
	}

	if( evt.IsChecked() )
	{
		zhAnimationSystem->unpauseAnimation();

		if( !zhAnimationSystem->isAnimationPlaying() )
		{
			// Start playing animation
			wxTextCtrl* txt_framerate = static_cast<wxTextCtrl*>( FindWindowById( ID_txtFrameRate, mTbPlayer ) );
			int framerate = fromString<int>( txt_framerate->GetValue().To8BitData() );
			if( framerate <= 0 ) framerate = anim->getFrameRate();
			zhAnimationSystem->setAnimationRate( anim->getFrameRate() > 0 ? framerate/anim->getFrameRate() : 1 );
			zhAnimationSystem->playAnimationNow( anim->getFullName() );
		}
	}
	else
	{
		if( zhAnimationSystem->isAnimationPlaying() )
			// Pause currently playing animation
			zhAnimationSystem->pauseAnimation();
	}
}

void AnimationStudioFrame::OnTool_PlaySequence( wxCommandEvent& evt )
{
	std::vector<zh::Animation*> anims;
	mWndAnimSequence->getAnimSequence(anims);

	gApp->deselectAnimation();
	zhAnimationSystem->stopAnimation();
	for( std::vector<zh::Animation*>::const_iterator anim_i = anims.begin();
		anim_i != anims.end(); ++anim_i )
		zhAnimationSystem->playAnimation( (*anim_i)->getFullName() );
	// TODO: simpler way of doing this...
	zhAnimationSystem->getAnimationTree()->getRoot()->setOrigin( zh::Skeleton::Situation(0,0,0) );
}

void AnimationStudioFrame::OnTool_Stop( wxCommandEvent& evt )
{
	zhAnimationSystem->stopAnimation();
	
	mWndAnimSequence->clearAnimSequence();
	mTbPlayer->ToggleTool( ID_btnPlay, false );
}

void AnimationStudioFrame::OnScroll_PlaySlider( wxScrollEvent& evt )
{
	zhAnimationSystem->setAnimationTime( evt.GetInt()/1000.f *
		zhAnimationSystem->getAnimationLength() );
}

void AnimationStudioFrame::OnTextEnter_FrameRate( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnTool_Select( wxCommandEvent& evt )
{
	wxSlider* sl_play = static_cast<wxSlider*>( FindWindowById( ID_slPlaySlider, mTbPlayer ) );
	mSelectMode = evt.IsChecked();

	if( mSelectMode )
	{
		mSelectStart = sl_play->GetValue() / 1000.f;
	}
	else
	{
		mSelectEnd = sl_play->GetValue() / 1000.f;

		if( mSelectEnd < mSelectStart )
			mSelectEnd = mSelectStart;
	}
}

void AnimationStudioFrame::OnTool_Deselect( wxCommandEvent& evt )
{
	mSelectMode = false;
	mSelectStart = 0;
	mSelectEnd = -1000.f;
}

void AnimationStudioFrame::OnIdle( wxIdleEvent& evt )
{
	zh::Animation* anim = gApp->getCurrentAnimation();
	if( anim == NULL )
		return;
	
	float play_time = zhAnimationSystem->getAnimationTime(),
		play_length = zhAnimationSystem->getAnimationLength();
	int framerate = anim->getFrameRate();
	char play_time_str[100];
	char play_length_str[100];
	char framerate_str[100];
	sprintf( play_time_str, "%.2f", play_time );
	sprintf( play_length_str, "%.2f", play_length );
	sprintf( framerate_str, "%d", framerate );

	// Update play time display
	wxStaticText* stx_playtime = static_cast<wxStaticText*>( FindWindowById( ID_stxPlayTime, mTbPlayer ) );
	wxStaticText* stx_playlength = static_cast<wxStaticText*>( FindWindowById( ID_stxPlayLength, mTbPlayer ) );
	stx_playtime->SetLabel(play_time_str);
	stx_playlength->SetLabel(play_length_str);

	// Update play rate display
	wxTextCtrl* txt_framerate = static_cast<wxTextCtrl*>( FindWindowById( ID_txtFrameRate, mTbPlayer ) );
	wxStaticText* stx_origfr = static_cast<wxStaticText*>( FindWindowById( ID_stxOrigFrameRate, mTbPlayer ) );
	stx_origfr->SetLabel(framerate_str);

	// Update play slider
	wxSlider* sl_play = static_cast<wxSlider*>( FindWindowById( ID_slPlaySlider, mTbPlayer ) );
	sl_play->SetValue( zhAnimationSystem->getAnimationTime() /
		zhAnimationSystem->getAnimationLength() * 1000.f );
}

BEGIN_EVENT_TABLE( AnimationStudioFrame, wxFrame )
	EVT_MENU( ID_mnViewShowSkel, AnimationStudioFrame::OnMenu_ViewShowSkel )
	EVT_MENU( ID_mnViewShowGround, AnimationStudioFrame::OnMenu_ViewShowGround )
	EVT_MENU( ID_mnViewShowSkybox, AnimationStudioFrame::OnMenu_ViewShowSkybox )
	EVT_MENU( ID_mnToolsDefineEndEffectors, AnimationStudioFrame::OnMenu_ToolsDefineEndEffectors )
	EVT_MENU( ID_mnToolsDetectPlantConstr, AnimationStudioFrame::OnMenu_ToolsDetectPlantConstr )
	EVT_MENU( ID_mnToolsFootskateCleanup, AnimationStudioFrame::OnMenu_ToolsFootskateCleanup )
	EVT_MENU( ID_mnToolsMirror, AnimationStudioFrame::OnMenu_ToolsMirror )
	EVT_MENU( ID_mnToolsBuildAnimIndex, AnimationStudioFrame::OnMenu_ToolsBuildAnimIndex )
	EVT_MENU( ID_mnToolsViewMatchWeb, AnimationStudioFrame::OnMenu_ToolsViewMatchWeb )
	EVT_MENU( ID_mnToolsSearchAnimIndex, AnimationStudioFrame::OnMenu_ToolsSearchAnimIndex )
	EVT_MENU( ID_mnToolsBuildMotionGraph, AnimationStudioFrame::OnMenu_ToolsBuildMotionGraph )
	EVT_MENU( ID_mnToolsViewMotionGraph, AnimationStudioFrame::OnMenu_ToolsViewMotionGraph )
	EVT_MENU( ID_mnToolsCreateAnimFromSeg, AnimationStudioFrame::OnMenu_ToolsCreateAnimFromSeg )
	EVT_MENU( ID_mnToolsBuildAnimSpace, AnimationStudioFrame::OnMenu_ToolsBuildAnimSpace )
	EVT_MENU( ID_mnToolsBuildTransitions, AnimationStudioFrame::OnMenu_ToolsBuildTransitions )
	EVT_MENU( ID_mnToolsViewMotionGraph2, AnimationStudioFrame::OnMenu_ToolsViewMotionGraph2 )
	EVT_MENU( ID_mnToolsDefineParam, AnimationStudioFrame::OnMenu_ToolsDefineParam )
	EVT_MENU( ID_mnToolsMatchAnnots, AnimationStudioFrame::OnMenu_ToolsMatchAnnots )
	EVT_MENU( ID_mnToolsBuildBlendCurves, AnimationStudioFrame::OnMenu_ToolsBuildBlendCurves )
	EVT_MENU( ID_mnHelpAbout, AnimationStudioFrame::OnMenu_HelpAbout )
	EVT_TOOL( ID_btnAnnotStart, AnimationStudioFrame::OnTool_AnnotStart )
	EVT_COMBOBOX( ID_cbAnnots, AnimationStudioFrame::OnComboBox_Annots )
	EVT_TOOL( ID_btnClearAnnots, AnimationStudioFrame::OnTool_ClearAnnots )
	EVT_TOOL( ID_btnPlay, AnimationStudioFrame::OnTool_Play )
	EVT_TOOL( ID_btnPlaySequence, AnimationStudioFrame::OnTool_PlaySequence )
	EVT_TOOL( ID_btnStop, AnimationStudioFrame::OnTool_Stop )
	EVT_TEXT_ENTER( ID_txtFrameRate, AnimationStudioFrame::OnTextEnter_FrameRate )
	EVT_COMMAND_SCROLL( ID_slPlaySlider, AnimationStudioFrame::OnScroll_PlaySlider )
	EVT_TOOL( ID_btnSelect, AnimationStudioFrame::OnTool_Select )
	EVT_TOOL( ID_btnDeselect, AnimationStudioFrame::OnTool_Deselect )
	EVT_IDLE( AnimationStudioFrame::OnIdle )
END_EVENT_TABLE()
