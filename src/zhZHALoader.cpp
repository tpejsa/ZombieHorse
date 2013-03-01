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

#include "zhZHALoader.h"
#include "zhBoneAnimationTrack.h"
#include "zhString.h"
#include "zhFileSystem.h"

#include <sstream>

namespace zh
{

bool ZHALoader::tryLoad( ResourcePtr res, const std::string& path )
{
	zhAssert( res != NULL );
	zhAssert( res->getClassId() == Resource_AnimationSet );

	std::string dir, filename, prefix, ext;
	parsePathStr( path, dir, filename, prefix, ext );

	if( ext != "zha" )
		return false;

	return true;
}

bool ZHALoader::load( ResourcePtr res, const std::string& path )
{
	zhAssert( res != NULL );
	zhAssert( res->getClassId() == Resource_AnimationSet );

	mAnimSet = AnimationSetPtr::DynamicCast<Resource>(res);
	mAnim = NULL;
	mPath = path;

	// open the ZHA file as binary
	FILE* vaf = fopen( path.c_str(), "rb" );
	if( vaf == NULL )
	{
		zhLog( "ZHALoader", "load", "ERROR: Failed to open ZHA file %s for reading.",
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
		zhLog( "ZHALoader", "load", "ERROR: Failed to read ZHA file %s.",
			path.c_str() );

		free(vaf_content);
		fclose(vaf);
		return false;
	}
	vaf_content[vaf_size] = 0;

	// build DOM
	rapidxml::xml_document<> doc;
	doc.parse<0>(vaf_content);

	// parse ZHA file
	rapidxml::xml_node<>* root = doc.first_node( "AnimationSet" );
	if( root == NULL )
	{
		// error reading file
		zhLog( "ZHALoader", "load", "ERROR: Invalid ZHA file. Missing root node." );

		// done parsing, free memory
		doc.clear();
		free(vaf_content);
		fclose(vaf);
		return false;
	}
	bool result = parseAnimationSet(root);

	// done parsing, free memory
	doc.clear();
	free(vaf_content);
	fclose(vaf);

	return result;
}

bool ZHALoader::parseAnimationSet( rapidxml::xml_node<>* node )
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
		zhLog( "ZHALoader", "parseAnimationSet", "ERROR: Invalid ZHA file. AnimationSet element missing version attribute." );
		return false;
	}
	version = attrib->value();
	
	attrib = node->first_attribute( "id" );
	if( attrib == NULL )
	{
		zhLog( "ZHALoader", "parseAnimationSet", "ERROR: Invalid ZHA file. AnimationSet element missing id attribute." );
		return false;
	}
	id = fromString<unsigned long>( attrib->value() );

	attrib = node->first_attribute( "name" );
	if( attrib == NULL )
	{
		zhLog( "ZHALoader", "parseAnimationSet", "ERROR: Invalid ZHA file. AnimationSet element missing name attribute." );
		return false;
	}
	name = attrib->value();

	// check attribute validity:

	if( id != mAnimSet->getId() )
	{
		zhLog( "ZHALoader", "parseAnimationSet", "WARNING: id attribute of AnimationSet element has inconsistent value: %u.",
			id );
		//return false;
	}

