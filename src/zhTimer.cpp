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

#include "zhTimer.h"
#include "zhString.h"
#include "zhMathMacros.h"

#if zhPlatform == zhPlatform_Win
#include <windows.h>
#endif

namespace zh
{

Timer::Timer()
{
	mTime0 = 0;
	mStopped = true;
}

void Timer::start()
{
	mTime0 = GetTime();
	mStopped = false;
}

void Timer::stop()
{
	mStopped = true;
}

void Timer::reset()
{
	mTime0 = 0;
	if( mStopped == false )
		start();
}

bool Timer::isRunning() const
{
	return !mStopped;
}

Timer::Time Timer::getTime() const
{
	if( mTime0 <= 0 )
		return Timer::Time();

	Timer::Time ct;
	double dtime = ( Timer::GetTime() - mTime0 ) * 1000000.0; // elapsed time in microseconds
	ct.hr = ( unsigned long )( dtime / 3600000000.0 );
	ct.min = ( unsigned long )( dtime / 60000000.0 - ct.hr * 60.0 );
	ct.s = ( unsigned long )( dtime / 1000000.0 - ct.hr * 3600.0 - ct.min * 60.0 );
	ct.ms = ( unsigned long )( dtime / 1000.0 - ct.hr * 3600000.0 - ct.min * 60000.0 - ct.s * 1000.0 );
	ct.us = ( unsigned long )( dtime - ct.hr * 3600000000.0 - ct.min * 60000000.0 - ct.s * 1000000.0 - ct.ms * 1000.0 );

	return ct;
}

std::string Timer::getTimeStr() const
{
	Timer::Time ct = getTime();

	return toString<unsigned long>( ct.hr ) + ":" +
		toString<unsigned long>( ct.min ) + ":" +
		toString<unsigned long>( ct.s ) + "." +
		toString<unsigned long>( ct.ms ) + "." + 
		toString<unsigned long>( ct.us );
}

Int64 Timer::GetTickCount()
{
	Int64 t;

	#if zhPlatform == zhPlatform_Win
	QueryPerformanceCounter( ( LARGE_INTEGER* )&t );
	#endif

	return t;
}

Int64 Timer::GetTickFreq()
{
	Int64 f;

	#if zhPlatform == zhPlatform_Win
	QueryPerformanceFrequency( ( LARGE_INTEGER* )&f );
	#endif

	return f;
}

double Timer::GetTime()
{
	return ( (double)GetTickCount() ) / GetTickFreq();
}

}
