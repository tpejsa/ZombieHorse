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

#ifndef __zhAnimationSystem_h__
#define __zhAnimationSystem_h__

#include "zhPrereq.h"
#include "zhIterators.h"
#include "zhError.h"
#include "zhSingleton.h"
#include "zhSkeleton.h"
#include "zhAnimationTree.h"

#define zhAnimationSystem zh::AnimationSystem::Instance()
#define zhA(animSetName, animName) ((animSetName)+"::"+(animName))

namespace zh
{

class AnimationManager;
class MemoryPool;

enum SystemError
{
	SystemError_None,
	SystemError_ResourceLoadingFailed
};

/**
* @brief AnimationSystem core.
*
* AnimationSystem core. Handles the following tasks:
* - system initialization and uninitialization
* - system configuration (loadable from config.xml file)
* - access to core functionality - skeletons, animation loading and control, animation tree...
*/
class zhDeclSpec AnimationSystem : public Singleton<AnimationSystem>
{

	friend class Singleton<AnimationSystem>;

public:

	zhDeclare_ErrorState
	
	typedef MapIterator< std::map<std::string, Skeleton*> > SkeletonIterator;
	typedef MapConstIterator< std::map<std::string, Skeleton*> > SkeletonConstIterator;

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
	* Creates a new Skeleton.
	*
	* @param name Skeleton name.
	* @return Pointer to the skeleton.
	*/
	Skeleton* createSkeleton( const std::string& name );

	/**
	* Deletes the skeleton with the specified name.
	*/
	void deleteSkeleton( const std::string& name );

	/**
	* Deletes all skeletons.
	*/
	void deleteAllSkeletons();

	/**
	* Returns true if the skeleton with the specified name exists, false otherwise.
	*/
	bool hasSkeleton( const std::string& name ) const;

	/**
	* Gets the skeleton with the specified name.
	*/
	Skeleton* getSkeleton( const std::string& name ) const;

	/**
	* Gets an iterator over the map of skeletons.
	*/
	SkeletonIterator getSkeletonIterator();

	/**
	* Gets a const iterator over the map of skeletons.
	*/
	SkeletonConstIterator getSkeletonConstIterator() const;

	/**
	* Gets the number of skeletons.
	*/
	unsigned int getNumSkeletons() const;

	/**
	* Loads an animation set from a file (BVH, ZHA...)
	*
	* @param path Animation set path.
	* @param skel Skeleton for the animation set.
	* @return Animation set pointer, or NULL if loading has failed.
	* @remark If skeleton is not specified, then a new skeleton will be
	* automatically created, and assigned the same name as the animation set.
	* However, if the loaded animation set is known to have the same skeleton
	* as one of the existing ones, you can specify that skeleton and thus
	* avoid spurious retargetting.
	*/
	AnimationSetPtr loadAnimationSet( const std::string& path, const std::string& skel = "" );

	// TODO: add serialization support

	/**
	* Deletes the animation set with the specified name.
	*/
	void deleteAnimationSet( const std::string& name );

	/**
	* Deletes all animations, across all animation sets.
	*/
	void deleteAllAnimations( const std::string& name );

	/**
	* Gets the animation set with the specified name.
	*/
	AnimationSetPtr getAnimationSet( const std::string& name ) const;

	/**
	* Return true if the animation set with the specified name exists, false otherwise.
	*/
	bool hasAnimationSet( const std::string& name ) const;

	/**
	* Creates a new animation clip from a segment of an existing animation.
	*
	* @param newAnimName New animation name.
	* @param origAnimName Fully-qualified original animation name.
	* @param startTime Animation segment start time.
	* @param length Animation segment length.
	* @return Pointer to the newly created animation.
	*/
	Animation* createAnimationFromSegment( const std::string& newAnimName, const std::string& origAnimName,
		float startTime, float length );

	/**
	* Deletes the specified animation.
	*
	* @param name Fully-qualified animation name.
	*/
	void deleteAnimation( const std::string& name );

	/**
	* Gets an animation clip.
	*
	* @param animName Fully-qualified animation name.
	* @return Pointer to the animation clip.
	*/
	Animation* getAnimation( const std::string& animName ) const;

	/**
	* Returns true if the specified animation clip exists.
	*
	* @param animName Fully-qualified animation name.
	* @return Pointer to the animation clip.
	*/
	bool hasAnimation( const std::string& animName ) const;

	/**
	* Gets the list of all currently loaded animations.
	*/
	void getAnimationList( std::vector<Animation*>& animList ) const;

	/**
	* Gets a pointer to the animation manager.
	*/
	AnimationManager* getAnimationManager() const;

	/**
	* Gets the current output skeleton.
	*
	* @return Pointer to the output skeleton.
	*/
	Skeleton* getOutputSkeleton() const;

	/**
	* Sets the output skeleton.
	*
	* @param name Output skeleton name.
	*/
	void setOutputSkeleton( const std::string& name );

	/**
	* Queues the specified animation for playback.
	*
	* @param animName Fully-qualified animation name.
	* @remark The specified animation will be added to the playback queue in
	* the animation tree. The animation will be spatially aligned with the
	* previous animation, and it will blend in gradually.
	*/
	void playAnimation( const std::string& animName );

	/**
	* Plays the specified animation right away, with original root position
	* and orientation.
	*
	* @param animName Fully-qualified animation name.
	*/
	void playAnimationNow( const std::string& animName );

	/**
	* Stops playback of current animation and empties the animation queue.
	*/
	void stopAnimation();

	/**
	* Gets the animation that's currently playing.
	*/
	Animation* getCurrentAnimation() const;

	/**
	* Returns true if animation is currently playing, false otherwise.
	*/
	bool isAnimationPlaying() const;

	/**
	* Pauses the current animation.
	*/
	void pauseAnimation();

	/**
	* Unpauses the current animation.
	*/
	void unpauseAnimation();
	
	/**
	* Returns true if animation is currently paused, false otherwise.
	*/
	bool isAnimationPaused() const;

	/**
	* Gets the current animation time.
	*
	* @return Current animation time.
	*/
	float getAnimationTime() const;

	/**
	* Sets the current animation time.
	*
	* @return Current animation time.
	*/
	void setAnimationTime( float time );

	/**
	* Gets the current animation playback rate.
	*
	* @return Current playback rate.
	* @remark 1 is the animation's normal playback rate.
	*/
	float getAnimationRate() const;

	/**
	* Sets the animation playback rate.
	*
	* @param rate New playback rate.
	* @remark 1 is the animation's normal playback rate.
	*/
	void setAnimationRate( float rate );

	/**
	* Gets the current animation length.
	*
	* @return Animation length.
	*/
	float getAnimationLength() const;

	/**
	* Updates and applies the currently playing animation.
	*/
	void update( float dt ) const;

	/**
	* Gets the animation tree used for playback, blending and queueing.
	*/
	AnimationTree* getAnimationTree() const;

	// TODO: add functions for end-effector specification and cleanup

	/**
	* Gets a pointer to the memory pool.
	*/
	MemoryPool* getMemoryPool() const;

	/**
	* Parses the fully-qualified animation name to obtain animation set name
	* and animation clip name.
	*/
	static void ParseAnimationName( const std::string& fullName,
		std::string& animSetName, std::string& animName );

private:

	std::map<std::string, Skeleton*> mSkeletons;
	Skeleton* mOutSkel;
	AnimationTree* mAnimTree;

};

}

#endif // __zhAnimationSystem_h__
