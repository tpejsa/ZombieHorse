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

#ifndef __zhLogger_h__
#define __zhLogger_h__

#include "zhPrereq.h"
#include "zhSingleton.h"
#include "zhTimer.h"

namespace zh
{

#if zhLog_Enabled
	#define zhLog(className,funcName,message,...) ( Logger::Instance()->write(className,funcName,message,__VA_ARGS__) )
#else
	#define zhLog(className,funcName,message,...)
#endif

/**
* Simple class for logging events, errors, warnings etc.
*/
class zhDeclSpec Logger : public Singleton<Logger>
{

	friend class Singleton<Logger>;

private:

	/**
	* Constructor.
	*/
	Logger();

	/**
	* Destructor.
	*/
	~Logger();

public:

	/**
	* Writes an entry into the log.
	*/
	void write( const char* className, const char* funcName, const char* msg, ... );

private:

	FILE* mLogfile;
	Timer mTmr;

};

}

#endif // __zhLogger_h__
