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

#ifndef __zhSystem_h__
#define __zhSystem_h__

#include "zhPrereq.h"
#include "zhIterators.h"
#include "zhError.h"
#include "zhObjectFactory.h"
#include "zhSingleton.h"
#include "zhCharacter.h"

#define zhAnimationSystem AnimationSystem::Instance()

#define zhModelController_ClassId 1
#define zhModelController_ClassName "ModelController"
#define zhAnimationController_ClassId 2
#define zhAnimationController_ClassName "AnimationController"
#define zhLocomotionController_ClassId 3
#define zhLocomotionController_ClassName "LocomotionController"
#define zhSensoryController_ClassId 4
#define zhSensoryController_ClassName "SensoryController"
#define zhNavigationController_ClassId 5
#define zhNavigationController_ClassName "NavigationController"

#define zhDeclare_CharacterController( CC, classId, className ) \
	zhDeclare_Class( CharacterController, CC, classId, className, std::string )
#define zhRegister_CharacterController( CC ) \
	AnimationSystem::Instance()->_getCharacterControllerFactory()->registerClass( CC::ClassId(), CC::ClassName(), &CC::Create )
#define zhUnregister_CharacterController( CC ) \
	AnimationSystem::Instance()->_getCharacterControllerFactory()->registerClass( CC::ClassId() )

namespace zh
{

class CharacterController;
class AnimationManager;
class AnimationTreeManager;
class Plugin;
class MemoryPool;

enum SystemError
{
	SystemError_None,
	SystemError_FileNotFound,
	SystemError_ClassNotRegistered
};

/**
* @brief AnimationSystem core.
*
* AnimationSystem core. Handles the following tasks:
* - system initialization and uninitialization
* - system configuration (loadable from config.xml file)
* - plugin load and registration
* - access to core subsystems - characters, resource managers etc.
*/
class zhDeclSpec AnimationSystem : public Singleton<AnimationSystem>
{

	friend class Singleton<AnimationSystem>;

public:

	zhDeclare_ErrorState
	
	typedef MapIterator< std::map<std::string, Character*> > CharacterIterator;
	typedef MapConstIterator< std::map<std::string, Character*> > CharacterConstIterator;

	typedef MapConstIterator< std::map<std::string, CharacterTemplate> > CharacterTemplateConstIterator;

	typedef MapIterator< std::map<unsigned long, Plugin*> > PluginIterator;
	typedef MapConstIterator< std::map<unsigned long, Plugin*> > PluginConstIterator;

	typedef ObjectFactory< CharacterController, std::string > CharacterControllerFactory;

private:

	/**
	* Constructor.
	*/
	AnimationSystem();

	/**
	* Destructor.
	*/
	~AnimationSystem();

public:

	/**
	* Initializes the system.
	*
	* @param cfgPath Configuration file path.
	* @return true if initialization has been successful, false otherwise.
	* Sets error codes:
	* - SystemError_None - no errors
	* - SystemError_FileNotFound - config. file not found or invalid
	*/
	bool init( const std::string& cfgPath = "system.config.xml" );

	/**
	* Updates and applies the state of each Character.
	*
	* @param dt Elapsed time.
	*/
	void update( float dt );

	/**
	* Creates a new Character.
	*
	* @param id Character name.
	* @return Pointer to the character.
	*/
	Character* createCharacter( const std::string& id );

	/**
	* Creates a new Character using the specified CharacterTemplate.
	*
	* @param id Character name.
	* @param templName CharacterTemplate name.
	* @return Pointer to the character.
	* Sets error codes:
	* - SystemError_None - no errors
	* - SystemError_ClassNotRegistered - CharacterController class
	* specified in the template not registered
	*/
	Character* createCharacter( const std::string& id, const std::string& templName );

	/**
	* Deletes the character with the specified name.
	*/
	void deleteCharacter( const std::string& id );

	/**
	* Deletes all characters.
	*/
	void deleteAllCharacters();

	/**
	* Returns true if the character with the specified name exists, false otherwise.
	*/
	bool hasCharacter( const std::string& id ) const;

	/**
	* Gets the character with the specified name.
	*/
	Character* getCharacter( const std::string& id ) const;

	/**
	* Gets an iterator over the map of characters.
	*/
	CharacterIterator getCharacterIterator();

	/**
	* Gets a const iterator over the map of characters.
	*/
	CharacterConstIterator getCharacterConstIterator() const;

	/**
	* Gets the number of characters.
	*/
	unsigned int getNumCharacters() const;

	/**
	* Adds a new CharacterTemplate.
	*/
	void addCharacterTemplate( const CharacterTemplate& templ );

	/**
	* Removes the specified CharacterTemplate.
	*
	* @param templName CharacterTemplate name.
	*/
	void removeCharacterTemplate( const std::string& templName );

	/**
	* Removes all CharacterTemplates.
	*/
	void removeAllCharacterTemplates();

	/**
	* Returns true if the specified CharacterTemplate exists,
	* otherwise false.
	*
	* @param templName CharacterTemplate name.
	* @return true if the specified CharacterTemplate exists,
	* false otherwise.
	*/
	bool hasCharacterTemplate( const std::string& templName ) const;

	/**
	* Gets the specified CharacterTemplate.
	*
	* @param templName CharacterTemplateName.
	* @return CharacterTemplate.
	*/
	const CharacterTemplate& getCharacterTemplate( const std::string& templName ) const;

	/**
	* Gets the number of CharacterTemplates.
	*/
	unsigned int getNumCharacterTemplates() const;

	/**
	* Gets a const iterator over the map of CharacterTemplates.
	*/
	CharacterTemplateConstIterator getCharacterTemplateConstIterator() const;
	
	/**
	* Gets a pointer to the memory pool.
	*/
	MemoryPool* getMemoryPool() const;

	/**
	* Gets a pointer to the animation manager.
	*/
	AnimationManager* getAnimationManager() const;

	/**
	* Gets a pointer to the animation tree manager.
	*/
	AnimationTreeManager* getAnimationTreeManager() const;

	/**
	* Gets the factory that produces character controllers.
	* This function is meant to be called only
	* by Character class.
	*/
	CharacterControllerFactory* _getCharacterControllerFactory() const;

private:

	std::map<std::string, Character*> mCharacters;
	std::map<std::string, CharacterTemplate> mCharacterTemplates;

	std::string mPluginDir;
	std::map<unsigned long, Plugin*> mPluginsByClassId;
	std::map<std::string, Plugin*> mPluginsByClassName;

	CharacterControllerFactory* mCharCtrlFact;

};

}

#endif // __zhSystem_h__
