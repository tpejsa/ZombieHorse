/*
Copyright (c) 2008 Erik Beran

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef _CONNEX_SPACE_H
#define _CONNEX_SPACE_H

#include "wx/window.h"
#include <list>
#include <wx/scrolwin.h>

// menu list elements
enum
{
	ID_mnCreateAnimationSampler,
	ID_mnCreateFaceController,
	ID_mnCreateAnimationBlender,
	ID_mnCreateAnimationTransitionBlender,
	ID_mnCreateAnimationMixer,
	ID_mnCreateBoneTransformController,
	ID_mnCreateBoneIKController,
	ID_mnCreateNodeForEachAnim,
	ID_mnAutoArrangeNodes
};
//

class wxConnexNode;
class wxConnexPortBase;

class wxConnexSpace : public wxWindow
{
public:
	typedef std::list<wxConnexNode*>	NodeList;
	typedef NodeList::iterator			NodeListIterator;

	//
	typedef std::list<unsigned short>	NodeIdList;
	typedef NodeIdList::iterator		NodeIdListIterator;
	//

	enum MOUSE_MODE
	{
		MOUSE_DRAG,
		MOUSE_CONNECT,
	};
public:
	bool			m_DrawConnectionShadows;
	MOUSE_MODE		m_MouseMode;
	int				m_MouseFromPlace;
	int				m_MouseToPlace;
	wxConnexNode	*m_FocusNode;
	wxPoint			m_LastMousePos;

	NodeList		m_NodeList;

	NodeIdList		m_NodeIdList;

	wxSize			window_size;

	wxConnexNode*	m_root;
	wxConnexNode*	m_first;
	wxConnexNode*	m_selected;

public:
	wxConnexSpace();
	~wxConnexSpace();
	wxConnexSpace(	wxWindow *parent, wxWindowID id = wxID_ANY,
					const wxPoint& pos = wxDefaultPosition,
					const wxSize& size = wxDefaultSize, long style = wxNO_BORDER,
					const wxValidator& validator = wxDefaultValidator,
					const wxString& name = wxPanelNameStr );

	void AddNode( wxConnexNode *node );
	
	//node controls
	wxConnexNode* FindNode( unsigned short id );
	wxConnexNode* FindNode( const std::string name );
	virtual void RemoveNode( unsigned short id );
	void RemoveNode( const std::string& name );
	void RemoveAllNodes();
	wxConnexNode* GetRoot();
	virtual void SetRoot( wxConnexNode* root );
	wxConnexNode* GetFirst();
	virtual void SetFirst( wxConnexNode* first );
	wxConnexNode* GetSelectedNode();
	virtual void SetSelectedNode( wxConnexNode* selected );
	virtual void UpdateNodeName( wxConnexNode* node );

	//connecting nodes
	virtual void ConnectPorts( wxConnexPortBase *port0, wxConnexPortBase *port1 );
	virtual void RemoveConnection( wxConnexPortBase *port);

	void OnPaint( wxPaintEvent& event );
	virtual void OnMouseEvent( wxMouseEvent& evt );

	DECLARE_CLASS(wxConnexSpace)
	DECLARE_EVENT_TABLE()

};

#endif
