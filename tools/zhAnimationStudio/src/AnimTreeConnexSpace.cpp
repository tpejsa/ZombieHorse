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

#include "AnimTreeConnexSpace.h"
#include "AnimationStudioApp.h"
#include "AnimationStudioFrame.h"
#include "PropertiesWindow.h"
#include "AnimationBlenderPropGrid.h"
#include "AnimationMixerPropGrid.h"
#include "AnimationSamplerPropGrid.h"
#include "AnimationTransitionBlenderPropGrid.h"
#include "FaceControllerPropGrid.h"
#include "BoneIKControllerPropGrid.h"
#include "BoneTransformControllerPropGrid.h"
#include "Connex.h"
#include "ConnexSpace.h"

AnimTreeConnexSpace::AnimTreeConnexSpace( wxWindow *parent, wxWindowID id,	wxPoint& pos,	wxSize& size, 
										 long style, const wxValidator& validator, const wxString& name )
: wxConnexSpace( parent, id, pos, size, style, validator, name )
{
	window_size = parent->GetSize();
}

void AnimTreeConnexSpace::OnSize( wxSizeEvent& evt )
{
	//this->SetSize( window_size );
	//this->SetScrollbars( 1, 1, this->GetSize().GetWidth(), this->GetSize().GetHeight() );
	//mWndAnimTreeConnexSpaceScroll->SetScrollbars( window_size );
}

void AnimTreeConnexSpace::autoArrangeNodes( AnimationTreeInstance* anim_tree )
{
	if (anim_tree == NULL )
		return;

	// arrange nodes in animation tree with root node
	int level = 0;
	int pos_x = 20;
	int pos_y = 0; 
	int first_lvl = 1, max_size = 0, first_lvl_size = 0;
	wxPoint node_pos;
	wxConnexNode* con_node;
	std::vector<std::vector<AnimationNode*>>::size_type sz = mNodeGrid.size();
	for(int i=sz-1; i >= 0; i--)
	{
		pos_y = 0;
		if ( (int) mNodeGrid[i].size() > 0)
		{
			if ( first_lvl == 0 )
				pos_x += 250;
			std::vector<AnimationNode*>::size_type sz2 = mNodeGrid[i].size();
			for(int j=0; j < sz2; j++)
			{
				pos_y += 20;
				if ( pos_y > max_size )
						max_size = pos_y;
				node_pos = wxPoint( pos_x , pos_y );
				con_node = this->FindNode( mNodeGrid[i][j]->getId() );
				if ( con_node != NULL )
				{
					con_node->SetPosition( node_pos );
					pos_y += con_node->GetSize().GetHeight();
					
					if ( mNodeGrid[i][j]->getParent() != NULL )
					{
						wxConnexNode* parent = this->FindNode( mNodeGrid[i][j]->getParent()->getId() );
						if ( parent->getFreePort() != 0 )
						{
							this->ConnectPorts( con_node->getOutputPort(), parent->getFreePort() );
							con_node->connexNode_parent = parent;
						}
					}

					first_lvl = 0;
					con_node->connexNode_level = level;
				}
			}
			level++;
			if ( i == sz-1 )
				first_lvl_size = pos_y;
		}
	}

	// arrange nodes which are not connected to anim tree
	AnimationTree::NodeIterator node_it = anim_tree->getNodeIterator();
	AnimationNode* anim_node;
	while( !node_it.end() )
	{
		anim_node = node_it.next();
		if ( enum_Nodes.find( anim_node ) == enum_Nodes.end() )
		{
			con_node = this->FindNode( anim_node->getId() );
			enum_Nodes.insert( anim_node );
			mNodeGrid[mNodeGrid.size()-1].push_back( anim_node );
			con_node->connexNode_level = mNodeGrid.size() - 1;
			first_lvl_size += 20;
			con_node->SetPosition( wxPoint(20,first_lvl_size ) );
			first_lvl_size += con_node->GetSize().GetHeight();
			if ( first_lvl_size > max_size )
				max_size = first_lvl_size;
		}
	}

	//arrange bone controllers connected to anim tree (from mJCGrid)
	pos_x += 200;
	int firstJC_pos = pos_x;
	if ( mJCGrid.size() > 0)
	{
		first_lvl = 1;
		std::vector<std::vector<BoneController*>>::size_type sz_jc = mJCGrid.size();
		//for(int i=sz_jc-1; i >= 0; i--)
		for(int i=0; i <= sz_jc-1; i++)
		{
			pos_y = 0;	
			pos_x += 220;
			std::vector<BoneController*>::size_type sz2_jc = mJCGrid[i].size();
			for(int j=0; j < sz2_jc; j++)
			{
				pos_y += 20;
				node_pos = wxPoint( pos_x, pos_y );
				con_node = this->FindNode( mJCGrid[i][j]->getId() );
				if ( con_node != NULL )
				{
					con_node->SetPosition( node_pos );
					pos_y += con_node->GetSize().GetHeight();
					
					if ( mJCGrid[i][j]->getNext() != NULL )
					{
						wxConnexNode* parent = this->FindNode( mJCGrid[i][j]->getNext()->getId() );
						if ( parent->getFreePort() != 0 )
						{
							this->ConnectPorts( con_node->getOutputPort(), parent->getFreePort() );
							con_node->connexNode_parent = parent;
						}
					}
					con_node->connexNode_level = level;
				}
			}
			level++;
			if ( i == sz_jc-1 )
				firstJC_pos = pos_x;
		}
		this->ConnectPorts( this->FindNode( anim_tree->getRoot()->getId() )->getOutputPort(), this->FindNode( mJCGrid[sz_jc-1][0]->getId() )->getFreePort() );
	}

	//arrange other nodes which are not in enum_JC
	AnimationTree::BoneControllerIterator jc_it = anim_tree->getBoneControllerIterator();
	BoneController* jc;
	pos_y = 150;
	while( !jc_it.end() )
	{
		jc = jc_it.next();
		if ( enum_JC.find( jc ) == enum_JC.end() )
		{
			con_node = this->FindNode( jc->getId() );
			enum_JC.insert( jc );
			mJCGrid[mJCGrid.size()-1].push_back( jc );
			con_node->connexNode_level = mNodeGrid.size() - 1;
			con_node->SetPosition( wxPoint( firstJC_pos, pos_y ) );
			pos_y += con_node->GetSize().GetHeight() + 20;
			if ( pos_y > max_size )
				max_size = pos_y;
		}
	}

	//update window size
	if ( pos_x < this->GetSize().GetWidth() )
		window_size = wxSize( this->GetSize().GetX()+200, max_size+200 );
	else
		window_size = wxSize( pos_x, max_size+200 );
	this->SetSize( window_size );

	//update root / first
	if ( anim_tree->getRoot() != NULL )
		this->SetRoot( this->FindNode( anim_tree->getRoot()->getId() ) );
	if ( anim_tree->getFirst() != NULL )
		this->SetFirst( this->FindNode( anim_tree->getFirst()->getId() ) );
	if ( anim_tree->getRoot() != NULL && anim_tree->getFirst() != NULL )
		ConnectPorts( this->GetRoot()->getOutputPort(), this->GetFirst()->getFreePort() );
	Refresh();
}

