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

#include "ConnexNode.h"
#include "Connex.h"
#include "ConnexSpace.h"
#include <wx/menu.h>

IMPLEMENT_CLASS( wxConnexNode, wxWindow )

BEGIN_EVENT_TABLE( wxConnexNode, wxWindow )
	EVT_PAINT( wxConnexNode::OnPaint )
	EVT_MOUSE_EVENTS( wxConnexNode::OnMouseEvent )
	EVT_MENU( ID_mnRemove, wxConnexNode::OnMenu_RemoveNode ) 
	EVT_MENU( ID_mnSelect, wxConnexNode::OnMenu_SelectNode )
	EVT_MENU( ID_mnSetRoot, wxConnexNode::OnMenu_SetRoot )
	EVT_MENU( ID_mnSetFirst, wxConnexNode::OnMenu_SetFirst )
END_EVENT_TABLE()

wxConnexNode::wxConnexNode()
: wxWindow()
{
	m_FocusPort = NULL;
}

wxConnexNode::wxConnexNode(	wxConnexSpace *parent, wxWindowID id,
							const wxPoint& pos,
							const wxSize& size, long style,
							const wxValidator& validator,
							const wxString& name )
: wxWindow( parent, id, pos, size, style, name )
{
	m_Space			= parent;
	m_NonScaleSize	= size;
	m_FocusPort		= NULL;

	connexNode_name = name;
	connexNode_parent = NULL;
	connexNode_level = 0;
	connexNode_numberOfInputPorts = 0;
	connexNode_fixedNumberOfInputPorts = true;
	
	if ( id == -1 )
		connexNode_id = ++parent->m_NodeIdList.back();
	else
		connexNode_id = id;
	parent->m_NodeIdList.push_back(connexNode_id);
}

wxConnexNode::~wxConnexNode() 
{
}

void wxConnexNode::AddCXPort( wxConnexPortBase *port )
{
	if( !port )
		return;

	port->m_Parent = this;

	m_Ports.push_back( port ); //bilo push_front
}

wxConnexPortBase* wxConnexNode::GetCXPort( const wxPoint &client_pos )
{
	PortListIterator port_iter( m_Ports.begin() );

	while( port_iter != m_Ports.end() )
	{
		if( (*port_iter)->m_Rect.Contains( client_pos ) )
			return *port_iter;

		port_iter++;
	}

	return NULL;
}

//

void wxConnexNode::setNumberOfInputPorts (unsigned short num)
{
	if ( connexNode_fixedNumberOfInputPorts == true )
		return;
	wxConnexPortBase* portOut;
	std::list<wxConnexPortBase*>::iterator it;
	for( it = m_Ports.begin(); it != m_Ports.end(); it++ )
	{
		if ( !((*it)->IsInput()) )
		{
			portOut = (*it);
		}
	}
	this->m_Ports.clear();
	//addports
	this->connexNode_numberOfInputPorts = 0;
	this->AddCXPort(portOut);
	for(int i = 0; i < num; i++)
	{
		this->addInputPort();
	}
}

void wxConnexNode::refreshPorts()
{
	int current_port = 1;
	std::list<wxConnexPortBase*>::iterator it;
	for( it = m_Ports.begin(); it != m_Ports.end(); it++ )
	{
		if ( (*it)->IsInput() )
		{
			(*it)->m_Rect.SetPosition( wxPoint( 0, current_port * ( (this->GetSize().GetHeight() - 7) / (connexNode_numberOfInputPorts+1) ) ) );
			current_port++;
		}
	}
	this->Refresh();
}

void wxConnexNode::addInputPort()
{
	wxConnexPortBase* portIn1 = new wxConnexPortBase();
	portIn1->m_Placement = wxConnexPortBase::PLACE_LEFT;
	portIn1->SetDirection( wxConnexPortBase::DIR_INPUT );
	portIn1->m_Rect.SetPosition( wxPoint( portIn1->m_Rect.GetWidth(), this->GetSize().GetHeight() - this->GetSize().GetHeight() / 2) );
	this->AddCXPort( portIn1 );
	connexNode_numberOfInputPorts++;
}