	if( name != mAnimSet->getName() )
	{
		zhLog( "ZHALoader", "parseAnimationSet", "WARNING: name attribute of AnimationSet element has inconsistent value: %s.",
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

	child = node->first_node( "AnimationSpaces" );
	if( child != NULL )
	{
		if( !parseAnimationSpaces(child) )
			return false;
	}
	
	return true;
}

bool ZHALoader::parseAnimations( rapidxml::xml_node<>* node )
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

bool ZHALoader::parseAnimation( rapidxml::xml_node<>* node )
{
	rapidxml::xml_node<>* child;
	rapidxml::xml_attribute<>* attrib;

	std::string classid_str;
	unsigned short id;
	std::string name;
	std::string interp_method_str;
	KFInterpolationMethod interp_method;

	// parse node attributes:
	
	attrib = node->first_attribute( "id" );
	if( attrib == NULL )
	{
		zhLog( "ZHALoader", "parseAnimation", "ERROR: Invalid ZHA file. Animation element missing id attribute." );
		return false;
	}
	id = fromString<unsigned short>( attrib->value() );

	attrib = node->first_attribute( "name" );
	if( attrib == NULL )
	{
		zhLog( "ZHALoader", "parseAnimation", "ERROR: Invalid ZHA file. Animation element missing name attribute." );
		return false;
	}
	name = attrib->value();
	
	attrib = node->first_attribute( "interpMethod" );
	if( attrib == NULL )
	{
		zhLog( "ZHALoader", "parseAnimation", "ERROR: Invalid ZHA file. Animation element missing interpMethod attribute." );
		return false;
	}
	interp_method_str = attrib->value();

	// check attribute validity:

	if( mAnimSet->hasAnimation(id) || mAnimSet->hasAnimation(name) )
	{
		zhLog( "ZHALoader", "parseAnimation", "ERROR: Invalid ZHA file. Duplicate Animation element with id %u, name %s.",
			id, name.c_str() );
		return false;
	}

	if( !parseKFInterpolationMethod( interp_method_str, interp_method ) )
	{
		zhLog( "ZHALoader", "parseAnimation", "ERROR: Invalid ZHA file. interpMethod attribute of Animation element has invalid value: %s.",
			interp_method_str.c_str() );
		return false;
	}

	mAnim = mAnimSet->createAnimation( id, name );
	mAnim->setKFInterpolationMethod( interp_method );

	// parse child elements:

	child = node->first_node( "Tracks" );
	if( child != NULL )
	{
		if( !parseTracks(child) )
			return false;
	}

	child = node->first_node( "Annotations" );
	if( child != NULL )
	{
		if( !parseAnnotations(child) )
			return false;
	}

	mAnim = NULL;
	return true;
}

bool ZHALoader::parseKFInterpolationMethod( std::string interpMethodStr, KFInterpolationMethod& interpMethod )
{
	if( interpMethodStr == "Linear" )
		interpMethod = KFInterp_Linear;
	else if( interpMethodStr == "Spline" )
		interpMethod = KFInterp_Spline;
	else
		return false;

	return true;
}

bool ZHALoader::parseTracks( rapidxml::xml_node<>* node )
{
	rapidxml::xml_node<>* child;

	// parse child elements:

	child = node->first_node();
	while( child != NULL )
	{
		if( !strcmp( child->name(), "BoneTrack" ) )
		{
			if( !parseBoneTrack(child) )
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

bool ZHALoader::parseBoneTrack( rapidxml::xml_node<>* node )
{
	rapidxml::xml_node<>* child;
	rapidxml::xml_attribute<>* attrib;

	unsigned short bone_id;

	// parse node attributes:
	
	attrib = node->first_attribute( "boneId" );
	if( attrib == NULL )
	{
		zhLog( "ZHALoader", "parseBoneTrack", "ERROR: Invalid ZHA file. BoneTrack element missing boneId attribute." );
		return false;
	}
	bone_id = fromString<unsigned short>( attrib->value() );

	// check attribute validity:

	BoneAnimationTrack* bat = mAnim->createBoneTrack( bone_id );

	// parse child elements:

	child = node->first_node( "TransformKeyFrames" );
	if( child != NULL )
	{
		if( !parseTransformKeyFrames( child, bat ) )
			return false;
	}

	return true;
}

bool ZHALoader::parseTransformKeyFrames( rapidxml::xml_node<>* node, BoneAnimationTrack* track )
{
	rapidxml::xml_node<>* child;

	// parse child elements:

	child = node->first_node();
	while( child != NULL )
	{
		if( !strcmp( child->name(), "TransformKeyFrame" ) )
		{
			if( !parseTransformKeyFrame( child, track ) )
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

bool ZHALoader::parseTransformKeyFrame( rapidxml::xml_node<>* node, BoneAnimationTrack* track )
{
	rapidxml::xml_attribute<>* attrib;

	float time;
	Vector3 trans;
	Quat rot;
	Vector3 scal;

	// parse node attributes:

	attrib = node->first_attribute( "time" );
	if( attrib == NULL )
	{
		zhLog( "ZHALoader", "parseTransformKeyFrame", "ERROR: Invalid ZHA file. TransformKeyFrame element missing time attribute." );
		return false;
	}
	time = fromString<float>( attrib->value() );

	attrib = node->first_attribute( "translation" );
	if( attrib == NULL )
	{
		zhLog( "ZHALoader", "parseTransformKeyFrame", "ERROR: Invalid ZHA file. TransformKeyFrame element missing translation attribute." );
		return false;
	}
	if( !parseVector3( attrib->value(), trans ) )
	{
		zhLog( "ZHALoader", "parseTransformKeyFrame", "ERROR: Invalid ZHA file. translation attribute of TransformKeyFrame element has invalid value." );
		return false;
	}

	attrib = node->first_attribute( "rotation" );
	if( attrib == NULL )
	{
		zhLog( "ZHALoader", "parseTransformKeyFrame", "ERROR: Invalid ZHA file. TransformKeyFrame element missing rotation attribute." );
		return false;
	}
	if( !parseQuat( attrib->value(), rot ) )
	{
		zhLog( "ZHALoader", "parseTransformKeyFrame", "ERROR: Invalid ZHA file. rotation attribute of TransformKeyFrame element has invalid value." );
		return false;
	}

	attrib = node->first_attribute( "scale" );
	if( attrib == NULL )
	{
		zhLog( "ZHALoader", "parseTransformKeyFrame", "ERROR: Invalid ZHA file. TransformKeyFrame element missing scale attribute." );
		return false;
	}
	if( !parseVector3( attrib->value(), scal ) )
	{
		zhLog( "ZHALoader", "parseTransformKeyFrame", "ERROR: Invalid ZHA file. scale attribute of TransformKeyFrame element has invalid value." );
		return false;
	}

	// check attribute validity:

	// assign attribute values:

	TransformKeyFrame* tkf = static_cast<TransformKeyFrame*>( track->createKeyFrame(time) );
	tkf->setTranslation(trans);
	tkf->setRotation(rot);
	tkf->setScale(scal);

	return true;
}

bool ZHALoader::parseVector3( const std::string& zhtr, Vector3& v )
{
	std::istringstream iss(zhtr);
	if( iss.eof() )
		return false;
	iss >> v.x;
	if( iss.eof() )
		return false;
	iss >> v.y;
	if( iss.eof() )
		return false;
	iss >> v.z;

	return true;
}

bool ZHALoader::parseQuat( const std::string& qstr, Quat& q )
{
	Vector3 axis;
	float angle;

	std::istringstream iss(qstr);
	if( iss.eof() )
		return false;
	iss >> axis.x;
	if( iss.eof() )
		return false;
	iss >> axis.y;
	if( iss.eof() )
		return false;
	iss >> axis.z;
	if( iss.eof() )
		return false;
	iss >> angle;
	q = Quat( axis, angle );

	return true;
}

bool ZHALoader::parseVector( const std::string& zhtr, Vector& v )
{
	std::vector<float> v0;
	float x;

	std::istringstream iss(zhtr);
	while( !iss.eof() )
	{
		iss >> x;
		v0.push_back(x);
	}

	if( v0.size() <= 0 )
		return false;

	v = Vector( v0.size() );
	for( unsigned int i = 0; i < v0.size(); ++i )
		v.set( i, v0[i] );

	return true;
}

bool ZHALoader::parseAnnotations( rapidxml::xml_node<>* node )
{
	rapidxml::xml_node<>* child;

	// parse child elements:

	child = node->first_node( "Annotation" );
	while( child != NULL )
	{
		if( !parseAnnotation(child) )
			return false;

		child = child->next_sibling( "Annotation" );
	}

	return true;
}

bool ZHALoader::parseAnnotation( rapidxml::xml_node<>* node )
{
	rapidxml::xml_node<>* child;
	rapidxml::xml_attribute<>* attrib;

	float start_time;
	float end_time;
	std::string classid_str;
	AnimationAnnotationClass class_id;

	TransitionAnnotationContainer* tannots = mAnim->getTransitionAnnotations();
	ParamTransitionAnnotationContainer* ptannots = mAnim->getParamTransitionAnnotations();
	PlantConstraintAnnotationContainer* pcannots = mAnim->getPlantConstraintAnnotations();
	SimEventAnnotationContainer* seannots = mAnim->getSimEventAnnotations();

	TransitionAnnotation* tannot = NULL;
	ParamTransitionAnnotation* ptannot = NULL;
	PlantConstraintAnnotation* pcannot = NULL;
	SimEventAnnotation* seannot = NULL;

	// parse node attributes:

	attrib = node->first_attribute( "startTime" );
	if( attrib == NULL )
	{
		zhLog( "ZHALoader", "parseAnnotation", "ERROR: Invalid ZHA file. AnimationAnnotation element missing startTime attribute." );
		return false;
	}
	start_time = fromString<float>( attrib->value() );

	attrib = node->first_attribute( "endTime" );
	if( attrib == NULL )
	{
		zhLog( "ZHALoader", "parseAnnotation", "ERROR: Invalid ZHA file. AnimationAnnotation element missing endTime attribute." );
		return false;
	}
	end_time = fromString<float>( attrib->value() );

	attrib = node->first_attribute( "classId" );
	if( attrib == NULL )
	{
		zhLog( "ZHALoader", "parseAnnotation", "ERROR: Invalid ZHA file. AnimationAnnotation element missing classId attribute." );
		return false;
	}
	classid_str = attrib->value();
	
	// check attribute validity:

	if( !parseAnnotationClass( classid_str, class_id ) )
	{
		zhLog( "ZHALoader", "parseAnnotation", "ERROR: Invalid ZHA file. classId attribute of AnimationAnnotation element has invalid value: %s.",
			classid_str.c_str() );
		return false;
	}

	if( class_id == AnimAnnot_Transition )
		tannot = static_cast<TransitionAnnotation*>( tannots->createAnnotation( start_time, end_time ) );
	else if( class_id == AnimAnnot_ParamTransition )
		ptannot = static_cast<ParamTransitionAnnotation*>( ptannots->createAnnotation( start_time, end_time ) );
	else if( class_id == AnimAnnot_PlantConstraint )
		pcannot = static_cast<PlantConstraintAnnotation*>( pcannots->createAnnotation( start_time, end_time ) );
	else // if( class_id == AnimAnnot_SimEvent )
		seannot = static_cast<SimEventAnnotation*>( seannots->createAnnotation( start_time, end_time ) );

	// parse child elements:

	child = node->first_node();
	if( child != NULL &&
		!strcmp( child->name(), "TransitionAnnotation" ) )
	{
		if( !parseTransitionAnnotation( child, tannot ) )
		{
			return false;
		}
	}
	else if( child != NULL &&
		!strcmp( child->name(), "ParamTransitionAnnotation" ) )
	{
		if( !parseParamTransitionAnnotation( child, ptannot ) )
		{
			return false;
		}
	}
	else if( child != NULL &&
		!strcmp( child->name(), "PlantConstraintAnnotation" ) )
	{
		if( !parsePlantConstraintAnnotation( child, pcannot ) )
		{
			return false;
		}
	}
	else if( child != NULL &&
		!strcmp( child->name(), "SimEventAnnotation" ) )
	{
		if( !parseSimEventAnnotation( child, seannot ) )
		{
			return false;
		}
	}
	else
	{
		zhLog( "ZHALoader", "parseAnnotation", "ERROR: Invalid ZHA file. Annotation element missing one of the following child elements: TransitionAnnotation|ParamTransitionAnnotation|PlantConstraintAnnotation|SimEventAnnotation|GesturePhaseAnnotation." );
		return false;
	}

	return true;
}

bool ZHALoader::parseAnnotationClass( std::string annotClassStr, AnimationAnnotationClass& annotClass )
{
	if( annotClassStr == "Transition" )
		annotClass = AnimAnnot_Transition;
	else if( annotClassStr == "ParamTransition" )
		annotClass = AnimAnnot_ParamTransition;
	else if( annotClassStr == "PlantConstraint" )
		annotClass = AnimAnnot_PlantConstraint;
	else if( annotClassStr == "SimEvent" )
		annotClass = AnimAnnot_SimEvent;
	else
		return false;

	return true;
}

bool ZHALoader::parseTransitionAnnotation( rapidxml::xml_node<>* node, TransitionAnnotation* annot )
{
	rapidxml::xml_attribute<>* attrib;

	unsigned long target_setid = mAnimSet->getId();
	unsigned short target_id;
	float target_time;
	std::string aligntransf_str;
	Skeleton::Situation align_transf;

	// parse node attributes:

	attrib = node->first_attribute( "targetSetId" );
	if( attrib != NULL )
	{
		target_setid = fromString<unsigned long>( attrib->value() );
	}

	attrib = node->first_attribute( "targetId" );
	if( attrib == NULL )
	{
		zhLog( "ZHALoader", "parseTransitionAnnotation", "ERROR: Invalid ZHA file. TransitionAnnotation element missing targetId attribute." );
		return false;
	}
	target_id = fromString<unsigned short>( attrib->value() );
	
	attrib = node->first_attribute( "targetTime" );
	if( attrib == NULL )
	{
		zhLog( "ZHALoader", "parseTransitionAnnotation", "ERROR: Invalid ZHA file. TransitionAnnotation element missing targetTime attribute." );
		return false;
	}
	target_time = fromString<float>( attrib->value() );

	attrib = node->first_attribute( "alignTransf" );
	if( attrib == NULL )
	{
		zhLog( "ZHALoader", "parseTransitionAnnotation", "ERROR: Invalid ZHA file. TransitionAnnotation element missing alignTransf attribute." );
		return false;
	}
	aligntransf_str = attrib->value();
	if( !parseSituation( aligntransf_str, align_transf ) )
	{
		zhLog( "ZHALoader", "parseTransitionAnnotation", "ERROR: Invalid ZHA file. alignTransf attribute of TransitionAnnotation element has invalid value." );
		return false;
	}

	// check attribute validity:

	// TODO: check if target animation exists as post-process

	// assign attribute values:

	annot->setTargetSetId( target_setid );
	annot->setTargetId( target_id );
	annot->setTargetTime( target_time );
	annot->setAlignTransf( align_transf );

	return true;
}

bool ZHALoader::parseSituation( const std::string& sitStr, Skeleton::Situation& sit )
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
	
	sit = Skeleton::Situation( x, z, ay );

	return true;
}

bool ZHALoader::parseParamTransitionAnnotation( rapidxml::xml_node<>* node, ParamTransitionAnnotation* annot )
{
	rapidxml::xml_attribute<>* attrib;

	unsigned long target_setid = mAnimSet->getId();
	unsigned short target_id;
	std::string lbound_str, ubound_str;
	Vector ubound, lbound;
	float target_time;
	std::string aligntransf_str;
	Skeleton::Situation align_transf;

	// parse node attributes:

	attrib = node->first_attribute( "targetSetId" );
	if( attrib != NULL )
	{
		target_setid = fromString<unsigned long>( attrib->value() );
	}

	attrib = node->first_attribute( "targetId" );
	if( attrib == NULL )
	{
		zhLog( "ZHALoader", "parseParamTransitionAnnotation", "ERROR: Invalid ZHA file. ParamTransitionAnnotation element missing targetId attribute." );
		return false;
	}
	target_id = fromString<unsigned short>( attrib->value() );

	attrib = node->first_attribute( "lowerBound" );
	if( attrib == NULL )
	{
		zhLog( "ZHALoader", "parseParamTransitionAnnotation", "ERROR: Invalid ZHA file. ParamTransitionAnnotation element missing lowerBound attribute." );
		return false;
	}
	lbound_str = attrib->value();
	if( !parseVector( lbound_str, lbound ) )
	{
		zhLog( "ZHALoader", "parseParamTransitionAnnotation", "ERROR: Invalid ZHA file. lowerBound attribute of ParamTransitionAnnotation element has invalid value." );
		return false;
	}

	attrib = node->first_attribute( "upperBound" );
	if( attrib == NULL )
	{
		zhLog( "ZHALoader", "parseParamTransitionAnnotation", "ERROR: Invalid ZHA file. ParamTransitionAnnotation element missing upperBound attribute." );
		return false;
	}
	ubound_str = attrib->value();
	if( !parseVector( ubound_str, ubound ) )
	{
		zhLog( "ZHALoader", "parseParamTransitionAnnotation", "ERROR: Invalid ZHA file. upperBound attribute of ParamTransitionAnnotation element has invalid value." );
		return false;
	}
	
	attrib = node->first_attribute( "targetTime" );
	if( attrib == NULL )
	{
		zhLog( "ZHALoader", "parseParamTransitionAnnotation", "ERROR: Invalid ZHA file. ParamTransitionAnnotation element missing targetTime attribute." );
		return false;
	}
	target_time = fromString<float>( attrib->value() );

	attrib = node->first_attribute( "alignTransf" );
	if( attrib == NULL )
	{
		zhLog( "ZHALoader", "parseParamTransitionAnnotation", "ERROR: Invalid ZHA file. ParamTransitionAnnotation element missing alignTransf attribute." );
		return false;
	}
	aligntransf_str = attrib->value();
	if( !parseSituation( aligntransf_str, align_transf ) )
	{
		zhLog( "ZHALoader", "parseParamTransitionAnnotation", "ERROR: Invalid ZHA file. alignTransf attribute of ParamTransitionAnnotation element has invalid value." );
		return false;
	}

	// check attribute validity:

	if( !AnimationManager::Instance()->hasResource( target_setid ) ||
		!AnimationSetPtr::DynamicCast<Resource>( AnimationManager::Instance()->getResource( target_setid ) )->hasAnimationSpace(target_id) )
	{
		zhLog( "ZHALoader", "parseParamTransitionAnnotation", "ERROR: Invalid ZHA file. ParamTransitionAnnotation element specifies non-existent target animation." );
		return false;
	}

	// assign attribute values:

	annot->setTargetSetId(target_setid);
	annot->setTargetId(target_id);
	annot->setLowerBound(lbound);
	annot->setUpperBound(ubound);
	annot->setTargetTime(target_time);
	annot->setAlignTransf(align_transf);

	return true;
}

bool ZHALoader::parsePlantConstraintAnnotation( rapidxml::xml_node<>* node, PlantConstraintAnnotation* annot )
{
	rapidxml::xml_attribute<>* attrib;

	unsigned short bone_id;

	// parse node attributes:

	attrib = node->first_attribute( "boneId" );
	if( attrib == NULL )
	{
		zhLog( "ZHALoader", "parsePlantConstraintAnnotation", "ERROR: Invalid ZHA file. PlantConstraintAnnotation element missing boneId attribute." );
		return false;
	}
	bone_id = fromString<unsigned short>( attrib->value() );

	// check attribute validity:

	// assign attribute values:

	annot->setBoneId( bone_id );

	return true;
}

bool ZHALoader::parseSimEventAnnotation( rapidxml::xml_node<>* node, SimEventAnnotation* annot )
{
	rapidxml::xml_attribute<>* attrib;

	unsigned short evt_classid, evt_id = USHRT_MAX;

	// parse node attributes:

	attrib = node->first_attribute( "eventClassId" );
	if( attrib == NULL )
	{
		zhLog( "ZHALoader", "parseSimEventAnnotation", "ERROR: Invalid ZHA file. SimEventAnnotation element missing eventClassId attribute." );
		return false;
	}
	evt_classid = fromString<unsigned short>( attrib->value() );

	attrib = node->first_attribute( "eventId" );
	if( attrib != NULL )
		evt_id = fromString<unsigned short>( attrib->value() );

	// check attribute validity:

	// assign attribute values:

	annot->setEventClassId( evt_classid );
	annot->setEventId( evt_id );

	return true;
}

bool ZHALoader::parseAnimationSpaces( rapidxml::xml_node<>* node )
{
	rapidxml::xml_node<>* child;

	// parse child elements:

	child = node->first_node();
	while( child != NULL )
	{
		if( !strcmp( child->name(), "AnimationSpace" ) )
		{
			if( !parseAnimationSpace(child) )
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

bool ZHALoader::parseAnimationSpace( rapidxml::xml_node<>* node )
{
	rapidxml::xml_node<>* child;
	rapidxml::xml_attribute<>* attrib;

	std::string classid_str;
	unsigned short id;
	std::string name;

	// parse node attributes:
	
	attrib = node->first_attribute( "id" );
	if( attrib == NULL )
	{
		zhLog( "ZHALoader", "parseAnimationSpace", "ERROR: Invalid ZHA file. Animation element missing id attribute." );
		return false;
	}
	id = fromString<unsigned short>( attrib->value() );

	attrib = node->first_attribute( "name" );
	if( attrib == NULL )
	{
		zhLog( "ZHALoader", "parseAnimationSpace", "ERROR: Invalid ZHA file. Animation element missing name attribute." );
		return false;
	}
	name = attrib->value();

	// check attribute validity:

	if( mAnimSet->hasAnimationSpace(id) || mAnimSet->hasAnimationSpace(name) )
	{
		zhLog( "ZHALoader", "parseAnimationSpace", "ERROR: Invalid ZHA file. Duplicate Animation element with id %u, name %s.",
			id, name.c_str() );
		return false;
	}

	mAnimSpace = mAnimSet->createAnimationSpace( id, name );

	// parse child elements:

	child = node->first_node( "BaseAnimations" );
	if( child != NULL )
	{
		if( !parseBaseAnimations(child) )
			return false;
	}

	child = node->first_node( "TimewarpCurve" );
	if( child != NULL )
	{
		if( !parseTimewarpCurve(child) )
			return false;
	}

	child = node->first_node( "AlignmentCurve" );
	if( child != NULL )
	{
		if( !parseAlignmentCurve(child) )
			return false;
	}

	child = node->first_node( "Parametrization" );
	if( child != NULL )
	{
		if( !parseParametrization(child) )
			return false;
	}

	mAnimSpace = NULL;
	return true;
}

bool ZHALoader::parseBaseAnimations( rapidxml::xml_node<>* node )
{
	rapidxml::xml_attribute<>* attrib;

	std::vector<unsigned short> banim_list;

	// parse node attributes:

	attrib = node->first_attribute( "animations" );
	if( attrib == NULL )
	{
		zhLog( "ZHALoader", "parseBaseAnimations", "ERROR: Invalid ZHA file. BaseAnimations element missing animations attribute." );
		return false;
	}
	if( !parseBaseAnimationList( attrib->value(), banim_list ) )
	{
		zhLog( "ZHALoader", "parseBaseAnimations", "ERROR: Invalid ZHA file. animations attribute of BaseAnimations element has invalid value." );
		return false;
	}

	// check attribute validity:

	// assign attribute values:

	for( unsigned int banim_i = 0; banim_i < banim_list.size(); ++banim_i )
	{
		Animation* banim = mAnimSet->getAnimation( banim_list[banim_i] );
		
		if( banim == NULL )
		{
			zhLog( "ZHALoader", "parseBaseAnimations", "ERROR: Invalid ZHA file. animations attribute of BaseAnimations element specifies non-existent animation with id: %u.",
				banim_list[banim_i] );
			return false;
		}

		mAnimSpace->addBaseAnimation(banim);
	}

	return true;
}

bool ZHALoader::parseBaseAnimationList( const std::string& listStr, std::vector<unsigned short>& list )
{
	unsigned short x;

	std::istringstream iss(listStr);
	while( !iss.eof() )
	{
		iss >> x;
		list.push_back(x);
	}

	return true;
}

bool ZHALoader::parseTimewarpCurve( rapidxml::xml_node<>* node )
{
	rapidxml::xml_node<>* child;

	CatmullRomSpline<Vector> tw_curve;

	// parse child elements:

	child = node->first_node();
	while( child != NULL )
	{
		if( !strcmp( child->name(), "CtrlPoint" ) )
		{
			if( !parseCtrlPoint( child, tw_curve ) )
				return false;
		}
		else
		{
			return false;
		}

		child = child->next_sibling();
	}

	if( tw_curve.getNumControlPoints() > 0 )
	{
		if( tw_curve.getControlPoint(0).size() != mAnimSpace->getNumBaseAnimations() )
		{
			zhLog( "ZHALoader", "parseTimewarpCurve", "ERROR: Invalid ZHA file. Timewarp curve dimensionality differs from number of base animations in the animation space." );
			return false;
		}

		tw_curve.calcTangents();
		mAnimSpace->setTimewarpCurve(tw_curve);
	}
	
	return true;
}

bool ZHALoader::parseCtrlPoint( rapidxml::xml_node<>* node, CatmullRomSpline<Vector>& spline )
{
	rapidxml::xml_attribute<>* attrib;

	Vector pos;

	// parse node attributes:

	attrib = node->first_attribute( "position" );
	if( attrib == NULL )
	{
		zhLog( "ZHALoader", "parseCtrlPoint", "ERROR: Invalid ZHA file. CtrlPoint element missing position attribute." );
		return false;
	}
	if( !parseVector( attrib->value(), pos ) )
	{
		zhLog( "ZHALoader", "parseCtrlPoint", "ERROR: Invalid ZHA file. position attribute of CtrlPoint element has invalid value." );
		return false;
	}

	// check attribute validity:

	if( spline.getNumControlPoints() > 0 && pos.size() != spline.getControlPoint(0).size() )
	{
		zhLog( "ZHALoader", "parseCtrlPoint", "ERROR: Invalid ZHA file. Inconsistent dimensionality of position attribute of CtrlPoint element." );
		return false;
	}

	// assign attribute values:

	spline.addControlPoint(pos);

	return true;
}

bool ZHALoader::parseAlignmentCurve( rapidxml::xml_node<>* node )
{
	rapidxml::xml_node<>* child;

	CatmullRomSpline<Vector> align_curve;

	// parse child elements:

	child = node->first_node();
	while( child != NULL )
	{
		if( !strcmp( child->name(), "CtrlPoint" ) )
		{
			if( !parseCtrlPoint( child, align_curve ) )
				return false;
		}
		else
		{
			return false;
		}

		child = child->next_sibling();
	}

	if( align_curve.getNumControlPoints() > 0 )
	{
		if( align_curve.getControlPoint(0).size() != 3 * mAnimSpace->getNumBaseAnimations() )
		{
			zhLog( "ZHALoader", "parseAlignmentCurve", "ERROR: Invalid ZHA file. Alignment curve dimensionality differs from 3 * number of base animations in the animation space." );
			return false;
		}

		align_curve.calcTangents();
		mAnimSpace->setAlignmentCurve(align_curve);
	}
	
	return true;
}

bool ZHALoader::parseParametrization( rapidxml::xml_node<>* node )
{
	rapidxml::xml_node<>* child;
	rapidxml::xml_attribute<>* attrib;

	unsigned int num_params;
	AnimationParamClass class_id;

	// parse node attributes:
	
	attrib = node->first_attribute( "numParams" );
	if( attrib == NULL )
	{
		zhLog( "ZHALoader", "parseParametrization", "ERROR: Invalid ZHA file. Parametrization element missing numParams attribute." );
		return false;
	}
	num_params = fromString<unsigned int>( attrib->value() );

	attrib = node->first_attribute( "classId" );
	if( attrib == NULL )
	{
		zhLog( "ZHALoader", "parseParametrization", "ERROR: Invalid ZHA file. Parametrization element missing classId attribute." );
		return false;
	}
	if( !parseParamClass( attrib->value(), class_id ) )
	{
		zhLog( "ZHALoader", "parseParametrization", "ERROR: Invalid ZHA file. classId attribute of Parametrization element has invalid value." );
		return false;
	}

	// check attribute validity:

	if( num_params <= 0 )
	{
		zhLog( "ZHALoader", "parseParametrization", "ERROR: numParams attribute of Parametrization element has value 0." );
		return false;
	}

	// parse child elements:

	AnimationParametrization* anim_param = mAnimSpace->createParametrization( class_id, num_params, mAnimSpace->getNumBaseAnimations() );

	child = node->first_node( "Params" );
	if( child != NULL )
	{
		if( !parseParams(child) )
			return false;
	}
	else
	{
		zhLog( "ZHALoader", "parseParametrization", "ERROR: Parametrization element missing child element Params." );
		return false;
	}

	child = node->first_node( "BaseSamples" );
	if( child != NULL )
	{
		if( !parseBaseSamples(child) )
			return false;
	}
	else
	{
		zhLog( "ZHALoader", "parseParametrization", "ERROR: Parametrization element missing child element BaseSamples." );
		return false;
	}

	// if( class_id == AnimationParam_DenseSampling )
	{
		child = node->first_node( "DenseSamplingParametrization" );
		if( child != NULL )
		{
			if( !parseDenseSamplingParametrization(child) )
				return false;
		}
		else
		{
			zhLog( "ZHALoader", "parseParametrization", "ERROR: Parametrization element missing child element DenseSamplingParametrization." );
			return false;
		}

		static_cast<DenseSamplingParametrization*>( anim_param )->buildKDTree();
	}
	
	return true;
}

bool ZHALoader::parseParamClass( const std::string& paramClassStr, AnimationParamClass& paramClass )
{
	if( paramClassStr == "DenseSampling" )
		paramClass = AnimationParam_DenseSampling;
	else
		return false;

	return true;
}

bool ZHALoader::parseParams( rapidxml::xml_node<>* node )
{
	rapidxml::xml_node<>* child;

	unsigned int param_i = 0;

	// parse child elements:

	child = node->first_node();
	while( child != NULL )
	{
		if( !strcmp( child->name(), "Param" ) )
		{
			if( !parseParam( child, param_i++ ) )
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

bool ZHALoader::parseParam( rapidxml::xml_node<>* node, unsigned int paramIndex )
{
	rapidxml::xml_attribute<>* attrib;

	std::string name;

	// parse node attributes:

	attrib = node->first_attribute( "name" );
	if( attrib == NULL )
	{
		zhLog( "ZHALoader", "parseParam", "ERROR: Invalid ZHA file. Param element missing name attribute." );
		return false;
	}
	name = attrib->value();

	// check attribute validity:

	// assign attribute values:

	mAnimSpace->getParametrization()->setParam( paramIndex, name );

	return true;
}

bool ZHALoader::parseBaseSamples( rapidxml::xml_node<>* node )
{
	rapidxml::xml_node<>* child;

	unsigned int bsi = 0;

	// parse child elements:

	child = node->first_node();
	while( child != NULL )
	{
		if( !strcmp( child->name(), "Sample" ) )
		{
			if( !parseSample( child, bsi++ ) )
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

bool ZHALoader::parseSample( rapidxml::xml_node<>* node, unsigned int sampleIndex, bool baseSample )
{
	rapidxml::xml_attribute<>* attrib;

	Vector params, weights;

	// parse node attributes:

	attrib = node->first_attribute( "paramValues" );
	if( attrib == NULL )
	{
		zhLog( "ZHALoader", "parseSample", "ERROR: Invalid ZHA file. Sample element missing paramValues attribute." );
		return false;
	}
	if( !parseVector( attrib->value(), params ) )
	{
		zhLog( "ZHALoader", "parseSample", "ERROR: Invalid ZHA file. paramValues attribute of Sample element has invalid value." );
		return false;
	}

	if( !baseSample )
	{
		attrib = node->first_attribute( "weights" );
		if( attrib == NULL )
		{
			zhLog( "ZHALoader", "parseSample", "ERROR: Invalid ZHA file. Sample element missing weights attribute." );
			return false;
		}
		if( !parseVector( attrib->value(), weights ) )
		{
			zhLog( "ZHALoader", "parseSample", "ERROR: Invalid ZHA file. weights attribute of Sample element has invalid value." );
			return false;
		}
	}

	// check attribute validity:

	if( params.size() != mAnimSpace->getParametrization()->getNumParams() )
	{
		zhLog( "ZHALoader", "parseSample", "ERROR: Invalid ZHA file. Number of values in paramValues attribute of Sample element differs from number of parameters." );
		return false;
	}

	if( !baseSample && weights.size() != mAnimSpace->getParametrization()->getNumBaseSamples() )
	{
		zhLog( "ZHALoader", "parseSample", "ERROR: Invalid ZHA file. Number of values in weights attribute of Sample element differs from number of base animations." );
		return false;
	}

	// assign attribute values:

	if( baseSample )
		mAnimSpace->getParametrization()->setBaseSample( sampleIndex, params );
	else
		static_cast<DenseSamplingParametrization*>( mAnimSpace->getParametrization() )->addSample( params, weights );

	return true;
}

bool ZHALoader::parseDenseSamplingParametrization( rapidxml::xml_node<>* node )
{
	rapidxml::xml_node<>* child;

	// parse child elements:

	child = node->first_node( "Samples" );
	if( child != NULL )
	{
		if( !parseSamples(child) )
			return false;
	}
	else
	{
		zhLog( "ZHALoader", "parseDenseSamplingParametrization", "ERROR: Invalid ZHA file. DenseSamplingParametrization element missing child element Samples." );
		return false;
	}

	return true;
}

bool ZHALoader::parseSamples( rapidxml::xml_node<>* node )
{
	rapidxml::xml_node<>* child;

	unsigned int si = 0;

	// parse child elements:

	child = node->first_node();
	while( child != NULL )
	{
		if( !strcmp( child->name(), "Sample" ) )
		{
			if( !parseSample( child, si++, false ) )
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

}