void AnimTreeConnexSpace::enumNodes( int level, AnimationNode* node )
{
	if ( node == NULL )
		return;
	if ( level >= mNodeGrid.size() )
		mNodeGrid.push_back( tree_level );
	mNodeGrid[level].push_back( node );
	enum_Nodes.insert( node );
	if( node->getNumChildren() != 0)
	{
		level++;
		AnimationNode::ChildIterator children = node->getChildIterator();
		while( !children.end() )
		{
			enumNodes(level, children.next());
		}
	}
}

void AnimTreeConnexSpace::enumJC( int level, BoneController* node )
{
	if ( node == NULL )
		return;
	if ( level >= mJCGrid.size() )
		mJCGrid.push_back( jc_level );
	mJCGrid[level].push_back( node );
	enum_JC.insert( node );
	if( node->getNext() != NULL )
	{
		level++;
		enumJC( level, node->getNext() );
	}
}

int AnimTreeConnexSpace::getTreeLevel( int level, AnimationNode* parent ) 
{
	if( parent->getNumChildren() != 0 )
	{
		level++;
		AnimationNode::ChildIterator children = parent->getChildIterator();
		while( !children.end() )
		{
			getTreeLevel( level, children.next() );
		}
	}
	else
		return level;
	return level;
}

void AnimTreeConnexSpace::addNodeToAnimTree( wxConnexNode* node )
{
	AnimationTreeInstance* anim_tree = gApp->getCurrentAnimationTree();
	if ( anim_tree == NULL )
		return;
	if ( anim_tree->getNode( node->getNodeID() ) != NULL ) //node exists
	{
		return;
	}
	else
	{
		unsigned short index = 0;
		while( anim_tree->hasNode( node->getNodeClassName() + toString<unsigned short>(index) ) )
			index++;
		node->setNodeName( node->getNodeClassName() + toString<unsigned short>(index) );
		anim_tree->createNode( node->getNodeClassID(), node->getNodeID(), node->getNodeName() );
		anim_tree->getAnimationTree()->createNode( node->getNodeClassID(), node->getNodeID(), node->getNodeName() );
	}
}

