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

#ifndef __zhTimer_h__
#define __zhTimer_h__

#include "zhPrereq.h"

using namespace std;

namespace zh
{

/**
* Platform-independent timer with microsecond precision.
*/
class zhDeclSpec Timer
{

public:

	struct Time
	{
		unsigned long hr;
		unsigned long min;
		unsigned long s;
		unsigned long ms;
		unsigned long us;

		Time()
		{
			hr = min = s = ms = us = 0;
		}
	};

	Timer(); ///< Constructor.
	void start(); ///< Starts the timer.
	void stop(); ///< Stops the timer.
	void reset(); ///< Resets the timer.
	bool isRunning() const; ///< Returns true if the timer is running, otherwise false.
	Timer::Time getTime() const; ///< Gets elapsed time.
	std::string getTimeStr() const; ///< Gets elapsed time as string.
	
	static Int64 GetTickCount(); ///< Gets the number of clock ticks since system started.
	static Int64 GetTickFreq(); ///< Gets the system clock frequency.
	static double GetTime(); ///< Gets the time (in seconds) since system started.

private:

	double mTime0;
	bool mStopped;

};

}

#endif // __zhTimer_h__
