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

#include "ConnexSpace.h"
#include "ConnexNode.h"
#include "Connex.h"
#include <wx/menu.h>

IMPLEMENT_CLASS( wxConnexSpace, wxWindow )

BEGIN_EVENT_TABLE( wxConnexSpace, wxWindow )
	EVT_PAINT( wxConnexSpace::OnPaint )
	EVT_MOUSE_EVENTS( wxConnexSpace::OnMouseEvent )
END_EVENT_TABLE()

wxConnexSpace::wxConnexSpace()
: wxWindow()
{
	m_FocusNode				= NULL;
	m_DrawConnectionShadows	= true;
	m_MouseToPlace			= wxConnexPortBase::PLACE_LEFT;
	m_MouseFromPlace		= wxConnexPortBase::PLACE_LEFT;
}

wxConnexSpace::wxConnexSpace(	wxWindow *parent, wxWindowID id,
								const wxPoint& pos,
								const wxSize& size, long style,
								const wxValidator& validator,
								const wxString& name )
	: wxWindow( parent, id, pos, size, style, name )
{
	m_FocusNode				= NULL;
	m_DrawConnectionShadows	= true;
	m_MouseToPlace			= wxConnexPortBase::PLACE_LEFT;
	m_MouseFromPlace		= wxConnexPortBase::PLACE_LEFT;
	//
	m_NodeIdList.push_front(0);
	//
	window_size = size;

	m_root = NULL;
	m_first = NULL;
	m_selected = NULL;
}

wxConnexSpace::~wxConnexSpace()
{
}

void wxConnexSpace::AddNode( wxConnexNode* node )
{
	m_NodeList.push_front( node );
}

wxConnexNode* wxConnexSpace::FindNode( unsigned short id )
{
	NodeListIterator iter( m_NodeList.begin() );
	//iterate through all the nodes
	std::list<wxConnexNode*>::iterator it; 
	for( it = m_NodeList.begin(); it != m_NodeList.end(); ++it )
	{
		if ( (*it)->getNodeID() == id )
			return (*it);
	}
	return NULL;
}

wxConnexNode* wxConnexSpace::FindNode( const std::string name )
{
	NodeListIterator iter( m_NodeList.begin() );
	std::list<wxConnexNode*>::iterator it; 
	for( it = m_NodeList.begin(); it != m_NodeList.end(); ++it )
	{
		if ( (*it)->getNodeName() == name )
			return (*it);
	}
	return NULL;
}

void wxConnexSpace::RemoveNode( unsigned short id )
{
	std::list<wxConnexNode*>::iterator it; 
	for( it = m_NodeList.begin(); it != m_NodeList.end(); ++it )
	{
		if ( (*it)->getNodeID() == id )
		{
			PortListIterator port_iter;
			for( port_iter = (*it)->m_Ports.begin(); port_iter != (*it)->m_Ports.end(); ++port_iter )
				(*port_iter)->RemoveAllConnections();
			delete (*it);
			m_NodeList.erase(it);
			return;
		}
	}	
}

void wxConnexSpace::RemoveNode( const std::string& name )
{
	std::list<wxConnexNode*>::iterator it; 
	for( it = m_NodeList.begin(); it != m_NodeList.end(); ++it )
	{
		if ( (*it)->getNodeName() == name )
		{
			PortListIterator port_iter;
			for( port_iter = (*it)->m_Ports.begin(); port_iter != (*it)->m_Ports.end(); ++port_iter )
				(*port_iter)->RemoveAllConnections();
			delete (*it);
			m_NodeList.erase(it);
			return;
		}
	}	
}

void wxConnexSpace::RemoveAllNodes()
{
	m_NodeIdList.clear();
	m_NodeIdList.push_back(0);
	while( !m_NodeList.empty() )
	{
		m_NodeList.front()->Destroy();
		m_NodeList.pop_front();
	}
}

wxConnexNode* wxConnexSpace::GetRoot()
{
	return m_root;
}

wxConnexNode* wxConnexSpace::GetFirst()
{
	return m_first;
}

void wxConnexSpace::SetRoot( wxConnexNode* root )
{
	m_root = root;
}

void wxConnexSpace::SetFirst( wxConnexNode* first )
{
	m_first = first;
}

wxConnexNode* wxConnexSpace::GetSelectedNode()
{
	return m_selected;
}

void wxConnexSpace::SetSelectedNode( wxConnexNode* selected )
{
	m_selected = selected;
}

void wxConnexSpace::UpdateNodeName( wxConnexNode* node )
{
}