void wxConnexNode::removeInputPort()
{
	wxConnexPortBase* port = getFreePort();
	std::list<wxConnexPortBase*>::iterator it;
	for( it = m_Ports.begin(); it != m_Ports.end(); it++ )
	{
		if ( (*it)->GetID() == port->GetID() )
		{
			m_Ports.erase( it );
			connexNode_numberOfInputPorts--;
			refreshPorts();
			break;
		}
	}
}

wxConnexPortBase* wxConnexNode::getOutputPort()
{
	std::list<wxConnexPortBase*>::iterator it;
	for( it = m_Ports.begin(); it != m_Ports.end(); it++ )
	{
		if ( !(*it)->IsInput() )
		{
			return *it;
		}
	}

	return NULL;
}

int wxConnexNode::getNumberOfFreePorts()
{
	int free_ports_num = 0;
	std::list<wxConnexPortBase*>::iterator it;
	for( it = m_Ports.begin(); it != m_Ports.end(); it++ )
	{
		if ( (*it)->IsInput() )
		{
			if ( !(*it)->HasConnections() )
				free_ports_num++;
		}
	}
	return free_ports_num;
}

wxConnexPortBase* wxConnexNode::getFreePort()
{
	if ( connexNode_fixedNumberOfInputPorts == false && getNumberOfFreePorts() < 1 )
	{
		addInputPort();
		refreshPorts();
	}
	wxConnexPortBase* free_port;
	std::list<wxConnexPortBase*>::iterator it;
	for( it = m_Ports.begin(); it != m_Ports.end(); it++ )
	{
		if ( (*it)->IsInput() )
		{
			if ( !(*it)->HasConnections() )
				return *it;
				free_port = *it;
		}
	}
	return NULL;
}

///
unsigned long wxConnexNode::getNodeClassID()
{
	return this->connexNode_classID;
}

string wxConnexNode::getNodeClassName()
{
	return this->connexNode_className;
}

void wxConnexNode::setNodeID( unsigned short id )
{
	wxConnexSpace::NodeIdListIterator it;
	wxConnexSpace::NodeIdListIterator it1;
	for( it = m_Space->m_NodeIdList.begin(); it != m_Space->m_NodeIdList.end(); it++ )
	{
		if ( (*it) == id )
		{
			//this->setNodeID( m_Space->m_NodeIdList.back()++ );
			this->connexNode_id = m_Space->m_NodeIdList.back()++;
			m_Space->m_NodeIdList.push_back(this->connexNode_id);
			return;
		}
		else
		{
			this->connexNode_id = id;
			m_Space->m_NodeIdList.push_back(id);
			return;
		}
	}
}

unsigned short wxConnexNode::getNodeID()
{
	return this->connexNode_id;
}

void wxConnexNode::setNodeName( string name )
{
	this->connexNode_name = name;
}

string wxConnexNode::getNodeName()
{
	return connexNode_name;
}

void wxConnexNode::setParent( wxConnexNode *parent )
{
	connexNode_parent = parent;
}

wxConnexNode* wxConnexNode::getParent()
{
	return connexNode_parent;
}

bool wxConnexNode::isAnimationNode()
{
	if ( getNodeClassID() >= 1 && getNodeClassID() <= 5 )
		return true;
	else
		return false;
}

bool wxConnexNode::isBoneController()
{
	if ( getNodeClassID() >= 6 && getNodeClassID() <= 7 )
		return true;
	else
		return false;
}

//

