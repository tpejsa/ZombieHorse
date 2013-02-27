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

#ifndef __zhModelController_h__
#define __zhModelController_h__

#include "zhPrereq.h"
#include "zhObjectFactory.h"
#include "zhCharacterController.h"
#include "zhModel.h"

namespace zh
{

class Model;

/**
* @brief Character model controller. Handles skeleton and mesh pose updates.
*/
class zhDeclSpec ModelController : public CharacterController
{

public:

	zhDeclare_CharacterController( ModelController, zhModelController_ClassId, zhModelController_ClassName )

	/**
	* Constructor.
	*/
	ModelController() : mMdl(NULL)
	{
		mMdl = new Model();
	}

	/**
	* Destructor.
	*/
	~ModelController()
	{
	}

	/**
	* Gets the Model.
	*/
	Model* getModel() const;

	/**
	* Updates the controller with elapsed time.
	*/
	void update( float dt );

protected:

	Model* mMdl;

};

}

#endif // __zhModelController_h__