void wxConnexSpace::ConnectPorts( wxConnexPortBase* port0, wxConnexPortBase* port1	)
{
	if( !port0 || !port1 || (port0->IsInput() == port1->IsInput()) )
		return;

	if( port0->IsConnected(port1) )
		return;

	bool make_connection = false;

	wxConnexPortBase* inputp;
	wxConnexPortBase* outputp;

	if( port0->IsInput() )
	{
		inputp	= port0;
		outputp	= port1;
		if( port0->CanTakeAsInput( *port1->GetPortClass() ) )
		{
			make_connection = true;
		}
	}
	else
	{
		inputp	= port1;
		outputp	= port0;
		if( port1->CanTakeAsInput( *port0->GetPortClass() ) )
		{
			make_connection = true;
		}
	}

	// eliminate multiple connections
	make_connection = false;
	if ( port0->HasConnections() == true || port1->HasConnections() == true )
		return;
	
	// eliminate loops
	unsigned short output_node = outputp->m_Parent->getNodeID();
	wxConnexPortBase* out = inputp->m_Parent->getOutputPort();
	int done = 0;
	while ( !done )
	{
		if ( out->m_Connections.size() == 0 ) //no parent
		{
			done = 1;
			make_connection = true;
		}
		else if ( out->m_Connections.front()->m_Parent->getNodeID() == output_node )
		{
			return;
		}
		else 
			out = out->m_Connections.front()->m_Parent->getOutputPort();
	}
	//limit bone controller connections
	if ( outputp->m_Parent->getNodeClassID() > 5 && inputp->m_Parent->getNodeClassID() <= 5 )
		make_connection = false;
	//

	if( make_connection )
	{
		inputp->m_Connections.clear();
		inputp->m_Connections.push_back( outputp );
		outputp->m_Connections.push_back( inputp );

		inputp->RefreshConnections();
		if( inputp->m_Parent->isAnimationNode() ) 
			inputp->m_Parent->getFreePort();
	}
}

void wxConnexSpace::RemoveConnection( wxConnexPortBase *port )
{
	if( !port )
		return;

	if( !port->IsInput() )
	{
		wxConnexNode* conn = port->m_Connections.front()->m_Parent;
		port->RemoveAllConnections();
		if ( conn->getNumberOfFreePorts() > 1 )
			conn->removeInputPort();
	}
	else
	{
		port->RemoveAllConnections();
		if ( port->m_Parent->getNumberOfFreePorts() > 1 )
			port->m_Parent->removeInputPort();
	}
}

void wxConnexSpace::OnPaint( wxPaintEvent& event )
{
	static wxPen pen_lines( wxColour(0,0,0), 2, wxSOLID );
	static wxPen pen_shadow( wxColour(82,82,82), 2, wxSOLID );
	static wxPen pen_red( wxColour(255,0,0), 3, wxSOLID );

#ifdef WIN32
	wxWindow::OnPaint( event );
#endif

	static wxPoint points[4];
	static wxPoint shadow[4];

	wxPaintDC dc(this);

	int init_pass = 0;
	if( !m_DrawConnectionShadows )
		init_pass = 1;

	//pass 0 = connection spline shadows
	//pass 1 = diffuse connection splines
	int draw_pass;
	for( draw_pass = init_pass; draw_pass < 2; draw_pass++ )
	{
		if( draw_pass == 0 )
			dc.SetPen( pen_shadow );
		else
			dc.SetPen( pen_lines );

		NodeListIterator iter(m_NodeList.begin());
		//iterate through all the nodes
		while( iter != m_NodeList.end() )
		{
			wxPoint node_pos = (*iter)->GetPosition();

			PortListIterator port_iter = (*iter)->m_Ports.begin();
			//iterate through all the nodes ports
			while( port_iter != (*iter)->m_Ports.end() )
			{
				//render only the output ports connections
				if( !(*port_iter)->IsInput() && (*port_iter)->HasConnections())
				{
					PortListIterator conn_iter = (*port_iter)->m_Connections.begin();
					//iterate through all the output ports connections and render them
					while( conn_iter != (*port_iter)->m_Connections.end() )
					{
						wxSize shift = (*port_iter)->m_Rect.GetSize();
						shift.x /= 2;
						shift.y /= 2;
						points[0] = node_pos + (*port_iter)->m_Rect.GetPosition() + shift;
						points[1] = points[0] + wxConnexSplineDelta[(*port_iter)->m_Placement];

						shift = (*conn_iter)->m_Rect.GetSize();
						shift.x /= 2;
						shift.y /= 2;

						points[3] = (*conn_iter)->m_Parent->GetPosition() + (*conn_iter)->m_Rect.GetPosition() + shift;
						points[2] = points[3] + wxConnexSplineDelta[(*conn_iter)->m_Placement];

						if( draw_pass == 0 )
						{
							points[1].x += 3;
							points[2].x += 3;
						}

						dc.DrawSpline(4, points);

						conn_iter++;
					}
				}

				port_iter++;
			}

			iter++;
		}
	}
	

	if( m_MouseMode == MOUSE_CONNECT )
	{
		//render the output to mouse pointer connetion (usually red)
		if( m_FocusNode && m_FocusNode->m_FocusPort )
		{
			wxPoint pos			= m_FocusNode->m_FocusPort->m_Rect.GetPosition();
			wxSize	half_size	= m_FocusNode->m_FocusPort->m_Rect.GetSize();
			half_size.x /= 2;
			half_size.y /= 2;
			points[0] = m_FocusNode->GetPosition() + pos + half_size;
			points[1] = points[0] + wxConnexSplineDelta[m_MouseFromPlace];
			points[3] = m_LastMousePos;
			points[2] = points[3] + wxConnexSplineDelta[m_MouseToPlace];


			if( m_DrawConnectionShadows )
			{
				shadow[0] = points[0];
				shadow[1] = points[1] + wxPoint(3,0);
				shadow[2] = points[2] + wxPoint(3,0);
				shadow[3] = points[3];
				dc.SetPen( pen_shadow );
				dc.DrawSpline( 4, shadow );
			}

			dc.SetPen(pen_red);
			dc.DrawSpline(4, points);
		}
	}

	wxSize sz = this->GetSize();
	if ( window_size.x > sz.x ) 
		this->SetSize( window_size.x, this->GetSize().y );
	if ( window_size.y > sz.y )
		this->SetSize( this->GetSize().x, window_size.y );
}