void AnimTreeConnexSpace::addJCToAnimTree( wxConnexNode* node )
{
	AnimationTreeInstance* anim_tree = gApp->getCurrentAnimationTree();
	AnimationTreePtr anim_tree_ptr = anim_tree->getAnimationTree();
	if ( anim_tree == NULL )
		return;
	if ( anim_tree->getBoneController( node->getNodeID() ) != NULL ) //node exists
	{
		return;
	}
	else
	{
		unsigned short index = 0;
		while( anim_tree->hasBoneController( node->getNodeClassName() + toString<unsigned short>(index) ) )
			index++;
		node->setNodeName( node->getNodeClassName() + toString<unsigned short>(index) );
		anim_tree->createBoneController( BoneTransformController::ClassId(), node->getNodeID(), node->getNodeName() );
		anim_tree_ptr->createBoneController( BoneTransformController::ClassId(), node->getNodeID(), node->getNodeName() );
	}
}

void AnimTreeConnexSpace::removeNodeFromAnimTree( unsigned short id )
{
	AnimationTreeInstance* anim_tree = gApp->getCurrentAnimationTree();
	AnimationTreePtr anim_tree_ptr = anim_tree->getAnimationTree();
	if ( anim_tree == NULL )
		return;
	if ( anim_tree->getNode( id ) == NULL ) 
	{
		return;
	}
	else
	{
		anim_tree->deleteNode( id );
		anim_tree_ptr->deleteNode( id );
	}
	if ( GetSelectedNode()->getNodeID() == id )
	{
		SetSelectedNode( NULL );
		gApp->getAppFrame()->getPropertiesWindow()->SetPropertyGrid(NULL);
	}
}
	
void AnimTreeConnexSpace::removeJCFromAnimTree( unsigned short id )
{
	AnimationTreeInstance* anim_tree = gApp->getCurrentAnimationTree();
	AnimationTreePtr anim_tree_ptr = anim_tree->getAnimationTree();
	if ( anim_tree == NULL )
		return;
	if ( anim_tree->getBoneController( id ) == NULL ) 
	{
		return;
	}
	else
	{
		anim_tree->deleteBoneController( id );
		anim_tree_ptr->deleteBoneController( id );
	}
	if ( GetSelectedNode()->getNodeID() == id )
	{
		SetSelectedNode( NULL );
		gApp->getAppFrame()->getPropertiesWindow()->SetPropertyGrid(NULL);
	}
}

void AnimTreeConnexSpace::RemoveNode( unsigned short id )
{
	wxConnexSpace::RemoveNode( id );
	removeNodeFromAnimTree( id );
	removeJCFromAnimTree( id );
}

// not used .. remove
void AnimTreeConnexSpace::updateNodeConnections( wxConnexNode* parent, wxConnexNode* child )
{
	if ( parent == NULL || child == NULL )
		return;
	
	AnimationTreeInstance* anim_tree = gApp->getCurrentAnimationTree();
	AnimationTreePtr anim_tree_ptr = gApp->getCurrentAnimationTree()->getAnimationTree();

	AnimationNode* anim_tree_parent = anim_tree->getNode( parent->getNodeID() );
	AnimationNode* anim_tree_child = anim_tree->getNode( child->getNodeID() );

	if ( anim_tree_parent == NULL || anim_tree_child == NULL )
		return;
	if ( anim_tree_parent->hasChild( child->getNodeID() ) )
		return;
	anim_tree_parent->addChild( anim_tree_child );
}

