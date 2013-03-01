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

#include "zhZHASerializer.h"
#include "zhBoneAnimationTrack.h"
#include "zhString.h"
#include "zhFileSystem.h"
#include "rapidxml_print.hpp"

#include <fstream>

namespace zh
{

bool ZHASerializer::trySerialize( ResourcePtr res, const std::string& path )
{
	zhAssert( res != NULL );

	std::string dir, filename, prefix, ext;
	parsePathStr( path, dir, filename, prefix, ext );

	if( ext != "zh" )
		return false;

	return true;
}

bool ZHASerializer::serialize( ResourcePtr res, const std::string& path )
{
	zhAssert( res != NULL );

	mAnimSet = AnimationSetPtr::DynamicCast<Resource>(res);
	mAnim = NULL;
	mPath = path;

	// write ZHA file
	char* node_name = mDoc.allocate_string( "AnimationSet" );
	rapidxml::xml_node<>* node = mDoc.allocate_node( rapidxml::node_element, node_name );
	mDoc.append_node(node);
	if( !writeAnimationSet(node) )
		return false;

	// serialize DOM
	ofstream vaf( path.c_str() );
	vaf << mDoc;

	// free memory
	vaf.close();
	mDoc.clear();

	return true;
}

bool ZHASerializer::writeAnimationSet( rapidxml::xml_node<>* node )
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
	attrib_value = mDoc.allocate_string( toString<unsigned long>( mAnimSet->getId() ).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	attrib_name = mDoc.allocate_string( "name" );
	attrib_value = mDoc.allocate_string( mAnimSet->getName().c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	// write children:

	node_name = mDoc.allocate_string( "Animations" );
	child = mDoc.allocate_node( rapidxml::node_element, node_name );
	node->append_node(child);

	if( !writeAnimations(child) )
		return false;

	node_name = mDoc.allocate_string( "AnimationSpaces" );
	child = mDoc.allocate_node( rapidxml::node_element, node_name );
	node->append_node(child);

	if( !writeAnimationSpaces(child) )
		return false;

	return true;
}

bool ZHASerializer::writeAnimations( rapidxml::xml_node<>* node )
{
	rapidxml::xml_node<>* child;

	char* node_name;
	
	// write children:

	AnimationSet::AnimationConstIterator kfai = mAnimSet->getAnimationConstIterator();
	while( !kfai.end() )
	{
		mAnim = kfai.next();

		node_name = mDoc.allocate_string( "Animation" );
		child = mDoc.allocate_node( rapidxml::node_element, node_name );
		node->append_node(child);

		if( !writeAnimation(child) )
			return false;

		mAnim = NULL;
	}

	return true;
}

bool ZHASerializer::writeAnimation( rapidxml::xml_node<>* node )
{
	rapidxml::xml_attribute<>* attrib;
	rapidxml::xml_node<>* child;

	char* node_name;
	char* attrib_name;
	char* attrib_value;

	// write attributes:

	attrib_name = mDoc.allocate_string( "id" );
	attrib_value = mDoc.allocate_string( toString<unsigned short>( mAnim->getId() ).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	attrib_name = mDoc.allocate_string( "name" );
	attrib_value = mDoc.allocate_string( mAnim->getName().c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	attrib_name = mDoc.allocate_string( "interpMethod" );
	attrib_value = mDoc.allocate_string(
		writeKFInterpolationMethod( static_cast<Animation*>(mAnim)->getKFInterpolationMethod() ).c_str()
		);
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	// write children:

	node_name = mDoc.allocate_string( "Tracks" );
	child = mDoc.allocate_node( rapidxml::node_element, node_name );
	node->append_node(child);
	
	if( !writeTracks(child) )
		return false;

	node_name = mDoc.allocate_string( "Annotations" );
	child = mDoc.allocate_node( rapidxml::node_element, node_name );
	node->append_node(child);
	
	if( !writeAnnotations(child) )
		return false;

	return true;
}

std::string ZHASerializer::writeKFInterpolationMethod( KFInterpolationMethod interpMethod )
{
	if( interpMethod == KFInterp_Linear )
		return "Linear";
	else // if( interpMethod == KFInterp_Spline )
		return "Spline";
}

bool ZHASerializer::writeTracks( rapidxml::xml_node<>* node )
{
	rapidxml::xml_node<>* child;

	char* node_name;

	// write attributes:

	// write children:

	Animation::BoneTrackConstIterator bti = mAnim->getBoneTrackConstIterator();
	while( !bti.end() )
	{
		node_name = mDoc.allocate_string( "BoneTrack" );
		child = mDoc.allocate_node( rapidxml::node_element, node_name );
		node->append_node(child);

		if( !writeBoneTrack( child, bti.next() ) )
		{
			return false;
		}
	}

	return true;
}

bool ZHASerializer::writeBoneTrack( rapidxml::xml_node<>* node, BoneAnimationTrack* track )
{
	rapidxml::xml_attribute<>* attrib;
	rapidxml::xml_node<>* child;

	char* node_name;
	char* attrib_name;
	char* attrib_value;

	// write attributes:

	attrib_name = mDoc.allocate_string( "boneId" );
	attrib_value = mDoc.allocate_string( toString<unsigned short>( track->getBoneId() ).c_str()  );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	// write children:

	node_name = mDoc.allocate_string( "TransformKeyFrames" );
	child = mDoc.allocate_node( rapidxml::node_element, node_name );
	node->append_node(child);
	
	if( !writeTransformKeyFrames( child, track ) )
		return false;

	return true;
}

bool ZHASerializer::writeTransformKeyFrames( rapidxml::xml_node<>* node, BoneAnimationTrack* track )
{
	rapidxml::xml_node<>* child;

	char* node_name;

	// write attributes:

	// write children:

	BoneAnimationTrack::KeyFrameConstIterator kfi = track->getKeyFrameConstIterator();
	while( !kfi.end() )
	{
		node_name = mDoc.allocate_string( "TransformKeyFrame" );
		child = mDoc.allocate_node( rapidxml::node_element, node_name );
		node->append_node(child);

		if( !writeTransformKeyFrame( child, static_cast<TransformKeyFrame*>( kfi.next() ) ) )
		{
			return false;
		}
	}

	return true;
}

bool ZHASerializer::writeTransformKeyFrame( rapidxml::xml_node<>* node, TransformKeyFrame* kf )
{
	rapidxml::xml_attribute<>* attrib;

	char* attrib_name;
	char* attrib_value;

	// write attributes:
	
	attrib_name = mDoc.allocate_string( "time" );
	attrib_value = mDoc.allocate_string( toString<float>( kf->getTime() ).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	attrib_name = mDoc.allocate_string( "translation" );
	attrib_value = mDoc.allocate_string( writeVector3( kf->getTranslation() ).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	attrib_name = mDoc.allocate_string( "rotation" );
	attrib_value = mDoc.allocate_string( writeQuat( kf->getRotation() ).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	attrib_name = mDoc.allocate_string( "scale" );
	attrib_value = mDoc.allocate_string( writeVector3( kf->getScale() ).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	return true;
}

std::string ZHASerializer::writeVector3( const Vector3& v )
{
	return toString<float>( v.x ) + " " + toString<float>( v.y ) + " " + toString<float>( v.z );
}

std::string ZHASerializer::writeQuat( const Quat& q )
{
	Vector3 axis;
	float angle;
	q.getAxisAngle( axis, angle );

	return toString<float>(axis.x) + " " + toString<float>(axis.y) + " " + toString<float>(axis.z) + " " + 
		toString<float>(angle);
}

std::string ZHASerializer::writeVector( const Vector& v )
{
	std::string zhtr;

	for( unsigned int i = 0; i < v.size(); ++i )
	{
		zhtr += toString<float>( v.get(i) );
		if( i < v.size() - 1 )
			zhtr += " ";
	}

	return zhtr;
}

bool ZHASerializer::writeAnnotations( rapidxml::xml_node<>* node )
{
	rapidxml::xml_node<>* child;

	char* node_name;

	TransitionAnnotationContainer* tannots = mAnim->getTransitionAnnotations();
	ParamTransitionAnnotationContainer* ptannots = mAnim->getParamTransitionAnnotations();
	PlantConstraintAnnotationContainer* pcannots = mAnim->getPlantConstraintAnnotations();
	SimEventAnnotationContainer* seannots = mAnim->getSimEventAnnotations();

	// write attributes:

	// write children:

	TransitionAnnotationContainer::AnnotationConstIterator tani = tannots->getAnnotationConstIterator();
	while( !tani.end() )
	{
		node_name = mDoc.allocate_string( "Annotation" );
		child = mDoc.allocate_node( rapidxml::node_element, node_name );
		node->append_node(child);

		if( !writeAnnotation( child, tani.next() ) )
		{
			return false;
		}
	}

	ParamTransitionAnnotationContainer::AnnotationConstIterator ptani = ptannots->getAnnotationConstIterator();
	while( !ptani.end() )
	{
		node_name = mDoc.allocate_string( "Annotation" );
		child = mDoc.allocate_node( rapidxml::node_element, node_name );
		node->append_node(child);

		if( !writeAnnotation( child, ptani.next() ) )
		{
			return false;
		}
	}

	PlantConstraintAnnotationContainer::AnnotationConstIterator pcani = pcannots->getAnnotationConstIterator();
	while( !pcani.end() )
	{
		node_name = mDoc.allocate_string( "Annotation" );
		child = mDoc.allocate_node( rapidxml::node_element, node_name );
		node->append_node(child);

		if( !writeAnnotation( child, pcani.next() ) )
		{
			return false;
		}
	}

	SimEventAnnotationContainer::AnnotationConstIterator seani = seannots->getAnnotationConstIterator();
	while( !seani.end() )
	{
		node_name = mDoc.allocate_string( "Annotation" );
		child = mDoc.allocate_node( rapidxml::node_element, node_name );
		node->append_node(child);

		if( !writeAnnotation( child, seani.next() ) )
		{
			return false;
		}
	}

	return true;
}

bool ZHASerializer::writeAnnotation( rapidxml::xml_node<>* node, AnimationAnnotation* annot )
{
	rapidxml::xml_attribute<>* attrib;
	rapidxml::xml_node<>* child;

	char* node_name;
	char* attrib_name;
	char* attrib_value;

	// write attributes:

	attrib_name = mDoc.allocate_string( "classId" );
	attrib_value = mDoc.allocate_string( writeAnnotationClass( annot->getClassId() ).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	attrib_name = mDoc.allocate_string( "startTime" );
	attrib_value = mDoc.allocate_string( toString<float>( annot->getStartTime() ).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	attrib_name = mDoc.allocate_string( "endTime" );
	attrib_value = mDoc.allocate_string( toString<float>( annot->getEndTime() ).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	// write children:

	if( annot->getClassId() == AnimAnnot_Transition )
	{
		node_name = mDoc.allocate_string( "TransitionAnnotation" );
		child = mDoc.allocate_node( rapidxml::node_element, node_name );
		node->append_node(child);

		if( !writeTransitionAnnotation( child, static_cast<TransitionAnnotation*>(annot) ) )
			return false;
	}
	else if( annot->getClassId() == AnimAnnot_ParamTransition )
	{
		node_name = mDoc.allocate_string( "ParamTransitionAnnotation" );
		child = mDoc.allocate_node( rapidxml::node_element, node_name );
		node->append_node(child);

		if( !writeParamTransitionAnnotation( child, static_cast<ParamTransitionAnnotation*>(annot) ) )
			return false;
	}
	else if( annot->getClassId() == AnimAnnot_PlantConstraint )
	{
		node_name = mDoc.allocate_string( "PlantConstraintAnnotation" );
		child = mDoc.allocate_node( rapidxml::node_element, node_name );
		node->append_node(child);

		if( !writePlantConstraintAnnotation( child, static_cast<PlantConstraintAnnotation*>(annot) ) )
			return false;
	}
	else // if( annot->getClassId() == AnimAnnot_SimEvent )
	{
		node_name = mDoc.allocate_string( "SimEventAnnotation" );
		child = mDoc.allocate_node( rapidxml::node_element, node_name );
		node->append_node(child);

		if( !writeSimEventAnnotation( child, static_cast<SimEventAnnotation*>(annot) ) )
			return false;
	}

	return true;
}

std::string ZHASerializer::writeAnnotationClass( AnimationAnnotationClass annotClass )
{
	if( annotClass == AnimAnnot_Transition )
		return "Transition";
	else if( annotClass == AnimAnnot_ParamTransition )
		return "ParamTransition";
	else if( annotClass == AnimAnnot_PlantConstraint )
		return "PlantConstraint";
	else // if( annotClass == AnimAnnot_SimEvent )
		return "SimEvent";
}

bool ZHASerializer::writeTransitionAnnotation( rapidxml::xml_node<>* node, TransitionAnnotation* annot )
{
	rapidxml::xml_attribute<>* attrib;

	char* attrib_name;
	char* attrib_value;

	// write attributes:

	if( annot->getTargetSetId() != mAnimSet->getId() )
	{
		attrib_name = mDoc.allocate_string( "targetSetId" );
		attrib_value = mDoc.allocate_string( toString<unsigned long>( annot->getTargetSetId() ).c_str() );
		attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
		node->append_attribute(attrib);
	}
	
	attrib_name = mDoc.allocate_string( "targetId" );
	attrib_value = mDoc.allocate_string( toString<unsigned short>( annot->getTargetId() ).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	attrib_name = mDoc.allocate_string( "targetTime" );
	attrib_value = mDoc.allocate_string( toString<float>( annot->getTargetTime() ).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	attrib_name = mDoc.allocate_string( "alignTransf" );
	attrib_value = mDoc.allocate_string( writeSituation( annot->getAlignTransf() ).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	// write children:

	return true;
}

std::string ZHASerializer::writeSituation( const Skeleton::Situation& sit )
{
	return toString<float>( sit.getPosX() ) + " " + toString<float>( sit.getPosZ() ) + " " + toString<float>( sit.getOrientY() );
}

bool ZHASerializer::writeParamTransitionAnnotation( rapidxml::xml_node<>* node, ParamTransitionAnnotation* annot )
{
	rapidxml::xml_attribute<>* attrib;

	char* attrib_name;
	char* attrib_value;

	// write attributes:

	if( annot->getTargetSetId() != mAnimSet->getId() )
	{
		attrib_name = mDoc.allocate_string( "targetSetId" );
		attrib_value = mDoc.allocate_string( toString<unsigned long>( annot->getTargetSetId() ).c_str() );
		attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
		node->append_attribute(attrib);
	}
	
	attrib_name = mDoc.allocate_string( "targetId" );
	attrib_value = mDoc.allocate_string( toString<unsigned short>( annot->getTargetId() ).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	attrib_name = mDoc.allocate_string( "lowerBound" );
	attrib_value = mDoc.allocate_string( writeVector( annot->getLowerBound() ).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	attrib_name = mDoc.allocate_string( "upperBound" );
	attrib_value = mDoc.allocate_string( writeVector( annot->getUpperBound() ).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	attrib_name = mDoc.allocate_string( "targetTime" );
	attrib_value = mDoc.allocate_string( toString<float>( annot->getTargetTime() ).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	attrib_name = mDoc.allocate_string( "alignTransf" );
	attrib_value = mDoc.allocate_string( writeSituation( annot->getAlignTransf() ).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	// write children:

	return true;
}

bool ZHASerializer::writePlantConstraintAnnotation( rapidxml::xml_node<>* node, PlantConstraintAnnotation* annot )
{
	rapidxml::xml_attribute<>* attrib;

	char* attrib_name;
	char* attrib_value;

	// write attributes:
	
	attrib_name = mDoc.allocate_string( "boneId" );
	attrib_value = mDoc.allocate_string( toString<unsigned short>( annot->getBoneId() ).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	// write children:

	return true;
}

bool ZHASerializer::writeSimEventAnnotation( rapidxml::xml_node<>* node, SimEventAnnotation* annot )
{
	rapidxml::xml_attribute<>* attrib;

	char* attrib_name;
	char* attrib_value;

	// write attributes:
	
	attrib_name = mDoc.allocate_string( "eventClassId" );
	attrib_value = mDoc.allocate_string( toString<unsigned short>( annot->getEventClassId() ).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	if( annot->getEventId() != USHRT_MAX )
	{
		attrib_name = mDoc.allocate_string( "eventId" );
		attrib_value = mDoc.allocate_string( toString<unsigned short>( annot->getEventId() ).c_str() );
		attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
		node->append_attribute(attrib);
	}

	// write children:

	return true;
}

bool ZHASerializer::writeAnimationSpaces( rapidxml::xml_node<>* node )
{
	rapidxml::xml_node<>* child;

	char* node_name;
	
	// write children:

	AnimationSet::AnimationSpaceConstIterator asi = mAnimSet->getAnimationSpaceConstIterator();
	while( !asi.end() )
	{
		mAnimSpace = asi.next();

		node_name = mDoc.allocate_string( "AnimationSpace" );
		child = mDoc.allocate_node( rapidxml::node_element, node_name );
		node->append_node(child);

		if( !writeAnimationSpace(child) )
			return false;

		mAnimSpace = NULL;
	}

	return true;
}

bool ZHASerializer::writeAnimationSpace( rapidxml::xml_node<>* node )
{
	rapidxml::xml_attribute<>* attrib;
	rapidxml::xml_node<>* child;

	char* node_name;
	char* attrib_name;
	char* attrib_value;

	// write attributes:

	attrib_name = mDoc.allocate_string( "id" );
	attrib_value = mDoc.allocate_string( toString<unsigned short>( mAnimSpace->getId() ).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	attrib_name = mDoc.allocate_string( "name" );
	attrib_value = mDoc.allocate_string( mAnimSpace->getName().c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	// write children:

	node_name = mDoc.allocate_string( "BaseAnimations" );
	child = mDoc.allocate_node( rapidxml::node_element, node_name );
	node->append_node(child);
	
	if( !writeBaseAnimations(child) )
		return false;

	if( mAnimSpace->hasTimewarpCurve() )
	{
		node_name = mDoc.allocate_string( "TimewarpCurve" );
		child = mDoc.allocate_node( rapidxml::node_element, node_name );
		node->append_node(child);
	
		if( !writeTimewarpCurve(child) )
			return false;
	}

	if( mAnimSpace->hasAlignmentCurve() )
	{
		node_name = mDoc.allocate_string( "AlignmentCurve" );
		child = mDoc.allocate_node( rapidxml::node_element, node_name );
		node->append_node(child);
		
		if( !writeAlignmentCurve(child) )
			return false;
	}

	if( mAnimSpace->hasParametrization() )
	{
		node_name = mDoc.allocate_string( "Parametrization" );
		child = mDoc.allocate_node( rapidxml::node_element, node_name );
		node->append_node(child);
		
		if( !writeParametrization(child) )
			return false;
	}

	return true;
}

bool ZHASerializer::writeBaseAnimations( rapidxml::xml_node<>* node )
{
	rapidxml::xml_attribute<>* attrib;

	char* attrib_name;
	char* attrib_value;

	// write attributes:
	
	attrib_name = mDoc.allocate_string( "animations" );
	attrib_value = mDoc.allocate_string( writeBaseAnimationList().c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	return true;
}

std::string ZHASerializer::writeBaseAnimationList()
{
	std::string zhtr;

	for( unsigned int i = 0; i < mAnimSpace->getNumBaseAnimations(); ++i )
	{
		zhtr += toString<unsigned short>( mAnimSpace->getBaseAnimation(i)->getId() );
		if( i < mAnimSpace->getNumBaseAnimations() - 1 )
			zhtr += " ";
	}

	return zhtr;
}

bool ZHASerializer::writeTimewarpCurve( rapidxml::xml_node<>* node )
{
	rapidxml::xml_node<>* child;

	char* node_name;
	
	// write children:

	const CatmullRomSpline<Vector>& tw_curve = mAnimSpace->getTimewarpCurve();
	for( unsigned int cpi = 0; cpi < tw_curve.getNumControlPoints(); ++cpi )
	{
		node_name = mDoc.allocate_string( "CtrlPoint" );
		child = mDoc.allocate_node( rapidxml::node_element, node_name );
		node->append_node(child);

		if( !writeCtrlPoint( child, tw_curve.getControlPoint(cpi) ) )
			return false;
	}

	return true;
}

bool ZHASerializer::writeCtrlPoint( rapidxml::xml_node<>* node, const Vector& pos )
{
	rapidxml::xml_attribute<>* attrib;

	char* attrib_name;
	char* attrib_value;

	// write attributes:
	
	attrib_name = mDoc.allocate_string( "position" );
	attrib_value = mDoc.allocate_string( writeVector(pos).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	return true;
}

bool ZHASerializer::writeAlignmentCurve( rapidxml::xml_node<>* node )
{
	rapidxml::xml_node<>* child;

	char* node_name;
	
	// write children:

	const CatmullRomSpline<Vector>& tw_curve = mAnimSpace->getAlignmentCurve();
	for( unsigned int cpi = 0; cpi < tw_curve.getNumControlPoints(); ++cpi )
	{
		node_name = mDoc.allocate_string( "CtrlPoint" );
		child = mDoc.allocate_node( rapidxml::node_element, node_name );
		node->append_node(child);

		if( !writeCtrlPoint( child, tw_curve.getControlPoint(cpi) ) )
			return false;
	}

	return true;
}

bool ZHASerializer::writeParametrization( rapidxml::xml_node<>* node )
{
	rapidxml::xml_attribute<>* attrib;
	rapidxml::xml_node<>* child;

	char* node_name;
	char* attrib_name;
	char* attrib_value;

	AnimationParametrization* animparam = mAnimSpace->getParametrization();

	// write attributes:

	attrib_name = mDoc.allocate_string( "numParams" );
	attrib_value = mDoc.allocate_string( toString<unsigned int>( animparam->getNumParams() ).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	attrib_name = mDoc.allocate_string( "classId" );
	attrib_value = mDoc.allocate_string( writeParamClass( animparam->getClassId() ).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	// write children:

	node_name = mDoc.allocate_string( "Params" );
	child = mDoc.allocate_node( rapidxml::node_element, node_name );
	node->append_node(child);
	
	if( !writeParams(child) )
		return false;

	node_name = mDoc.allocate_string( "BaseSamples" );
	child = mDoc.allocate_node( rapidxml::node_element, node_name );
	node->append_node(child);
	
	if( !writeBaseSamples(child) )
		return false;

	// if( animparam->getClassId() == AnimationParam_DenseSampling )
	{
		node_name = mDoc.allocate_string( "DenseSamplingParametrization" );
		child = mDoc.allocate_node( rapidxml::node_element, node_name );
		node->append_node(child);
		
		if( !writeDenseSamplingParametrization(child) )
			return false;
	}

	return true;
}

std::string ZHASerializer::writeParamClass( AnimationParamClass paramClass )
{
	// if( paramClass == AnimationParam_DenseSampling )
	return "DenseSampling";
}

bool ZHASerializer::writeParams( rapidxml::xml_node<>* node )
{
	rapidxml::xml_node<>* child;

	char* node_name;

	AnimationParametrization* animparam = mAnimSpace->getParametrization();

	// write children:

	for( unsigned int param_i = 0; param_i < animparam->getNumParams(); ++param_i )
	{
		node_name = mDoc.allocate_string( "Param" );
		child = mDoc.allocate_node( rapidxml::node_element, node_name );
		node->append_node(child);

		if( !writeParam( child, animparam->getParam(param_i) ) )
		{
			return false;
		}
	}

	return true;
}

bool ZHASerializer::writeParam( rapidxml::xml_node<>* node, const std::string& paramName )
{
	rapidxml::xml_attribute<>* attrib;

	char* attrib_name;
	char* attrib_value;

	// write attributes:
	
	attrib_name = mDoc.allocate_string( "name" );
	attrib_value = mDoc.allocate_string( paramName.c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	return true;
}

bool ZHASerializer::writeBaseSamples( rapidxml::xml_node<>* node )
{
	rapidxml::xml_node<>* child;

	char* node_name;

	AnimationParametrization* animparam = mAnimSpace->getParametrization();

	// write children:

	for( unsigned int bsi = 0; bsi < animparam->getNumBaseSamples(); ++bsi )
	{
		node_name = mDoc.allocate_string( "Sample" );
		child = mDoc.allocate_node( rapidxml::node_element, node_name );
		node->append_node(child);

		if( !writeSample( child, animparam->getBaseSample(bsi) ) )
		{
			return false;
		}
	}

	return true;
}

bool ZHASerializer::writeSample( rapidxml::xml_node<>* node, const Vector& paramValues )
{
	rapidxml::xml_attribute<>* attrib;

	char* attrib_name;
	char* attrib_value;

	// write attributes:
	
	attrib_name = mDoc.allocate_string( "paramValues" );
	attrib_value = mDoc.allocate_string( writeVector(paramValues).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	return true;
}

bool ZHASerializer::writeDenseSamplingParametrization( rapidxml::xml_node<>* node )
{
	rapidxml::xml_node<>* child;

	char* node_name;

	// write children:

	node_name = mDoc.allocate_string( "Samples" );
	child = mDoc.allocate_node( rapidxml::node_element, node_name );
	node->append_node(child);
	
	if( !writeSamples(child) )
		return false;

	return true;
}

bool ZHASerializer::writeSamples( rapidxml::xml_node<>* node )
{
	rapidxml::xml_node<>* child;

	char* node_name;

	DenseSamplingParametrization* animparam = static_cast<DenseSamplingParametrization*>( mAnimSpace->getParametrization() );
	Vector params( animparam->getNumParams() ),
		weights( animparam->getNumBaseSamples() );

	// write children:

	for( unsigned int si = 0; si < animparam->getNumSamples(); ++si )
	{
		node_name = mDoc.allocate_string( "Sample" );
		child = mDoc.allocate_node( rapidxml::node_element, node_name );
		node->append_node(child);

		animparam->getSample( si, params, weights );
		
		if( !writeSample( child, params, weights ) )
		{
			return false;
		}
	}

	return true;
}

bool ZHASerializer::writeSample( rapidxml::xml_node<>* node, const Vector& paramValues, const Vector& weights )
{
	rapidxml::xml_attribute<>* attrib;

	char* attrib_name;
	char* attrib_value;

	// write attributes:
	
	attrib_name = mDoc.allocate_string( "paramValues" );
	attrib_value = mDoc.allocate_string( writeVector(paramValues).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	attrib_name = mDoc.allocate_string( "weights" );
	attrib_value = mDoc.allocate_string( writeVector(weights).c_str() );
	attrib = mDoc.allocate_attribute( attrib_name, attrib_value );
	node->append_attribute(attrib);

	return true;
}

}
