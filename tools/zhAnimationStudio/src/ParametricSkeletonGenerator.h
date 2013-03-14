#include "zhBoneAnimationTrack.h"
#ifndef __ParametricSkeletonGenerator_h__
#define __ParametricSkeletonGenerator_h__
enum BonePairClass{
	BODY = 0,
	ARMLEFT = 11,
	ARMRIGHT = 12,
	LEGLEFT = 21,
	LEFRIGHT = 22,
	LEFT = 1,
	RIGHT = 2,
	ARM = 10,
	LEG = 20
};
struct SkeletonParameters{
	double scaleGlobal;
	double scaleBody;
	double scaleArmLeft;
	double scaleArmRight;
	double scaleLegLeft;
	double scaleLegRight;
	SkeletonParameters(){
		scaleGlobal = 1;
		scaleBody = 1;
		scaleArmLeft = 1;
		scaleArmRight = 1;
		scaleLegLeft = 1;
		scaleLegRight = 1;
	}
	double operator[](BonePairClass classID){
		switch(classID){
		case BODY:
			return scaleBody;
		case ARMLEFT:
			return scaleArmLeft;
		case ARMRIGHT:
			return scaleArmRight;
		case LEGLEFT:
			return scaleLegLeft;
		case LEFRIGHT:
			return scaleLegRight;
		default:
			return 1;
		}
	};
};
bool ParametricSkeletonGenerator(zh::Skeleton* skelOut,zh::Skeleton* templateSkel,SkeletonParameters parameters);
#endif