void AnimTreeConnexSpace::ConnectPorts( wxConnexPortBase *port0, wxConnexPortBase *port1 )
{
	if ( port0 == NULL || port1 == NULL)
		return;
	wxConnexSpace::ConnectPorts( port0, port1 );
	
	AnimationTreeInstance* anim_tree = gApp->getCurrentAnimationTree();
	AnimationTreePtr anim_tree_ptr = gApp->getCurrentAnimationTree()->getAnimationTree();
	
	wxConnexNode* parent;
	wxConnexNode* child;
	if ( port0->IsInput() && port0->IsConnected( port1 ) && !port1->IsInput() )
	{
		parent = port0->m_Parent;
		child = port1->m_Parent;
	}
	else if ( port1->IsInput() && port1->IsConnected( port0 ) && !port0->IsInput() )
	{
		parent = port1->m_Parent;
		child = port0->m_Parent;
	}
	else
		return;
	
	if ( parent->isAnimationNode() && child->isAnimationNode() )
	{
		AnimationNode* anim_tree_parent = anim_tree->getNode( parent->getNodeID() );
		AnimationNode* anim_tree_child = anim_tree->getNode( child->getNodeID() );

		if ( anim_tree_parent == NULL || anim_tree_child == NULL )
			return;
		if ( anim_tree_parent->hasChild( child->getNodeID() ) )
			return;
		if ( anim_tree->getRoot() != NULL ) 
			if ( anim_tree->getRoot()->getId() == child->getNodeID() )
				SetRoot( NULL );
		anim_tree_parent->addChild( anim_tree_child );

		anim_tree_parent = anim_tree_ptr->getNode( parent->getNodeID() );
		anim_tree_child = anim_tree_ptr->getNode( child->getNodeID() );

		if ( anim_tree_parent == NULL || anim_tree_child == NULL )
			return;
		if ( anim_tree_parent->hasChild( child->getNodeID() ) )
			return;
		anim_tree_parent->addChild( anim_tree_child );
	}
	else if ( parent->isBoneController() && child->isBoneController() )
	{
		//connex node child = bone controller parent.getPrevious()
		BoneController* anim_tree_parent = anim_tree->getBoneController( parent->getNodeID() );
		BoneController* anim_tree_child = anim_tree->getBoneController( child->getNodeID() );

		if ( anim_tree_parent == NULL || anim_tree_child == NULL )
			return;
		anim_tree_child->setNext( anim_tree_parent );

		anim_tree_parent = anim_tree_ptr->getBoneController( parent->getNodeID() );
		anim_tree_child = anim_tree_ptr->getBoneController( child->getNodeID() );

		if ( anim_tree_parent == NULL || anim_tree_child == NULL )
			return;
		anim_tree_child->setNext( anim_tree_parent );
		
		if ( anim_tree->getFirst() == NULL || anim_tree->getFirst()->getId() == parent->getNodeID() )
				SetFirst( child );
	}
	Refresh();
}

void AnimTreeConnexSpace::RemoveConnection( wxConnexPortBase *port )
{
	if ( port == NULL )
		return;

	if ( !port->HasConnections() )
		return;

	wxConnexNode* parent;
	wxConnexNode* child;
	if ( port->IsInput() )
	{
		parent = port->m_Parent;
		child = port->m_Connections.front()->m_Parent;
	}
	else
	{
		child = port->m_Parent;
		parent = port->m_Connections.front()->m_Parent;
	}

	wxConnexSpace::RemoveConnection( port );

	if ( child->GetId() == child->m_Space->GetRoot()->GetId() && parent->GetId() == child->m_Space->GetFirst()->GetId() )
		return;

	AnimationTreeInstance* anim_tree = gApp->getCurrentAnimationTree();
	AnimationTreePtr anim_tree_ptr = gApp->getCurrentAnimationTree()->getAnimationTree();

	if ( parent->isAnimationNode() && child->isAnimationNode() )
	{
		AnimationNode* anim_tree_parent = anim_tree->getNode( parent->getNodeID() );
		AnimationNode* anim_tree_child = anim_tree->getNode( child->getNodeID() );

		if ( anim_tree_parent == NULL || anim_tree_child == NULL )
			return;
		anim_tree_parent->removeChild( anim_tree_child->getId() );

		anim_tree_parent = anim_tree_ptr->getNode( parent->getNodeID() );
		anim_tree_child = anim_tree_ptr->getNode( child->getNodeID() );

		if ( anim_tree_parent == NULL || anim_tree_child == NULL )
			return;
		anim_tree_parent->removeChild( anim_tree_child->getId() );
	}
	else if ( parent->isBoneController() && child->isBoneController() )
	{
		BoneController* anim_tree_parent = anim_tree->getBoneController( parent->getNodeID() );
		BoneController* anim_tree_child = anim_tree->getBoneController( child->getNodeID() );

		if ( anim_tree_parent == NULL || anim_tree_child == NULL )
			return;
		anim_tree_child->setNext( NULL );

		anim_tree_parent = anim_tree_ptr->getBoneController( parent->getNodeID() );
		anim_tree_child = anim_tree_ptr->getBoneController( child->getNodeID() );

		if ( anim_tree_parent == NULL || anim_tree_child == NULL )
			return;
		anim_tree_child->setNext( NULL );
	}
}

