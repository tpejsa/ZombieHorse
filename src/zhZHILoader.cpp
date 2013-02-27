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

#include "zhZHILoader.h"
#include "zhString.h"
#include "zhFileSystem.h"
#include "zhAnimationSystem.h"
#include "zhAnimationManager.h"

namespace zh
{

bool ZHILoader::tryLoad( ResourcePtr res, const std::string& path )
{
	zhAssert( res != NULL );
	zhAssert( res->getClassId() == Resource_AnimationIndex );

	std::string dir, filename, prefix, ext;
	parsePathStr( path, dir, filename, prefix, ext );

	if( ext != "vai" )
		return false;

	return true;
}

bool ZHILoader::load( ResourcePtr res, const std::string& path )
{
	zhAssert( res != NULL );
	zhAssert( res->getClassId() == Resource_AnimationIndex );

	mAnimIndex = AnimationIndexPtr::DynamicCast<Resource>(res);
	mPath = path;

	// open the ZHI file as binary
	FILE* vaf = fopen( path.c_str(), "rb" );
	if( vaf == NULL )
	{
		zhLog( "ZHILoader", "load", "ERROR: Failed to open ZHI file %s for reading.",
			path.c_str() );
		return false;
	}

	// load file contents into memory
	int vaf_size;
	char* vaf_content = NULL;
	fseek( vaf, 0, SEEK_END );
	vaf_size = ftell(vaf);
	fseek( vaf, 0, SEEK_SET );
	vaf_content = ( char* )malloc( vaf_size+1 );
	if( vaf_size != fread( vaf_content, sizeof(char), vaf_size, vaf ) )
	{
		// error reading file
		zhLog( "ZHILoader", "load", "ERROR: Failed to read ZHI file %s.",
			path.c_str() );

		free(vaf_content);
		fclose(vaf);
		return false;
	}
	vaf_content[vaf_size] = 0;

	// build DOM
	rapidxml::xml_document<> doc;
	doc.parse<0>(vaf_content);

	// parse ZHI file
	bool result = true;
	rapidxml::xml_node<>* root = doc.first_node( "AnimationIndex" );
	if( root == NULL )
	{
		// error reading file
		zhLog( "ZHILoader", "load", "ERROR: Invalid ZHI file. Missing root node." );

		// done parsing, free memory
		doc.clear();
		free(vaf_content);
		fclose(vaf);
		return false;
	}
	result = parseAnimationIndex(root);

	// done parsing, free memory
	doc.clear();
	free(vaf_content);
	fclose(vaf);

	return result;
}

bool ZHILoader::parseAnimationIndex( rapidxml::xml_node<>* node )
{
	rapidxml::xml_node<>* child;
	rapidxml::xml_attribute<>* attrib;

	std::string version;
	unsigned long id;
	std::string name;

	// parse node attributes:

	attrib = node->first_attribute( "version" );
	if( attrib == NULL )
	{
		zhLog( "ZHILoader", "parseAnimationIndex", "ERROR: Invalid ZHI file. AnimationIndex element missing version attribute." );
		return false;
	}
	version = attrib->value();
	
	attrib = node->first_attribute( "id" );
	if( attrib == NULL )
	{
		zhLog( "ZHILoader", "parseAnimationIndex", "ERROR: Invalid ZHI file. AnimationIndex element missing id attribute." );
		return false;
	}
	id = fromString<unsigned long>( attrib->value() );

	attrib = node->first_attribute( "name" );
	if( attrib == NULL )
	{
		zhLog( "ZHILoader", "parseAnimationIndex", "ERROR: Invalid ZHI file. AnimationIndex element missing name attribute." );
		return false;
	}
	name = attrib->value();

	// check attribute validity:

	if( id != mAnimIndex->getId() )
	{
		zhLog( "ZHILoader", "parseAnimationIndex", "WARNING: id attribute of AnimationIndex element has inconsistent value: %u.",
			id );
		//return false;
	}

	if( name != mAnimIndex->getName() )
	{
		zhLog( "ZHILoader", "parseAnimationIndex", "WARNING: name attribute of AnimationIndex element has inconsistent value: %s.",
			name.c_str() );
		//return false;
	}

	// parse child elements:

	child = node->first_node( "Animations" );
	if( child != NULL )
	{
		if( !parseAnimations(child) )
			return false;
	}

	child = node->first_node( "MatchWebs" );
	if( child != NULL )
	{
		if( !parseMatchWebs(child) )
			return false;
	}
	
	return true;
}

bool ZHILoader::parseAnimations( rapidxml::xml_node<>* node )
{
	rapidxml::xml_node<>* child;

	// parse child elements:

	child = node->first_node();
	while( child != NULL )
	{
		if( !strcmp( child->name(), "Animation" ) )
		{
			if( !parseAnimation(child) )
				return false;
		}
		else
		{
			return false;
		}

		child = child->next_sibling();
	}
	
	return true;
}

bool ZHILoader::parseAnimation( rapidxml::xml_node<>* node )
{
	rapidxml::xml_attribute<>* attrib;

	unsigned long animset_id;
	unsigned short anim_id;
	float stime, etime;

	// parse node attributes:

	attrib = node->first_attribute( "animationSetId" );
	if( attrib == NULL )
	{
		zhLog( "ZHILoader", "parseAnimation", "ERROR: Invalid ZHI file. Animation element missing animationSetId attribute." );
		return false;
	}
	animset_id = fromString<unsigned long>( attrib->value() );

	attrib = node->first_attribute( "animationId" );
	if( attrib == NULL )
	{
		zhLog( "ZHILoader", "parseAnimation", "ERROR: Invalid ZHI file. Animation element missing animationId attribute." );
		return false;
	}
	anim_id = fromString<unsigned short>( attrib->value() );

	attrib = node->first_attribute( "startTime" );
	if( attrib == NULL )
	{
		zhLog( "ZHILoader", "parseAnimation", "ERROR: Invalid ZHI file. Animation element missing startTime attribute." );
		return false;
	}
	stime = fromString<float>( attrib->value() );

	attrib = node->first_attribute( "endTime" );
	if( attrib == NULL )
	{
		zhLog( "ZHILoader", "parseAnimation", "ERROR: Invalid ZHI file. Animation element missing endTime attribute." );
		return false;
	}
	etime = fromString<float>( attrib->value() );

	// check attribute validity:

	if( !zhAnimationSystem->getAnimationManager()->hasResource(animset_id) ||
		!AnimationSetPtr::DynamicCast<Resource>( zhAnimationSystem->getAnimationManager()->getResource(animset_id) )->hasAnimation(anim_id) )
	{
		zhLog( "ZHILoader", "parseBaseAnimations", "ERROR: Invalid ZHI file. Animation element specifies non-existent animation with id: %u, %u.",
			animset_id, anim_id );
		return false;
	}

	// assign attribute values:

	AnimationSegment anim_seg( AnimationSetPtr::DynamicCast<Resource>( zhAnimationSystem->getAnimationManager()->getResource(animset_id) )->getAnimation(anim_id),
		stime, etime );
	mAnimIndex->addAnimationSegment(anim_seg);

	return true;
}

bool ZHILoader::parseMatchWebs( rapidxml::xml_node<>* node )
{
	rapidxml::xml_node<>* child;

	// parse child elements:

	child = node->first_node();
	while( child != NULL )
	{
		if( !strcmp( child->name(), "MatchWeb" ) )
		{
			if( !parseMatchWeb(child) )
				return false;
		}
		else
		{
			return false;
		}

		child = child->next_sibling();
	}
	
	return true;
}

bool ZHILoader::parseMatchWeb( rapidxml::xml_node<>* node )
{
	rapidxml::xml_node<>* child;
	rapidxml::xml_attribute<>* attrib;

	unsigned int anim1, anim2, sample_rate;

	// parse node attributes:
	
	attrib = node->first_attribute( "animation1" );
	if( attrib == NULL )
	{
		zhLog( "ZHILoader", "parseMatchWeb", "ERROR: Invalid ZHI file. Animation element missing animation1 attribute." );
		return false;
	}
	anim1 = fromString<unsigned int>( attrib->value() );

	attrib = node->first_attribute( "animation2" );
	if( attrib == NULL )
	{
		zhLog( "ZHILoader", "parseMatchWeb", "ERROR: Invalid ZHI file. Animation element missing animation2 attribute." );
		return false;
	}
	anim2 = fromString<unsigned int>( attrib->value() );

	attrib = node->first_attribute( "sampleRate" );
	if( attrib == NULL )
	{
		zhLog( "ZHILoader", "parseMatchWeb", "ERROR: Invalid ZHI file. Animation element missing sampleRate attribute." );
		return false;
	}
	sample_rate = fromString<unsigned int>( attrib->value() );

	// check attribute validity:

	MatchWeb::Index mwi( anim1, anim2 );
	if( anim1 >= mAnimIndex->getNumAnimationSegments() || anim2 >= mAnimIndex->getNumAnimationSegments() ||
		mAnimIndex->hasMatchWeb(mwi) )
	{
		zhLog( "ZHILoader", "parseMatchWeb", "ERROR: Invalid ZHI file. Unable to load match web %u, %u.",
			anim1, anim2 );
		return false;
	}

	mMatchWeb = mAnimIndex->createMatchWeb( mwi, sample_rate );

	// parse child elements:

	child = node->first_node( "Paths" );
	if( child != NULL )
	{
		if( !parsePaths(child) )
			return false;
	}

	mMatchWeb = NULL;
	return true;
}

bool ZHILoader::parsePaths( rapidxml::xml_node<>* node )
{
	rapidxml::xml_node<>* child;

	// parse child elements:

	child = node->first_node();
	while( child != NULL )
	{
		if( !strcmp( child->name(), "Path" ) )
		{
			if( !parsePath(child) )
				return false;
		}
		else
		{
			return false;
		}

		child = child->next_sibling();
	}
	
	return true;
}

bool ZHILoader::parsePath( rapidxml::xml_node<>* node )
{
	rapidxml::xml_node<>* child;
	rapidxml::xml_attribute<>* attrib;

	unsigned int npath = UINT_MAX, npt = 0;

	// parse node attributes:
	
	attrib = node->first_attribute( "nextPath" );
	if( attrib != NULL )
		npath = fromString<unsigned int>( attrib->value() );

	attrib = node->first_attribute( "nextPoint" );
	if( attrib != NULL )
		npt = fromString<unsigned int>( attrib->value() );

	// check attribute validity:

	// parse child elements:

	MatchWeb::Path path;
	path.setNext( npath, npt );

	child = node->first_node( "Points" );
	if( child != NULL )
	{
		if( !parsePoints( child, path ) )
			return false;
	}
	else
	{
		zhLog( "ZHILoader", "parsePath", "ERROR: Path element missing child element Points." );
		return false;
	}

	child = node->first_node( "Branches" );
	if( child != NULL )
	{
		if( !parseBranches( child, path ) )
			return false;
	}

	mMatchWeb->addPath(path);
	
	return true;
}

bool ZHILoader::parsePoints( rapidxml::xml_node<>* node, MatchWeb::Path& path )
{
	rapidxml::xml_node<>* child;

	// parse child elements:

	child = node->first_node();
	while( child != NULL )
	{
		if( !strcmp( child->name(), "Point" ) )
		{
			if( !parsePoint( child, path ) )
				return false;
		}
		else
		{
			return false;
		}

		child = child->next_sibling();
	}
	
	return true;
}

bool ZHILoader::parsePoint( rapidxml::xml_node<>* node, MatchWeb::Path& path )
{
	rapidxml::xml_attribute<>* attrib;

	unsigned int fr1, fr2;
	float dist;
	Model::Situation transf;
	std::string transf_str;

	// parse node attributes:

	attrib = node->first_attribute( "frame1" );
	if( attrib == NULL )
	{
		zhLog( "ZHILoader", "parsePoint", "ERROR: Invalid ZHI file. Point element missing frame1 attribute." );
		return false;
	}
	fr1 = fromString<unsigned int>( attrib->value() );

	attrib = node->first_attribute( "frame2" );
	if( attrib == NULL )
	{
		zhLog( "ZHILoader", "parsePoint", "ERROR: Invalid ZHI file. Point element missing frame2 attribute." );
		return false;
	}
	fr2 = fromString<unsigned int>( attrib->value() );

	attrib = node->first_attribute( "distance" );
	if( attrib == NULL )
	{
		zhLog( "ZHILoader", "parsePoint", "ERROR: Invalid ZHI file. Point element missing distance attribute." );
		return false;
	}
	dist = fromString<float>( attrib->value() );

	attrib = node->first_attribute( "alignTransf" );
	if( attrib == NULL )
	{
		zhLog( "ZHILoader", "parsePoint", "ERROR: Invalid ZHI file. Point element missing alignTransf attribute." );
		return false;
	}
	transf_str = attrib->value();
	if( !parseSituation( transf_str, transf ) )
	{
		zhLog( "ZHILoader", "parsePoint", "ERROR: Invalid ZHI file. alignTransf attribute of Point element has invalid value." );
		return false;
	}

	// check attribute validity:

	if( fr1 >= mMatchWeb->getNumSamples1() || fr2 >= mMatchWeb->getNumSamples2() )
	{
		zhLog( "ZHILoader", "parsePoint", "ERROR: Invalid ZHI file. Point element specifies non-existent frame pair: %u, %u.",
			fr1, fr2 );
		return false;
	}

	// assign attribute values:

	path.addPoint( AnimationDistanceGrid::Point( AnimationDistanceGrid::Index( fr1, fr2 ), dist, transf ) );

	return true;
}

bool ZHILoader::parseSituation( const std::string& sitStr, Model::Situation& sit )
{
	float x, z, ay;

	std::istringstream iss(sitStr);
	if( iss.eof() )
		return false;
	iss >> x;
	if( iss.eof() )
		return false;
	iss >> z;
	if( iss.eof() )
		return false;
	iss >> ay;

	sit = Model::Situation( x, z, ay );

	return true;
}

bool ZHILoader::parseBranches( rapidxml::xml_node<>* node, MatchWeb::Path& path )
{
	rapidxml::xml_node<>* child;

	// parse child elements:

	child = node->first_node();
	while( child != NULL )
	{
		if( !strcmp( child->name(), "Branch" ) )
		{
			if( !parseBranch( child, path ) )
				return false;
		}
		else
		{
			return false;
		}

		child = child->next_sibling();
	}
	
	return true;
}

bool ZHILoader::parseBranch( rapidxml::xml_node<>* node, MatchWeb::Path& path )
{
	rapidxml::xml_attribute<>* attrib;

	unsigned int bpt, bpath;

	// parse node attributes:

	attrib = node->first_attribute( "point" );
	if( attrib == NULL )
	{
		zhLog( "ZHILoader", "parseBranch", "ERROR: Invalid ZHI file. Point element missing point attribute." );
		return false;
	}
	bpt = fromString<unsigned int>( attrib->value() );

	attrib = node->first_attribute( "path" );
	if( attrib == NULL )
	{
		zhLog( "ZHILoader", "parseBranch", "ERROR: Invalid ZHI file. Point element missing path attribute." );
		return false;
	}
	bpath = fromString<unsigned int>( attrib->value() );

	// check attribute validity:

	if( bpt >= path.getNumPoints() )
	{
		zhLog( "ZHILoader", "parseBranch", "ERROR: Invalid ZHI file. Branch element specifies non-existent point: %u.",
			bpt );
		return false;
	}

	// assign attribute values:

	path.setBranch( bpt, bpath );

	return true;
}

}