void wxConnexNode::OnPaint( wxPaintEvent& event )
{
	wxPaintDC dc(this);

	static wxPen pen_lines( wxColour(255,255,255, wxALPHA_TRANSPARENT), 1, wxSOLID );
	static wxPen pen_black( wxColour(0,0,0, wxALPHA_OPAQUE), 1, wxSOLID );
	static wxBrush brush_black( wxColour(0,0,0, wxALPHA_OPAQUE) );
	static wxBrush brush_label( wxColour(200,200,200, wxALPHA_OPAQUE), wxSOLID );

	wxRect rect = GetRect();
	rect.x = 8;
	rect.width -= 16;
	rect.y = 1;
	rect.height -= 10;
	dc.SetPen( pen_lines );
	dc.DrawRoundedRectangle( rect, 5 );

	//not sure if this is a correct way to use this flag :/
	if( GetWindowStyle() & wxCAPTION )
	{
		long height = 16;
		dc.GetTextExtent( GetLabel(), NULL, &height );

		rect.height = height;
		dc.SetBrush( brush_label );
		dc.DrawRoundedRectangle( rect, 5 );
		dc.DrawLabel( GetLabel(), rect, wxALIGN_CENTER );
	}

	PortListIterator port_iter( m_Ports.begin() );

	while( port_iter != m_Ports.end() )
	{
		(*port_iter)->Paint( dc, wxPoint(1, 0) );
		port_iter++;
	}

#ifdef WIN32
	wxWindow::OnPaint( event );
#endif
}

void wxConnexNode::OnMouseEvent( wxMouseEvent &event )
{
	if( event.GetEventType() == wxEVT_LEFT_DOWN )
	{
		m_ClickPoint				= event.GetPosition();
		m_Space->m_FocusNode		= this;
		if( (m_FocusPort = GetCXPort( event.GetPosition() )) )
		{
			m_Space->m_MouseMode		= wxConnexSpace::MOUSE_CONNECT;
			m_Space->m_MouseFromPlace	= m_FocusPort->m_Placement;
		}
		else
		{
			m_Space->m_MouseMode	= wxConnexSpace::MOUSE_DRAG;
			Raise();
		}
	}
	else if( event.GetEventType() == wxEVT_LEFT_UP )
	{
		if( m_Space->m_FocusNode
			&& m_Space->m_FocusNode != this
			&& (m_FocusPort = GetCXPort( event.GetPosition() )) )
		{
			m_Space->ConnectPorts( m_Space->m_FocusNode->m_FocusPort, m_FocusPort );
		}

		m_Space->m_FocusNode		= NULL;
		m_Space->SetSelectedNode( this );
		GetParent()->Refresh();
	}
	else if( event.GetEventType() == wxEVT_MOTION && event.ButtonIsDown( wxMOUSE_BTN_LEFT ) && event.Dragging() )
	{
		if (m_Space->m_MouseMode == wxConnexSpace::MOUSE_CONNECT)
		{
			wxConnexPortBase* port = GetCXPort( event.GetPosition() );
			if( port && m_Space->m_FocusNode != this )
			{
				wxRect mrect = port->m_Rect;
				mrect.x += mrect.width / 2;
				mrect.y += mrect.height / 2;
				m_Space->m_LastMousePos		= mrect.GetPosition() + GetPosition();
				m_Space->m_MouseToPlace		= port->m_Placement;
			}
			else
			{
				m_Space->m_LastMousePos = event.GetPosition() + GetPosition();
			}

			GetParent()->Refresh();
		}
		else if( m_Space->m_FocusNode == this )
		{
			int x = event.m_x + GetPosition().x - m_ClickPoint.x;
			int y = event.m_y + GetPosition().y - m_ClickPoint.y;

			wxPoint pos = GetPosition();
			pos.x = x;
			pos.y = y;

			SetPosition(pos);
			GetParent()->Refresh();

			//check if the node is outside of window borders and resize window if needed
			if( pos.x > ( m_Space->window_size.x - GetSize().x ) )
				m_Space->window_size.x += GetSize().x;
			if( pos.y > ( m_Space->window_size.y - GetSize().y ) )
				m_Space->window_size.y += GetSize().y;
			m_Space->SetSize(m_Space->window_size);
			m_Space->Refresh();
		}
	}

	if( event.GetEventType() == wxEVT_RIGHT_DOWN )
	{
		wxMenu menu;
		menu.Append( ID_mnSelect, "Select" );
		menu.Append( ID_mnRemove, "Remove" );
		menu.Append( ID_mnRename, "Rename" );
		if ( this->isAnimationNode() )
			menu.Append( ID_mnSetRoot, "Set Root Node" );
		else if ( this->isBoneController() )
			menu.Append( ID_mnSetFirst, "Set First JC" );
		PopupMenu(&menu);
	}
}

