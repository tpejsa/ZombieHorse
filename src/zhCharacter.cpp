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

#include "zhCharacter.h"
#include "zhCharacterController.h"
#include "zhModelController.h"
#include "zhAnimationController.h"
#include "zhAnimationSystem.h"

namespace zh
{

CharacterTemplate::CharacterTemplate( const std::string& name ) : mName(name)
{
}

CharacterTemplate::~CharacterTemplate()
{
}

const std::string& CharacterTemplate::getName() const
{
	return mName;
}

void CharacterTemplate::addController( const std::string& ctrlName )
{
	if( !hasController(ctrlName) )
		mCtrlNames.push_back(ctrlName);
}

void CharacterTemplate::removeController( const std::string& ctrlName )
{
	for( unsigned int ci = 0; ci < mCtrlNames.size(); ++ci )
		if( mCtrlNames[ci] == ctrlName )
		{
			mCtrlNames.erase( mCtrlNames.begin() + ci );
			return;
		}
}

void CharacterTemplate::removeAllControllers()
{
	mCtrlNames.clear();
}

bool CharacterTemplate::hasController( const std::string& ctrlName )
{
	for( unsigned int ci = 0; ci < mCtrlNames.size(); ++ci )
		if( mCtrlNames[ci] == ctrlName )
			return true;

	return false;
}

unsigned int CharacterTemplate::getNumControllers()
{
	return mCtrlNames.size();
}

CharacterTemplate::ControllerIterator CharacterTemplate::getControllerIterator()
{
	return ControllerIterator( mCtrlNames );
}

CharacterTemplate::ControllerConstIterator CharacterTemplate::getControllerConstIterator() const
{
	return ControllerConstIterator( mCtrlNames );
}

const std::list<std::string>& CharacterTemplate::getControllerUpdateOrder() const
{
	return mCtrlUpdateOrder;
}

void CharacterTemplate::setControllerUpdateOrder( const std::list<std::string>& order )
{
	#ifdef _DEBUG
	for( std::list<std::string>::const_iterator cci = order.begin();
		cci != order.end(); ++cci )
		zhAssert( hasController( *cci ) );
	#endif

	mCtrlUpdateOrder = order;
}

Character::Character( const std::string& id ) : mId(id), mEnabled(true)
{
}

Character::~Character()
{
	deleteAllControllers();
}

void Character::update( float dt )
{
	if( !mEnabled )
		return;

	if( mCtrlUpdateOrder.empty() )
	{
		ControllerIterator ci = getControllerIterator();
		while( !ci.end() )
		{
			ci.value()->update(dt);
			ci.next();
		}
	}
	else
	{
		for( std::list<CharacterController*>::iterator ci = mCtrlUpdateOrder.begin();
			ci != mCtrlUpdateOrder.end(); ++ci )
		{
			(*ci)->update(dt);
		}
	}
}

CharacterController* Character::createController( unsigned long classId, const std::string& name )
{
	zhAssert( !hasController(name) );

	CharacterController* cc = zhAnimationSystem->_getCharacterControllerFactory()->createObject(
		classId, name );

	zhLog( "Character", "createController",
		"Creating CharacterController %s %s on Character %s.",
		cc->getClassName().c_str(), cc->getId().c_str(), mId.c_str() );
	
	mControllers[name] = cc;
	cc->_setCharacter(this);

	return cc;
}

void Character::deleteController( const std::string& name )
{
	CharacterController* cc = getController(name);

	if( cc == NULL )
		return;

	zhLog( "Character", "deleteController",
		"Deleting character controller %s %s from Character %s.",
		cc->getClassName().c_str(), cc->getId().c_str(), mId.c_str() );

	mControllers.erase(name);
	delete cc;
}

void Character::deleteAllControllers()
{
	for( std::map<std::string, CharacterController*>::iterator cci = mControllers.begin();
		cci != mControllers.end(); ++cci )
		delete cci->second;

	zhLog( "Character", "deleteController",
		"Deleting all character controllers from character %s.", mId.c_str() );

	mControllers.clear();
}

bool Character::hasController( const std::string& name ) const
{
	return mControllers.count(name) > 0;
}

CharacterController* Character::getController( const std::string& name ) const
{
	std::map<std::string, CharacterController*>::const_iterator cci = mControllers.find(name);

	if( cci != mControllers.end() )
		return cci->second;

	return NULL;
}

Character::ControllerIterator Character::getControllerIterator()
{
	return ControllerIterator(mControllers);
}

Character::ControllerConstIterator Character::getControllerConstIterator() const
{
	return ControllerConstIterator(mControllers);
}

unsigned int Character::getNumControllers() const
{
	return mControllers.size();
}

const std::list<CharacterController*>& Character::getControllerUpdateOrder() const
{
	return mCtrlUpdateOrder;
}

void Character::setControllerUpdateOrder( const std::list<CharacterController*>& order )
{
	#ifdef _DEBUG
	for( std::list<CharacterController*>::const_iterator cci = order.begin();
		cci != order.end(); ++cci )
		zhAssert( *cci != NULL && hasController( (*cci)->getId() ) );
	#endif

	mCtrlUpdateOrder = order;
}

ModelController* Character::getModelController() const
{
	ControllerConstIterator ci = getControllerConstIterator();
	while( !ci.end() )
	{
		ModelController* mc = dynamic_cast<ModelController*>( ci.value() );
		if( mc != NULL )
			return mc;

		ci.next();
	}

	return NULL;
}

AnimationController* Character::getAnimationController() const
{
	ControllerConstIterator ci = getControllerConstIterator();
	while( !ci.end() )
	{
		AnimationController* mc = dynamic_cast<AnimationController*>( ci.value() );
		if( mc != NULL )
			return mc;
	}

	return NULL;
}

}
