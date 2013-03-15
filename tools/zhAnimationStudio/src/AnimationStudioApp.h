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

#ifndef __AnimationStudioApp_h__
#define __AnimationStudioApp_h__

#include "AnimationStudio.h"

class AnimationStudioFrame;

/**
* @brief Timer used for rendering at constant framerate.
*/
class RenderTimer : public wxTimer
{

public:

	void Notify();
};

/**
* @brief Animation Studio application class.
*/
class AnimationStudioApp : public wxApp, public FrameListener
{
	
public:

	/**
	* Constructor.
	*/
	AnimationStudioApp();

	/**
	* Destructor.
	*/
	~AnimationStudioApp();

	/**
	* Gets a pointer to the main application frame.
	*/
	AnimationStudioFrame* getAppFrame() const;

	/**
	* Gets the OGRE system root.
	*/
	Ogre::Root* getOgreRoot() const { return mOgreRoot; }

	/**
	* Gets a pointer to the OGRE camera.
	*/
	Camera* getCamera() const;

	/**
	* Gets a pointer to the OGRE SceneManager.
	*/
	SceneManager* getSceneManager() const;

	/**
	* Selects the skeleton with the specified name,
	* loading and displaying the corresponding OGRE entity.
	*
	* @param name Skeleton name.
	* @return Pointer to the character or NULL if no skeleton
	* has been selected.
	*/
	zh::Skeleton* selectSkeleton( const string& name );

	/**
	* Removes the specified skeleton.
	*
	* @param name Skeleton name.
	*/
	void removeSkeleton( const string& name );

	/**
	* Selects the animation clip with the specified name
	* for playback and editing.
	*
	* @param anim Fully qualified name of the animation.
	* @return Pointer to the animation clip or NULL if no animation
	* has been selected.
	*/
	zh::Animation* selectAnimation( const string& anim );

	/**
	* Deselect current animation.
	*/
	void deselectAnimation();

	/**
	* Gets a pointer to the currently selected animation clip.
	*/
	zh::Animation* getCurrentAnimation() const;

	/**
	* If true, animation is enabled (updates are sent to ZombieHorse system).
	*/
	bool getAnimationEnabled() const { return mAnimEnabled; }

	/**
	* If true, animation is enabled (updates are sent to ZombieHorse system).
	*/
	void setAnimationEnabled( bool enabled = true ) { mAnimEnabled = enabled; }

	/**
	* Displays visual markers on specified skeleton joints.
	*
	* @remark Specify empty set to display markers on ALL joints.
	*/
	void displayJointMarkers( const std::set<std::string>& boneNames, bool enable = true );

	/**
	* Displays a visual marker on a single skeleton joint.
	*/
	void displayJointMarker( const std::string& boneName, bool enable = true );

	/**
	* Returns true if there is a marker on the specified joint,
	* false otherwise.
	*/
	bool hasJointMarker( const std::string& boneName ) const;

	/**
	* Enables tracing of motion paths of specified skeleton joint markers.
	*
	* @remark Specify empty set to display traces on ALL joints.
	*/
	void traceJointPaths( const std::set<std::string>& boneNames, bool enable = true );

	/**
	* Enables tracing of the motion path of a single skeleton joint marker.
	*/
	void traceJointPath( const std::string& boneName, bool enable = true );

	/**
	* Returns true if there is path trace on the specified joint,
	* false otherwise.
	*/
	bool hasJointTrace( const std::string& boneName ) const;

	/**
	* Create a dummy environment object at the specified location.
	*
	* @param name Object name.
	* @param pos Object position in world space.
	* @return Pointer to the object.
	*/
	zh::Bone* createEnvironmentObject( const std::string& name, const zh::Vector3& pos );

	/**
	* Delete the specified environment object.
	*
	* @param name Object name.
	*/
	void deleteEnvironmentObject( const std::string& name );

	/**
	* Delete all environment objects.
	*/
	void deleteAllEnvironmentObjects();

	/**
	* Toggles rendering at constant framerate on/off.
	*
	* @remark Frame rate will be locked to the frame rate of the currently
	* selected animation, as long as the latter is 20 fps or higher.
	*/
	void useConstFrameRate( bool useConstFR = true );

	/**
	* OGRE calls this method at the beginning of a new frame.
	*/
	bool frameStarted( const FrameEvent& evt );

	/**
	* OGRE calls this method at the end of the current frame.
	*/
	bool frameEnded( const FrameEvent& evt );

	/**
	* Resizes the render window.
	*/
	void resize( unsigned int width, unsigned int height );

	/**
	* This method is called on application initialization.
	*/
	bool OnInit();

	/**
	* This method is called on application exit.
	*/
	int OnExit();

	/**
	* This method is called when there are no messages left to process
	* (extremely often, so this is where we put the render loop).
	*/
	void OnIdle( wxIdleEvent& evt );

protected:

	bool init( wxWindow* wnd = NULL ); ///< Initializes OGRE and ZombieHorse.
	bool initOgreCore( wxWindow* renderWnd = NULL ); ///< Initializes OGRE system core.
	bool createSceneManager(); ///< Creates a scene manager.
	bool createCamera(); ///< Creates a camera.
	bool createViewports(); ///< Creates viewports.
	bool createFrameListener(); ///< Sets up a frame listener.
	bool loadResourceLocations(); ///< Loads resource locations from resources.cfg.
	bool createScene(); ///< Creates the default scene.
	bool destroyScene(); ///< Destroys the default scene.
	bool initZombieHorse(); ///< Initializes ZombieHorse system.

	// Compute joint trace path (piecewise linear) for the specified set of bones and the current animation...
	void _computeJointTracePath( const std::string& boneName, std::vector<Ogre::Vector3>& path );

	DECLARE_EVENT_TABLE()

	AnimationStudioFrame* mFrmMain;
	RenderTimer* mRenderTimer;

	Root *mOgreRoot; ///< OGRE system.
	Camera* mCam; ///< Camera.
	SceneManager* mSceneMgr; ///< Scene manager.
	RenderWindow* mRenderWnd; ///< OGRE render window.

	zh::Animation* mCurAnim; ///< Currently selected animation.
	bool mAnimEnabled; ///< Updates to the animation system enabled/disabled.

	// Motion visualization
	std::set<std::string> mJointsWithMarkers;
	std::set<std::string> mTracedJoints;
	zh::Animation* mTracedJointAnim;
	std::map<std::string, std::vector<std::pair<float,zh::Vector3>>> mTracedJointPaths;
};

extern AnimationStudioApp* gApp;

#endif // __AnimationStudioApp_h__