void wxConnexSpace::OnMouseEvent( wxMouseEvent& event )
{
	if( event.GetEventType() == wxEVT_LEFT_UP )
	{
		if( m_MouseMode == MOUSE_CONNECT)
		{
			if( m_FocusNode && m_FocusNode->m_FocusPort )
			{
				RemoveConnection( m_FocusNode->m_FocusPort );
			}
		}
		else
		{
		}
		Refresh();
		m_FocusNode = NULL;
		SetSelectedNode( NULL );
	}

	if( event.GetEventType() == wxEVT_MOTION )
	{
		m_LastMousePos = event.GetPosition();
	}

	if( event.GetEventType() == wxEVT_MOTION && event.ButtonIsDown( wxMOUSE_BTN_LEFT ) && event.Dragging() )
	{
		if( m_FocusNode )
		{
			if( m_MouseMode == MOUSE_CONNECT)
			{
				Refresh();
			}
			else
			{
				wxPoint pos = event.GetPosition() - m_FocusNode->m_ClickPoint;
				m_FocusNode->SetPosition( pos );
			}
		}
	}
	
	//right click menu
	if( event.GetEventType() == wxEVT_RIGHT_DOWN )
	{
		wxMenu menu;
		wxMenu* submenu1 = new wxMenu();
		wxMenu* submenu2 = new wxMenu();
		submenu1->Append( ID_mnCreateAnimationSampler, wxT( "AnimationSampler" ) );
		submenu1->Append( ID_mnCreateFaceController, wxT( "FaceController" ) );
		submenu1->Append( ID_mnCreateAnimationBlender, wxT( "AnimationBlender" ) );
		submenu1->Append( ID_mnCreateAnimationTransitionBlender, wxT( "AnimationTransitionBlender" ) );
		submenu1->Append( ID_mnCreateAnimationMixer, wxT( "AnimationMixer" ) );
		submenu2->Append( ID_mnCreateBoneTransformController, wxT( "BoneTransformController" ) );
		submenu2->Append( ID_mnCreateBoneIKController, wxT( "BoneIKController" ) );
		menu.AppendSubMenu( submenu1, "Create Node" );
		menu.AppendSubMenu( submenu2, "Create Bone Controller" );
		menu.Append( ID_mnCreateNodeForEachAnim, "Create Node for Each Anim." );
		menu.AppendSeparator();
		//menu.Append( ID_mnSave, "Save" );
		menu.Append( ID_mnAutoArrangeNodes, "Auto Arrange Nodes" );

		PopupMenu(&menu);
	}
}
