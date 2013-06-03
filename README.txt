****************************
ZombieHorse Animation System
****************************

ZombieHorse is an animation system and testbed written in C++.

OBTAINING AND COMPILING THE SYSTEM

Source code of the system can be obtained from the following public repository:

	https://github.com/tpejsa/ZombieHorse
	
The system can be compiled using Visual Studio 2010. The solution file ZombieHorse.sln can be found in the repository root.

The following external libraries and SDKs are required to compile and use the software:
	
	boost
	wxWidgets 2.8
	OGRE 1.8 SDK

HOME variables must be defined for all libraries and set to their root directions (BOOST_HOME, WXWIDGETS_HOME, and OGRE_HOME).

RUNNING THE TESTBED

Once the software is compiled, you can try out the animation testbed by running bin\release\AnimationStudio.exe. The testbed application will automatically load any BVH motions found in samples\data\animations and calculate the reduced frameset. It will then output two files in bin\release, both containing the training data for GPLVM:

	TrainSet.csv - for loading into MATLAB and such
	TrainSet.svml - for use with the GPLVM tool (gplvm.exe, found in bin\release)

Selected representative poses can be seen in the Animation Studio application. They are listed in the project view on the left-hand side, and have names like "RFrame0." To view a pose, simply double-click it, and then click Play.

EXAMPLE OUTPUT

Examples of training data output, as well as pictures of selected poses, are submitted along with this README file. The data comes from a small dataset containing reaching, martial arts, and high-fiving motions.

LICENSE

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