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

#ifndef _CONNEX_NODE_H
#define _CONNEX_NODE_H

#include "Connex.h"
#include "wx/window.h"

enum
{
	ID_mnSelect,
	ID_mnRemove,
	ID_mnRename,
	ID_mnSetRoot,
	ID_mnSetFirst
};

class wxConnexSpace;
class wxConnexPortBase;

class wxConnexNode : public wxWindow
{
public:
	wxConnexSpace		*m_Space;
	wxSize				m_NonScaleSize;
	wxPoint				m_ClickPoint;

	PortList			m_Ports;
	wxConnexPortBase	*m_FocusPort;


	unsigned long		connexNode_classID;
	string				connexNode_className;
	unsigned short		connexNode_id;
	string				connexNode_name;
	wxConnexNode		*connexNode_parent;
	unsigned short		connexNode_level;
	int					connexNode_numberOfInputPorts;
	bool				connexNode_fixedNumberOfInputPorts;

public:
	wxConnexNode();
	wxConnexNode(	wxConnexSpace *parent, wxWindowID id = wxID_ANY,
					const wxPoint& pos = wxDefaultPosition,
					const wxSize& size = wxSize(50, 50), long style = wxNO_BORDER | wxCAPTION,
					const wxValidator& validator = wxDefaultValidator,
					const wxString& name = wxPanelNameStr );
	~wxConnexNode();
	void AddCXPort( wxConnexPortBase *port );
	wxConnexPortBase* GetCXPort( const wxPoint &client_pos );

	//node connections
	void setNumberOfInputPorts (unsigned short num);
	void addInputPort();
	void removeInputPort();
	void refreshPorts();
	wxConnexPortBase* getOutputPort();
	int getNumberOfFreePorts();
	wxConnexPortBase* getFreePort();

	//node parameters
	unsigned long getNodeClassID();
	string getNodeClassName();
	void setNodeID( unsigned short id );
	unsigned short getNodeID();
	void setNodeName( string name );
	string getNodeName();
	void setParent( wxConnexNode *parent );
	wxConnexNode* getParent();
	bool isAnimationNode();
	bool isBoneController();

	//right click menu
	void OnMenu_RemoveNode( wxCommandEvent& evt );
	void OnMenu_SelectNode( wxCommandEvent& evt );
	void OnMenu_SetRoot( wxCommandEvent& evt );
	void OnMenu_SetFirst( wxCommandEvent& evt );

	//// from original connex project - not used
	//when an input port data/value has changed this will get called
	//if port == NULL you should refresh all your ports
	virtual void OnPortDataUpdate( wxConnexPortBase* port ) {}

	virtual void OnPaint( wxPaintEvent& event );
	void OnMouseEvent( wxMouseEvent &event );

	DECLARE_CLASS( wxConnexNode )
	DECLARE_EVENT_TABLE()
};


////////////////////////
///custom node classes//
////////////////////////

/**
* one input port ... not used
*/
class wxConnexNodeOutput: public wxConnexNode
{
public:
	wxConnexNodeOutput(	wxConnexSpace *parent, wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxSize(50, 50), long style = wxNO_BORDER | wxCAPTION,
		const wxValidator& validator = wxDefaultValidator,
		const wxString& name = wxPanelNameStr );
	//void OnPaint( wxPaintEvent& event );

};

/**
* one output port .. used for animation sampler and face controller
*/
class wxConnexNodeInput: public wxConnexNode
{
public:
	wxConnexNodeInput(	wxConnexSpace *parent, wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxSize(50, 50), long style = wxNO_BORDER | wxCAPTION,
		const wxValidator& validator = wxDefaultValidator,
		const wxString& name = wxPanelNameStr );
	void OnPaint( wxPaintEvent& event );

};

/**
* custom number of input ports and one output port .. used for animation blenders / mixers
*/
class wxConnexNodeMultiplePorts: public wxConnexNode
{
public:
	wxConnexNodeMultiplePorts(	wxConnexSpace *parent, wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxSize(100, 100), long style = wxNO_BORDER | wxCAPTION,
		const wxValidator& validator = wxDefaultValidator,
		const wxString& name = wxPanelNameStr );
	void OnPaint( wxPaintEvent& event );
};

/**
* one input and one output port .. for bone controllers
*/
class wxConnexNodeBoneController: public wxConnexNode
{
public:
	wxConnexNodeBoneController(	wxConnexSpace *parent, wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxSize(100, 100), long style = wxNO_BORDER | wxCAPTION,
		const wxValidator& validator = wxDefaultValidator,
		const wxString& name = wxPanelNameStr );
	void OnPaint( wxPaintEvent& event );
};

#endif
