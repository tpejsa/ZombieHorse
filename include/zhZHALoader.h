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

#ifndef __zhZHALoader_h__
#define __zhZHALoader_h__

#include "zhPrereq.h"
#include "zhLogger.h"
#include "zhMath.h"
#include "zhResourceManager.h"
#include "zhAnimationManager.h"
#include "zhAnimation.h"
#include "zhAnimationSpace.h"
#include "rapidxml.hpp"

namespace zh
{

/**
* @brief ZHA animation file loader class.
*/
class zhDeclSpec ZHALoader : public ResourceLoader
{

public:

	zhDeclare_ResourceLoader( ZHALoader, zhZHALoader_ClassId, zhZHALoader_ClassName )

	/**
	* Checks if the loader can load the specified resource
	* (i.e. if it is supported file type).
	* This pure virtual function needs to be implemented in
	* a derived resource IO class.
	*
	* @param resource Pointer to the resource that should be initialized
	* from the file.
	* @param path Resource file path.
	* @return true if resource can be loaded, otherwise false.
	*/
	bool tryLoad( ResourcePtr res, const std::string& path );

	/**
	* Loads the resource from a file.
	*
	* @param res Pointer to the resource that should be initialized
	* from the file.
	* @param path Resource file path.
	* @return true if resource has been successfully loaded, otherwise false.
	*/
	bool load( ResourcePtr res, const std::string& path );

protected:

	bool parseAnimationSet( rapidxml::xml_node<>* node );
	bool parseAnimations( rapidxml::xml_node<>* node );
	bool parseAnimation( rapidxml::xml_node<>* node );
	bool parseKFInterpolationMethod( std::string interpMethodStr, KFInterpolationMethod& interpMethod );
	bool parseTracks( rapidxml::xml_node<>* node );
	bool parseBoneTrack( rapidxml::xml_node<>* node );
	bool parseTransformKeyFrames( rapidxml::xml_node<>* node, BoneAnimationTrack* track );
	bool parseTransformKeyFrame( rapidxml::xml_node<>* node, BoneAnimationTrack* track );
	bool parseVector3( const std::string& zhtr, Vector3& v );
	bool parseQuat( const std::string& qstr, Quat& q );
	bool parseVector( const std::string& zhtr, Vector& v );
	bool parseAnnotations( rapidxml::xml_node<>* node );
	bool parseAnnotation( rapidxml::xml_node<>* node );
	bool parseAnnotationClass( std::string annotClassStr, AnimationAnnotationClass& annotClass );
	bool parseTransitionAnnotation( rapidxml::xml_node<>* node, TransitionAnnotation* annot );
	bool parseSituation( const std::string& sitStr, Skeleton::Situation& sit );
	bool parseParamTransitionAnnotation( rapidxml::xml_node<>* node, ParamTransitionAnnotation* annot );
	bool parsePlantConstraintAnnotation( rapidxml::xml_node<>* node, PlantConstraintAnnotation* annot );
	bool parseSimEventAnnotation( rapidxml::xml_node<>* node, SimEventAnnotation* annot );
	bool parseAnimationSpaces( rapidxml::xml_node<>* node );
	bool parseAnimationSpace( rapidxml::xml_node<>* node );
	bool parseBaseAnimations( rapidxml::xml_node<>* node );
	bool parseBaseAnimationList( const std::string& listStr, std::vector<unsigned short>& list );
	bool parseTimewarpCurve( rapidxml::xml_node<>* node );
	bool parseCtrlPoint( rapidxml::xml_node<>* node, CatmullRomSpline<Vector>& spline );
	bool parseAlignmentCurve( rapidxml::xml_node<>* node );
	bool parseParametrization( rapidxml::xml_node<>* node );
	bool parseParamClass( const std::string& paramClassStr, AnimationParamClass& paramClass );
	bool parseParams( rapidxml::xml_node<>* node );
	bool parseParam( rapidxml::xml_node<>* node, unsigned int paramIndex );
	bool parseBaseSamples( rapidxml::xml_node<>* node );
	bool parseSample( rapidxml::xml_node<>* node, unsigned int sampleIndex, bool baseSample = true );
	bool parseDenseSamplingParametrization( rapidxml::xml_node<>* node );
	bool parseSamples( rapidxml::xml_node<>* node );

	AnimationSetPtr mAnimSet;
	Animation* mAnim;
	AnimationSpace* mAnimSpace;
	std::string mPath;

};

}

#endif // __zhZHALoader_h__