void AnimTreeConnexSpace::SetRoot( wxConnexNode* root )
{
	AnimationTreeInstance* anim_tree = gApp->getCurrentAnimationTree();
	AnimationTreePtr anim_tree_ptr = gApp->getCurrentAnimationTree()->getAnimationTree();
	if ( root == NULL )
	{
		anim_tree->setRoot( (unsigned short)0 );
		anim_tree_ptr->setRoot( (unsigned short)0 );
		wxConnexSpace::SetRoot(root);
		return;
	}
	//return if root has parent
	if ( anim_tree->getNode( root->getNodeID() )->getParent() != NULL )
		return;
	wxConnexSpace::SetRoot(root);
	anim_tree->setRoot( anim_tree->getNode( root->getNodeID() ) );
	anim_tree_ptr->setRoot( anim_tree->getNode( root->getNodeID() ) );

}

void AnimTreeConnexSpace::SetFirst( wxConnexNode* first )
{
	AnimationTreeInstance* anim_tree = gApp->getCurrentAnimationTree();
	AnimationTreePtr anim_tree_ptr = gApp->getCurrentAnimationTree()->getAnimationTree();
	if ( anim_tree->getBoneController( first->getNodeID() )->getPrevious() != NULL )
		return;
	wxConnexSpace::SetFirst(first);
	anim_tree->setFirst( anim_tree->getBoneController( first->getNodeID() ) );
	anim_tree_ptr->setFirst( anim_tree->getBoneController( first->getNodeID() ) );
}

void AnimTreeConnexSpace::SetSelectedNode( wxConnexNode* selected )
{
	wxConnexSpace::SetSelectedNode(selected);
	AnimationTreeInstance* anim_tree = gApp->getCurrentAnimationTree();
	AnimationTreePtr anim_tree_ptr = gApp->getCurrentAnimationTree()->getAnimationTree();
	gApp->getAppFrame()->getPropertiesWindow()->SetPropertyGrid( NULL );

	if ( selected == NULL )
		return;
	else if ( selected->getNodeClassID() == 1 )
	{
		AnimationSampler* anim_sampler = static_cast<AnimationSampler*>( anim_tree->getNode( selected->getNodeID() ) );
		gApp->getAppFrame()->getPropertiesWindow()->SetPropertyGrid( new AnimationSamplerPropGrid( gApp->getAppFrame()->getPropertiesWindow(), anim_sampler ) );
	}
	else if ( selected->getNodeClassID() == 2 )
	{
		AnimationBlender* blender = static_cast<AnimationBlender*>( anim_tree->getNode( selected->getNodeID() ) );
		gApp->getAppFrame()->getPropertiesWindow()->SetPropertyGrid( new AnimationBlenderPropGrid( gApp->getAppFrame()->getPropertiesWindow(), blender ) );
	}
	else if ( selected->getNodeClassID() == 3 )
	{
		AnimationMixer* mixer = static_cast<AnimationMixer*>( anim_tree->getNode( selected->getNodeID() ) );
		gApp->getAppFrame()->getPropertiesWindow()->SetPropertyGrid( new AnimationMixerPropGrid( gApp->getAppFrame()->getPropertiesWindow(), mixer ) );
	}
	else if ( selected->getNodeClassID() == 4 )
	{
		AnimationTransitionBlender* trans_blender = static_cast<AnimationTransitionBlender*>( anim_tree->getNode( selected->getNodeID() ) );
		gApp->getAppFrame()->getPropertiesWindow()->SetPropertyGrid( new AnimationTransitionBlenderPropGrid( gApp->getAppFrame()->getPropertiesWindow(), trans_blender ) );
	}
	else if ( selected->getNodeClassID() == 5 )
	{
		FaceController* face_controller = static_cast<FaceController*>( anim_tree->getNode( selected->getNodeID() ) );
		gApp->getAppFrame()->getPropertiesWindow()->SetPropertyGrid( new FaceControllerPropGrid( gApp->getAppFrame()->getPropertiesWindow(), face_controller ) );
	}
	else if ( selected->getNodeClassID() == 6 )
	{
		BoneTransformController* trans_controller = static_cast<BoneTransformController*>( anim_tree->getBoneController( selected->getNodeID() ) );
		gApp->getAppFrame()->getPropertiesWindow()->SetPropertyGrid( new BoneTransformControllerPropGrid( gApp->getAppFrame()->getPropertiesWindow(), trans_controller ) );
	}
	else if ( selected->getNodeClassID() == 7 )
	{
		BoneIKController* ik_controller = static_cast<BoneIKController*>( anim_tree->getBoneController( selected->getNodeID() ) );
		gApp->getAppFrame()->getPropertiesWindow()->SetPropertyGrid( new BoneIKControllerPropGrid( gApp->getAppFrame()->getPropertiesWindow(), ik_controller ) );
	}
	else if ( selected->isAnimationNode() )
	{
		AnimationNode* selected_node = anim_tree->getNode( selected->getNodeID() );
		gApp->getAppFrame()->getPropertiesWindow()->SetPropertyGrid( new AnimationNodePropGrid( gApp->getAppFrame()->getPropertiesWindow(), selected_node ) );
	}
	else if ( selected->isBoneController() )
	{
		BoneController* selected_jc = anim_tree->getBoneController( selected->getNodeID() );
		gApp->getAppFrame()->getPropertiesWindow()->SetPropertyGrid( new BoneControllerPropGrid( gApp->getAppFrame()->getPropertiesWindow(), selected_jc ) );
	}
}

