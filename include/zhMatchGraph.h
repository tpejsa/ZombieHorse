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

#ifndef __zhMatchGraph_h__
#define __zhMatchGraph_h__

#include "zhPrereq.h"
#include "zhIterators.h"
#include "zhMatchWeb.h"

namespace zh
{

/**
* @brief Class representing a graph of matching animation segments.
* Each graph edge encodes the temporal alignment (match sequence) between
* two matching animation segments.
*/
class zhDeclSpec MatchGraph
{

public:

	struct Node;
	struct Edge;
	struct MatchPoint;

	typedef MapIterator< std::map<unsigned short, Node*> > NodeIterator;
	typedef MapConstIterator< std::map<unsigned short, Node*> > NodeConstIterator;

	/**
	* @brief Point of a match sequence.
	*/
	struct zhDeclSpec MatchPoint
	{

		/**
		* Constructor.
		*
		* @param time1 Time in the first animation.
		* @param time2 Time in the second animation.
		* @param alignTransf Aligning 2D tranformation.
		*/
		MatchPoint( float time1, float time2, const Skeleton::Situation& alignTransf ) : mTime1(time1), mTime2(time2), mAlignTransf(alignTransf) { }

		/**
		* Gets the time in the first animation.
		*/
		float getTime1() const { return mTime1; }

		/**
		* Gets the time in the second animation.
		*/
		float getTime2() const { return mTime2; }

		/**
		* Gets the aligning 2D transformation at the match point.
		*/
		const Skeleton::Situation& getAlignTransf() const { return mAlignTransf; }

	private:

		float mTime1, mTime2;
		Skeleton::Situation mAlignTransf;

	};

	/**
	* @brief Match graph node.
	*/
	struct zhDeclSpec Node
	{

		friend class MatchGraph;

	public:

		/**
		* Constructor.
		*
		* @param handle Node handle.
		* @param animSeg Animation segment.
		*/
		Node( unsigned short handle, const AnimationSegment& animSeg );

		/**
		* Gets the node handle.
		*/
		unsigned short getHandle() const;

		/**
		* Gets this node's animation segment.
		*/
		const AnimationSegment& getAnimationSegment() const;

		/**
		* Gets a node that precedes this one.
		*
		* @param nodeHandle Previous node handle.
		* @return Pointer to the node or NULL if the specified
		* node is not connected to this one.
		*/
		Node* getPrev( unsigned short nodeHandle ) const;

		/**
		* Gets the number of previous nodes.
		*/
		unsigned int getNumPrevNodes() const;

		/**
		* Gets an iterator over the map of nodes that
		* precede this one.
		*/
		NodeConstIterator getPrevConstIterator() const;

		/**
		* Gets a node that follows this one.
		*
		* @param nodeHandle Next node handle.
		* @return Pointer to the node or NULL if the specified
		* node is not connected to this one.
		*/
		Node* getNext( unsigned short nodeHandle ) const;

		/**
		* Gets the number of next nodes.
		*/
		unsigned int getNumNextNodes() const;

		/**
		* Gets an iterator over the map of nodes that
		* follow this one.
		*/
		NodeConstIterator getNextConstIterator() const;

	private:

		unsigned short mHandle;
		AnimationSegment mAnimSeg;

		std::map<unsigned short, Node*> mPrevNodes;
		std::map<unsigned short, Node*> mNextNodes;

	};

	/**
	* @brief Edge connecting two match graph nodes.
	*/
	struct zhDeclSpec Edge
	{

		friend class MatchGraph;

	public:

		/**
		* Constructor.
		*
		* @param node1 The first node.
		* @param node2 The second node.
		* @param matchSeq Match sequence that represents a temporal alignment
		* between the two nodes' animation segments.
		* @param cost Edge cost
		* (actually averaged animation distance along the match sequence).
		*/
		Edge( Node* node1, Node* node2, const std::vector<MatchPoint>& matchSeq, float cost );

		/**
		* Gets a pointer the first node.
		*/
		Node* getNode1() const;

		/**
		* Gets a pointer the second node.
		*/
		Node* getNode2() const;

		/**
		* Gets the match sequence representing a temporal alignment
		* between the two nodes' animation segments.
		*/
		const std::vector<MatchPoint>& getMatchSequence() const;

