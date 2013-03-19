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

#ifndef __zhBone_h__
#define __zhBone_h__

#include "zhPrereq.h"
#include "zhIterators.h"
#include "zhMath.h"

namespace zh
{

class Skeleton;

enum BoneTag
{
	BT_Root = 0,
	BT_Chest = 1,
	BT_LShoulder = 102,
	BT_LElbow = 103,
	BT_LWrist = 104,
	BT_RShoulder = 202,
	BT_RElbow = 203,
	BT_RWrist = 204,
	BT_LHip = 100,
	BT_LKnee = 105,
	BT_LAnkle = 106,
	BT_RHip = 200,
	BT_RKnee = 205,
	BT_RAnkle = 206,
	BT_LCollar= 107,
	BT_RCollar= 207,
	BT_Neck = 8,
	BT_Head = 9,
	BT_LFingers = 110,
	BT_RFingers = 210,
	BT_LThumb = 111,
	BT_RThumb = 211,
	BT_LToe = 112,
	BT_RToe = 212,
	BT_LowerBack = 13,
	BT_Unknown = 999
};

/**
* Skeleton bone.
*/
class zhDeclSpec Bone
{

public:

	typedef MapIterator< std::map<unsigned short, Bone*> > ChildIterator;
	typedef MapConstIterator< std::map<unsigned short, Bone*> > ChildConstIterator;

	/**
	* Constructor.
	*/
	Bone( unsigned short id, const std::string& name, Skeleton* skel );

	/**
	* Destructor.
	*/
	~Bone();

	/**
	* Get the owner skeleton.
	*/
	Skeleton* getSkeleton() const;

	/**
	* Gets the name of the bone.
	*/
	unsigned short getId() const { return mId; }

	/**
	* Gets the name of the bone.
	*/
	const std::string& getName() const { return mName; }

	// TODO: cache world transf.

	/**
	* Gets the bone's local position.
	*/
	const Vector3& getPosition() const;

	/**
	* Sets the bone's local position.
	*/
	void setPosition( const Vector3& pos );

	/**
	* Gets the bone's world position.
	*/
	Vector3 getWorldPosition() const;

	/**
	* Gets the bone's initial local position.
	*/
	const Vector3& getInitialPosition() const;

	/**
	* Sets the bone's initial local position.
	*/
	void setInitialPosition( const Vector3& pos );

	/**
	* Gets the bone's local orientation.
	*/
	const Quat& getOrientation() const;

	/**
	* Sets the bone's local orientation.
	*/
	void setOrientation( const Quat& orient );

	/**
	* Gets the bone's world orientation.
	*/
	Quat getWorldOrientation() const;

	/**
	* Gets the bone's initial local orientation.
	*/
	const Quat& getInitialOrientation() const;

	/**
	* Sets the bone's initial local orientation.
	*/
	void setInitialOrientation( const Quat& orient );

	/**
	* Gets the bone's local scale.
	*/
	const Vector3& getScale() const;

	/**
	* Sets the bone's local scale.
	*/
	void setScale( const Vector3& scal );

	/**
	* Gets the bone's world scale.
	*/
	Vector3 getWorldScale() const;

	/**
	* Gets the bone's initial local scale.
	*/
	const Vector3& getInitialScale() const;

	/**
	* Sets the bone's initial local scale.
	*/
	void setInitialScale( const Vector3& scal );

	/**
	* Resets the bone transformations to initial values.
	*/
	void resetToInitialPose();

	/**
	* Translates the bone.
	* 
	* The bone is translated relative to he parent system.
	*/
	void translate( const Vector3& trans );

	/**
	* Rotates the bone.
	* 
	* The bone is rotated relative to the local system.
	*/
	void rotate( const Quat& rot );

	/**
	* Rotates the bone.
	* 
	* The bone is rotated relative to the local system.
	*/
	void rotate( float yaw, float pitch, float roll );

	/**
	* Scales the bone.
	* 
	* The bone is scaled relative to the local system.
	*/
	void scale( const Vector3& scal );

	/**
	* Gets the bone's parent.
	*/
	Bone* getParent() const;

	/**
	* Attaches a bone to this one as a child bone.
	*/
	void addChild( Bone* bone );

	/**
	* Removes one of this bone's children.
	* The bone won't be deleted - to actually delete it,
	* use Skeleton::deleteBone().
	*
	* @param childId Child ID.
	*/
	void removeChild( unsigned short childId );

	/**
	* Removes one of this bone's children.
	* The bone won't be deleted - to actually delete it,
	* use Skeleton::deleteBone().
	*
	* @param childName Child name.
	*/
	void removeChild( const std::string& childName );

	/**
	* Removes all of this bone's children.
	* The bones won't be deleted - to actually delete them,
	* use Skeleton::deleteBone().
	*/
	void removeAllChildren();

	/**
	* Detaches the specified child bone from this bone
	* and attaches it to another bone.
	*
	* @param childId Child ID.
	* @param bone New parent.
	*/
	void moveChild( unsigned short childId, Bone* bone );

	/**
	* Detaches the specified child bone from this bone
	* and attaches it to another bone.
	*
	* @param childName Child name.
	* @param bone New parent.
	*/
	void moveChild( const std::string& childName, Bone* bone );

	/**
	* Checks if the specified bone is a child of this bone.
	*
	* @param childId Child ID.
	* @return true if the bone is a child of this bone,
	* false otherwise.
	*/
	bool hasChild( unsigned short childId ) const;

	/**
	* Checks if the specified bone is a child of this bone.
	*
	* @param childName Child name.
	* @return true if the bone is a child of this bone,
	* false otherwise.
	*/
	bool hasChild( const std::string& childName ) const;

	/**
	* Gets the specified child bone.
	*
	* @param childId Child ID.
	* @return Pointer to the child bone.
	*/
	Bone* getChild( unsigned short childId ) const;

	/**
	* Gets the specified child bone.
	*
	* @param childName Child name.
	* @return Pointer to the child bone.
	*/
	Bone* getChild( const std::string& childName ) const;

	/**
	* Gets the number of the bone's children.
	*/
	unsigned int getNumChildren() const;

	/**
	* Gets an iterator over the map of children.
	*/
	ChildIterator getChildIterator();

	/**
	* Gets an iterator over the map of children.
	*/
	ChildConstIterator getChildConstIterator() const;

	/**
	* Finds a chain of bones in the skeleton starting at this bone.
	*
	* @param endBone Chain end bone.
	* @param chain Bone chain.
	* @return true if a chain has been found, false otherwise.
	*/
	bool findChain( Bone* endBone, std::vector<Bone*>& chain );

	/**
	* Adds a semantic tag to the bone.
	*/
	void tag( BoneTag tag );

	/**
	* Removes all semantic tags from the bone.
	*/
	void untag();

private:

	unsigned short mId;
	std::string mName;
	Skeleton* mSkel;
	Bone* mParent;
	std::map<unsigned short, Bone*> mChildrenById;
	std::map<std::string, Bone*> mChildrenByName;

	Vector3 mInitPos;
	Quat mInitOrient;
	Vector3 mInitScal;
	Vector3 mPos;
	Quat mOrient;
	Vector3 mScal;

};

}

#endif // __zhBone_h__
