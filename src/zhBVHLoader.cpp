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
	std::string channelsReg("CHANNELS[ \t\v\f]+([0-9]+)([ \t\v\f]+[A-Za-z]+)+");
	std::string channelReg6("CHANNELS[ \t\v\f]+[0-9]+[ \t\v\f]+([A-Za-z]+)[ \t\v\f]+([A-Za-z]+)[ \t\v\f]+([A-Za-z]+)[ \t\v\f]+([A-Za-z]+)[ \t\v\f]+([A-Za-z]+)[ \t\v\f]+([A-Za-z]+)");
	std::string channelReg3("CHANNELS[ \t\v\f]+[0-9]+[ \t\v\f]+([A-Za-z]+)[ \t\v\f]+([A-Za-z]+)[ \t\v\f]+([A-Za-z]+)");
	std::string nFramesReg("Frames:[ \t\v\f]+([0-9]+)");
	std::string frameTimeReg("Frame Time:[ \t\v\f]+([0-9]*.[0-9]+)");
	std::string keyFrameReg("[ \n\t\v\f]+(-?[0-9]*.[0-9]+)");
	enum JointNames{
		Hip,
		Chest,
		Shoulder,
		Elbow,
		Wrist,
		Knee,
		Ankle,
		Collar,
		Neck,
		Head,
		Fingers,
		Thumb,
		Toe,
		LowerBack
	};
	std::string jointNamesReg[] = {
			"(H|h)ip",
			"(C|c)hest",
			"(S|s)houlder",
			"(E|e)lbow",
			"(W|w)rist",
			"(K|k)nee",
			"(A|a)nkle",
			"(C|c)ollar",
			"(N|n)eck",
			"(H|h)ead",
			"(F|f)ingers",
			"(T|t)humb",
			"(T|t)oe",
			"(L|l)ower(B|b)ack"
	};
	enum JointSides{
		Center = 0,
		Left = 1,
		Right = 2
	};
	std::string jointSideReg[] = {
			"(L|l)(eft)*",
			"(R|r)(ight)*",
	};
	BoneTag ConvertTagAndSide(JointNames a,JointSides b){
		return BoneTag(a + (b+1) * 100);
	}
	zh::BoneTag ParseTag(string Name){
		//first test name
		boost::cmatch what;
		for(int i = Hip;i <= LowerBack;++i){
			if(boost::regex_search(Name.c_str(), what, boost::regex(jointNamesReg[i]))){
				string suffix;
				suffix.assign(what.suffix().first,what.suffix().second);
				string preffix;
				preffix.assign(what.prefix().first,what.prefix().second);
				for(int j = Left;j <= Right;++j){
					if(boost::regex_search(suffix.c_str(), what, boost::regex(jointSideReg[j - 1])) || boost::regex_search(preffix.c_str(), what, boost::regex(jointSideReg[j - 1]))){
						return ConvertTagAndSide(JointNames(i),JointSides(j));
					}
				}
				return ConvertTagAndSide(JointNames(i),JointSides(Center));
			}
		}
		return BT_Unknown;
	}
	enum CHANNELS{
		Xposition,
		Yposition,
		Zposition,
		Zrotation,
		Yrotation,
		Xrotation 
	};
	std::vector<std::vector<float>*> channelToKeyFrame;
	int totalChannels = 0;
	float frameTime;
	int nFrames;
	class JointInfo{
	public:
		zh::Bone* bone;
		JointInfo* parent;
		std::vector<JointInfo*> children;
		int jointID;
		std::string name;
		int numberOfChannels;
		int nChannelsSubTree;
		EulerRotOrder channelOrder;
		float offsets[3];
		std::vector<float> keyframes[6];
		static int  jointCount;
		static void resetID(){jointCount = 0;}
		static void increaseID(){jointCount++;}
		JointInfo(std::string jointName){
			name = jointName;
			jointID = jointCount;
			increaseID();
			numberOfChannels = 0;
		}
		~JointInfo(){
			for(int i = 0;i < children.size();++i){
				delete children[i];
			}
		}
		zh::Bone* ConvertToBones(Skeleton* skel){
			zh::Bone* parent = skel->createBone(jointID,boost::lexical_cast<string>(jointID)+ "_" + name);
			parent->tag(ParseTag(name));
			parent -> setInitialPosition(zh::Vector3(offsets[0],offsets[1],offsets[2]));
			for(int i = 0;i < children.size();++i){
				zh::Bone* tmp = children[i] -> ConvertToBones(skel);
				parent -> addChild(tmp);
			}
			bone = parent;
			return parent;
		}
		bool ConvertToAnimation(Animation* mAnim){
			if( frameTime > 0.00001f )
				mAnim->setFrameRate( zhRoundi(1.f/frameTime) );
			BoneAnimationTrack* root_tr = mAnim->createBoneTrack(jointID);
			for(int i = 0;i < nFrames;++i){
				TransformKeyFrame* tkf = static_cast<TransformKeyFrame*>(root_tr->createKeyFrame(i * frameTime));
				if(this->numberOfChannels == 6){
					tkf->setTranslation(Vector3(this->keyframes[0][i],this->keyframes[1][i],this->keyframes[2][i]));
					tkf->setRotation( Quat(zhRad(this->keyframes[3][i]),zhRad(this->keyframes[4][i]),zhRad(this->keyframes[5][i]),this->channelOrder));
				}else if(this->numberOfChannels == 3){
					tkf->setRotation( Quat(zhRad(this->keyframes[0][i]),zhRad(this->keyframes[1][i]),zhRad(this->keyframes[2][i]),this->channelOrder));
				}
			}
			for(int i = 0;i < children.size();++i){
				children[i] -> ConvertToAnimation(mAnim);
			}
			return true;
		}
	};
	void resetParsingData(){
		channelToKeyFrame.clear();
		totalChannels = 0;
		JointInfo::resetID();
	}
	EulerRotOrder ParseChannelOrder(JointInfo* joint,string channels){
		boost::cmatch what;
		string channel[3];
		int offset = 0;
		if(joint ->numberOfChannels == 6){
			boost::regex_search(channels.c_str(), what, boost::regex(channelReg6));
			channelToKeyFrame.push_back(&joint->keyframes[0]);
			channelToKeyFrame.push_back(&joint->keyframes[1]);
			channelToKeyFrame.push_back(&joint->keyframes[2]);
			channel[0].assign(what[4].first,what[4].second);
			channel[1].assign(what[5].first,what[5].second);
			channel[2].assign(what[6].first,what[6].second);
			offset = 3;
		}else if(joint ->numberOfChannels == 3){
			boost::regex_search(channels.c_str(), what, boost::regex(channelReg3));
			channel[0].assign(what[1].first,what[1].second);
			channel[1].assign(what[2].first,what[2].second);
			channel[2].assign(what[3].first,what[3].second);
		}
		int order = 0;
		int tmp = std::string("Xrotation").compare("Xrotation");
		for(int i = 0;i < 3;++i){
			if(channel[i].compare("Xrotation") == 0){
				channelToKeyFrame.push_back(&joint->keyframes[0 + offset]);
				order = order*10 + 1;
			}else if(channel[i].compare("Yrotation") == 0){
				channelToKeyFrame.push_back(&joint->keyframes[1 + offset]);
				order = order*10 + 2;
			}else if(channel[i].compare("Zrotation") == 0){
				channelToKeyFrame.push_back(&joint->keyframes[2 + offset]);
				order = order*10 + 3;
			}
		}
		return EulerRotOrder(order);
	}
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
			
			
			totalChannels += joint->numberOfChannels;
			joint->channelOrder = ParseChannelOrder(joint,jointContent);
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
	float findNextData(const char* &data){
		string result = "";
		while(data[0] == ' ' || data[0] == '\t' || data[0] == '\n' || data[0] == '\r'){
			data++;
		}
		while((data[0] <= '9' && data[0] >= '0') || data[0] == '.' || data[0] == '-' || data[0] == 'e'){
			result.insert(result.end(),1,*data++);
		}

		return std::atof(result.c_str());
	}
	bool parseMotion(std::string & motionContent){
	
		boost::cmatch what;
		boost::regex_search(motionContent.c_str(), what, boost::regex(nFramesReg));
		std::string nFrameString;
		nFrameString.assign(what[1].first, what[1].second);
		nFrames = std::atoi(nFrameString.c_str());
	
		boost::regex_search(what.suffix().first, what, boost::regex(frameTimeReg));
		std::string frameTimeString;
		frameTimeString.assign(what[1].first, what[1].second);
		frameTime = std::atof(frameTimeString.c_str());

		//printf("%s",motionContent.c_str());
		const char* pointer = what.suffix().first;
		for(int f = 0;f < nFrames;++f){
			std::string keyFrameString;
			for(int i = 0;i < totalChannels;++i){
				channelToKeyFrame[i]->push_back(findNextData(pointer));
			}
		}
		return true;
	}

	/*bool parseMotion(std::string & motionContent){
	
		boost::cmatch what;
		boost::regex_search(motionContent.c_str(), what, boost::regex(nFramesReg));
		std::string nFrameString;
		nFrameString.assign(what[1].first, what[1].second);
		nFrames = std::atoi(nFrameString.c_str());
	
		boost::regex_search(what.suffix().first, what, boost::regex(frameTimeReg));
		std::string frameTimeString;
		frameTimeString.assign(what[1].first, what[1].second);
		frameTime = std::atof(frameTimeString.c_str());

		//printf("%s",motionContent.c_str());
		boost::regex_search(what.suffix().first, what, boost::regex(keyFrameReg));

		for(int f = 0;f < nFrames;++f){
			std::string keyFrameString;
			for(int i = 0;i < totalChannels;++i){
				keyFrameString.assign(what[1].first, what[1].second);
				channelToKeyFrame[i]->push_back(std::atof(keyFrameString.c_str()));
				boost::regex_search(what.suffix().first, what, boost::regex(keyFrameReg));
			}
		}
		return true;
	}*/
	bool BVHLoader::tryLoad( ResourcePtr res, const std::string& path )
	{
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
		resetParsingData();

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
		parseMotion(motionContent);

		mAnim = mAnimSet->createAnimation(0, mAnimSet->getName());
		root -> ConvertToAnimation(mAnim);
		return true;
	}
}