		/**
		* Gets the cost assigned to the edge.
		*/
		float getCost() const;

		/**
		* Samples this edge's match sequence at the specified time
		* in of the two animation segments.
		*
		* @param time Sample time.
		* @param firstAnim If true, sample time is specified in
		* the time frame of the first node's animation segment,
		* otherwise it is specified in the time frame of the second
		* node's animation segment.
		* @return Spatio-temporal alignment information for the two
		* animation segments at the specified time.
		*/
		MatchPoint sample( float time, bool firstAnim = true ) const;

	private:

		Node *mNode1, *mNode2;
		std::vector<MatchPoint> mMatchSeq;
		float mCost;

	};

	/**
	* Constructor.
	*/
	MatchGraph();

	/**
	* Destructor.
	*/
	~MatchGraph();

	/**
	* Creates the match graph's root node,
	* representing the initial query animation segment.
	*
	* @param animSeg Initial query animation segment.
	* @return Pointer to the root node or NULL if the root
	* node already exists.
	*/
	Node* createRoot( const AnimationSegment& animSeg );

	/**
	* Creates a new node in the match graph.
	*
	* @param connHandle Handle of the node that the new node should
	* be connected to.
	* @param animSeg Animation segment associated with the new node.
	* @param matchSeq Match sequence that matches the new node
	* to the connecting node.
	* @param cost Edge cost
	* (actually averaged animation distance along the match sequence).
	* @return Pointer to the new node or NULL if the node could not
	* be created.
	*/
	Node* createNode( unsigned short connHandle, const AnimationSegment& animSeg,
		const std::vector<MatchPoint>& matchSeq, float cost );

	/**
	* Deletes the specified node.
	*
	* @param nodeHandle Node handle.
	*/
	void deleteNode( unsigned short nodeHandle );

	/**
	* Deletes all nodes in the match graph.
	*/
	void deleteAllNodes();

	/**
	* Checks if the specified node exists.
	*
	* @param nodeHandle Node handle.
	* @return true if the node exists, false otherwise.
	*/
	bool hasNode( unsigned short nodeHandle ) const;

	/**
	* Gets the specified node.
	*
	* @param nodeHandle Node handle.
	* @return Pointer to the specified node or NULL
	* if the node with that handle does not exist.
	*/
	Node* getNode( unsigned short nodeHandle ) const;

	/**
	* Gets the number of nodes.
	*/
	unsigned int getNumNodes() const;

	/**
	* Gets an iterator over the map of nodes.
	*/
	NodeIterator getNodeIterator();

	/**
	* Gets a const iterator over the map of nodes.
	*/
	NodeConstIterator getNodeConstIterator() const;

	/**
	* Gets the root node.
	*/
	Node* getRoot() const;

	/**
	* Checks if there is an edge between two nodes.
	*
	* @param node1Handle The first node.
	* @param node2Handle The second node.
	* @return true if the edge exists, otherwise false.
	*/
	bool hasEdge( unsigned short node1Handle, unsigned short node2Handle ) const;

	/**
	* Gets the edge between two nodes.
	*
	* @param node1Handle The first node.
	* @param node2Handle The second node.
	* @return Pointer to the edge or NULL if it does not exist.
	*/
	Edge* getEdge( unsigned short node1Handle, unsigned short node2Handle ) const;

	/**
	* Computes optimal (lowest-cost) path between a source node
	* and one or more target nodes.
	* 
	* @param srcHandle Source node handle.
	* @param trgHandles Target node handles.
	* @param paths Optimal paths (given as vectors storing
	* node sequences). If a target node is unreachable from
	* the given source node, then a path will not be computed
	* for that target node.
	* @return false if the shortest path could not be computed,
	* otherwise true.
	*/
	void computeOptimalPath( unsigned short srcHandle, const std::vector<unsigned short>& trgHandles,
		std::map< unsigned short, std::vector<Node*> >& paths ) const;

private:

	void _deleteNode( unsigned short nodeHandle );

	std::map<unsigned short, Node*> mNodes;
	std::map< std::pair<unsigned short, unsigned short>, Edge* > mEdges;

	unsigned short mNextHandle;

};

}

#endif // __zhMatchGraph_h__