///////////////
///custom nodes
///////////////

wxConnexNodeOutput::wxConnexNodeOutput(	wxConnexSpace *parent, wxWindowID id,
											   const wxPoint& pos,
											   const wxSize& size, long style,
											   const wxValidator& validator,
											   const wxString& name)
	: wxConnexNode( parent, id, pos, size, style, validator, name )
{
	wxConnexPortBase* port = new wxConnexPortBase(); // wxConnexPortBase instead of custom port
	port->m_Placement = wxConnexPortBase::PLACE_LEFT;
	port->SetDirection( wxConnexPortBase::DIR_INPUT );
	port->m_Rect.SetPosition( wxPoint( port->m_Rect.GetWidth(), size.GetHeight() / 2 ) );
	this->AddCXPort( port );

}

wxConnexNodeInput::wxConnexNodeInput( wxConnexSpace *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style,
									 const wxValidator& validator, const wxString& name ) 
									 : wxConnexNode( parent, id, pos, size, style, validator, name )
{
	wxConnexPortBase* port = new wxConnexPortBase();
	port->m_Placement = wxConnexPortBase::PLACE_RIGHT;
	port->SetDirection( wxConnexPortBase::DIR_OUTPUT );
	port->m_Rect.SetPosition( wxPoint( size.x - 10, size.GetHeight() / 2 ) );
	this->AddCXPort( port );
	this->connexNode_fixedNumberOfInputPorts = true;

}

void wxConnexNodeInput::OnPaint( wxPaintEvent& event )
{
	wxPaintDC dc(this);

	wxPen pen_black( wxColour(0,0,0, wxALPHA_OPAQUE), 1, wxSOLID );
	wxBrush brush_color( wxColour(0,0,255, wxALPHA_OPAQUE) );
	static wxBrush brush_label( wxColour(20,20,220, wxALPHA_OPAQUE), wxSOLID );
	if ( this->m_Space->GetSelectedNode() != NULL )
		if ( this->m_Space->GetSelectedNode()->getNodeID() == this->getNodeID() )
		{
			pen_black = wxPen( wxColour(0,0,0, wxALPHA_OPAQUE), 3, wxSOLID );
			this->m_Space->UpdateNodeName( this );
		}
	wxRect rect = GetRect();
	rect.x = 8;
	rect.width -= 16;
	rect.y = 0;
	rect.height -= 2;
	dc.SetPen( pen_black );
	dc.SetBrush( brush_color );
	dc.DrawRoundedRectangle( rect, 5 );

	if( GetWindowStyle() & wxCAPTION )
	{
		long height = 16;
		dc.GetTextExtent( GetLabel(), NULL, &height );

		rect.height = height;
		dc.SetBrush( brush_label );
		dc.DrawRoundedRectangle( rect, 5 );
		dc.DrawLabel( GetLabel(), rect, wxALIGN_CENTER );
		dc.DrawText( this->getNodeName(), wxCoord(20), wxCoord(20));
		dc.DrawText( wxString::Format(wxT("%u"),this->getNodeID()), wxCoord(20), wxCoord(40));
	}

	PortListIterator port_iter( m_Ports.begin() );

	while( port_iter != m_Ports.end() )
	{
		(*port_iter)->Paint( dc, wxPoint(1, 0) );
		port_iter++;
	}

#ifdef WIN32
	wxWindow::OnPaint( event );
#endif
}

