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

#include "zhLogger.h"

// C++
#include <cstdarg>

namespace zh
{

Logger::Logger()
{
	mLogfile = fopen( zhLogFile_Path, "w" );
	mTmr.start();
}

Logger::~Logger()
{
	if( mLogfile != NULL )
		fclose(mLogfile);
}

void Logger::write( const char* className, const char* funcName, const char* msg, ... )
{
	if( mLogfile == NULL )
		return;

	fputs( mTmr.getTimeStr().c_str(), mLogfile );
	fputs( "\t", mLogfile );
	fputs( className, mLogfile );
	fputs( "::", mLogfile );
	fputs( funcName, mLogfile );
	fputs( "()\t", mLogfile );

	va_list args;
	va_start( args, msg );
	vfprintf( mLogfile, msg, args );
	va_end(args);

	fputs( "\n", mLogfile );
	fflush(mLogfile);
}

}
