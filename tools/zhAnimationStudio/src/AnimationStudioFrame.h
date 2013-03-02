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

#ifndef __AnimationStudioFrame_h__
#define __AnimationStudioFrame_h__

#include "AnimationStudio.h"

#include "wx/aui/aui.h"

class AnimationStudioApp;
class OgreWindow;
class TimelineWindow;
class ProjectViewWindow;
class PropertiesWindow;
class AnimTreeWindow;

enum
{
	ID_frmMain,
	ID_mnViewShowSkel,
	ID_mnViewShowGround,
	ID_mnViewShowSkybox,
	ID_mnTools,
	ID_mnToolsDefineEndEffectors,
	ID_mnToolsDetectPlantConstr,
	ID_mnToolsFootskateCleanup,
	ID_mnToolsMirror,
	ID_mnToolsBuildAnimIndex,
	ID_mnToolsViewMatchWeb,
	ID_mnToolsSearchAnimIndex,
	ID_mnToolsBuildMotionGraph,
	ID_mnToolsViewMotionGraph,
	ID_mnToolsCreateAnimFromSeg,
	ID_mnToolsBuildAnimSpace,
	ID_mnToolsBuildTransitions,
	ID_mnToolsViewMotionGraph2,
	ID_mnToolsDefineParam,
	ID_mnToolsMatchAnnots,
	ID_mnToolsBuildBlendCurves,
	ID_mnHelp,
	ID_mnHelpAbout,
	ID_dlgProjectSettings,
	ID_dlgBuildAnimIndex,
	ID_dlgFindSimilarMotion,
	ID_tbAnnots,
	ID_btnAnnotStart,
	ID_cbAnnots,
	ID_btnClearAnnots,
	ID_tbPlayer,
	ID_btnPlay,
	ID_btnStop,
	ID_txtParams,
	ID_stxPlayTime,
	ID_stxPlayLength,
	ID_slPlaySlider,
	ID_btnSelect,
	ID_btnDeselect,
	ID_wndOgre,
	ID_wndTimeline,
	ID_wndProjectView
};

/**
* AnimationStudio main window.
*/
class AnimationStudioFrame : public wxFrame
{

public:

	/**
	* Constructor.
	*/
	AnimationStudioFrame( wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size );

	/**
	* Destructor.
	*/
	~AnimationStudioFrame();

	/**
	* Gets a pointer to the OGRE rendering window.
	*/
	OgreWindow* getOgreWindow() const;

	/**
	* Gets a pointer to the timeline window.
	*/
	TimelineWindow* getTimelineWindow() const;

	/**
	* Gets a pointer to the project view window.
	*/
	ProjectViewWindow* getProjectViewWindow() const;

	/**
	* true if an animation segment is selected,
	* false otherwise.
	*/
	bool hasSelection() const;

	/**
	* Gets the currently selected animation segment.
	*/
	AnimationSegment getSelection() const;

	void OnMenu_ViewShowSkel( wxCommandEvent& evt );
	void OnMenu_ViewShowGround( wxCommandEvent& evt );
	void OnMenu_ViewShowSkybox( wxCommandEvent& evt );
	void OnMenu_ToolsDefineEndEffectors( wxCommandEvent& evt );
	void OnMenu_ToolsDetectPlantConstr( wxCommandEvent& evt );
	void OnMenu_ToolsFootskateCleanup( wxCommandEvent& evt );
	void OnMenu_ToolsMirror( wxCommandEvent& evt );
	void OnMenu_ToolsBuildAnimIndex( wxCommandEvent& evt );
	void OnMenu_ToolsViewMatchWeb( wxCommandEvent& evt );
	void OnMenu_ToolsSearchAnimIndex( wxCommandEvent& evt );
	void OnMenu_ToolsBuildMotionGraph( wxCommandEvent& evt );
	void OnMenu_ToolsViewMotionGraph( wxCommandEvent& evt );
	void OnMenu_ToolsCreateAnimFromSeg( wxCommandEvent& evt );
	void OnMenu_ToolsBuildAnimSpace( wxCommandEvent& evt );
	void OnMenu_ToolsBuildTransitions( wxCommandEvent& evt );
	void OnMenu_ToolsViewMotionGraph2( wxCommandEvent& evt );
	void OnMenu_ToolsDefineParam( wxCommandEvent& evt );
	void OnMenu_ToolsMatchAnnots( wxCommandEvent& evt );
	void OnMenu_ToolsBuildBlendCurves( wxCommandEvent& evt );
	void OnMenu_HelpAbout( wxCommandEvent& evt );
	void OnTool_AnnotStart( wxCommandEvent& evt );
	void OnComboBox_Annots( wxCommandEvent& evt );
	void OnTool_ClearAnnots( wxCommandEvent& evt );
	void OnTool_Play( wxCommandEvent& evt );
	void OnTool_Stop( wxCommandEvent& evt );
	void OnScroll_PlaySlider( wxScrollEvent& evt );
	void OnTool_Select( wxCommandEvent& evt );
	void OnTool_Deselect( wxCommandEvent& evt );
	void OnIdle( wxIdleEvent& evt );

	DECLARE_EVENT_TABLE()

protected:

	void _refreshComboBox_TargetAnims();

	wxAuiManager mAuiMgr;
	std::map<std::string, wxBitmap*> mBitmaps;
	wxMenuBar* mbMain; ///< Pointer to the main menu.
	wxToolBar* mTbAnnots; ///< Pointer to the annotation toolbar.
	wxToolBar* mTbPlayer; ///< Pointer to the player toolbar.

	bool mSelectMode;
	float mSelectStart, mSelectEnd;

	OgreWindow* mWndOgre;
	TimelineWindow* mWndTimeline;
	ProjectViewWindow* mWndProjectView;

};

#endif // __AnimationStudioFrame_h__
