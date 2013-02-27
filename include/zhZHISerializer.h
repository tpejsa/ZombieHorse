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
#include "zhResourceManager.h"
#include "zhAnimationIndexManager.h"
#include "rapidxml.hpp"

namespace zh
{

/**
* @brief ZHI animation index file serializer class.
*/
class zhDeclSpec ZHISerializer : public ResourceSerializer
{

public:

	zhDeclare_ResourceSerializer( ZHISerializer, zhZHISerializer_ClassId, zhZHISerializer_ClassName )

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

	bool writeAnimationIndex( rapidxml::xml_node<>* node );
	bool writeAnimations( rapidxml::xml_node<>* node );
	bool writeAnimation( rapidxml::xml_node<>* node, const AnimationSegment& animSeg );
	bool writeMatchWebs( rapidxml::xml_node<>* node );
	bool writeMatchWeb( rapidxml::xml_node<>* node );
	bool writePaths( rapidxml::xml_node<>* node );
	bool writePath( rapidxml::xml_node<>* node, const MatchWeb::Path& path );
	bool writePoints( rapidxml::xml_node<>* node, const MatchWeb::Path& path );
	bool writePoint( rapidxml::xml_node<>* node, const AnimationDistanceGrid::Point& point );
	std::string writeSituation( const Model::Situation& sit );
	bool writeBranches( rapidxml::xml_node<>* node, const MatchWeb::Path& path );
	bool writeBranch( rapidxml::xml_node<>* node, unsigned int point, unsigned int branch );

	AnimationIndexPtr mAnimIndex;
	MatchWeb* mMatchWeb;
	std::string mPath;

	rapidxml::xml_document<> mDoc;

};

}

#endif // __zhZHISerializer_h__
