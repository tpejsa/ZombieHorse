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

#ifndef __zhAnimationDatabaseEvents_h__
#define __zhAnimationDatabaseEvents_h__

#include "zhPrereq.h"
#include "zhEvent.h"
#include "zhMatchGraph.h"

namespace zh
{

class AnimationDatabaseSystem;

class zhDeclSpec MatchWebBuiltEvent : public Event<MatchWebBuiltEvent>
{

public:

	/**
	* Constructor.
	*
	* @param mw Match web.
	*/
	MatchWebBuiltEvent( AnimationDatabaseSystem* system, MatchWeb* mw );

	/**
	* Gets the match web that has been built.
	*/
	virtual MatchWeb* getMatchWeb() const;

protected:

	MatchWeb* mMW;

};

class zhDeclSpec MatchFoundEvent : public Event<MatchFoundEvent>
{

public:

	/**
	* Constructor.
	*
	* @param match Animation search match.
	*/
	MatchFoundEvent( AnimationDatabaseSystem* system, MatchGraph::Node* match );

	/**
	* Gets the animation search match that has been found.
	*/
	virtual MatchGraph::Node* getMatch() const;

protected:

	MatchGraph::Node* mMatch;

};

}

#endif // __zhAnimationDatabaseEvents_h__
