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

#ifndef __zhZHASerializer_h__
#define __zhZHASerializer_h__

#include "zhPrereq.h"
#include "zhLogger.h"
#include "zhMath.h"
#include "zhResourceManager.h"
#include "zhAnimationManager.h"
#include "zhAnimationSpace.h"
#include "rapidxml.hpp"

namespace zh
{

/**
* @brief ZHA animation file serializer class.
*/
class zhDeclSpec ZHASerializer : public ResourceSerializer
{

public:

	zhDeclare_ResourceSerializer( ZHASerializer, zhZHASerializer_ClassId, zhZHASerializer_ClassName )

	/**
	* Checks if the serializer can load the specified resource
	* (i.e. if it is supported file type).
	* This pure virtual function needs to be implemented in
	* a derived resource IO class.
	*
	* @param resource Pointer to the resource that should be initialized
	* from the file.
	* @param path Resource file path.
	* @return true if resource can be loaded, otherwise false.
	*/
	bool trySerialize( ResourcePtr resource, const std::string& path );

	/**
	* Loads the resource from a file.
	*
	* @param res Pointer to the resource that should be initialized
	* from the file.
	* @param path Resource file path.
	* @return true if resource has been successfully loaded, otherwise false.
	*/
	bool serialize( ResourcePtr res, const std::string& path );

protected:

	bool writeAnimationSet( rapidxml::xml_node<>* node );
	bool writeAnimations( rapidxml::xml_node<>* node );
	bool writeAnimation( rapidxml::xml_node<>* node );
	std::string writeKFInterpolationMethod( KFInterpolationMethod interpMethod );
	bool writeTracks( rapidxml::xml_node<>* node );
	bool writeBoneTrack( rapidxml::xml_node<>* node, BoneAnimationTrack* track );
	bool writeTransformKeyFrames( rapidxml::xml_node<>* node, BoneAnimationTrack* track );
	bool writeTransformKeyFrame( rapidxml::xml_node<>* node, TransformKeyFrame* kf );
	std::string writeVector3( const Vector3& v );
	std::string writeQuat( const Quat& q );
	std::string writeVector( const Vector& v );
	bool writeAnnotations( rapidxml::xml_node<>* node );
	bool writeAnnotation( rapidxml::xml_node<>* node, AnimationAnnotation* annot );
	std::string writeAnnotationClass( AnimationAnnotationClass annotClass );
	bool writeTransitionAnnotation( rapidxml::xml_node<>* node, TransitionAnnotation* annot );
	std::string writeSituation( const Skeleton::Situation& sit );
	bool writeParamTransitionAnnotation( rapidxml::xml_node<>* node, ParamTransitionAnnotation* annot );
	bool writePlantConstraintAnnotation( rapidxml::xml_node<>* node, PlantConstraintAnnotation* annot );
	bool writeSimEventAnnotation( rapidxml::xml_node<>* node, SimEventAnnotation* annot );
	bool writeAnimationSpaces( rapidxml::xml_node<>* node );
	bool writeAnimationSpace( rapidxml::xml_node<>* node );
	bool writeBaseAnimations( rapidxml::xml_node<>* node );
	std::string writeBaseAnimationList();
	bool writeTimewarpCurve( rapidxml::xml_node<>* node );
	bool writeCtrlPoint( rapidxml::xml_node<>* node, const Vector& pos );
	bool writeAlignmentCurve( rapidxml::xml_node<>* node );
	bool writeParametrization( rapidxml::xml_node<>* node );
	std::string writeParamClass( AnimationParamClass paramClass );
	bool writeParams( rapidxml::xml_node<>* node );
	bool writeParam( rapidxml::xml_node<>* node, const std::string& paramName );
	bool writeBaseSamples( rapidxml::xml_node<>* node );
	bool writeSample( rapidxml::xml_node<>* node, const Vector& paramValues );
	bool writeDenseSamplingParametrization( rapidxml::xml_node<>* node );
	bool writeSamples( rapidxml::xml_node<>* node );
	bool writeSample( rapidxml::xml_node<>* node, const Vector& paramValues, const Vector& weights );

	AnimationSetPtr mAnimSet;
	Animation* mAnim;
	AnimationSpace* mAnimSpace;
	std::string mPath;

	rapidxml::xml_document<> mDoc;

};

}

#endif // __zhZHASerializer_h__
