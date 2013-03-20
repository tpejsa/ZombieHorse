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

#include "zhAnimationAdaptor.h"
#include "zhAnimationNode.h"
#include "zhAnimationSystem.h"

namespace zh
{

AnimationAdaptor::AnimationAdaptor( Skeleton* origSkel, AnimationNode* animNode ) :
mOrigSkel(origSkel), mAnimNode(animNode), mPredFact(0.f)
{
	zhAssert( animNode != NULL );

	// TODO: add functions for defining end-effectors of interest
	mEndEffectors.push_back(BT_LWrist);
	mEndEffectors.push_back(BT_RWrist);
	mEndEffectors.push_back(BT_LAnkle);
	mEndEffectors.push_back(BT_RAnkle);
}

AnimationAdaptor::~AnimationAdaptor()
{
}

Skeleton* AnimationAdaptor::getOriginalSkeleton() const
{
	return mOrigSkel;
}

AnimationNode* AnimationAdaptor::getAnimationNode() const
{
	return mAnimNode;
}

float AnimationAdaptor::getPredictionFactor() const
{
	return mPredFact;
}

void AnimationAdaptor::setPredictionFactor( float predFact )
{
	mPredFact = predFact >= 0 ? predFact : 0;
}

void AnimationAdaptor::adapt( Skeleton* targetSkel )
{
	zhAssert( targetSkel != NULL );

	// Set initial pose estimate
	Skeleton::BoneIterator bone_i = mOrigSkel->getBoneIterator();
	while( bone_i.hasMore() )
	{
		Bone* orig_bone = bone_i.next();
		Bone* trg_bone = targetSkel->getBone( orig_bone->getName() );
		if( trg_bone == NULL ) continue;
		if( trg_bone == targetSkel->getRoot() )
			trg_bone->setPosition( orig_bone->getPosition() );
		trg_bone->setOrientation( orig_bone->getOrientation() );
	}
	
	_computeIKGoals(targetSkel);
	targetSkel->solveIK();
}

void AnimationAdaptor::_computeIKGoals( Skeleton* targetSkel ) const
{
	for( std::vector<BoneTag>::const_iterator eei = mEndEffectors.begin();
			eei != mEndEffectors.end(); ++eei )
	{
		BoneTag eetag = *eei;
		if( !mOrigSkel->hasBoneWithTag(eetag) || !targetSkel->hasBoneWithTag(eetag) )
			continue;

		Bone* orig_ee = mOrigSkel->getBoneByTag(eetag);
		float weight = _computeIKGoalWeight(orig_ee);
		IKGoal goal( targetSkel->getBoneByTag(eetag)->getId(),
			orig_ee->getWorldPosition(), weight );

		Skeleton::IKSolverIterator solver_i = targetSkel->getIKSolverIterator();
		while( solver_i.hasMore() )
			solver_i.next()->setGoal(goal);
	}
}

float AnimationAdaptor::_computeIKGoalWeight(Bone* endEff) const
{
	float wmax = 0;

	// Compute weight based on proximity to environment objects
	Skeleton* env = zhAnimationSystem->getEnvironment();
	Bone* env_root = env->getRoot();
	Bone::ChildConstIterator obj_i = env_root->getChildConstIterator();
	while( obj_i.hasMore() )
	{
		Bone* obj = obj_i.next();
		float dist = _computeEnvObjDistance( endEff, obj );
		float dist2 = dist*dist;
		float w = dist < 1 ? ( 2.f*dist2*dist - 3.f*dist2 + 1.f ) : 0;
		wmax = std::max( w, wmax );
	}

	// Also check proximity to ground
	float dist = _computeGroundDistance(endEff);
	float dist2 = dist*dist;
	float w = dist < 1 ? ( 2.f*dist2*dist - 3.f*dist2 + 1.f ) : 0;
	wmax = std::max( w, wmax );

	return wmax;
}

float AnimationAdaptor::_computeEnvObjDistance( Bone* endEff, Bone* envObj ) const
{
	// Compute current distance
	float dist = endEff->getWorldPosition().distance( envObj->getWorldPosition() );

	// Incorporate predicted distance
	float d_dist = 0;
	float dt = mAnimNode->_getDeltaTime();
	std::map<std::pair<unsigned short, unsigned short>, float>::iterator pdist_i =
		mPrevEnvObjDist.find( make_pair(endEff->getId(), envObj->getId()) );
	if( pdist_i != mPrevEnvObjDist.end() && dt > .00001f )
		// Estimate derivative of distance
		d_dist = ( dist - pdist_i->second )/dt;
	mPrevEnvObjDist[ make_pair(endEff->getId(), envObj->getId()) ] = dist; // Store current distance
	dist += mPredFact*d_dist;
	// TODO: fix prediction - frame rate needs to be constant to get good 1st derivative estimates

	// Normalize distance
	// TODO: there should be some reasonable and principled way of specifying
	// the normalization factor, but this will do for now
	float dnorm = 5.f;
	//
	dist /= dnorm;

	return dist;
}

float AnimationAdaptor::_computeGroundDistance(Bone* endEff) const
{
	// Compute current distance
	Vector3 ee_wpos = endEff->getWorldPosition();
	float dist = fabs( ee_wpos.y - zhAnimationSystem->getGroundHeightAt( ee_wpos.x, ee_wpos.z ) );

	// Incorporate predicted distance
	float d_dist = 0;
	float dt = mAnimNode->_getDeltaTime();
	std::map<unsigned short, float>::iterator pdist_i =
		mPrevGroundDist.find(endEff->getId());
	if( pdist_i != mPrevGroundDist.end() && dt > .00001f )
		// Estimate derivative of distance
		d_dist = ( dist - pdist_i->second )*60;
	mPrevGroundDist[endEff->getId()] = dist; // Store current distance
	dist += mPredFact*d_dist;
	// TODO: fix prediction - frame rate needs to be constant to get good 1st derivative estimates

	// Normalize distance
	// TODO: there should be some reasonable and principled way of specifying
	// the normalization factor, but this will do for now
	float dnorm = 5.f;
	//
	dist /= dnorm;

	return dist;
}

}