void AnimTreeConnexSpace::UpdateNodeName( wxConnexNode* node )
{
	AnimationTreeInstance* anim_tree = gApp->getCurrentAnimationTree();
	std::string node_name = "";
	if ( node->isAnimationNode() )
		node_name = anim_tree->getNode( node->getNodeID() )->getName();
	else if ( node->isBoneController() )
		node_name = anim_tree->getBoneController( node->getNodeID() )->getName();
	node->setNodeName(node_name);
}

void AnimTreeConnexSpace::OnMouseEvent( wxMouseEvent& evt )
{
	wxConnexSpace::OnMouseEvent( evt );
	this->SetSize( window_size );
}

//////////////////////

wxConnexNode* AnimTreeConnexSpace::createAnimationNode( std::string class_name, unsigned short node_id, std::string node_name )
{
	wxConnexNode* new_node;
	if ( class_name == "AnimationSampler" )
		new_node = _createAnimationSampler( node_id, node_name );
	else if ( class_name == "AnimationBlender" )
		new_node = _createAnimationBlender( node_id, node_name );
	else if ( class_name == "AnimationMixer" )
		new_node = _createAnimationMixer( node_id, node_name );
	else if ( class_name == "AnimationTransitionBlender" )
		new_node = _createAnimationTransitionBlender( node_id, node_name );
	else if ( class_name == "FaceController" )
		new_node = _createFaceController( node_id, node_name );
	else if ( class_name == "BoneTransformController" )
		new_node = _createBoneTransformController( node_id, node_name );
	else if ( class_name == "BoneIKController" )
		new_node = _createBoneIKController( node_id, node_name );
	else
		return NULL;
	return new_node;
}

wxConnexNode* AnimTreeConnexSpace::_createAnimationSampler( unsigned short node_id, std::string node_name )
{
	wxConnexNode* n1 = new wxConnexNodeInput( this, wxID_ANY, wxPoint(100,20), wxSize(160, 60) );
	n1->SetLabel ( "AnimationSampler" );
	n1->setNodeID ( node_id );
	n1->setNodeName ( node_name );
	n1->connexNode_classID = 1;
	n1->connexNode_className = "AnimationSampler";
	this->AddNode( n1 );
	return n1;
}

