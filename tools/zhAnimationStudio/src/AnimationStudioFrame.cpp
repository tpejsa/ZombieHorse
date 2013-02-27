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
#include "PropertiesWindow.h"
#include "AnimTreeWindow.h"
#include "OgreWindow.h"
#include "NewResourceDialog.h"
#include "BuildMotionGraphDialog.h"
#include "MotionGraphViewDialog.h"
#include "BuildIndexDialog.h"
#include "MatchWebViewDialog.h"
#include "MatchGraphViewDialog.h"
#include "BuildAnimSpaceDialog.h"
#include "DefineAnimParamDialog.h"

AnimationStudioFrame::AnimationStudioFrame( wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size )
: wxFrame( NULL, id, title, pos, size, wxDEFAULT_FRAME_STYLE, "frmAnimationStudio" ),
mbMain(NULL), mTbAnnots(NULL), mTbPlayer(NULL), mTbTransitions(NULL),
mSelectMode(false), mSelectStart(0), mSelectEnd(-1000),
mWndOgre(NULL), mWndTimeline(NULL), mWndProjectView(NULL), mWndProperties(NULL), mWndAnimTree(NULL)
{
	// init AUI manager
	mAuiMgr.SetManagedWindow(this);

	// load bitmaps
	mBitmaps["ID_bmpPlay"] = new wxBitmap( "ID_bmpPlay", wxBITMAP_TYPE_BMP_RESOURCE );
	mBitmaps["ID_bmpPlayDisabled"] = new wxBitmap( "ID_bmpPlayDisabled", wxBITMAP_TYPE_BMP_RESOURCE );
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
	mWndTimeline = new TimelineWindow( this, ID_wndTimeline );
	mWndProjectView = new ProjectViewWindow( this, ID_wndProjectView );
	mWndProperties = new PropertiesWindow( this, ID_wndProperties );
	mWndAnimTree = new AnimTreeWindow( this, ID_wndAnimTree );

	//
	// Create main menu:
	//

	mbMain = new wxMenuBar();

	wxMenu* menu = new wxMenu();
	menu->Append( ID_mnFileNewProject, wxT( "New Project" ) );
	menu->Append( ID_mnFileOpenProject, wxT( "Open Project" ) );
	menu->Append( ID_mnFileSaveProject, wxT( "Save Project" ) );
	menu->AppendSeparator();
	menu->Append( ID_mnFileProjectSettings, wxT( "Project Settings" ) );
	menu->AppendSeparator();
	wxMenu* submenu = new wxMenu();
	submenu->Append( ID_mnFileCreateCharacter, wxT( "Character" ) );
	submenu->Append( ID_mnFileCreateAnimSet, wxT( "Animation Set" ) );
	submenu->Append( ID_mnFileCreateAnimTree, wxT( "Animation Tree" ) );
	menu->AppendSubMenu( submenu, "Create New" );
	submenu = new wxMenu();
	submenu->Append( ID_mnFileAddRawAnimSet, wxT( "Raw Animation Set" ) );
	submenu->Append( ID_mnFileAddAnimIndex, wxT( "Animation Index" ) );
	submenu->Append( ID_mnFileAddAnimSet, wxT( "Animation Set" ) );
	submenu->Append( ID_mnFileAddAnimTree, wxT( "Animation Tree" ) );
	menu->AppendSubMenu( submenu, "Add" );
	menu->AppendSeparator();
	menu->Append( ID_mnFileExit, wxT( "Exit" ) );
	mbMain->Append( menu, wxT( "&File" ) );
	
	menu = new wxMenu();
	menu->Append( ID_mnEditUndo, wxT( "Undo" ) );
	menu->Append( ID_mnEditRedo, wxT( "Redo" ) );
	menu->AppendSeparator();
	menu->Append( ID_mnEditCut, wxT( "Cut" ) );
	menu->Append( ID_mnEditCopy, wxT( "Copy" ) );
	menu->Append( ID_mnEditPaste, wxT( "Paste" ) );
	menu->Append( ID_mnEditDelete, wxT( "Delete" ) );
	menu->AppendSeparator();
	menu->AppendCheckItem( ID_mnEditSelect, wxT("Select") );
	menu->Append( ID_mnEditDeselect, wxT( "Deselect" ) );
	mbMain->Append( menu, wxT( "&Edit" ) );
	
	menu = new wxMenu();
	menu->AppendCheckItem( ID_mnViewTimeline, wxT("Timeline") );
	menu->Check( ID_mnViewTimeline, true );
	menu->AppendCheckItem( ID_mnViewProject, wxT("Project View") );
	menu->Check( ID_mnViewProject, true );
	menu->AppendCheckItem( ID_mnViewProperties, wxT("Properties") );
	menu->Check( ID_mnViewProperties, true );
	menu->AppendCheckItem( ID_mnViewAnimTree, wxT("Animation Tree") );
	menu->Check( ID_mnViewAnimTree, true );
	menu->AppendSeparator();
	menu->AppendCheckItem( ID_mnViewShowSkel, wxT("Show Skeleton") );
	menu->Check( ID_mnViewShowSkel, false );
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
	menu->AppendSeparator();
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
	mbMain->Append( menu, wxT( "&Tools" ) );

	menu = new wxMenu();
	submenu = new wxMenu();
	submenu->Append( ID_mnAnimTreeCreateAnimSampler, wxT( "Animation Sampler" ) );
	submenu->Append( ID_mnAnimTreeCreateFaceController, wxT( "Face Controller" ) );
	submenu->Append( ID_mnAnimTreeCreateAnimBlender, wxT( "Animation Blender" ) );
	submenu->Append( ID_mnAnimTreeCreateAnimTransBlender, wxT( "Anim. Transition Blender" ) );
	submenu->Append( ID_mnAnimTreeCreateAnimMixer, wxT( "Animation Mixer" ) );
	submenu->Append( ID_mnAnimTreeCreateNodeOther, wxT( "Other..." ) );
	menu->AppendSubMenu( submenu, "Create Node" );
	submenu = new wxMenu();
	submenu->Append( ID_mnAnimTreeCreateBoneTransfController, wxT( "Bone Transf. Controller" ) );
	submenu->Append( ID_mnAnimTreeCreateBoneIKController, wxT( "Bone IK Controller" ) );
	submenu->Append( ID_mnAnimTreeCreateBoneControllerOther, wxT( "Other..." ) );
	menu->AppendSubMenu( submenu, "Create Bone Controller" );
	submenu->Append( ID_mnAnimTreeCreateNodeForEachAnim, wxT( "Create Node for each Anim." ) );
	menu->AppendSeparator();
	menu->Append( ID_mnAnimTreeAutoArrangeNodes, wxT( "Auto-arrange Nodes" ) );
	mbMain->Append( menu, wxT( "&Animation Tree" ) );

	menu = new wxMenu();
	menu->Append( ID_mnHelpManual, wxT( "Manual" ) );
	menu->Append( ID_mnHelpAbout, wxT( "About" ) );
	mbMain->Append( menu, wxT( "&Help" ) );
	
	this->SetMenuBar(mbMain);

	// create annotations toolbar
	mTbAnnots = new wxToolBar( this, ID_tbAnnots, wxDefaultPosition, wxDefaultSize,
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
	mTbAnnots->Realize();

	// create player toolbar
	mTbPlayer = new wxToolBar( this, ID_tbPlayer, wxDefaultPosition, wxDefaultSize,
		wxNO_BORDER | wxTB_HORIZONTAL | wxTB_FLAT, "tbPlayer" );
	mTbPlayer->AddCheckTool( ID_btnPlay, "btnPlay",
		*mBitmaps["ID_bmpPlay"], *mBitmaps["ID_bmpPlayDisabled"],
		"Play Animations" );
	mTbPlayer->AddTool( ID_btnStop, "Stop",
		*mBitmaps["ID_bmpStop"], *mBitmaps["ID_bmpStopDisabled"],
		wxITEM_NORMAL, "Stop Animation" );
	mTbPlayer->AddControl( new wxTextCtrl( mTbPlayer, ID_txtParams,
		wxEmptyString, wxDefaultPosition, wxSize( 80, 24 ) )
		);
	mTbPlayer->AddSeparator();
	mTbPlayer->AddControl( new wxStaticText( mTbPlayer, ID_stxPlayTime, "00.00" ) );
	mTbPlayer->AddControl( new wxStaticText( mTbPlayer, -1, "/" ) );
	mTbPlayer->AddControl( new wxStaticText( mTbPlayer, ID_stxPlayLength, "00.00" ) );
	mTbPlayer->AddControl( new wxStaticText( mTbPlayer, -1, "s" ) );
	mTbPlayer->AddControl( new wxSlider( mTbPlayer, ID_slPlaySlider, 0, 0, 1000, wxDefaultPosition, wxSize( 480, 24 ) ) );
	mTbPlayer->AddSeparator();
	mTbPlayer->AddCheckTool( ID_btnSelect, "btnSelect",
		*mBitmaps["ID_bmpSelect"], *mBitmaps["ID_bmpSelectDisabled"],
		"Select Animation Segment" );
	mTbPlayer->AddTool( ID_btnDeselect, "Deselect",
		*mBitmaps["ID_bmpDeselect"], *mBitmaps["ID_bmpDeselectDisabled"],
		wxITEM_NORMAL, "Deselect Animation Segment" );
	mTbPlayer->Realize();

	// create transitions toolbar
	mTbTransitions = new wxToolBar( this, ID_tbTransitions, wxDefaultPosition, wxDefaultSize,
		wxNO_BORDER | wxTB_HORIZONTAL | wxTB_FLAT, "tbTransitions" );
	mTbTransitions->AddControl( new wxStaticText( mTbTransitions, -1, "Target Animation: " ) );
	mTbTransitions->AddControl( new wxComboBox( mTbTransitions, ID_cbTargetAnims, wxEmptyString,
		wxDefaultPosition, wxSize( 120, 24 ) )
		);
	mTbTransitions->Realize();

	// add mWndOgre to AUI manager
	wxAuiPaneInfo wndOgre_pi;
	wndOgre_pi.CentrePane();
	wndOgre_pi.Dock();
	wndOgre_pi.Floatable(false);
	wndOgre_pi.CloseButton(false);
	mAuiMgr.AddPane( mWndOgre, wndOgre_pi );

	// add mWndTimeline to AUI manager
	mWndTimeline->SetSize( 272, size.GetHeight() );
	wxAuiPaneInfo wndTimeline_pi;
	wndTimeline_pi.Top();
	wndTimeline_pi.Floatable();
	wndTimeline_pi.CloseButton();
	wndTimeline_pi.Caption( "Timeline" );
	wndTimeline_pi.Show(true);
	mAuiMgr.AddPane( mWndTimeline, wndTimeline_pi );

	// add mWndProjectView to AUI manager
	mWndProjectView->SetSize( 272, size.GetHeight() );
	wxAuiPaneInfo wndProjectView_pi;
	wndProjectView_pi.Left();
	wndProjectView_pi.Floatable();
	wndProjectView_pi.CloseButton();
	wndProjectView_pi.Caption( "Project View" );
	mAuiMgr.AddPane( mWndProjectView, wndProjectView_pi );

	// add mWndProperties to AUI manager
	mWndProperties->SetSize( 272, size.GetHeight() );
	wxAuiPaneInfo wndProperties_pi;
	wndProperties_pi.Left();
	wndProperties_pi.Floatable();
	wndProperties_pi.CloseButton();
	wndProperties_pi.Caption( "Properties" );
	mAuiMgr.AddPane( mWndProperties, wndProperties_pi );

	// add mWndAnimTree to AUI manager
	mWndAnimTree->SetSize( 272, size.GetHeight() );
	wxAuiPaneInfo wndAnimTree_pi;
	wndAnimTree_pi.Bottom();
	wndAnimTree_pi.Floatable();
	wndAnimTree_pi.CloseButton();
	wndAnimTree_pi.Caption( "Animation Tree" );
	mAuiMgr.AddPane( mWndAnimTree, wndAnimTree_pi );

	// add tbAnnots to AUI manager
	wxAuiPaneInfo tbAnnots_pi;
	tbAnnots_pi.ToolbarPane();
	tbAnnots_pi.Top();
	tbAnnots_pi.Floatable();
	tbAnnots_pi.CloseButton();
	mAuiMgr.AddPane( mTbAnnots, tbAnnots_pi );

	// add tbPlayer to AUI manager
	wxAuiPaneInfo tbPlayer_pi;
	tbPlayer_pi.ToolbarPane();
	tbPlayer_pi.Top();
	tbPlayer_pi.Floatable();
	tbPlayer_pi.CloseButton();
	mAuiMgr.AddPane( mTbPlayer, tbPlayer_pi );

	// add tbTransitions to AUI manager
	wxAuiPaneInfo tbTransitions_pi;
	tbTransitions_pi.ToolbarPane();
	tbTransitions_pi.Top();
	tbTransitions_pi.Floatable();
	tbTransitions_pi.CloseButton();
	mAuiMgr.AddPane( mTbTransitions, tbTransitions_pi );

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

PropertiesWindow* AnimationStudioFrame::getPropertiesWindow() const
{
	return mWndProperties;
}

AnimTreeWindow* AnimationStudioFrame::getAnimTreeWindow() const
{
	return mWndAnimTree;
}

bool AnimationStudioFrame::hasSelection() const
{
	return gApp->getCurrentAnimationNode() != NULL &&
		gApp->getCurrentAnimationNode()->getClassId() == AnimationSampler::ClassId() &&
		!mSelectMode && mSelectStart <= mSelectEnd;
}

AnimationSegment AnimationStudioFrame::getSelection() const
{
	zhAssert( hasSelection() );

	zh::Animation* anim = static_cast<AnimationSampler*>( gApp->getCurrentAnimationNode() )->getAnimation();

	return AnimationSegment( anim, mSelectStart * anim->getLength(), mSelectEnd * anim->getLength() );
}

void AnimationStudioFrame::OnMenu_FileNewProject( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnMenu_FileOpenProject( wxCommandEvent& evt )
{
	if( !gApp->loadProject() )
		return;

	// refresh controls
	_refreshComboBox_TargetAnims();
	mWndProjectView->refresh();
}

void AnimationStudioFrame::OnMenu_FileSaveProject( wxCommandEvent& evt )
{
	gApp->saveProject();
}

void AnimationStudioFrame::OnMenu_FileProjectSettings( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnMenu_FileCreateCharacter( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnMenu_FileCreateAnimSet( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnMenu_FileCreateAnimTree( wxCommandEvent& evt )
{
	Character* ch = gApp->getCurrentCharacter();
	gApp->createAnimationTree( ch->getId() );
	
	mWndProjectView->refresh();
}

void AnimationStudioFrame::OnMenu_FileAddRawAnimSet( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnMenu_FileAddAnimIndex( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnMenu_FileAddAnimSet( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnMenu_FileAddAnimTree( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnMenu_FileExit( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnMenu_EditUndo( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnMenu_EditRedo( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnMenu_EditCut( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnMenu_EditCopy( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnMenu_EditPaste( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnMenu_EditDelete( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnMenu_EditSelect( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnMenu_EditDeselect( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnMenu_ViewTimeline( wxCommandEvent& evt )
{
	mAuiMgr.GetPane( mWndTimeline ).Show( mbMain->IsChecked( ID_mnViewTimeline ) );
	mAuiMgr.Update();
}

void AnimationStudioFrame::OnMenu_ViewProject( wxCommandEvent& evt )
{
	mAuiMgr.GetPane( mWndProjectView ).Show( mbMain->IsChecked( ID_mnViewProject ) );
	mAuiMgr.Update();
}

void AnimationStudioFrame::OnMenu_ViewProperties( wxCommandEvent& evt )
{
	mAuiMgr.GetPane( mWndProperties ).Show( mbMain->IsChecked( ID_mnViewProperties ) );
	mAuiMgr.Update();
}

void AnimationStudioFrame::OnMenu_ViewAnimTree( wxCommandEvent& evt )
{
	mAuiMgr.GetPane( mWndAnimTree ).Show( mbMain->IsChecked( ID_mnViewAnimTree ) );
	mAuiMgr.Update();
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
	Model* mdl = NULL;
	zh::Animation* anim = NULL;
	if( gApp->getCurrentCharacter() != NULL )
		mdl = gApp->getCurrentCharacter()->getModelController()->getModel();
	if( gApp->getCurrentAnimationNode() != NULL )
		anim = static_cast<AnimationSampler*>( gApp->getCurrentAnimationNode() )->getAnimation();

	if( mdl == NULL || anim == NULL )
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

	// detect plant constraints
	const std::set<std::string>& ee_names = gApp->getEndEffectorSet( gApp->getCurrentCharacter()->getId() );
	for( std::set<std::string>::const_iterator eei = ee_names.begin(); eei != ee_names.end(); ++eei )
	{
		PlantConstrDetector* pcd = new PlantConstrDetector( mdl, anim );
		pcd->setMaxPosChange(max_poschange);
		pcd->setMinConstrLength(min_constrlength);
		pcd->detect(*eei);
		delete pcd;
	}

	// TODO: refresh appropriate displays
}

void AnimationStudioFrame::OnMenu_ToolsFootskateCleanup( wxCommandEvent& evt )
{
	Model* mdl = NULL;
	zh::Animation* anim = NULL;
	if( gApp->getCurrentCharacter() != NULL )
		mdl = gApp->getCurrentCharacter()->getModelController()->getModel();
	if( gApp->getCurrentAnimationNode() != NULL && gApp->getCurrentAnimationNode()->getClassId() == AnimationSampler::ClassId() )
		anim = static_cast<AnimationSampler*>( gApp->getCurrentAnimationNode() )->getAnimation();

	if( mdl == NULL || anim == NULL )
	{
		wxMessageBox( "Cannot clean up footskate errors. Character model or animation not loaded.",
			"Error", wxOK | wxICON_EXCLAMATION );
		return;
	}

	// TODO: pop settings dialog (solver class, solver settings)

	// clean up end-effectors
	PlantConstrDetector* pcd = new PlantConstrDetector( mdl, anim );
	pcd->cleanup();
	delete pcd;

	// TODO: refresh appropriate displays
}

void AnimationStudioFrame::OnMenu_ToolsMirror( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnMenu_ToolsBuildAnimIndex( wxCommandEvent& evt )
{
	Character* ch = gApp->getCurrentCharacter();
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
	_refreshComboBox_TargetAnims();
	mWndProjectView->refresh();
}

void AnimationStudioFrame::OnMenu_ToolsViewMatchWeb( wxCommandEvent& evt )
{
	Character* ch = gApp->getCurrentCharacter();
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
	}
}

void AnimationStudioFrame::OnMenu_ToolsSearchAnimIndex( wxCommandEvent& evt )
{
	Character* ch = gApp->getCurrentCharacter();
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
	_refreshComboBox_TargetAnims();
	mWndProjectView->refresh();
}

void AnimationStudioFrame::OnMenu_ToolsBuildMotionGraph( wxCommandEvent& evt )
{
	Character* ch = gApp->getCurrentCharacter();
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
		"Build Complete", wxOK|wxICON_INFORMATION );
}

void AnimationStudioFrame::OnMenu_ToolsViewMotionGraph( wxCommandEvent& evt )
{
	if( gApp->getCurrentCharacter() == NULL )
	{
		wxMessageBox( "Cannot view motion graph. Character model not loaded.",
			"Error", wxOK|wxICON_EXCLAMATION );
		return;
	}

	MotionGraphViewDialog dlg( this, wxID_ANY, MGView_Raw );
	dlg.ShowModal();
}

void AnimationStudioFrame::OnMenu_ToolsCreateAnimFromSeg( wxCommandEvent& evt )
{
	Character* ch = gApp->getCurrentCharacter();
	Model* mdl = ch->getModelController()->getModel();
	AnimationSetPtr anim_set;
	zh::Animation *raw_anim, *anim;

	if( !hasSelection() )
	{
		wxMessageBox( "You must select the animation segment first.", "Select Animation Segment",
			wxOK|wxICON_INFORMATION, this );
		return;
	}

	AnimationSegment anim_seg = getSelection();
	raw_anim = anim_seg.getAnimation();
	
	// get the list of animation sets
	wxArrayString anims_str;
	AnimationStudioApp::AnimationSetConstIterator animset_i = gApp->getAnimationSetConstIterator( ch->getId() );
	while( !animset_i.end() )
	{
		std::string animset_name = animset_i.next()->getName();
		anims_str.push_back(animset_name);
	}

	// prompt the user to select an animation set
	wxSingleChoiceDialog dlg( this, "Select the animation set for the new animation:", "Select Animation Set", anims_str );
	if( dlg.ShowModal() != wxID_OK )
	{
		// user cancelled
		return;
	}
	anim_set = gApp->getAnimationSet( ch->getId(), dlg.GetStringSelection().c_str() );

	// prompt the user to specify anim. name
	std::string anim_name = "NewAnim";
	unsigned int i = 0;
	while( anim_set->hasAnimation( anim_name + toString<unsigned int>(i) ) )
		++i;
	anim_name += toString<unsigned int>(i);
	wxTextEntryDialog dlg2( this, "Specify animation name:", "Animation Name", anim_name );
	if( dlg2.ShowModal() != wxID_OK )
	{
		// user cancelled
		return;
	}
	
	// get anim. name
	anim_name = dlg2.GetValue().c_str();
	if( anim_set->hasAnimation(anim_name) )
	{
		wxMessageBox( "Animation " + anim_name + " already exists in the animation set.",
			"Error", wxOK|wxICON_EXCLAMATION );
		return;
	}

	// create new animation
	unsigned short anim_id = 0;
	while( anim_set->hasAnimation(anim_id) ) ++anim_id;
	anim = anim_set->createAnimation( anim_id, anim_name );

	// fill up animation with key-frames
	// first create bone tracks
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
	// then create mesh tracks
	zh::Animation::MeshTrackConstIterator mti = raw_anim->getMeshTrackConstIterator();
	while( !mti.end() )
	{
		MeshAnimationTrack* rmat = mti.next();
		MeshAnimationTrack* mat = anim->createMeshTrack( rmat->getMeshId() );
		
		// create initial key-frame
		zh::MorphKeyFrame* mkf = static_cast<MorphKeyFrame*>( mat->createKeyFrame(0) );
		rmat->getInterpolatedKeyFrame( anim_seg.getStartTime(), mkf );

		// create final key-frame
		mkf = static_cast<MorphKeyFrame*>( mat->createKeyFrame( anim_seg.getEndTime() - anim_seg.getStartTime() ) );
		rmat->getInterpolatedKeyFrame( anim_seg.getEndTime(), mkf );

		// copy intervening key-frames
		for( unsigned int kfi = 0; kfi < rmat->getNumKeyFrames(); ++kfi )
		{
			zh::MorphKeyFrame* rmkf = static_cast<MorphKeyFrame*>( rmat->getKeyFrame(kfi) );
			
			if( rmkf->getTime() <= anim_seg.getStartTime() )
				continue;
			if( rmkf->getTime() >= anim_seg.getEndTime() )
				break;

			mkf = static_cast<MorphKeyFrame*>( mat->createKeyFrame( rmkf->getTime() - anim_seg.getStartTime() ) );
			mkf->setMorphTargetWeights( rmkf->getMorphTargetWeights() );
		}
	}

	// TODO: copy annots as well

	// add corresponding nodes to the internal animation tree
	gApp->stopAnimation();
	ch->getAnimationController()->removeAnimationTree(zhIntAnimTree_Name);
	AnimationTreePtr atree_int = AnimationTreePtr::DynamicCast<zh::Resource>( zhAnimationSystem->getAnimationTreeManager()->getResource(zhIntAnimTree_Name) );
	gApp->_addAnimsToAnimTree( anim_set, atree_int, atree_int->getRoot() );
	ch->getAnimationController()->addAnimationTree(atree_int);
	gApp->selectAnimationTree(zhIntAnimTree_Name);

	// refresh controls
	_refreshComboBox_TargetAnims();
	mWndProjectView->refresh();
}

void AnimationStudioFrame::OnMenu_ToolsBuildAnimSpace( wxCommandEvent& evt )
{
	// TODO: implement this

	// refresh controls
	_refreshComboBox_TargetAnims();
	mWndProjectView->refresh();
}

void AnimationStudioFrame::OnMenu_ToolsBuildTransitions( wxCommandEvent& evt )
{
	Model* mdl = NULL;
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
	}
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

void AnimationStudioFrame::OnMenu_AnimTreeCreateAnimSampler( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnMenu_AnimTreeCreateFaceController( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnMenu_AnimTreeCreateAnimBlender( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnMenu_AnimTreeCreateAnimTransBlender( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnMenu_AnimTreeCreateAnimMixer( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnMenu_AnimTreeCreateNodeOther( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnMenu_AnimTreeCreateBoneTransfController( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnMenu_AnimTreeCreateBoneIKController( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnMenu_AnimTreeCreateBoneControllerOther( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnMenu_AnimTreeCreateNodeForEachAnim( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnMenu_AnimTreeAutoArrangeNodes( wxCommandEvent& evt )
{
}

void AnimationStudioFrame::OnMenu_HelpManual( wxCommandEvent& evt )
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
	if( gApp->getCurrentAnimationTree() != gApp->getInternalAnimationTree() )
	{
		return;
	}

	// TODO: remove this
	/*
	if( gApp->getCharacter() != NULL &&
		gApp->getCharacter()->getName() == "Ninja" )
	{
		AnimationTree* atree = gApp->getAnimationTree();

		AnimationTransitionBlender* root = static_cast<AnimationTransitionBlender*>( atree->getRoot() );
		root->addTransition( "Idle1" );
		root->addTransition( "Idle2" );
		root->addTransition(
			AnimationTransitionBlender::Transition( 3.1666, 3.6666, 0, root->getChild( "Walk" ), Vector() )
			);
		root->addTransition( "Walk" );
		root->addTransition( "Walk" );
		root->addTransition( "Walk" );
		root->addTransition( "Attack1" );
		root->addTransition( "Block" );
		root->addTransition( "Attack2" );
		root->addTransition( "Spin" );
		root->addTransition( "Kick" );
		root->addTransition( "Crouch" );
		root->addTransition( "Jump" );
		root->addTransition( "Attack3" );
		root->addTransition( "Attack1" );
		root->addTransition( "Block" );
		root->addTransition( "Death1" );
	}
	*/
	//

	AnimationNode* root = gApp->getCurrentAnimationTree()->getRoot();
	
	if( !root->getPlaying() )
		root->setPlaying();

	root->setPaused( !evt.IsChecked() );
}

void AnimationStudioFrame::OnTool_Stop( wxCommandEvent& evt )
{
	if( gApp->getCurrentAnimationTree() != gApp->getInternalAnimationTree() )
	{
		return;
	}

	AnimationNode* root = gApp->getCurrentAnimationTree()->getRoot();
	root->setPlaying(false);
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

void AnimationStudioFrame::OnScroll_PlaySlider( wxScrollEvent& evt )
{
	if( gApp->getCurrentAnimationTree() != gApp->getInternalAnimationTree() )
	{
		return;
	}

	AnimationNode* root = gApp->getCurrentAnimationTree()->getRoot();
	root->setPlayTime( evt.GetInt()/1000.f * root->getPlayLength() );
}

void AnimationStudioFrame::OnComboBox_TargetAnims( wxCommandEvent& evt )
{
	std::string anim_name = evt.GetString().c_str();

	if( anim_name == "<none>" )
		return;

	AnimationTransitionBlender* root = static_cast<AnimationTransitionBlender*>( gApp->getCurrentAnimationTree()->getRoot() );
	if( root->hasChild(anim_name) )
		root->addTransition(anim_name);
}

void AnimationStudioFrame::OnIdle( wxIdleEvent& evt )
{
	AnimationNode* anim_node = gApp->getCurrentAnimationNode();

	if( anim_node == NULL )
	{
		return;
	}
	
	float play_time = anim_node->getPlayTime(),
		play_length = anim_node->getPlayLength();
	char play_time_str[100];
	char play_length_str[100];
	sprintf( play_time_str, "%.2f", play_time );
	sprintf( play_length_str, "%.2f", play_length );

	// update play time display
	wxStaticText* stx_playtime = static_cast<wxStaticText*>( FindWindowById( ID_stxPlayTime, mTbPlayer ) );
	wxStaticText* stx_playlength = static_cast<wxStaticText*>( FindWindowById( ID_stxPlayLength, mTbPlayer ) );
	stx_playtime->SetLabel(play_time_str);
	stx_playlength->SetLabel(play_length_str);

	// update play slider
	wxSlider* sl_play = static_cast<wxSlider*>( FindWindowById( ID_slPlaySlider, mTbPlayer ) );
	sl_play->SetValue( anim_node->getPlayTime()/anim_node->getPlayLength() * 1000.f );
}

void AnimationStudioFrame::_refreshComboBox_TargetAnims()
{
	// clear target anims list
	wxComboBox* cb_tanims = static_cast<wxComboBox*>( FindWindowById( ID_cbTargetAnims, mTbTransitions ) );
	cb_tanims->Clear();
	cb_tanims->Append( "<none>" );

	AnimationTransitionBlender* root = static_cast<AnimationTransitionBlender*>( gApp->getCurrentAnimationTree()->getRoot() );
	AnimationNode::ChildConstIterator child_i = root->getChildConstIterator();
	while( child_i.hasMore() )
	{
		AnimationNode* child = child_i.next();
		cb_tanims->Append( child->getName() );
	}
}

BEGIN_EVENT_TABLE( AnimationStudioFrame, wxFrame )
	EVT_MENU( ID_mnFileNewProject, AnimationStudioFrame::OnMenu_FileNewProject )
	EVT_MENU( ID_mnFileOpenProject, AnimationStudioFrame::OnMenu_FileOpenProject )
	EVT_MENU( ID_mnFileSaveProject, AnimationStudioFrame::OnMenu_FileSaveProject )
	EVT_MENU( ID_mnFileProjectSettings, AnimationStudioFrame::OnMenu_FileProjectSettings )
	EVT_MENU( ID_mnFileCreateCharacter, AnimationStudioFrame::OnMenu_FileCreateCharacter )
	EVT_MENU( ID_mnFileCreateAnimSet, AnimationStudioFrame::OnMenu_FileCreateAnimSet )
	EVT_MENU( ID_mnFileCreateAnimTree, AnimationStudioFrame::OnMenu_FileCreateAnimTree )
	EVT_MENU( ID_mnFileAddRawAnimSet, AnimationStudioFrame::OnMenu_FileAddRawAnimSet )
	EVT_MENU( ID_mnFileAddAnimIndex, AnimationStudioFrame::OnMenu_FileAddAnimIndex )
	EVT_MENU( ID_mnFileAddAnimSet, AnimationStudioFrame::OnMenu_FileAddAnimSet )
	EVT_MENU( ID_mnFileAddAnimTree, AnimationStudioFrame::OnMenu_FileAddAnimTree )
	EVT_MENU( ID_mnFileExit, AnimationStudioFrame::OnMenu_FileExit )
	EVT_MENU( ID_mnEditUndo, AnimationStudioFrame::OnMenu_EditUndo )
	EVT_MENU( ID_mnEditRedo, AnimationStudioFrame::OnMenu_EditRedo )
	EVT_MENU( ID_mnEditCut, AnimationStudioFrame::OnMenu_EditCut )
	EVT_MENU( ID_mnEditCopy, AnimationStudioFrame::OnMenu_EditCopy )
	EVT_MENU( ID_mnEditPaste, AnimationStudioFrame::OnMenu_EditPaste )
	EVT_MENU( ID_mnEditDelete, AnimationStudioFrame::OnMenu_EditDelete )
	EVT_MENU( ID_mnEditSelect, AnimationStudioFrame::OnMenu_EditSelect )
	EVT_MENU( ID_mnEditDeselect, AnimationStudioFrame::OnMenu_EditDeselect )
	EVT_MENU( ID_mnViewTimeline, AnimationStudioFrame::OnMenu_ViewTimeline )
	EVT_MENU( ID_mnViewProject, AnimationStudioFrame::OnMenu_ViewProject )
	EVT_MENU( ID_mnViewProperties, AnimationStudioFrame::OnMenu_ViewProperties )
	EVT_MENU( ID_mnViewAnimTree, AnimationStudioFrame::OnMenu_ViewAnimTree )
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
	EVT_MENU( ID_mnAnimTreeCreateAnimSampler, AnimationStudioFrame::OnMenu_AnimTreeCreateAnimSampler )
	EVT_MENU( ID_mnAnimTreeCreateFaceController, AnimationStudioFrame::OnMenu_AnimTreeCreateFaceController )
	EVT_MENU( ID_mnAnimTreeCreateAnimBlender, AnimationStudioFrame::OnMenu_AnimTreeCreateAnimBlender )
	EVT_MENU( ID_mnAnimTreeCreateAnimTransBlender, AnimationStudioFrame::OnMenu_AnimTreeCreateAnimTransBlender )
	EVT_MENU( ID_mnAnimTreeCreateAnimMixer, AnimationStudioFrame::OnMenu_AnimTreeCreateAnimMixer )
	EVT_MENU( ID_mnAnimTreeCreateNodeOther, AnimationStudioFrame::OnMenu_AnimTreeCreateNodeOther )
	EVT_MENU( ID_mnAnimTreeCreateBoneTransfController, AnimationStudioFrame::OnMenu_AnimTreeCreateBoneTransfController )
	EVT_MENU( ID_mnAnimTreeCreateBoneIKController, AnimationStudioFrame::OnMenu_AnimTreeCreateBoneIKController )
	EVT_MENU( ID_mnAnimTreeCreateBoneControllerOther, AnimationStudioFrame::OnMenu_AnimTreeCreateBoneControllerOther )
	EVT_MENU( ID_mnAnimTreeCreateNodeForEachAnim, AnimationStudioFrame::OnMenu_AnimTreeCreateNodeForEachAnim )
	EVT_MENU( ID_mnAnimTreeAutoArrangeNodes, AnimationStudioFrame::OnMenu_AnimTreeAutoArrangeNodes )
	EVT_MENU( ID_mnHelpManual, AnimationStudioFrame::OnMenu_HelpManual )
	EVT_MENU( ID_mnHelpAbout, AnimationStudioFrame::OnMenu_HelpAbout )
	EVT_TOOL( ID_btnAnnotStart, AnimationStudioFrame::OnTool_AnnotStart )
	EVT_COMBOBOX( ID_cbAnnots, AnimationStudioFrame::OnComboBox_Annots )
	EVT_TOOL( ID_btnClearAnnots, AnimationStudioFrame::OnTool_ClearAnnots )
	EVT_TOOL( ID_btnPlay, AnimationStudioFrame::OnTool_Play )
	EVT_TOOL( ID_btnStop, AnimationStudioFrame::OnTool_Stop )
	EVT_COMMAND_SCROLL( ID_slPlaySlider, AnimationStudioFrame::OnScroll_PlaySlider )
	EVT_TOOL( ID_btnSelect, AnimationStudioFrame::OnTool_Select )
	EVT_TOOL( ID_btnDeselect, AnimationStudioFrame::OnTool_Deselect )
	EVT_COMBOBOX( ID_cbTargetAnims, AnimationStudioFrame::OnComboBox_TargetAnims )
	EVT_IDLE( AnimationStudioFrame::OnIdle )
END_EVENT_TABLE()
