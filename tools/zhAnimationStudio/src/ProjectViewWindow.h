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
	ID_mnSkeletonSelect,
	ID_mnSkeletonRemove,
	/*ID_mnAnimIndexRemove,
	ID_mnAnimIndexRename,
	ID_mnAnimSetRemove,
	ID_mnAnimSetRename,
	ID_mnAnimSetClear,*/
	ID_mnAnimSelect,
	ID_mnAnimRemove,
	ID_mnAnimRename/*,
	ID_mnAnimSpaceRemove,
	ID_mnAnimSpaceRename,
	ID_mnAnimSpaceDefineParam,
	ID_mnAnimSpaceMatchAnnots,
	ID_mnAnimSpaceBuildBlendCurves*/

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
	* Updates the project view contents.
	*/
	void refresh();

	bool Show( bool show = true );

	void OnSize( wxSizeEvent& evt );
	void OnDrag_ProjTree( wxTreeEvent& evt );
	void OnDblClick_ProjTree( wxTreeEvent& evt );
	void OnRightClick_ProjTree( wxTreeEvent& evt );
	void OnMenu_SkeletonSelect( wxCommandEvent& evt );
	void OnMenu_SkeletonRemove( wxCommandEvent& evt );
	/*void OnMenu_AnimIndexRemove( wxCommandEvent& evt );
	void OnMenu_AnimIndexRename( wxCommandEvent& evt );
	void OnMenu_AnimSetRemove( wxCommandEvent& evt );
	void OnMenu_AnimSetRename( wxCommandEvent& evt );
	void OnMenu_AnimSetClear( wxCommandEvent& evt );*/
	void OnMenu_AnimSelect( wxCommandEvent& evt );
	void OnMenu_AnimRemove( wxCommandEvent& evt );
	void OnMenu_AnimRename( wxCommandEvent& evt );
	/*void OnMenu_AnimSpaceDefineParam( wxCommandEvent& evt );
	void OnMenu_AnimSpaceMatchAnnots( wxCommandEvent& evt );
	void OnMenu_AnimSpaceBuildBlendCurves( wxCommandEvent& evt );*/

	DECLARE_EVENT_TABLE()

private:

	wxTreeCtrl* trcProjTree;

};

#endif // __ProjectViewWindow_h__
