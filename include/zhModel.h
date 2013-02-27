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

#ifndef __zhModel_h__
#define __zhModel_h__

#include "zhPrereq.h"
#include "zhIterators.h"
#include "zhVector3.h"
#include "zhQuat.h"
#include "zhSkeleton.h"
#include "zhMesh.h"

namespace zh
{

/**
* @brief Character Model class.
* Holds skeletal and mesh pose configuration
* before it is passed to the renderer.
*/
class zhDeclSpec Model
{

public:

	/**
	* @brief Specifies the position and orientation of the character
	* in world coordinates in 2D horizontal plane.
	*/
	struct zhDeclSpec Situation
	{

	public:

		/**
		* Constructor.
		*/
		Situation();

		/**
		* Constructor.
		*
		* @param posX x-position.
		* @param posZ z-position.
		* @param orientY y-angle.
		*/
		Situation( float posX, float posZ, float orientY );

		/**
		* Constructor.
		* 
		* @param pos 3D position.
		* @param orient 3D orientation.
		*/
		Situation( const Vector3& pos, const Quat& orient );

		/**
		* Gets the position as a 3D vector.
		*/
		Vector3 getPosition() const;

		/**
		* Sets the position.
		*/
		void setPosition( const Vector3& pos );

		/**
		* Gets the orientation as a quaternion.
		*/
		Quat getOrientation() const;

		/**
		* Sets the orientation.
		*/
		void setOrientation( const Quat& orient );

		/**
		* Gets the x position.
		*/
		float getPosX() const;

		/**
		* Gets the z position.
		*/
		float getPosZ() const;

		/**
		* Gets the orientation about vertical axis.
		*
		* @param posX x-position.
		* @param posZ z-position.
		* @param orientY y-angle.
		*/
		float getOrientY() const;

		/**
		* Inverts this situation.
		*/
		Situation& invert();

		/**
		* Gets the inverse of this situation.
		*/
		Situation getInverse() const;

		/**
		* Applies a transformation to this situation.
		*/
		Situation& transform( const Situation& transf );

		/**
		* Gets this situation with a transformation applied.
		*/
		Situation getTransformed( const Situation& transf ) const;

		/**
		* Gets the transformation from this situation to another one.
		*/
		Situation getTransformTo( const Situation& sit ) const;

		/**
		* Projects this situation to ground plane (x-z).
		*/
		const Situation& projectToGround();

		/**
		* Gets the projection of this situation to ground plane (x-z).
		*/
		Situation getProjToGround() const;

		static const Situation Identity;

	private:

		Vector3 mPos;
		Quat mOrient;

	};

	typedef MapIterator< std::map<unsigned short, Mesh*> > MeshIterator;
	typedef MapConstIterator< std::map<unsigned short, Mesh*> > MeshConstIterator;

	/**
	* Constructor.
	*/
	Model();

	/**
	* Destructor.
	*/
	~Model();

	/**
	* Gets a pointer to the skeleton.
	*/
	Skeleton* getSkeleton() const;

	/**
	* Gets the position and orientation of the character model
	* in the 2D horizontal plane.
	*/
	Situation getSituation() const;

	/**
	* Sets the position and orientation of the character model
	* in the 2D horizontal plane.
	*/
	void setSituation( const Situation& sit );

	/**
	* Creates a new mesh.
	*
	* @param name Mesh name.
	* @return Pointer to the mesh.
	*/
	Mesh* createMesh( unsigned short id, const std::string& name );

	/**
	* Deletes the specified mesh.
	*/
	void deleteMesh( unsigned short id );

	/**
	* Deletes the specified mesh.
	*/
	void deleteMesh( const std::string& name );

	/**
	* Deletes all the meshes in the model.
	*/
	void deleteAllMeshes();

	/**
	* Returns true if the specified mesh exists,
	* otherwise false.
	*/
	bool hasMesh( unsigned short id );

	/**
	* Returns true if the specified mesh exists,
	* otherwise false.
	*/
	bool hasMesh( const std::string& name );

	/**
	* Gets the specified mesh.
	*/
	Mesh* getMesh( unsigned short id );

	/**
	* Gets the specified mesh.
	*/
	Mesh* getMesh( const std::string& name );

	/**
	* Gets the number of meshes.
	*/
	unsigned int getNumMeshes() const;

	/**
	* Gets an iterator over the vector of meshes.
	*/
	MeshIterator getMeshIterator();

	/**
	* Gets a const iterator over the vector of meshes.
	*/
	MeshConstIterator getMeshConstIterator() const;

private:

	Skeleton* mSkel;

	std::map<unsigned short, Mesh*> mMeshesById;
	std::map<std::string, Mesh*> mMeshesByName;

};

template <>
inline std::string toString<Model::Situation>( const Model::Situation& sit )
{
	std::ostringstream oss;

	oss << sit.getPosX();
	oss << " ";
	oss << sit.getPosZ();
	oss << " ";
	oss << sit.getOrientY();

	return oss.str();
}

template <>
inline Model::Situation fromString<Model::Situation>( const std::string& tstr )
{
	float px, pz, oy;
	std::istringstream iss(tstr);

	if( iss.eof() )
		return Model::Situation::Identity;
	iss >> px;

	if( iss.eof() )
		return Model::Situation::Identity;
	iss >> pz;

	if( iss.eof() )
		return Model::Situation::Identity;
	iss >> oy;

	return Model::Situation( px, pz, oy );
}

}

#endif // __zhModel_h__
