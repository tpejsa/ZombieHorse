#include "zhBone.h"
#include <map>
#include <vector>
#include <cstring>
#ifndef __ParametricSkeletonGenerator_h__
#define __ParametricSkeletonGenerator_h__
struct BoneGroup{
	std::string Name;
	std::vector<std::pair<zh::BoneTag,zh::BoneTag>> BoneChains;//First is the start chain tag,Second is the end chain tag
	bool operator==(const BoneGroup& b)const{
		return this->Name == b.Name;
	};
	bool operator>(const BoneGroup& b)const{
		return this->Name > b.Name;
	};
	bool operator<(const BoneGroup& b)const{
		return this->Name < b.Name;
	};
	bool contain(zh::Bone* bone)const{
		for(std::vector<std::pair<zh::BoneTag,zh::BoneTag>>::const_iterator i = BoneChains.begin();i != BoneChains.end();++i){
			if(searchChain(bone,*i)){
				return true;
			}
		}
		return false;
	}
	void getChain(zh::Bone* bone,std::pair<zh::BoneTag,zh::BoneTag>,std::vector<zh::Bone*>& chain)const{
		//use dijkstra algorithm to calculate the chain between two arbirary bones.
		//TODO!!!!!!
		return;
	};
	bool searchChain(zh::Bone* bone,std::pair<zh::BoneTag,zh::BoneTag>)const{
		//Current version the begining must be an ancester of the end
		return true;
	}
};
struct SkeletonParameters{
	double scaleGlobal;
	std::map<zh::BoneTag,double> tagScales;
	std::map<BoneGroup,double> groupScales;
	SkeletonParameters(){
		tagScales.clear();
		groupScales.clear();
		scaleGlobal = 1.0;
	}
	double operator[](zh::Bone* bone)const{
		double groupScales = 1.0;
		for(std::map<BoneGroup,double>::const_iterator iter = this -> groupScales.begin(); iter != this -> groupScales.end(); ++iter){
			if(iter->first.contain(bone)){
				groupScales *= iter->second;
			}
		}
		return 1.0 * groupScales;
	};
	double operator[](const BoneGroup& groupName)const{
		std::map<BoneGroup,double>::const_iterator result = groupScales.find(groupName);
		if(result != groupScales.end()){
			return result->second;
		}
		return 1.0;
	};
	/*double operator[](const zh::BoneTag& BoneTag)const{
		double groupScales = 1.0;
		for(std::unordered_map<BoneGroup,double>::iterator iter = groupScales.begin(); iter != groupScales.end(); ++iter){
			if(iter->first->contain(BoneTag)){
				groupScales *= iter->second;
			}
		}
		std::unordered_map<zh::BoneTag,double>::const_iterator result = tagScales.find(BoneTag);
		if(result != tagScales.end()){
			return result->second * groupScales;
		}
		return 1.0 * groupScales;
	};*/
};
/*The two skeleton must have the same structure*/
bool ParametricSkeletonGenerator(zh::Skeleton* skelOut,zh::Skeleton* templateSkel,SkeletonParameters parameters);
#endif