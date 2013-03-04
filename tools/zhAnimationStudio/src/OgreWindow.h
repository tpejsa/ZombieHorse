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

#include "AnimationStudio.h"

class OgreWindow : public wxWindow, public FrameListener
{

public:

	/**
	* Constructor.
	*/
	OgreWindow( wxWindow *parent, wxWindowID id );

	/**
	* Gets the current frame rate of the application.
	*/
	float getFrameRate() const;

	/**
	* Toggles rendering of stylized skeleton.
	*/
	void showSkeleton( bool show = true );

	/**
	* Toggles rendering of generic ground.
	*/
	void showGround( bool show = true );

	/**
	* Toggles rendering of generic skybox.
	*/
	void showSkybox( bool show = true );

	/**
	* Toggles rendering of coordinate system axes on all bones.
	*/
	void showCoordAxesOnBones( bool show = true );

	/**
	* Toggles rendering of coordinate system axes on specified bone.
	*/
	void showCoordAxesOnBone( const Ogre::String& boneName, bool show = true );

	/**
	* Sets a new skeleton to be rendered in the viewport.
	*/
	void setRenderSkeleton( zh::Skeleton* skel );

	/**
	* Updates the render skeleton pose.
	*/
	void updateRenderSkeletonPose( zh::Skeleton* skel );

	/**
	* Creates a point set for rendering.
	*
	* @param name Point set name.
	* @param points Vector containing positions of points in the set.
	* @param pColor Point color.
	* @param pSize Point size (in screen pixels).
	*/
	void createPointSet( const Ogre::String& name, const std::vector<Ogre::Vector3>& points,
		const Ogre::ColourValue& pColor, int pSize = 1 );

	/**
	* Deletes a point set.
	*
	* @param name Point set name.
	*/
	void deletePointSet( const Ogre::String& name );

	/**
	* Checks if a point set exists.
	*
	* @param name Point set name.
	* @return true if the point set exists, false otherwise.
	*/
	bool hasPointSet( const Ogre::String& name ) const;

	/**
	* Toggles rendering of a point set.
	*
	* @param name Point set name.
	*/
	void showPointSet( const Ogre::String& name, bool show = true );

	/**
	* OGRE calls this method at the beginning of a new frame.
	*/
	bool frameStarted( const FrameEvent& evt );

	/**
	* OGRE calls this method at the end of the current frame.
	*/
	bool frameEnded( const FrameEvent& evt );

	/**
	* Event handler for mouse movement.
	*/
	void OnMouseMove( wxMouseEvent& evt );

	/**
	* Event handler for mouse wheel movement.
	*/
	void OnMouseWheel( wxMouseEvent& evt );

	/**
	* Event handler for size changes.
	*/
	void OnSize( wxSizeEvent& evt );

	DECLARE_EVENT_TABLE()

private:

	void _createRenderSkeleton( zh::Bone* bone, Ogre::SceneNode* renderParent, Ogre::SceneNode* parentObj );
	ManualObject* _createBox( const Ogre::String& name, const Ogre::String& matName, float size );

	zh::Skeleton* mOutSkel; // ZombieHorse output skeleton
	Ogre::SceneNode* mRenderSkel;
	Ogre::Timer mFPSTimer; ///< Timer (for computing FPS).
	float mFPS; ///< FPS count.

};
