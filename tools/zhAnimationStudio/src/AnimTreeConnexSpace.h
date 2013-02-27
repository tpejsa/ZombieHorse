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

#ifndef __AnimTreeConnexSpace_h__
#define __AnimTreeConnexSpace_h__

#include "AnimTreeWindow.h"

enum
{
	ID_wndAnimTreeConnexSpace
};

class AnimTreeConnexSpace : public wxConnexSpace
{

public:

	/**
	* Constructor.
	*/
	AnimTreeConnexSpace( wxWindow *parent, wxWindowID id, wxPoint& pos, wxSize& size, long style, 
		const wxValidator& validator = wxDefaultValidator, const wxString& name = wxPanelNameStr );
	
	void OnSize( wxSizeEvent& evt );

	DECLARE_EVENT_TABLE()

private:

	wxConnexSpace* mWndAnimTreeConnexSpace;

public:
	std::vector<AnimationNode*> tree_level;
	std::vector<std::vector<AnimationNode*>> mNodeGrid;
	std::set<AnimationNode*> enum_Nodes;
	std::vector<BoneController*> jc_level;
	std::vector<std::vector<BoneController*>> mJCGrid;
	std::set<BoneController*> enum_JC;

	void autoArrangeNodes( AnimationTreeInstance* anim_tree );
	void enumNodes( int level, AnimationNode* node );
	void enumJC( int level, BoneController* node );
	void addNodeToAnimTree( wxConnexNode* node );
	void removeNodeFromAnimTree( unsigned short id );
	void addJCToAnimTree( wxConnexNode* node );
	void removeJCFromAnimTree( unsigned short id );
	void updateNodeConnections( wxConnexNode* parent, wxConnexNode* child );
	int	getTreeLevel( int level, AnimationNode* parent );

	void RemoveNode( unsigned short id );
	void ConnectPorts( wxConnexPortBase *port0, wxConnexPortBase *port1 );
	void RemoveConnection( wxConnexPortBase *port);
	void SetRoot( wxConnexNode* root );
	void SetFirst( wxConnexNode* first );
	void SetSelectedNode( wxConnexNode* selected );
	void UpdateNodeName( wxConnexNode* node );
	void OnMouseEvent( wxMouseEvent& evt );

	wxConnexNode* createAnimationNode( std::string class_name, unsigned short node_id, std::string node_name );

	//right click menu events
	void OnMenu_CreateAnimationSampler( wxCommandEvent& evt );
	void OnMenu_CreateFaceController( wxCommandEvent& evt );
	void OnMenu_CreateAnimationBlender( wxCommandEvent& evt );
	void OnMenu_CreateAnimationTransitionBlender( wxCommandEvent& evt );
	void OnMenu_CreateAnimationMixer( wxCommandEvent& evt );
	void OnMenu_CreateBoneTransformController( wxCommandEvent& evt );
	void OnMenu_CreateBoneIKController( wxCommandEvent& evt );
	void OnMenu_CreateNodeForEachAnim( wxCommandEvent& evt );
	void OnMenu_AutoArrangeNodes( wxCommandEvent& evt );

protected:
	wxConnexNode* _createAnimationSampler( unsigned short node_id, std::string node_name );
	wxConnexNode* _createAnimationBlender( unsigned short node_id, std::string node_name );
	wxConnexNode* _createAnimationMixer( unsigned short node_id, std::string node_name );
	wxConnexNode* _createAnimationTransitionBlender( unsigned short node_id, std::string node_name );
	wxConnexNode* _createFaceController( unsigned short node_id, std::string node_name );
	wxConnexNode* _createBoneTransformController( unsigned short node_id, std::string node_name );
	wxConnexNode* _createBoneIKController( unsigned short node_id, std::string node_name );

private:
	wxScrolledWindow* mWndAnimTreeConnexSpaceScroll;	

};

#endif // __AnimTreeConnexSpace_h__