wxConnexNode* AnimTreeConnexSpace::_createAnimationBlender( unsigned short node_id, std::string node_name )
{
	wxConnexNode* n1 = new wxConnexNodeMultiplePorts( this, wxID_ANY, wxPoint(400,20), wxSize(200, 100) );
	n1->SetLabel( "AnimationBlender" );
	n1->setNodeID ( node_id );
	n1->setNodeName ( node_name );
	n1->connexNode_classID = 2;
	n1->connexNode_className = "AnimationBlender";
	this->AddNode( n1 );
	return n1;
}

wxConnexNode* AnimTreeConnexSpace::_createAnimationMixer( unsigned short node_id, std::string node_name )
{
	wxConnexNode* n1 = new wxConnexNodeMultiplePorts( this, wxID_ANY, wxPoint(400,20), wxSize(200, 100) );
	n1->SetLabel( "AnimationMixer" );
	n1->setNodeID ( node_id );
	n1->setNodeName ( node_name );
	n1->connexNode_classID = 3;
	n1->connexNode_className = "AnimationMixer";
	this->AddNode( n1 );
	return n1;
}

wxConnexNode* AnimTreeConnexSpace::_createAnimationTransitionBlender( unsigned short node_id, std::string node_name )
{
	wxConnexNode* n1 = new wxConnexNodeMultiplePorts( this, wxID_ANY, wxPoint(600,20), wxSize(220, 100) );
	n1->SetLabel( "AnimationTransitionBlender" );
	n1->setNodeID ( node_id );
	n1->setNodeName ( node_name );
	n1->connexNode_classID = 4;
	n1->connexNode_className = "AnimationTransitionBlender";
	this->AddNode( n1 );
	return n1;
}

wxConnexNode* AnimTreeConnexSpace::_createFaceController( unsigned short node_id, std::string node_name )
{
	wxConnexNode* n1 = new wxConnexNodeInput( this, wxID_ANY, wxPoint(100,20), wxSize(160, 60) );
	n1->SetLabel( "FaceController" );
	n1->setNodeID ( node_id );
	n1->setNodeName ( node_name );	
	n1->connexNode_classID = 5;
	n1->connexNode_className = "FaceController";
	this->AddNode( n1 );
	return n1;
}

wxConnexNode* AnimTreeConnexSpace::_createBoneTransformController( unsigned short node_id, std::string node_name )
{
	wxConnexNode* n1 = new wxConnexNodeBoneController( this, wxID_ANY, wxPoint(100,20), wxSize(200, 80) );
	n1->SetLabel( "BoneTransformController" );
	n1->setNodeID ( node_id );
	n1->setNodeName ( node_name );	
	n1->connexNode_classID = 6;
	n1->connexNode_className = "BoneTransformController";
	this->AddNode( n1 );
	return n1;
}

wxConnexNode* AnimTreeConnexSpace::_createBoneIKController( unsigned short node_id, std::string node_name )
{
	wxConnexNode* n1 = new wxConnexNodeBoneController( this, wxID_ANY, wxPoint(100,20), wxSize(200, 80) );
	n1->SetLabel( "BoneIKController" );
	n1->setNodeID ( node_id );
	n1->setNodeName ( node_name );	
	n1->connexNode_classID = 7;
	n1->connexNode_className = "BoneIKController";
	this->AddNode( n1 );
	return n1;
}

//////
//////right click menu
//////

void AnimTreeConnexSpace::OnMenu_CreateAnimationSampler( wxCommandEvent& evt )
{
	wxConnexNode* con_node = createAnimationNode( "AnimationSampler", 1, "AnimationSampler" );
	con_node->SetPosition( wxPoint(wxGetMousePosition().x - this->GetScreenPosition().x - 200, wxGetMousePosition().y - this->GetScreenPosition().y - 50) );
	addNodeToAnimTree( con_node );
}

void AnimTreeConnexSpace::OnMenu_CreateAnimationBlender( wxCommandEvent& evt )
{
	wxConnexNode* con_node = createAnimationNode( "AnimationBlender", 1, "AnimationBlender" );
	con_node->SetPosition( wxPoint(wxGetMousePosition().x - this->GetScreenPosition().x - 200, wxGetMousePosition().y - this->GetScreenPosition().y - 50) );
	addNodeToAnimTree( con_node );
}

void AnimTreeConnexSpace::OnMenu_CreateAnimationMixer( wxCommandEvent& evt )
{
	wxConnexNode* con_node = createAnimationNode( "AnimationMixer", 1, "AnimationMixer" );
	con_node->SetPosition( wxPoint(wxGetMousePosition().x - this->GetScreenPosition().x - 200, wxGetMousePosition().y - this->GetScreenPosition().y - 50) );
	addNodeToAnimTree( con_node );
}

