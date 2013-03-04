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
#include <boost/lexical_cast.hpp>
#include "zhBVHLoader.h"
#include "zhBoneAnimationTrack.h"
#include "zhString.h"
#include "zhAnimationSystem.h"
#include "zhFileSystem.h"
#include "boost\regex.hpp"
#include<cstring>
#include <sstream>

namespace zh
{
	std::string jointNameReg("(ROOT|JOINT|End)""[ \t\v\f]+""([A-Za-z]+)");
	std::string offsetReg("OFFSET[ \t\v\f]+(-?[0-9]+.[0-9]+|-?[0-9]+)[ \t\v\f]+(-?[0-9]+.[0-9]+|-?[0-9]+)[ \t\v\f]+(-?[0-9]+.[0-9]+|-?[0-9]+)");
	std::string channelsReg("CHANNELS ([0-9]+)([ \t\v\f]+[A-Za-z]+)+");
	enum CHANNELS{
		Xposition,
		Yposition,
		Zposition,
		Zrotation,
		Yrotation,
		Xrotation 
	};
	class JointInfo{
	public:
		JointInfo* parent;
		std::vector<JointInfo*> children;
		int jointID;
		std::string name;
		int numberOfChannels;
		float offsets[3];
		float* keyframes[6];
		static int  jointCount;
		static void increaseID(){jointCount++;}
		JointInfo(std::string jointName){
			name = jointName;
			jointID = jointCount++;
			increaseID();
		}
		~JointInfo(){
			for(int i = 0;i < children.size();++i){
				delete children[i];
			}
		}
		zh::Bone* ConvertToBones(Skeleton* skel){
			zh::Bone* parent = skel->createBone(jointID,boost::lexical_cast<string>(jointID) + name);
			parent -> setInitialPosition(zh::Vector3(offsets[0] * 100,offsets[1]*100,offsets[2]*100));
			for(int i = 0;i < children.size();++i){
				zh::Bone* tmp = children[i] -> ConvertToBones(skel);
				parent -> addChild(tmp);
			}
			return parent;
		}
	};
	int JointInfo::jointCount = 0;
	JointInfo* parseAJoint(std::string & jointContent,std::string jointName,JointInfo* parent=NULL){
		//printf("%s\n",jointName.c_str());
		JointInfo* joint = new JointInfo(jointName);
		joint->parent = parent;
		boost::cmatch what;
		boost::regex_search(jointContent.c_str(), what, boost::regex(offsetReg));
		//parse offset
		std::string offsetX;
		offsetX.assign(what[1].first, what[1].second);
		std::string offsetY;
		offsetY.assign(what[2].first, what[2].second);
		std::string offsetZ;
		offsetZ.assign(what[3].first, what[3].second);
	
		joint->offsets[0] = ::atof(offsetX.c_str());
		joint->offsets[1] = ::atof(offsetY.c_str());
		joint->offsets[2] = ::atof(offsetZ.c_str());
		//printf("%f %f %f\n",joint->offsets[0] ,joint->offsets[1] ,joint->offsets[2]);
		//parse channels if any.
		int next1 = jointContent.find("CHANNELS");
		int next2 = jointContent.find("}");
		if(next1 != std::string::npos && next1 < next2){
			boost::regex_search(jointContent.c_str(), what, boost::regex(channelsReg));
			std::string nChannels;
			nChannels.assign(what[1].first, what[1].second);
			joint->numberOfChannels = ::atoi(nChannels.c_str());
			jointContent = what.suffix();
		}
		while(next1 != std::string::npos && next1 < next2){
			//next joint
			boost::regex_search(jointContent.c_str(), what, boost::regex(jointNameReg));
			std::string jointName;
			jointName.assign(what[2].first, what[2].second);
			jointContent = what.suffix();
			joint->children.push_back(parseAJoint(jointContent,jointName,joint));
	
			//printf("%s\n",jointContent.c_str());
			//system("pause");
	
			next1 = jointContent.find("JOINT");
			int temp = jointContent.find("End");
			if(next1 == std::string::npos){
				next1 = temp;
			}else{
				next1 = (temp != std::string::npos && next1 > temp)?temp:next1;
			}

			next2 = jointContent.find("}");
		}
		jointContent = jointContent.substr(next2+1);
		//if(jointName == "LHipJoint"){
		//	printf("%s\n",jointContent.c_str());
		//	system("pause");
		//}
		return joint;
	}
	bool BVHLoader::tryLoad( ResourcePtr res, const std::string& path )
	{
		static bool first = true;
		if(first){
			first = false;
		}else{
			return false;
		}
		zhAssert( res != NULL );
		zhAssert( res->getClassId() == Resource_AnimationSet );

		std::string dir, filename, prefix, ext;
		parsePathStr( path, dir, filename, prefix, ext );

		//printf("bvh called!!!\n");
		if( ext != "bvh" )
			return false;

		return true;
	}

	bool BVHLoader::load( ResourcePtr res, const std::string& path )
	{
		
		zhAssert( res != NULL );
		zhAssert( res->getClassId() == Resource_AnimationSet );

		mAnimSet = AnimationSetPtr::DynamicCast<Resource>(res);
		mAnim = NULL;
		mPath = path;

		FILE* bvhf = fopen(mPath.c_str(), "rb" );
		int bvhf_size;
		char* bvhf_content = NULL;
		fseek( bvhf, 0, SEEK_END );
		bvhf_size = ftell(bvhf);
		fseek( bvhf, 0, SEEK_SET );
		bvhf_content = ( char* )malloc(bvhf_size+1 );
		if(bvhf_size != fread(bvhf_content, sizeof(char), bvhf_size, bvhf))
		{
			free(bvhf_content);
			fclose(bvhf);
			return false;
		}
		bvhf_content[bvhf_size] = 0;
		fclose(bvhf);
		std::string content(bvhf_content);
		int hierarchyStart = content.find("ROOT");
		int hierarchyEnd = content.find("MOTION");
		std::string skeletonContent = content.substr(hierarchyStart,hierarchyEnd - hierarchyStart);
		std::string motionContent = content.substr(hierarchyEnd + 7,content.length() - (hierarchyEnd + 7));;
		//parsing!!
		//std::string offsetReg("HIERARCHY[\t\r\n\v\f]+ROOT Hips{[A-Za-z]+}");
		
		const boost::regex jointNameStructure(jointNameReg.c_str());
		boost::cmatch what;
		boost::regex_search(skeletonContent.c_str(), what, jointNameStructure);
		// done parsing, free memory
		std::string jointName;
		jointName.assign(what[2].first, what[2].second);
		skeletonContent = what.suffix();
		JointInfo* root = parseAJoint(skeletonContent,jointName);
		free(bvhf_content);

		Skeleton* skel = zhAnimationSystem->createSkeleton(mAnimSet->getName());
		root -> ConvertToBones(skel);
		return true;
	}

}
