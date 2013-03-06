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

#ifndef __zhSkeleton_h__
#define __zhSkeleton_h__

#include "zhPrereq.h"
#include "zhIterators.h"
#include "zhBone.h"

namespace zh
{

/**
* @brief Skeletal animation state class.
* Holds skeletal pose configuration before it is passed to the renderer.
* Root bone always has zero ID.
*/
class zhDeclSpec Skeleton
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

	typedef MapIterator< std::map<unsigned short, Bone*> > BoneIterator;
	typedef MapConstIterator< std::map<unsigned short, Bone*> > BoneConstIterator;

	/**
	* Constructor.
	*
	* @param name Skeleton name.
	*/
	Skeleton( const std::string& name );

	/**
	* Destructor.
	*/
	~Skeleton();

	/**
	* Gets the skeleton name.
	*/
	const std::string& getName() const;

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
	* Gets the skeleton's root bone.
	*/
	Bone* getRoot() const;

	/**
	* Creates a new bone.
	*/
	Bone* createBone( unsigned short id, const std::string& name );

	/**
	* Deletes the specified bone.
	*/
	void deleteBone( unsigned short id );

	/**
	* Deletes the specified bone.
	*/
	void deleteBone( const std::string& name );

	/**
	* Deletes all bones in the skeleton.
	*/
	void deleteAllBones();

	/**
	* Returns true if the specified bone exists in the skeleton, otherwise false.
	*/
	bool hasBone( unsigned short id ) const;

	/**
	* Returns true if the specified bone exists in the skeleton, otherwise false.
	*/
	bool hasBone( const std::string& name ) const;

	/**
	* Gets the specified bone.
	*/
	Bone* getBone( unsigned short id ) const;

	/**
	* Gets the specified bone.
	*/
	Bone* getBone( const std::string& name ) const;

	/**
	* Gets the number of bones in the skeleton.
	*/
	unsigned int getNumBones() const;

	/**
	* Gets an iterator over the vector of bones.
	*/
	BoneIterator getBoneIterator();

	/**
	* Gets a const iterator over the vector of bones.
	*/
	BoneConstIterator getBoneConstIterator() const;

	/**
	* Resets the skeleton to the initial pose.
	*/
	void resetToInitialPose();

private:

	std::string mName;
	mutable Bone* mRoot;
	std::map<unsigned short, Bone*> mBonesById;
	std::map<std::string, Bone*> mBonesByName;

};

template <>
inline std::string toString<Skeleton::Situation>( const Skeleton::Situation& sit )
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
inline Skeleton::Situation fromString<Skeleton::Situation>( const std::string& tstr )
{
	float px, pz, oy;
	std::istringstream iss(tstr);

	if( iss.eof() )
		return Skeleton::Situation::Identity;
	iss >> px;

	if( iss.eof() )
		return Skeleton::Situation::Identity;
	iss >> pz;

	if( iss.eof() )
		return Skeleton::Situation::Identity;
	iss >> oy;

	return Skeleton::Situation( px, pz, oy );
}

}

#endif // __zhSkeleton_h__