void AnimTreeConnexSpace::OnMenu_CreateAnimationTransitionBlender( wxCommandEvent& evt )
{
	wxConnexNode* con_node = createAnimationNode( "AnimationTransitionBlender", 1, "AnimationTransitionBlender" );
	con_node->SetPosition( wxPoint(wxGetMousePosition().x - this->GetScreenPosition().x - 200, wxGetMousePosition().y - this->GetScreenPosition().y - 50) );
	addNodeToAnimTree( con_node );
}

void AnimTreeConnexSpace::OnMenu_CreateFaceController( wxCommandEvent& evt )
{
	wxConnexNode* con_node = createAnimationNode( "FaceController", 1, "FaceController" );
	con_node->SetPosition( wxPoint(wxGetMousePosition().x - this->GetScreenPosition().x - 200, wxGetMousePosition().y - this->GetScreenPosition().y - 50) );
	addNodeToAnimTree( con_node );
}

void AnimTreeConnexSpace::OnMenu_CreateBoneTransformController( wxCommandEvent& evt )
{
	wxConnexNode* con_node = createAnimationNode( "BoneTransformController", 1, "BoneTransformController" );
	con_node->SetPosition( wxPoint(wxGetMousePosition().x - this->GetScreenPosition().x - 200, wxGetMousePosition().y - this->GetScreenPosition().y - 50) );
	addJCToAnimTree( con_node );
}

void AnimTreeConnexSpace::OnMenu_CreateBoneIKController( wxCommandEvent& evt )
{
	wxConnexNode* con_node = createAnimationNode( "BoneIKController", 1, "BoneIKController" );
	con_node->SetPosition( wxPoint(wxGetMousePosition().x - this->GetScreenPosition().x - 200, wxGetMousePosition().y - this->GetScreenPosition().y - 50) );
	addJCToAnimTree( con_node );
}

void AnimTreeConnexSpace::OnMenu_AutoArrangeNodes( wxCommandEvent& evt )
{
	this->mNodeGrid.clear();
	this->mJCGrid.clear();
	this->enum_Nodes.clear();
	this->enum_JC.clear();
	this->enumNodes( 0, gApp->getCurrentAnimationTree()->getRoot() );
	if ( gApp->getCurrentAnimationTree()->getNumBoneControllers() > 0 )
	{
		BoneController* root = gApp->getCurrentAnimationTree()->getFirst();
		if ( root == NULL )
			return;
		while ( root->getPrevious() != NULL )
			root = root->getPrevious();
		this->enumJC( 0, root );
	}
	
	autoArrangeNodes( gApp->getCurrentAnimationTree() );
}

BEGIN_EVENT_TABLE( AnimTreeConnexSpace, wxWindow )
	EVT_SIZE( AnimTreeConnexSpace::OnSize )
	EVT_PAINT( wxConnexSpace::OnPaint )
	EVT_MOUSE_EVENTS( wxConnexSpace::OnMouseEvent )
	// create node menu events
	EVT_MENU( ID_mnCreateAnimationSampler, AnimTreeConnexSpace::OnMenu_CreateAnimationSampler ) 
	EVT_MENU( ID_mnCreateAnimationBlender, AnimTreeConnexSpace::OnMenu_CreateAnimationBlender )
	EVT_MENU( ID_mnCreateAnimationMixer, AnimTreeConnexSpace::OnMenu_CreateAnimationMixer )  
	EVT_MENU( ID_mnCreateAnimationTransitionBlender, AnimTreeConnexSpace::OnMenu_CreateAnimationTransitionBlender )
	EVT_MENU( ID_mnCreateFaceController, AnimTreeConnexSpace::OnMenu_CreateFaceController )
	EVT_MENU( ID_mnCreateBoneTransformController, AnimTreeConnexSpace::OnMenu_CreateBoneTransformController ) 
	EVT_MENU( ID_mnCreateBoneIKController, AnimTreeConnexSpace::OnMenu_CreateBoneIKController ) 
	//
	EVT_MENU( ID_mnAutoArrangeNodes, AnimTreeConnexSpace::OnMenu_AutoArrangeNodes )
END_EVENT_TABLE()