wxConnexNodeMultiplePorts::wxConnexNodeMultiplePorts( wxConnexSpace *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style,
											   const wxValidator& validator, const wxString& name )
											   : wxConnexNode( parent, id, pos, size, style, validator, name )
{
	this->connexNode_numberOfInputPorts = 2;

	wxConnexPortBase* portIn1 = new wxConnexPortBase();
	portIn1->m_Placement = wxConnexPortBase::PLACE_LEFT;
	portIn1->SetDirection( wxConnexPortBase::DIR_INPUT );
	portIn1->m_Rect.SetPosition( wxPoint( 0, size.GetHeight() - size.GetHeight() / 4) );
	
	wxConnexPortBase* portIn2 = new wxConnexPortBase();
	portIn2->m_Placement = wxConnexPortBase::PLACE_LEFT;
	portIn2->SetDirection( wxConnexPortBase::DIR_INPUT );
	portIn2->m_Rect.SetPosition( wxPoint( 0, size.GetHeight() / 4 ) );
	
	wxConnexPortBase* portOut = new wxConnexPortBase();
	portOut->m_Placement = wxConnexPortBase::PLACE_RIGHT;
	portOut->SetDirection( wxConnexPortBase::DIR_OUTPUT );
	portOut->m_Rect.SetPosition( wxPoint( size.x - 10, size.GetHeight() / 2 ) );
	
	this->connexNode_fixedNumberOfInputPorts = false;
	this->connexNode_numberOfInputPorts = 2;
	this->AddCXPort( portOut );
	this->AddCXPort( portIn1 );
	this->AddCXPort( portIn2 );
}

void wxConnexNodeMultiplePorts::OnPaint( wxPaintEvent& event )
{
	wxPaintDC dc(this);
	wxPen pen_black( wxColour(0,0,0, wxALPHA_OPAQUE), 1, wxSOLID );
	wxBrush brush_color( wxColour(255,0,0, wxALPHA_OPAQUE) );
	if ( this->m_Space->GetRoot() != NULL && this->m_Space->GetRoot()->getNodeID() == this->getNodeID() )
		brush_color = wxBrush( wxColour(200,0,0, wxALPHA_OPAQUE), wxSOLID );
	if ( this->m_Space->GetSelectedNode() != NULL )
		if ( this->m_Space->GetSelectedNode()->getNodeID() == this->getNodeID() )
		{
			pen_black = wxPen( wxColour(0,0,0, wxALPHA_OPAQUE), 3, wxSOLID );
			this->m_Space->UpdateNodeName( this );
		}
	static wxBrush brush_label( wxColour(200,20,20, wxALPHA_OPAQUE), wxSOLID );

	wxRect rect = GetRect();
	rect.x = 8;
	rect.width -= 16;
	rect.y = 0;
	rect.height -= 2;
	dc.SetPen( pen_black );
	dc.SetBrush( brush_color );
	dc.DrawRoundedRectangle( rect, 5 );

	long height = 16;
	dc.GetTextExtent( GetLabel(), NULL, &height );

	rect.height = height;
	dc.SetBrush( brush_label );
	dc.DrawRoundedRectangle( rect, 5 );
	dc.DrawLabel( GetLabel(), rect, wxALIGN_CENTER );
	dc.DrawText( this->getNodeName(), wxCoord(15), wxCoord(20));
	dc.DrawText( wxString::Format(wxT("%u"),this->getNodeID()), wxCoord(15), wxCoord(40));

	PortListIterator port_iter( m_Ports.begin() );

	while( port_iter != m_Ports.end() )
	{
		(*port_iter)->Paint( dc, wxPoint(1, 0) );
		port_iter++;
	}

#ifdef WIN32
	wxWindow::OnPaint( event );
#endif
}

