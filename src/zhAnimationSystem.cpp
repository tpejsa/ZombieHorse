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

#include "zhAnimationSystem.h"
#include "zhMemoryPool.h"
#include "zhAnimationManager.h"
#include "zhAnimation.h"
#include "zhAnimationSpace.h"
#include "zhAnimationTreeManager.h"
#include "zhAnimationSampler.h"
#include "zhFaceController.h"
#include "zhAnimationMixer.h"
#include "zhAnimationBlender.h"
#include "zhAnimationTransitionBlender.h"
#include "zhBoneTransformController.h"
#include "zhBoneIKController.h"
#include "zhZHALoader.h"
#include "zhZHASerializer.h"
#include "zhZHTLoader.h"
#include "zhZHTSerializer.h"
#include "zhModelController.h"
#include "zhAnimationController.h"

namespace zh
{

AnimationSystem::AnimationSystem()
{
	mPluginDir = "";
	mCharCtrlFact = new CharacterControllerFactory();
}

AnimationSystem::~AnimationSystem()
{
	//unloadAllPlugins();
	deleteAllCharacters();
	delete mCharCtrlFact;
}

bool AnimationSystem::init( const std::string& cfgPath )
{
	zhLog( "AnimationSystem", "init",
		"Initializing AnimationSystem. Using configuration file %s.",
		cfgPath.c_str() );

	// create instances of various singletons
	MemoryPool::Instance();
	AnimationManager::Instance();
	AnimationTreeManager::Instance();
	
	// register resource loaders and serializers
	zhRegister_ResourceLoader( getAnimationManager(), ZHALoader );
	zhRegister_ResourceSerializer( getAnimationManager(), ZHASerializer );
	zhRegister_ResourceLoader( getAnimationTreeManager(), ZHTLoader );
	zhRegister_ResourceSerializer( getAnimationTreeManager(), ZHTSerializer );

	// register character controllers
	zhRegister_CharacterController( ModelController );
	zhRegister_CharacterController( AnimationController );

	// register animation nodes and bone controllers
	zhRegister_AnimationNode( AnimationSampler );
	zhRegister_AnimationNode( FaceController );
	zhRegister_AnimationNode( AnimationMixer );
	zhRegister_AnimationNode( AnimationBlender );
	zhRegister_AnimationNode( AnimationTransitionBlender );
	zhRegister_BoneController( BoneTransformController );
	zhRegister_BoneController( BoneIKController );

	// register default plug-ins
	// TODO

	// load config.xml (configuration settings, res. dirs, plugin dir, plugins)
	// TODO

	zhSetErrorCode( SystemError_None );
	return true;
}

void AnimationSystem::update( float dt )
{
	CharacterIterator ci = getCharacterIterator();
	while( !ci.end() )
		ci.next()->update(dt);
}

Character* AnimationSystem::createCharacter( const std::string& id )
{
	zhAssert( !hasCharacter(id) );

	zhLog( "AnimationSystem", "createCharacter",
		"Creating Character  %s.", id.c_str() );

	Character* ch = new Character(id);
	mCharacters[id] = ch;

	return ch;
}

Character* AnimationSystem::createCharacter( const std::string& id, const std::string& templName )
{
	zhAssert( !hasCharacter(id) );
	zhAssert( hasCharacterTemplate(templName) );

	zhLog( "AnimationSystem", "createCharacter",
		"Creating Character %s using template %s.", id.c_str(), templName.c_str() );

	// create the Character
	Character* ch = new Character(id);
	mCharacters[id] = ch;

	// create all CharacterControllers specified in the template
	const CharacterTemplate& ch_templ = getCharacterTemplate(templName);
	CharacterTemplate::ControllerConstIterator ccni = ch_templ.getControllerConstIterator();
	while( !ccni.end() )
	{
		std::string ccn = ccni.next();

		unsigned long ccid = mCharCtrlFact->findClassIdByName(ccn);
		if( ccid == zhClass_NULL )
		{
			zhLog( "AnimationSystem", "createCharacter",
				"ERROR: Failed to create CharacterController %s specified in template %s. CharacterController class not registered.",
				ccn.c_str(), templName.c_str() );

			zhSetErrorCode( SystemError_ClassNotRegistered );
			continue;
		}

		ch->createController( ccid, ccn );
	}

	// set the controller update order specified in the template
	std::list<CharacterController*> order;
	for( std::list<std::string>::const_iterator uoi = ch_templ.getControllerUpdateOrder().begin();
		uoi != ch_templ.getControllerUpdateOrder().end(); ++uoi )
		order.push_back( ch->getController( *uoi ) );
	ch->setControllerUpdateOrder(order);

	zhSetErrorCode( SystemError_None );
	return ch;
}

void AnimationSystem::deleteCharacter( const std::string& id )
{
	zhLog( "AnimationSystem", "deleteCharacter", "Deleting character %s.", id.c_str() );

	Character* ch = getCharacter(id);

	if( ch == NULL )
		return;

	mCharacters.erase(id);
	delete ch;
}

void AnimationSystem::deleteAllCharacters()
{
	zhLog( "AnimationSystem", "deleteCharacter",
		"Deleting all characters." );

	for( std::map<std::string, Character*>::iterator ci = mCharacters.begin(); ci != mCharacters.end(); ++ci )
		delete ci->second;

	mCharacters.clear();
}

bool AnimationSystem::hasCharacter( const std::string& id ) const
{
	return mCharacters.count(id) > 0;
}

Character* AnimationSystem::getCharacter( const std::string& id ) const
{
	std::map<std::string, Character*>::const_iterator ci = mCharacters.find(id);

	if( ci != mCharacters.end() )
		return ci->second;

	return NULL;
}

AnimationSystem::CharacterIterator AnimationSystem::getCharacterIterator()
{
	return CharacterIterator( mCharacters );
}

AnimationSystem::CharacterConstIterator AnimationSystem::getCharacterConstIterator() const
{
	return CharacterConstIterator( mCharacters );
}

unsigned int AnimationSystem::getNumCharacters() const
{
	return mCharacters.size();
}

void AnimationSystem::addCharacterTemplate( const CharacterTemplate& templ )
{
	zhAssert( !hasCharacterTemplate( templ.getName() ) );

	zhLog( "AnimationSystem", "addCharacterTemplate",
		"Adding CharacterTemplate %s.", templ.getName().c_str() );

	mCharacterTemplates.insert( make_pair( templ.getName(), templ ) );
}

void AnimationSystem::removeCharacterTemplate( const std::string& templName )
{
	std::map<std::string, CharacterTemplate>::iterator cti = mCharacterTemplates.find(templName);
	if( cti != mCharacterTemplates.end() )
	{
		zhLog( "AnimationSystem", "removeCharacterTemplate",
			"Removing CharacterTemplate %s.", templName.c_str() );

		mCharacterTemplates.erase(cti);
	}
}

void AnimationSystem::removeAllCharacterTemplates()
{
	zhLog( "AnimationSystem", "removeAllCharacterTemplates",
		"Removing all CharacterTemplates." );

	mCharacterTemplates.clear();
}

bool AnimationSystem::hasCharacterTemplate( const std::string& templName ) const
{
	return mCharacterTemplates.count(templName) > 0;
}

const CharacterTemplate& AnimationSystem::getCharacterTemplate( const std::string& templName ) const
{
	zhAssert( hasCharacterTemplate(templName) );

	return mCharacterTemplates.find(templName)->second;
}

unsigned int AnimationSystem::getNumCharacterTemplates() const
{
	return mCharacterTemplates.size();
}

AnimationSystem::CharacterTemplateConstIterator AnimationSystem::getCharacterTemplateConstIterator() const
{
	return CharacterTemplateConstIterator( mCharacterTemplates );
}

AnimationManager* AnimationSystem::getAnimationManager() const
{
	return AnimationManager::Instance();
}

AnimationTreeManager* AnimationSystem::getAnimationTreeManager() const
{
	return AnimationTreeManager::Instance();
}

MemoryPool* AnimationSystem::getMemoryPool() const
{
	return MemoryPool::Instance();
}

AnimationSystem::CharacterControllerFactory* AnimationSystem::_getCharacterControllerFactory() const
{
	return mCharCtrlFact;
}

}
