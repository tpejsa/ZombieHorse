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

#ifndef __ProjectViewWindow_h__
#define __ProjectViewWindow_h__

#include "AnimationStudio.h"
#include <wx/treectrl.h>

enum
{
	ID_trcProjTree,
	ID_mnProjectSave,
	ID_mnProjectSettings,
	ID_mnProjectCreateCharacter,
	ID_mnProjectClearCharacters,
	ID_mnCharacterSelect,
	ID_mnCharacterRemove,
	ID_mnCharacterRename,
	ID_mnCharacterSave,
	ID_mnRawAnimSetsAdd,
	ID_mnRawAnimSetsClear,
	ID_mnRawAnimSetsBuildMotionGraph,
	ID_mnRawAnimSetsViewMotionGraph,
	ID_mnRawAnimSetRemove,
	ID_mnRawAnimSetRename,
	ID_mnRawAnimSetClear,
	ID_mnRawAnimSelect,
	ID_mnRawAnimRemove,
	ID_mnRawAnimRename,
	ID_mnAnimIndexesBuildIndex,
	ID_mnAnimIndexesAddIndex,
	ID_mnAnimIndexesClearIndexes,
	ID_mnAnimIndexesViewMatchWeb,
	ID_mnAnimIndexesSearch,
	ID_mnAnimIndexRemove,
	ID_mnAnimIndexRename,
	ID_mnAnimSetsCreate,
	ID_mnAnimSetsAdd,
	ID_mnAnimSetsClear,
	ID_mnAnimSetsBuildTransitions,
	ID_mnAnimSetsViewMotionGraph2,
	ID_mnAnimSetRemove,
	ID_mnAnimSetRename,
	ID_mnAnimSetClear,
	ID_mnBaseAnimSelect,
	ID_mnBaseAnimRemove,
	ID_mnBaseAnimRename,
	ID_mnAnimSpacesBuildAnimSpace,
	ID_mnAnimSpacesClear,
	ID_mnAnimSpaceSelect,
	ID_mnAnimSpaceRemove,
	ID_mnAnimSpaceRename,
	ID_mnAnimSpaceDefineParam,
	ID_mnAnimSpaceMatchAnnots,
	ID_mnAnimSpaceBuildBlendCurves,
	ID_mnAnimTreesCreate,
	ID_mnAnimTreesAdd,
	ID_mnAnimTreesClear,
	ID_mnAnimTreeSelect,
	ID_mnAnimTreeRemove,
	ID_mnAnimTreeRename

};

/**
* Project resources view and management window.
*/
class ProjectViewWindow : public wxWindow
{

public:

	/**
	* Constructor.
	*/
	ProjectViewWindow( wxWindow *parent, wxWindowID id );

	/**
	* Refreshes the project view.
	*/
	void refresh();

	bool Show( bool show = true );

	void OnSize( wxSizeEvent& evt );
	void OnRightClick_ProjTree( wxTreeEvent& evt );
	void OnMenu_ProjectSave( wxCommandEvent& evt );
	void OnMenu_ProjectSettings( wxCommandEvent& evt );
	void OnMenu_ProjectCreateCharacter( wxCommandEvent& evt );
	void OnMenu_ProjectClearCharacters( wxCommandEvent& evt );
	void OnMenu_CharacterSelect( wxCommandEvent& evt );
	void OnMenu_CharacterRemove( wxCommandEvent& evt );
	void OnMenu_CharacterRename( wxCommandEvent& evt );
	void OnMenu_CharacterSave( wxCommandEvent& evt );
	void OnMenu_RawAnimSetsAdd( wxCommandEvent& evt );
	void OnMenu_RawAnimSetsClear( wxCommandEvent& evt );
	void OnMenu_RawAnimSetsBuildMotionGraph( wxCommandEvent& evt );
	void OnMenu_RawAnimSetsViewMotionGraph( wxCommandEvent& evt );
	void OnMenu_RawAnimSetRemove( wxCommandEvent& evt );
	void OnMenu_RawAnimSetRename( wxCommandEvent& evt );
	void OnMenu_RawAnimSetClear( wxCommandEvent& evt );
	void OnMenu_RawAnimSelect( wxCommandEvent& evt );
	void OnMenu_RawAnimRemove( wxCommandEvent& evt );
	void OnMenu_RawAnimRename( wxCommandEvent& evt );
	void OnMenu_AnimIndexesBuildIndex( wxCommandEvent& evt );
	void OnMenu_AnimIndexesAddIndex( wxCommandEvent& evt );
	void OnMenu_AnimIndexesClearIndexes( wxCommandEvent& evt );
	void OnMenu_AnimIndexesViewMatchWeb( wxCommandEvent& evt );
	void OnMenu_AnimIndexesSearch( wxCommandEvent& evt );
	void OnMenu_AnimIndexRemove( wxCommandEvent& evt );
	void OnMenu_AnimIndexRename( wxCommandEvent& evt );
	void OnMenu_AnimSetsCreate( wxCommandEvent& evt );
	void OnMenu_AnimSetsAdd( wxCommandEvent& evt );
	void OnMenu_AnimSetsClear( wxCommandEvent& evt );
	void OnMenu_AnimSetsBuildTransitions( wxCommandEvent& evt );
	void OnMenu_AnimSetsViewMotionGraph2( wxCommandEvent& evt );
	void OnMenu_AnimSetRemove( wxCommandEvent& evt );
	void OnMenu_AnimSetRename( wxCommandEvent& evt );
	void OnMenu_AnimSetClear( wxCommandEvent& evt );
	void OnMenu_BaseAnimSelect( wxCommandEvent& evt );
	void OnMenu_BaseAnimRemove( wxCommandEvent& evt );
	void OnMenu_BaseAnimRename( wxCommandEvent& evt );
	void OnMenu_AnimSpacesBuildAnimSpace( wxCommandEvent& evt );
	void OnMenu_AnimSpacesClear( wxCommandEvent& evt );
	void OnMenu_AnimSpaceSelect( wxCommandEvent& evt );
	void OnMenu_AnimSpaceRemove( wxCommandEvent& evt );
	void OnMenu_AnimSpaceRename( wxCommandEvent& evt );
	void OnMenu_AnimSpaceDefineParam( wxCommandEvent& evt );
	void OnMenu_AnimSpaceMatchAnnots( wxCommandEvent& evt );
	void OnMenu_AnimSpaceBuildBlendCurves( wxCommandEvent& evt );
	void OnMenu_AnimTreesCreate( wxCommandEvent& evt );
	void OnMenu_AnimTreesAdd( wxCommandEvent& evt );
	void OnMenu_AnimTreesClear( wxCommandEvent& evt );
	void OnMenu_AnimTreeSelect( wxCommandEvent& evt );
	void OnMenu_AnimTreeRemove( wxCommandEvent& evt );
	void OnMenu_AnimTreeRename( wxCommandEvent& evt );

	DECLARE_EVENT_TABLE()

private:

	wxTreeCtrl* trcProjTree;

};

#endif // __ProjectViewWindow_h__