wxConnexNodeBoneController::wxConnexNodeBoneController( wxConnexSpace *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, 
														 long style, const wxValidator& validator, const wxString& name )
														 : wxConnexNode( parent, id, pos, size, style, validator, name )
{
	this->connexNode_numberOfInputPorts = 1;

	wxConnexPortBase* portIn = new wxConnexPortBase();
	portIn->m_Placement = wxConnexPortBase::PLACE_LEFT;
	portIn->SetDirection( wxConnexPortBase::DIR_INPUT );
	portIn->m_Rect.SetPosition( wxPoint( 0, size.GetHeight() / 2) );
		
	wxConnexPortBase* portOut = new wxConnexPortBase();
	portOut->m_Placement = wxConnexPortBase::PLACE_RIGHT;
	portOut->SetDirection( wxConnexPortBase::DIR_OUTPUT );
	portOut->m_Rect.SetPosition( wxPoint( size.x - 10, size.GetHeight() / 2 ) );

	this->connexNode_fixedNumberOfInputPorts = true;
	this->AddCXPort( portIn );
	this->AddCXPort( portOut );
}

void wxConnexNodeBoneController::OnPaint( wxPaintEvent& event )
{
	wxPaintDC dc(this);
	wxPen pen_black( wxColour(0,0,0, wxALPHA_OPAQUE), 1, wxSOLID );
	wxBrush brush_color( wxColour(0,240,0, wxALPHA_OPAQUE) );
	if ( this->m_Space->GetFirst() != NULL && this->m_Space->GetFirst()->getNodeID() == this->getNodeID() )
		brush_color = wxBrush( wxColour(0,200,0, wxALPHA_OPAQUE) );
	if ( this->m_Space->GetSelectedNode() != NULL )
		if ( this->m_Space->GetSelectedNode()->getNodeID() == this->getNodeID() )
		{
			pen_black = wxPen( wxColour(0,0,0, wxALPHA_OPAQUE), 3, wxSOLID );
			this->m_Space->UpdateNodeName( this );
		}
	static wxBrush brush_label( wxColour(0,150,0, wxALPHA_OPAQUE), wxSOLID );

	wxRect rect = GetRect();
	rect.x = 8;
	rect.width -= 16;
	rect.y = 0;
	rect.height -= 2;
	dc.SetPen( pen_black );
	dc.SetBrush( brush_color );
	dc.DrawRoundedRectangle( rect, 5 );


	long height = 16;
	dc.GetTextExtent( GetLabel(), NULL, &height );

	rect.height = height;
	dc.SetBrush( brush_label );
	dc.DrawRoundedRectangle( rect, 5 );
	dc.DrawLabel( GetLabel(), rect, wxALIGN_CENTER );
	dc.DrawText( this->getNodeName(), wxCoord(20), wxCoord(20));
	dc.DrawText( wxString::Format(wxT("%u"),this->getNodeID()), wxCoord(20), wxCoord(40));

	PortListIterator port_iter( m_Ports.begin() );
	while( port_iter != m_Ports.end() )
	{
		(*port_iter)->Paint( dc, wxPoint(1, 0) );
		port_iter++;
	}

#ifdef WIN32
	wxWindow::OnPaint( event );
#endif
}

//////
//////right click menu
//////

void wxConnexNode::OnMenu_RemoveNode( wxCommandEvent& evt )
{
	unsigned short id = this->getNodeID();
	m_Space->RemoveNode( id );
}

void wxConnexNode::OnMenu_SelectNode( wxCommandEvent& evt )
{
	m_Space->SetSelectedNode( this );
	m_Space->Refresh();
}

void wxConnexNode::OnMenu_SetRoot( wxCommandEvent& evt )
{
	m_Space->SetRoot( this );
	m_Space->Refresh();
}

void wxConnexNode::OnMenu_SetFirst( wxCommandEvent& evt )
{
	m_Space->SetFirst( this );
	m_Space->Refresh();
}


