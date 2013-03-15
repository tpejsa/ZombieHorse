#include "zhSkeleton.h"
#include "zhBone.h"
#include "ParametricSkeletonGenerator.h"
#include "cstring"
#include "boost\regex.hpp"
bool ParametricSkeletonScale(zh::Bone* currentOut,zh::Bone* currentTemplate,SkeletonParameters parameters){
	for(zh::Bone::ChildIterator i = currentTemplate->getChildIterator(),
		j = currentOut->getChildIterator();!i.end();){
		zh::Bone* childTemplate = i.next();
		zh::Bone* childOut = j.next();
		double scale = parameters.scaleGlobal * parameters[currentTemplate/*tagHere!!!*/];
		childOut -> setInitialPosition(childTemplate ->getInitialPosition() * scale);
		ParametricSkeletonScale(childOut,childTemplate,parameters);
	}
	return true;
}
bool ParametricSkeletonGenerator(zh::Skeleton* skelOut,zh::Skeleton* templateSkel,SkeletonParameters parameters){
	zh::Bone* rootTemplate = templateSkel -> getRoot();
	zh::Bone* rootOut = skelOut->getRoot();
	return ParametricSkeletonScale(rootOut,rootTemplate,parameters);
}
