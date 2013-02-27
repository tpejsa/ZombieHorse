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
*/
class zhDeclSpec Skeleton
{

public:

	typedef MapIterator< std::map<unsigned short, Bone*> > BoneIterator;
	typedef MapConstIterator< std::map<unsigned short, Bone*> > BoneConstIterator;

	/**
	* Constructor.
	*/
	Skeleton();

	/**
	* Destructor.
	*/
	~Skeleton();

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

	mutable Bone* mRoot;
	std::map<unsigned short, Bone*> mBonesById;
	std::map<std::string, Bone*> mBonesByName;

};

}

#endif // __zhSkeleton_h__
