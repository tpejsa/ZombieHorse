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

#include "zhPostureIKSolver.h"
#include"zhBone.h"
#include"zhSkeleton.h"

namespace zh
{

	PostureIKSolver::PostureIKSolver()
	{
		initSolve = true;
		initStepSize = 10;
		gredientThreshold = 100;//defines the threshold to stop iteration
		maxIteration = 30;
		countGoal = 0;
		countBone = 0;
		step = 0.0001;
	}

	PostureIKSolver::~PostureIKSolver()
	{
	}
	void PostureIKSolver::solve()
	{
		lineSearchStepSize = initStepSize;
		if(initSolve){
			initSolve = false;
			GoalConstIterator gi = getGoalConstIterator();
			while(!gi.end()){
				gi.next();
				countGoal++;
			}
			zh::Skeleton::BoneIterator bi = mSkel -> getBoneIterator();
			while(!bi.end()){
				bi.next();
				countBone++;
			}
			gredient.setDimension(countBone * 3 + 3);
		}
		snapshot(0);
		/*zh::Skeleton::BoneIterator bi = mSkel->getBoneIterator();
		if(!bi.end()){
			Bone* bone = bi.next();
			//bone ->setPosition(zh::Vector3(100,0,0));
			//bone->rotate(Quat(3.14/4,3.14/4,3.14/4,3.14/4).normalize());
			Quat tmp(0,0,0,3.14/4);
			tmp = tmp.exp();
			tmp = tmp.normalize();
			bone ->setOrientation(bone->getOrientation()*tmp);
		}*/
		//restoreSnapshot(1);
		/*while(!bi.end()){
			Bone* bone = bi.next();
			Quat tmp(0,0,0,3.14/4);
			bone ->setOrientation(bone->getOrientation()*tmp.exp());
		}*/
		//restoreSnapshot(1);
		GD();
		zh::Skeleton::BoneIterator bi = mSkel -> getBoneIterator();
		int counter = 0;
		if(!bi.end()){
			Bone* bone = bi.next();
			if(!this ->findBone(bone->getId())){
				bone ->setPosition(shot[0].rootPosition);
				bone ->setOrientation(shot[0].rotation[counter]);
			}
			counter++;
		}
		while(!bi.end()){
			Bone* bone = bi.next();
			if(!this ->findBone(bone->getId())){
				bone ->setOrientation(shot[0].rotation[counter]);
			}
			counter++;
		}
	}
	void PostureIKSolver::restoreSnapshot(int index){
		zh::Skeleton::BoneIterator bi = mSkel -> getBoneIterator();
		int counter = 0;
		if(!bi.end()){
			Bone* bone = bi.next();
			bone ->setPosition(shot[index].rootPosition);
			bone ->setOrientation(shot[index].rotation[counter++]);
		}
		while(!bi.end()){
			Bone* bone = bi.next();
			bone ->setOrientation(shot[index].rotation[counter++]);
		}	
	}
	void PostureIKSolver::snapshot(int index){
		shot[index].rotation.clear();
		zh::Skeleton::BoneIterator bi = mSkel -> getBoneIterator();
		if(!bi.end()){
			Bone* bone = bi.next();
			shot[index].rootPosition = bone->getPosition();
			shot[index].rotation.push_back(bone -> getOrientation());
		}
		while(!bi.end()){
			Bone* bone = bi.next();
			shot[index].rotation.push_back(bone -> getOrientation());
		}
	}
	void PostureIKSolver::CGIteration(){
		
	}
	void PostureIKSolver::applyConfiguration(VectorXD& config){
		zh::Skeleton::BoneIterator bi = mSkel -> getBoneIterator();
		int counter = 2;
		if(!bi.end()){
				Bone* bone = bi.next();
				bone -> setPosition(bone ->getPosition() + Vector3(config.values[0],config.values[1],config.values[2]));
				bone -> setOrientation(bone -> getOrientation() * Quat(0,config.values[3],config.values[4],config.values[5]).exp());
		}
		while(!bi.end()){
				Bone* bone = bi.next();
				bone -> setOrientation(bone -> getOrientation() * Quat(0,config.values[counter * 3],config.values[counter * 3 + 1],config.values[counter * 3 + 2]).exp());
				counter++;
		}
	}
	double PostureIKSolver::lineSearch(int index, VectorXD& direction){
		double energyO = energy();
		direction * lineSearchStepSize;
		do{
			//try step
			applyConfiguration(direction);
			double tmp = energy();
			if(tmp < energyO || lineSearchStepSize < step){
				return lineSearchStepSize;
			}
			restoreSnapshot(index);
			lineSearchStepSize /= 2;
			direction * 0.5;
		}while(1);
	}
	void PostureIKSolver::GD(){
		for(int i = 0;i < maxIteration;++i){
			computeGredient();
			double tmp = gredient.length();
			if(tmp > this->gredientThreshold && lineSearchStepSize > step){
				snapshot(1);
				lineSearch(1, gredient.revert());
			}else{
				break;
			}
		}
	}
	double PostureIKSolver::energy()const{
		//currently this only have the first part of the equation(distance matric)
		GoalConstIterator gi = getGoalConstIterator();
		double energy = 0;
		while(!gi.end()){
			IKGoal goal = gi.next();
			Vector3 tmp = mSkel->getBone(goal.boneId)->getWorldPosition();
			energy += goal.weight*goal.position.distanceSq(mSkel->getBone(goal.boneId)->getWorldPosition());
		}
		return energy;
	}
	void PostureIKSolver::computeGredient(){
		double energyO = energy();
		
		gredient[0] = 0;
		gredient[1] = 0;
		gredient[2] = 0;
		
		zh::Skeleton::BoneIterator bi = mSkel -> getBoneIterator();
		int boneCounter = 0;
		//gredient for root position
		/*GoalConstIterator gi = getGoalConstIterator();
		while(!gi.end()){
			IKGoal goal = gi.next();
			gredient[0] += 2 * (mSkel->getBone(goal.boneId)->getWorldPosition() - goal.position).x;
			gredient[1] += 2 * (mSkel->getBone(goal.boneId)->getWorldPosition() - goal.position).y;
			gredient[2] += 2 * (mSkel->getBone(goal.boneId)->getWorldPosition() - goal.position).z;
		}*/

		bi = mSkel -> getBoneIterator();
		Bone* bone = bi.next();
		Vector3 snap = bone ->getPosition();
		bone -> setPosition(snap + Vector3(step,0,0));
		gredient[0] = (energy() - energyO) / step;
		
		bone -> setPosition(snap + Vector3(0,step,0));
		gredient[1] = (energy() - energyO) / step;
		
		bone -> setPosition(snap + Vector3(0,0,step));
		gredient[2] = (energy() - energyO) / step;
		bi = mSkel -> getBoneIterator();
		bone -> setPosition(snap);

		while(!bi.end()){
				Bone* bone = bi.next();
				Quat snap = bone -> getOrientation();
				//x rotation
				bone -> setOrientation(snap*(Quat(0,step,0,0).exp()));
				gredient[boneCounter * 3 + 3] = (energy() - energyO) / step;
				//y rotation
				bone -> setOrientation(snap*(Quat(0,0,step,0).exp()));
				gredient[boneCounter * 3 + 4] = (energy() - energyO) / step;
				//z rotation
				bone -> setOrientation(snap*(Quat(0,0,0,step).exp()));
				gredient[boneCounter * 3 + 5] += (energy() - energyO) / step;
				//reset orientation
				bone -> setOrientation(snap);
				boneCounter++;
		}
	}
}
