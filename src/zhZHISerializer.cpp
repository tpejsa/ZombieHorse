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

#include "zhZHISerializer.h"
#include "zhString.h"
#include "zhFileSystem.h"
#include "zhAnimation.h"
#include "rapidxml_print.hpp"

namespace zh
{

bool ZHISerializer::trySerialize( ResourcePtr res, const std::string& path )
{
	zhAssert( res != NULL );

	std::string dir, filename, prefix, ext;
	parsePathStr( path, dir, filename, prefix, ext );

	if( ext != "vai" )
		return false;

	return true;
}

bool ZHISerializer::serialize( ResourcePtr res, const std::string& path )
{
	zhAssert( res != NULL );

	mAnimIndex = AnimationIndexPtr::DynamicCast<Resource>(res);
	mPath = path;

	// write ZHI file
	char* node_name = mDoc.allocate_string( "AnimationIndex" );
	rapidxml::xml_node<>* node = mDoc.allocate_node( rapidxml::node_element, node_name );
	mDoc.append_node(node);
	if( !writeAnimationIndex(node) )
		return false;

	// serialize DOM
	ofstream vaf( path.c_str() );
	vaf << mDoc;

	// free memory
	vaf.close();
	mDoc.clear();

	return true;
}

bool ZHISerializer::writeAnimationIndex( rapidxml::xml_node<>* node )
{
	rapidxml::xml_attribute<>* attrib;
	rapidxml::xml_node<>* child;

	char* node_name;
	char* attrib_name;
	char* attrib_value;

	// write attributes:

	attrib_name = mDoc.allocate_string( "version" );
	attrib_value = mDoc.allocate_string( "1.0" );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	attrib_name = mDoc.allocate_string( "id" );
	attrib_value = mDoc.allocate_string( toString<unsigned long>( mAnimIndex->getId() ).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	attrib_name = mDoc.allocate_string( "name" );
	attrib_value = mDoc.allocate_string( mAnimIndex->getName().c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	// write children:

	node_name = mDoc.allocate_string( "Animations" );
	child = mDoc.allocate_node( rapidxml::node_element, node_name );
	node->append_node(child);

	if( !writeAnimations(child) )
		return false;

	node_name = mDoc.allocate_string( "MatchWebs" );
	child = mDoc.allocate_node( rapidxml::node_element, node_name );
	node->append_node(child);

	if( !writeMatchWebs(child) )
		return false;

	return true;
}

bool ZHISerializer::writeAnimations( rapidxml::xml_node<>* node )
{
	rapidxml::xml_node<>* child;

	char* node_name;
	
	// write children:

	for( unsigned int segi = 0; segi < mAnimIndex->getNumAnimationSegments(); ++segi )
	{
		node_name = mDoc.allocate_string( "Animation" );
		child = mDoc.allocate_node( rapidxml::node_element, node_name );
		node->append_node(child);

		if( !writeAnimation( child, mAnimIndex->getAnimationSegment(segi) ) )
			return false;
	}

	return true;
}

bool ZHISerializer::writeAnimation( rapidxml::xml_node<>* node, const AnimationSegment& animSeg )
{
	rapidxml::xml_attribute<>* attrib;

	char* attrib_name;
	char* attrib_value;

	// write attributes:
	
	attrib_name = mDoc.allocate_string( "animationSetId" );
	attrib_value = mDoc.allocate_string( toString<unsigned long>( animSeg.getAnimation()->getAnimationSet()->getId() ).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	attrib_name = mDoc.allocate_string( "animationId" );
	attrib_value = mDoc.allocate_string( toString<unsigned short>( animSeg.getAnimation()->getId() ).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	attrib_name = mDoc.allocate_string( "startTime" );
	attrib_value = mDoc.allocate_string( toString<float>( animSeg.getStartTime() ).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	attrib_name = mDoc.allocate_string( "endTime" );
	attrib_value = mDoc.allocate_string( toString<float>( animSeg.getEndTime() ).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	return true;
}

bool ZHISerializer::writeMatchWebs( rapidxml::xml_node<>* node )
{
	rapidxml::xml_node<>* child;

	char* node_name;
	
	// write children:

	AnimationIndex::MatchWebConstIterator mwi = mAnimIndex->getMatchWebConstIterator();
	while( !mwi.end() )
	{
		mMatchWeb = mwi.next();
		
		node_name = mDoc.allocate_string( "MatchWeb" );
		child = mDoc.allocate_node( rapidxml::node_element, node_name );
		node->append_node(child);

		if( !writeMatchWeb(child) )
			return false;

		mMatchWeb = NULL;
	}

	return true;
}

bool ZHISerializer::writeMatchWeb( rapidxml::xml_node<>* node )
{
	rapidxml::xml_attribute<>* attrib;
	rapidxml::xml_node<>* child;

	char* node_name;
	char* attrib_name;
	char* attrib_value;

	// write attributes:

	attrib_name = mDoc.allocate_string( "animation1" );
	attrib_value = mDoc.allocate_string( toString<unsigned int>( mMatchWeb->getIndex().getSegIndex1() ).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	attrib_name = mDoc.allocate_string( "animation2" );
	attrib_value = mDoc.allocate_string( toString<unsigned int>( mMatchWeb->getIndex().getSegIndex2() ).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	attrib_name = mDoc.allocate_string( "sampleRate" );
	attrib_value = mDoc.allocate_string( toString<unsigned int>( mMatchWeb->getSampleRate() ).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	// write children:

	node_name = mDoc.allocate_string( "Paths" );
	child = mDoc.allocate_node( rapidxml::node_element, node_name );
	node->append_node(child);
	
	if( !writePaths(child) )
		return false;

	return true;
}

bool ZHISerializer::writePaths( rapidxml::xml_node<>* node )
{
	rapidxml::xml_node<>* child;

	char* node_name;

	// write attributes:

	// write children:

	for( unsigned int path_i = 0; path_i < mMatchWeb->getNumPaths(); ++path_i )
	{
		node_name = mDoc.allocate_string( "Path" );
		child = mDoc.allocate_node( rapidxml::node_element, node_name );
		node->append_node(child);

		if( !writePath( child, mMatchWeb->getPath(path_i) ) )
		{
			return false;
		}
	}

	return true;
}

bool ZHISerializer::writePath( rapidxml::xml_node<>* node, const MatchWeb::Path& path )
{
	rapidxml::xml_attribute<>* attrib;
	rapidxml::xml_node<>* child;

	char* node_name;
	char* attrib_name;
	char* attrib_value;

	// write attributes:

	if( path.hasNext() )
	{
		unsigned int npath, npt;
		path.getNext( npath, npt );

		attrib_name = mDoc.allocate_string( "nextPath" );
		attrib_value = mDoc.allocate_string( toString<unsigned int>(npath).c_str() );
		attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
		node->append_attribute(attrib);

		attrib_name = mDoc.allocate_string( "nextPoint" );
		attrib_value = mDoc.allocate_string( toString<unsigned int>(npt).c_str() );
		attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
		node->append_attribute(attrib);
	}

	// write children:

	node_name = mDoc.allocate_string( "Points" );
	child = mDoc.allocate_node( rapidxml::node_element, node_name );
	node->append_node(child);
	
	if( !writePoints( child, path ) )
		return false;

	node_name = mDoc.allocate_string( "Branches" );
	child = mDoc.allocate_node( rapidxml::node_element, node_name );
	node->append_node(child);
	
	if( !writeBranches( child, path ) )
		return false;

	return true;
}

bool ZHISerializer::writePoints( rapidxml::xml_node<>* node, const MatchWeb::Path& path )
{
	rapidxml::xml_node<>* child;

	char* node_name;

	// write attributes:

	// write children:

	for( unsigned int pti = 0; pti < path.getNumPoints(); ++pti )
	{
		node_name = mDoc.allocate_string( "Point" );
		child = mDoc.allocate_node( rapidxml::node_element, node_name );
		node->append_node(child);

		if( !writePoint( child, path.getPoint(pti) ) )
		{
			return false;
		}
	}

	return true;
}

bool ZHISerializer::writePoint( rapidxml::xml_node<>* node, const AnimationDistanceGrid::Point& point )
{
	rapidxml::xml_attribute<>* attrib;

	char* attrib_name;
	char* attrib_value;

	// write attributes:
	
	attrib_name = mDoc.allocate_string( "frame1" );
	attrib_value = mDoc.allocate_string( toString<unsigned int>( point.getIndex().first ).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	attrib_name = mDoc.allocate_string( "frame2" );
	attrib_value = mDoc.allocate_string( toString<unsigned int>( point.getIndex().second ).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	attrib_name = mDoc.allocate_string( "distance" );
	attrib_value = mDoc.allocate_string( toString<float>( point.getDistance() ).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	attrib_name = mDoc.allocate_string( "alignTransf" );
	attrib_value = mDoc.allocate_string( writeSituation( point.getAlignTransf() ).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	return true;
}

std::string ZHISerializer::writeSituation( const Model::Situation& sit )
{
	return toString<float>( sit.getPosX() ) + " " + toString<float>( sit.getPosZ() ) + " " + toString<float>( sit.getOrientY() );
}

bool ZHISerializer::writeBranches( rapidxml::xml_node<>* node, const MatchWeb::Path& path )
{
	rapidxml::xml_node<>* child;

	char* node_name;

	// write attributes:

	// write children:

	for( unsigned int pti = 0; pti < path.getNumPoints(); ++pti )
	{
		if( !path.hasBranch(pti) )
			continue;

		node_name = mDoc.allocate_string( "Branch" );
		child = mDoc.allocate_node( rapidxml::node_element, node_name );
		node->append_node(child);

		if( !writeBranch( child, pti, path.getBranch(pti) ) )
		{
			return false;
		}
	}

	return true;
}

bool ZHISerializer::writeBranch( rapidxml::xml_node<>* node, unsigned int point, unsigned int branch )
{
	rapidxml::xml_attribute<>* attrib;

	char* attrib_name;
	char* attrib_value;

	// write attributes:
	
	attrib_name = mDoc.allocate_string( "point" );
	attrib_value = mDoc.allocate_string( toString<unsigned int>(point).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	attrib_name = mDoc.allocate_string( "path" );
	attrib_value = mDoc.allocate_string( toString<unsigned int>(branch).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	return true;
}

}
