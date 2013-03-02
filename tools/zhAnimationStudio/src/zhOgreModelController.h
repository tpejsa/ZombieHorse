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

#ifndef __zhOgreModelController_h__
#define __zhOgreModelController_h__

#include "zh.h"
#include "ogre.h"

#define zhOgreVector3(v) Ogre::Vector3( (v).x, (v).y, (v).z )
#define zhOgreQuat(q) Ogre::Quaternion( (q).w, (q).x, (q).y, (q).z )
#define zhVisageVector3(v) zh::Vector3( (v).x, (v).y, (v).z )
#define zhVisageQuat(q) zh::Quat( (q).w, (q).x, (q).y, (q).z )

using namespace std;
using namespace Ogre;
using namespace zh;

/**
* @brief Implementation of the ModelController class for updating
* OGRE scene Entities.
*/
class OgreModelController// : public ModelController
{

public:

	typedef zh::VectorIterator< std::vector<Entity*> > EntityIterator;
	typedef zh::VectorConstIterator< std::vector<Entity*> > EntityConstIterator;

	//zhDeclare_CharacterController( OgreModelController, 1000, "OgreModelController" )

	/*OgreModelController() : ModelController(), mSkeleton(NULL)
	{
	}

	~OgreModelController()
	{
	}*/

	/**
	* Gets the OGRE scene manager.
	*/
	//SceneManager* getSceneManager() const;

	/**
	* Sets the OGRE scene manager.
	*/
	//void setSceneManager( SceneManager* sceneMgr );

	/**
	* Adds an OGRE Entity to the controller.
	*
	* @param ent Pointer to the Entity.
	*/
	//void addEntity( Entity* ent );

	/**
	* Removes an OGRE Entity from the controller.
	*
	* @param index Entity index.
	*/
	//void removeEntity( unsigned int index );

	/**
	* Remvoes all OGRE Entities from the controller.
	*/
	//void removeAllEntities();

	/**
	* Gets an OGRE Entity from the controller.
	*
	* @param index Entity index.
	* @return Pointer to the Entity.
	*/
	//Entity* getEntity( unsigned int index ) const;

	/**
	* Gets the number of Entities in the controller.
	*/
	//unsigned int getNumEntities() const;

	/**
	* Gets an iterator over the vector of Entities.
	*/
	//EntityIterator getEntityIterator();

	/**
	* Get a const iterator over the vector of Entities.
	*/
	//EntityConstIterator getEntityConstIterator() const;

	/**
	* Initializes the controller with provided Entities.
	*/
	//void init();

	/**
	* Updates the controller with elapsed time.
	*/
	//void update( float dt );

private:

	/*SceneManager* mSceneMgr;
	std::vector<Entity*> mEntities;
	SkeletonInstance* mSkeleton;*/

};


#endif // __zhOgreModelController_h__
