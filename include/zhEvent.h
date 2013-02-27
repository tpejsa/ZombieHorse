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

/**
* @file zhEvent.h
* @brief Generic, type-safe event system.
*/

#ifndef __zhEvent_h__
#define __zhEvent_h__

#include "zhPrereq.h"
#include "zhFunctor.h"

namespace zh
{

template <class Evt>
class EventEmitter;

template <class Evt>
class EventListener;

/**
* @brief Event class template.
*/
template <class Evt>
class Event
{

public:

	/**
	* Constructor.
	*
	* @param emitter Pointer to the emitting object for this event.
	*/
	Event( EventEmitter<Evt>* emitter )
	{
		zhAssert( emitter != NULL );

		mEmitter = emitter;
	}

	/**
	* Gets the emitter for this event.
	*
	* @return Pointer to the emitting object.
	*/
	virtual EventEmitter<Evt>* getEmitter() const
	{
		return mEmitter;
	}

	/**
	* Emits the event.
	*/
	virtual void emit()
	{
		mEmitter->_emitEvent( (const Evt&)*this );
	}

protected:

	EventEmitter<Evt>* mEmitter;

};

/**
* @brief Event handler functor class template.
*
* This functor is used to implement event handler callbacks.
*/
template <class Evt>
class EvtHFunctor : public Functor1< void, const Evt& >
{

public:

	typedef void(EventListener<Evt>::*EventHandler)( const Evt& );

	/**
	* Constructor.
	*/
	EvtHFunctor()
	{
		mListener = NULL;
		mHandler = NULL;
	}

	/**
	* Constructor. Maps the constructed functor to specified event handler.
	* 
	* @param evtL Event listener object.
	* @param evtH Event handler (member function of evtL).
	*/
	EvtHFunctor( EventListener<Evt>* evtL, EventHandler evtH )
	{
		mListener = evtL;
		mHandler = evtH;
	}

	void operator()( const Evt& evt )
	{
		(*mListener.*mHandler)( evt );
	}

	void call( const Evt& evt )
	{
		(*mListener.*mHandler)( evt );
	}

private:

	EventListener<Evt>* mListener;
	EventHandler mHandler;

};

/**
* @brief Event listener class template.
*/
template <class Evt>
class EventListener
{

public:

	/**
	* Constructor.
	*/
	EventListener()
	{
		mHFunctor = EvtHFunctor<Evt>( this, &EventListener::handleEvent );
	}

	/**
	* Destructor.
	*/
	virtual ~EventListener()
	{
		for( std::list< EventEmitter<Evt>* >::iterator emIt = mEmitters.begin();
			emIt != mEmitters.end(); ++emIt )
			(*emIt)->unsubscribeListener( mHFunctor );
	}
	
	/**
	* Subscribes the listener to events from the specified emitter.
	*/
	virtual void subscribe( EventEmitter<Evt>* emPtr )
	{
		zhAssert( emPtr != NULL );

		mEmitters.push_back(emPtr);
		emPtr->subscribeListener( mHFunctor );
	}

	/**
	* Unsubscribes the listener from events from the specified emitter.
	*/
	virtual void unsubscribe( EventEmitter<Evt>* emPtr )
	{
		zhAssert( emPtr != NULL );

		emPtr->unsubscribeListener( mHFunctor );
		mEmitters.remove(emPtr);
	}

	/**
	* Event handler for events of a specific class.
	* Derived event listener classes should implement event handling
	* functionality.
	*/
	virtual void handleEvent( const Evt& evt ) = 0;

protected:

	std::list< EventEmitter<Evt>* > mEmitters;
	EvtHFunctor<Evt> mHFunctor;

};

/**
* @brief Event emitter class template.
*/
template <class Evt>
class EventEmitter
{

public:

	/**
	* Constructor.
	*/
	EventEmitter()
	{
	}

	/**
	* Subscribes a new event listener.
	* @param evtHCB Reference to event handler callback.
	*/
	virtual void subscribeListener( Functor1< void, const Evt& >& evtHCB )
	{
		mHandlers.push_back(&evtHCB);
	}

	/**
	* Unsubscribes the specified event listener.
	* @param evtHCB Reference to event handler callback.
	*/
	virtual void unsubscribeListener( Functor1< void, const Evt& >& evtHCB )
	{
		for( size_t i = 0; i < mHandlers.size(); ++i )
		{
			if( mHandlers[i] == &evtHCB )
			{
				mHandlers.erase( mHandlers.begin() + i );
				return;
			}
		}
	}

	/**
	* Emits the specified event to all subscribeed listeners.
	*
	* @param evt Event object.
	*/
	virtual void _emitEvent( const Evt& evt )
	{
		for( size_t i = 0; i < mHandlers.size(); ++i )
			(*mHandlers[i])(evt);
	}

protected:

	std::vector< Functor1< void, const Evt& >* > mHandlers;

};

}

#endif // __zhEvent_h__
