#include "zhSkeleton.h"
#include "zhBone.h"
#include "ParametricSkeletonGenerator.h"
#include "cstring"
#include "boost\regex.hpp"
std::string leftReg("((L|l)(E|e)(F|f)(T|t))|(l|L)");
std::string rightReg("((R|r)(I|i)(G|g)(H|h)(T|t))|(r|R)");
std::string armJointsReg("((E|e)lbow|(W|w)rist|(H|h)and|(F|f)ingers|(T|t)humb)");
std::string bodyTagReg("((H|h)ip|(B|b)ack)|(C|c)hest|(N|n)eck|(H|h)ead)");
BonePairClass ClassifyBonePair(zh::Bone* parent,zh::Bone* child){

	return BODY;
}
bool ParametricSkeletonGenerate(zh::Bone* currentOut,zh::Bone* currentTemplate,zh::Skeleton* skelOut,SkeletonParameters parameters){
	for(zh::Bone::ChildIterator i = currentTemplate->getChildIterator();!i.end();){
		zh::Bone* childTemplate = i.next();
		zh::Bone* childOut = skelOut -> createBone(childTemplate -> getId(),childTemplate -> getName());
		double scale = parameters.scaleGlobal * parameters[ClassifyBonePair(currentTemplate,childTemplate)];
		childOut -> setInitialPosition(childTemplate ->getInitialPosition() * scale);
		childOut -> setInitialOrientation(childTemplate ->getInitialOrientation());
		currentOut -> addChild(childOut);
		ParametricSkeletonGenerate(childOut,childTemplate,skelOut,parameters);
	}
	return true;
}
bool ParametricSkeletonGenerator(zh::Skeleton* skelOut,zh::Skeleton* templateSkel,SkeletonParameters parameters){
	zh::Bone* rootTemplate = templateSkel -> getRoot();
	zh::Bone* rootOut = skelOut->createBone(rootTemplate->getId(),rootTemplate->getName());
	rootOut ->setInitialOrientation(rootTemplate->getInitialOrientation());
	rootOut ->setInitialPosition(rootTemplate->getInitialPosition());
	return ParametricSkeletonGenerate(rootOut,rootTemplate,skelOut,parameters);
}
