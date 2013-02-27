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

#ifndef __zhMesh_h__
#define __zhMesh_h__

#include "zhPrereq.h"
#include "zhIterators.h"
#include "zhVector.h"

namespace zh
{

/**
* @brief Mesh animation state class.
* Holds mesh pose configuration before it is passed to the renderer.
*/
class zhDeclSpec Mesh
{

public:

	/**
	* Constructor.
	*/
	Mesh( unsigned short id, const std::string& name );

	/**
	* Destructor.
	*/
	~Mesh();

	/**
	* Gets the mesh ID.
	*/
	unsigned short getId() const;

	/**
	* Gets the mesh name.
	*/
	const std::string& getName() const;

	/**
	* Creates a morph target on the mesh.
	*
	* @param name Morph target name.
	*/
	void createMorphTarget( const std::string& name );

	/**
	* Deletes a morph target from the mesh.
	*
	* @param name Morph target name.
	*/
	void deleteMorphTarget( const std::string& name );

	/**
	* Deletes a morph target from the mesh.
	*
	* @param index Morph target index.
	*/
	void deleteMorphTarget( unsigned int index );

	/**
	* Deletes all the morph targets on the mesh.
	*/
	void deleteAllMorphTargets();

	/**
	* Returns true if the specified morph target exists, otherwise false.
	*/
	bool hasMorphTarget( const std::string& name ) const;

	/**
	* Returns the number of morph targets on the mesh.
	*/
	unsigned int getNumMorphTargets() const;

	/**
	* Gets the weight of the specified morph target.
	*
	* @param name Morph target name.
	*/
	float getMorphTargetWeight( const std::string& name ) const;

	/**
	* Sets the weight of the specified morph target.
	*
	* @param name Morph target name.
	* @param weight Morph target weight.
	*/
	void setMorphTargetWeight( const std::string& name, float weight );

	/**
	* Gets the morph target weights.
	*/
	const Vector& getMorphTargetWeights() const;

	/**
	* Sets the morph target weights.
	*/
	void setMorphTargetWeights( const Vector& weights );

private:

	unsigned short mId;
	std::string mName;

	std::map<std::string, unsigned int> mMTNames;
	Vector mMTWeights;

};

}

#endif // __zhMesh_h__
