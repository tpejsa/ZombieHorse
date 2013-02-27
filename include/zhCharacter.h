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

#ifndef __zhCharacter_h__
#define __zhCharacter_h__

#include "zhPrereq.h"
#include "zhIterators.h"
#include "zhObjectFactory.h"

namespace zh
{

class CharacterController;
class ModelController;
class AnimationController;

/**
* @brief Struct representing a template for Character
* initialization and configuration.
*/
struct zhDeclSpec CharacterTemplate
{

public:

	typedef VectorIterator< std::vector<std::string> > ControllerIterator;
	typedef VectorConstIterator< std::vector<std::string> > ControllerConstIterator;

	/**
	* Constructor.
	*
	* @param name CharacterTemplate name.
	*/
	CharacterTemplate( const std::string& name );

	/**
	* Destructor.
	*/
	~CharacterTemplate();

	/**
	* Gets the CharacterTemplate name.
	*/
	const std::string& getName() const;

	/**
	* Adds a CharacterController class name to the template.
	*/
	void addController( const std::string& ctrlName );

	/**
	* Removes a CharacterController class name from the template.
	*/
	void removeController( const std::string& ctrlName );

	/**
	* Removes all CharacterController class names from the template.
	*/
	void removeAllControllers();

	/**
	* Returns true if the specified CharacterController class name is part
	* of the template, otherwise false.
	*/
	bool hasController( const std::string& ctrlName );

	/**
	* Gets the number of CharacterController class names in the template.
	*/
	unsigned int getNumControllers();

	/**
	* Gets an iterator over the vector of CharacterController class names
	* in the template.
	*/
	ControllerIterator getControllerIterator();

	/**
	* Gets a const iterator over the vector of CharacterController class names
	* in the template.
	*/
	ControllerConstIterator getControllerConstIterator() const;

	/**
	* Gets the update order of CharacterControllers given in the template.
	*/
	const std::list<std::string>& getControllerUpdateOrder() const;

	/**
	* Sets the update order of CharacterControllers in the template.
	*/
	void setControllerUpdateOrder( const std::list<std::string>& order );

private:

	std::string mName;

	std::vector<std::string> mCtrlNames;
	std::list<std::string> mCtrlUpdateOrder;

};

/**
* @brief Class representing an animated virtual character.
*/
class zhDeclSpec Character
{

public:

	typedef MapIterator< std::map<std::string, CharacterController*> > ControllerIterator;
	typedef MapConstIterator< std::map<std::string, CharacterController*> > ControllerConstIterator;

	/**
	* Constructor.
	*
	* @param id Character name.
	*/
	Character( const std::string& id );

	/**
	* Destructor.
	*/
	~Character();

	/**
	* Gets the character name.
	*/
	const std::string& getId() const { return mId; }

	/**
	* Returns true if the character is enabled, otherwise false.
	*
	* @remark Disabled characters don't get updated.
	*/
	bool getEnabled() const { return mEnabled; }

	/**
	* If set to true, the character will be enabled, otherwise
	* it will be disabled.
	*
	* @remark Disabled characters don't get updated.
	*/
	void setEnabled( bool enabled = true ) { mEnabled = enabled; }

	/**
	* Updates the character with elapsed time.
	*/
	void update( float dt );

	/**
	* Creates a new character controller on this character.
	*
	* @param classId Controller class ID.
	* @param name Controller name.
	* @return Pointer to the character controller.
	*/
	CharacterController* createController( unsigned long classId, const std::string& name );

	/**
	* Deletes a character controller from this character.
	*
	* @param name Controller name.
	*/
	void deleteController( const std::string& name );

	/**
	* Deletes all character controllers on this character.
	*/
	void deleteAllControllers();

	/**
	* Returns true if the specified character controller exists,
	* otherwise false.
	*/
	bool hasController( const std::string& name ) const;

	/**
	* Gets a pointer to the specified character controller.
	*
	* @param name Controller name.
	* @return Pointer to the controller or NULL if the controller doesn't exist.
	*/
	CharacterController* getController( const std::string& name ) const;

	/**
	* Gets an iterator over the map of character controllers.
	*/
	ControllerIterator getControllerIterator();

	/**
	* Gets a const iterator over the map of character controllers.
	*/
	ControllerConstIterator getControllerConstIterator() const;

	/**
	* Gets the number of character controllers on this character.
	*/
	unsigned int getNumControllers() const;

	/**
	* Gets the current update order of character controllers.
	*/
	const std::list<CharacterController*>& getControllerUpdateOrder() const;

	/**
	* Sets the update order of character controllers.
	*/
	void setControllerUpdateOrder( const std::list<CharacterController*>& order );

	/**
	* Gets this character's ModelController.
	*
	* @param Pointer to the ModelController or NULL if the character does not have one.
	*/
	ModelController* getModelController() const;

	/**
	* Gets this character's AnimationController.
	*
	* @param Pointer to the AnimationController or NULL if the character does not have one.
	*/
	AnimationController* getAnimationController() const;

private:

	std::string mId;
	
	bool mEnabled;

	std::map<std::string, CharacterController*> mControllers;
	std::list<CharacterController*> mCtrlUpdateOrder;

};

}

#endif // __zhCharacter_h__
