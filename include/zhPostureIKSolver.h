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

#ifndef __zhPostureIKSolver_h__
#define __zhPostureIKSolver_h__

#include "zhPrereq.h"
#include "zhIKSolver.h"
#include <vector>
namespace zh
{
	/**
	* @brief Class representing an IK solver for positioning the figure root.
	*/
	class zhDeclSpec PostureIKSolver : public IKSolver
	{

	public:

		zhDeclare_IKSolver( PostureIKSolver, zhPostureIKSolver_ClassId, zhPostureIKSolver_ClassName )

		/**
		* Constructor.
		*/
		PostureIKSolver();

		/**
		* Destructor.
		*/
		~PostureIKSolver();

		/**
		* Compute the skeletal pose that meets the current IK goals.
		*/
		void solve();

	protected:
		struct VectorXD{//high dimensional vector
			int dimension;
			vector<double> values;
			void add(VectorXD v2){
				if(dimension == v2.dimension){
					for(int i = 0;i < dimension;++i){
						values[i] += v2.values[i];
					}
				}
			}
			double& operator[](int index){
				return values[index];
			}
			void setDimension(int d){
				dimension = d;
				values.resize(d);
			}
			void subtract(VectorXD v2){
				if(dimension == v2.dimension){
					for(int i = 0;i < dimension;++i){
						values[i] -= v2.values[i];
					}
				}
			}
			VectorXD& revert(){//revert the value,  used to find the negative gredient
				for(int i = 0;i < dimension;++i){
					values[i] = -values[i];
				}
				return *this;
			}
			VectorXD& operator*(double c){
				for(int i = 0;i < dimension;++i){
					values[i] *= c;
				}
				return *this;
			}
			double length(){
				double length = 0;
				for(int i = 0;i < dimension;++i){
					length += values[i]*values[i];
				}
				return sqrt(length);
			}
		};
		int countGoal;
		int countBone;
		bool initSolve;
		VectorXD gredient;
		void CGIteration();//The conjegate gredient iteration
		double initStepSize;
		double gredientThreshold;//defines the threshold to stop iteration
		int maxIteration;
		double lineSearchStepSize;
		double lineSearch(int index, VectorXD& direction);//search argmin along that direction, the index is the index of the snapshot of the start point
		//the step is applied after the line search
		void applyConfiguration(VectorXD& config);//apply the current configuration to the skeleton
		void GD();//The gredient decent
		double energy()const;//Masures the energy function of te current configuration
		void computeGredient();
		void snapshot(int index);//take a snapshot of current joint configuration save it in shot[index];
		void restoreSnapshot(int index);
		struct Snapshot{
			Vector3 rootPosition;
			vector<Quat> rotation;
		};
		Snapshot shot[2];
	};
}
#endif // __zhPostureIKSolver_h__