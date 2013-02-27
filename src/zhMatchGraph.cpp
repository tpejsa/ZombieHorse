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

#include "zhMatchGraph.h"

namespace zh
{

MatchGraph::Node::Node( unsigned short handle, const AnimationSegment& animSeg )
: mHandle(handle), mAnimSeg(animSeg)
{
}

unsigned short MatchGraph::Node::getHandle() const
{
	return mHandle;
}

const AnimationSegment& MatchGraph::Node::getAnimationSegment() const
{
	return mAnimSeg;
}

MatchGraph::Node* MatchGraph::Node::getPrev( unsigned short nodeHandle ) const
{
	std::map<unsigned short, Node*>::const_iterator pnode_i = mPrevNodes.find(nodeHandle);

	if( pnode_i != mPrevNodes.end() )
		return pnode_i->second;

	return NULL;
}

unsigned int MatchGraph::Node::getNumPrevNodes() const
{
	return mPrevNodes.size();
}

MatchGraph::NodeConstIterator MatchGraph::Node::getPrevConstIterator() const
{
	return NodeConstIterator( mPrevNodes );
}

MatchGraph::Node* MatchGraph::Node::getNext( unsigned short nodeHandle ) const
{
	std::map<unsigned short, Node*>::const_iterator nnode_i = mNextNodes.find(nodeHandle);

	if( nnode_i != mNextNodes.end() )
		return nnode_i->second;

	return NULL;
}

unsigned int MatchGraph::Node::getNumNextNodes() const
{
	return mNextNodes.size();
}

MatchGraph::NodeConstIterator MatchGraph::Node::getNextConstIterator() const
{
	return NodeConstIterator( mNextNodes );
}

MatchGraph::Edge::Edge( Node* node1, Node* node2, const std::vector<MatchPoint>& matchSeq, float cost )
: mNode1(node1), mNode2(node2), mMatchSeq(matchSeq), mCost(cost)
{
	zhAssert( node1 != NULL && node2 != NULL );
}

MatchGraph::Node* MatchGraph::Edge::getNode1() const
{
	return mNode1;
}

MatchGraph::Node* MatchGraph::Edge::getNode2() const
{
	return mNode2;
}

const std::vector<MatchGraph::MatchPoint>& MatchGraph::Edge::getMatchSequence() const
{
	return mMatchSeq;
}

float MatchGraph::Edge::getCost() const
{
	return mCost;
}

MatchGraph::MatchPoint MatchGraph::Edge::sample( float time, bool firstAnim ) const
{
	zhAssert( mMatchSeq.size() > 0 );

	int min_i = 0,
		max_i = mMatchSeq.size() - 1,
		i = 0;

	do
	{
		i = min_i + ( max_i - min_i ) / 2;
		
		if( time > ( firstAnim ? mMatchSeq[i].getTime1() : mMatchSeq[i].getTime2() ) )
			min_i = i + 1;
		else
			max_i = i - 1;
	}
	while( min_i <= max_i );

	return mMatchSeq[i];
}

MatchGraph::MatchGraph()
: mNextHandle(0)
{
}

MatchGraph::~MatchGraph()
{
	deleteAllNodes();
}

MatchGraph::Node* MatchGraph::createRoot( const AnimationSegment& animSeg )
{
	if( getNumNodes() > 0 )
		return NULL;

	mNextHandle = 0;
	Node* node = new Node( mNextHandle++, animSeg );
	mNodes.insert( std::make_pair( node->getHandle(), node ) );

	return node;
}

MatchGraph::Node* MatchGraph::createNode( unsigned short connHandle, const AnimationSegment& animSeg,
										 const std::vector<MatchPoint>& matchSeq, float cost )
{
	zhAssert( hasNode(connHandle) );

	// create new node
	Node* cnode = getNode( connHandle );
	Node* node = new Node( mNextHandle++, animSeg );
	mNodes.insert( std::make_pair( node->getHandle(), node ) );
	
	// connect it to the specified node
	unsigned short nh1 = cnode->getHandle(), nh2 = node->getHandle();
	Edge* edge = new Edge( cnode, node, matchSeq, cost );
	if( nh1 > nh2 ) std::swap( nh1, nh2 );
	mEdges.insert( std::make_pair( std::make_pair( nh1, nh2 ), edge ) );
	cnode->mNextNodes.insert( std::make_pair( node->getHandle(), node ) );
	node->mPrevNodes.insert( std::make_pair( cnode->getHandle(), cnode ) );

	return node;
}

void MatchGraph::deleteNode( unsigned short nodeHandle )
{
	if( !hasNode(nodeHandle) || nodeHandle == 0 ) // (don't delete root node)
		return;

	// delete the node
	_deleteNode(nodeHandle);

	// detect nodes that are now unreachable from root
	std::vector<unsigned short> nodes;
	std::map< unsigned short, std::vector<Node*> > paths;
	NodeConstIterator node_i = getNodeConstIterator();
	while( !node_i.end() )
		nodes.push_back( node_i.next()->getHandle() );
	computeOptimalPath( 0, nodes, paths );

	// delete unreachable nodes
	node_i = getNodeConstIterator();
	while( !node_i.end() )
	{
		Node* node = node_i.next();
		unsigned short node_h = node->getHandle();

		if( paths.count(node_h) <= 0 )
			_deleteNode(node_h);
	}
}

void MatchGraph::deleteAllNodes()
{
	for( std::map< std::pair<unsigned short, unsigned short>, Edge* >::iterator edge_i = mEdges.begin();
		edge_i != mEdges.end(); ++edge_i )
		delete edge_i->second;
	
	for( std::map<unsigned short, Node*>::iterator node_i = mNodes.begin();
		node_i != mNodes.end(); ++node_i )
		delete node_i->second;

	mNodes.clear();
	mEdges.clear();
}

bool MatchGraph::hasNode( unsigned short nodeHandle ) const
{
	return mNodes.count(nodeHandle) > 0;
}

MatchGraph::Node* MatchGraph::getNode( unsigned short nodeHandle ) const
{
	std::map<unsigned short, Node*>::const_iterator node_i = mNodes.find(nodeHandle);

	if( node_i != mNodes.end() )
		return node_i->second;

	return NULL;
}

unsigned int MatchGraph::getNumNodes() const
{
	return mNodes.size();
}

MatchGraph::NodeIterator MatchGraph::getNodeIterator()
{
	return NodeIterator(mNodes);
}

MatchGraph::NodeConstIterator MatchGraph::getNodeConstIterator() const
{
	return NodeConstIterator(mNodes);
}

MatchGraph::Node* MatchGraph::getRoot() const
{
	return getNode(0);
}

bool MatchGraph::hasEdge( unsigned short node1Handle, unsigned short node2Handle ) const
{
	if( node2Handle < node1Handle )
		std::swap( node1Handle, node2Handle );

	return mEdges.count( std::pair<unsigned short, unsigned short>( node1Handle, node2Handle ) ) > 0;
}

MatchGraph::Edge* MatchGraph::getEdge( unsigned short node1Handle, unsigned short node2Handle ) const
{
	if( node2Handle < node1Handle )
		std::swap( node1Handle, node2Handle );

	std::map< std::pair<unsigned short, unsigned short>, Edge* >::const_iterator edge_i = mEdges.find(
		std::pair<unsigned short, unsigned short>( node1Handle, node2Handle ) );

	if( edge_i != mEdges.end() )
		return edge_i->second;

	return NULL;
}

void MatchGraph::computeOptimalPath( unsigned short srcHandle, const std::vector<unsigned short>& trgHandles,
									std::map< unsigned short, std::vector<Node*> >& paths ) const
{
	zhAssert( hasNode(srcHandle) );

	std::map<unsigned short, float> costs;
	std::map<unsigned short, Node*> prev_nodes;
	std::set<unsigned short> nodes;

	NodeConstIterator node_i = getNodeConstIterator();
	while( !node_i.end() )
	{
		Node* node = node_i.next();

		costs[ node->getHandle() ] = FLT_MAX;
		prev_nodes[ node->getHandle() ] = NULL;
		nodes.insert( node->getHandle() );
	}

	costs[srcHandle] = 0;
	
	while( !nodes.empty() )
	{
		unsigned short node_h = srcHandle;
		float cost = FLT_MAX;

		for( std::set<unsigned short>::const_iterator node_i = nodes.begin();
			node_i != nodes.end(); ++node_i )
		{
			float cur_cost = costs[*node_i];

			if( cur_cost < cost )
			{
				node_h = *node_i;
				cost = cur_cost;
			}
		}

		nodes.erase(node_h);

		Node* node = getNode(node_h);
		NodeConstIterator next_i = node->getNextConstIterator();
		while( !next_i.end() )
		{
			Node* next_node = next_i.next();
			float new_cost = cost + getEdge( node_h, next_node->getHandle() )->getCost();
			if( new_cost < costs[ next_node->getHandle() ] )
			{
				costs[ next_node->getHandle() ] = new_cost;
				prev_nodes[ next_node->getHandle() ] = getNode(node_h);
			}
		}
	}

	// trace optimal paths
	for( unsigned int trg_i = 0; trg_i < trgHandles.size(); ++trg_i )
	{
		zhAssert( hasNode( trgHandles[trg_i] ) );

		std::vector<Node*> path;
		unsigned short node_h = trgHandles[trg_i];

		while( prev_nodes[node_h] != NULL )
		{
			path.insert( path.begin(), getNode(node_h) );
			node_h = prev_nodes[node_h]->getHandle();
		}
		path.insert( path.begin(), getNode(node_h) );

		if( path.size() > 0 )
			paths[ trgHandles[trg_i] ] = path;
	}
}

void MatchGraph::_deleteNode( unsigned short nodeHandle )
{
	Node* node = getNode(nodeHandle);

	// disconnect the node from its predecessors
	NodeConstIterator pnode_i = node->getPrevConstIterator();
	while( !pnode_i.end() )
	{
		Node* pnode = pnode_i.next();

		pnode->mNextNodes.erase(nodeHandle);
		node->mPrevNodes.erase( pnode->getHandle() );

		unsigned short nh1 = pnode->getHandle(), nh2 = node->getHandle();
		if( nh1 > nh2 ) std::swap( nh1, nh2 );
		std::map< std::pair<unsigned short, unsigned short>, Edge* >::iterator edge_i = mEdges.find( std::make_pair( nh1, nh2 ) );
		if( edge_i != mEdges.end() )
		{
			Edge* edge = edge_i->second;
			
			delete edge;
			mEdges.erase(edge_i);
		}

	}

	// disconnect the node from its followers
	NodeConstIterator nnode_i = node->getNextConstIterator();
	while( !nnode_i.end() )
	{
		Node* nnode = nnode_i.next();

		nnode->mPrevNodes.erase(nodeHandle);
		node->mNextNodes.erase( nnode->getHandle() );

		unsigned short nh1 = nnode->getHandle(), nh2 = node->getHandle();
		if( nh1 > nh2 ) std::swap( nh1, nh2 );
		std::map< std::pair<unsigned short, unsigned short>, Edge* >::iterator edge_i = mEdges.find( std::make_pair( nh1, nh2 ) );
		if( edge_i != mEdges.end() )
		{
			Edge* edge = edge_i->second;
			
			delete edge;
			mEdges.erase(edge_i);
		}

	}

	// delete the node
	mNodes.erase(nodeHandle);
	delete node;
}

}
