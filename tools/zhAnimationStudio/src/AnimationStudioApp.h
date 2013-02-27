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

#define zhIntNodeName_AnimPtr(anim) ( (anim)->getAnimationSet()->getName() + "#" + (anim)->getName() )
#define zhIntNodeName_AnimName(animSetName,animName) ( (animSetName) + "#" + (animName) )

/**
* @brief Animation Studio application class.
*/
class AnimationStudioApp : public wxApp, public FrameListener
{
	
public:

	typedef zh::MapIterator< std::map<std::string, Character*> > CharacterIterator;
	typedef zh::MapConstIterator< std::map<std::string, Character*> > CharacterConstIterator;

	typedef zh::MapIterator< std::map<std::string, AnimationSetPtr> > AnimationSetIterator;
	typedef zh::MapConstIterator< std::map<std::string, AnimationSetPtr> > AnimationSetConstIterator;

	typedef zh::MapIterator< std::map<std::string, AnimationIndexPtr> > AnimationIndexIterator;
	typedef zh::MapConstIterator< std::map<std::string, AnimationIndexPtr> > AnimationIndexConstIterator;

	typedef zh::MapIterator< std::map<std::string, AnimationTreePtr> > AnimationTreeIterator;
	typedef zh::MapConstIterator< std::map<std::string, AnimationTreePtr> > AnimationTreeConstIterator;

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
	* Loads a project file.
	*/
	bool loadProject();

	/**
	* Saves the current project.
	*/
	bool saveProject();

	/**
	* Saves the current project under new filename.
	*/
	bool saveProjectAs();

	/**
	* Unloads the current project.
	*/
	void unloadProject();

	/**
	* Gets the project name.
	*/
	const std::string& getProjectName() const;

	/**
	* Sets the project name.
	*/
	void setProjectName( const std::string& name );

	/**
	* Adds a character to the project.
	*/
	void addCharacter( Character* ch );

	/**
	* Creates a new character in the project.
	*/
	Character* createCharacter();

	/**
	* Deletes a character from the project.
	*
	* @param name Character name.
	*/
	void deleteCharacter( const std::string& name );

	/**
	* Deletes all characters from the project.
	*/
	void deleteAllCharacters();

	/**
	* Checks if the specified character exists.
	*
	* @param name Character name.
	* @return true if the character exists, false otherwise.
	*/
	bool hasCharacter( const std::string& name ) const;

	/**
	* Gets a character.
	*
	* @param name Character name.
	* @return Pointer to the character.
	*/
	Character* getCharacter( const std::string& name ) const;

	/**
	* Gets an iterator over the map of characters.
	*/
	CharacterIterator getCharacterIterator();

	/**
	* Gets a const iterator over the map of characters.
	*/
	CharacterConstIterator getCharacterConstIterator() const;

	/**
	* Gets the number of characters in the project.
	*/
	unsigned int getNumCharacters() const;

	/**
	* Selects the character with the specified name,
	* loading and displaying the corresponding OGRE scene
	* and animations.
	*
	* @param name Character name.
	* @return Pointer to the character or NULL if no character
	* has been selected.
	*/
	Character* selectCharacter( const string& name );

	/**
	* Gets the current character.
	*
	* @return Pointer to the character.
	*/
	Character* getCurrentCharacter() const;

	/**
	* Gets the scene/mesh filename for the specified character.
	*/
	const std::string& getSceneFilename( const std::string& chId ) const;

	/**
	* Sets the scene/mesh filename for the specified character.
	*/
	void setSceneFilename( const std::string& chId, const std::string& filename );

	/**
	* Gets the entity name for the specified character.
	*/
	const std::string& getEntityName( const std::string& chId ) const;

	/**
	* Sets the entity name for the specified character.
	*/
	void setEntityName( const std::string& chId, const std::string& name );

	/**
	* Checks if the specified bone is an end-effector.
	*
	* @param chId Character name.
	* @param name End-effector name.
	* @return true if the bone is an end-effector, false otherwise.
	*/
	bool getEndEffector( const std::string& chId, const std::string& name ) const;

	/**
	* Designates the specified bone as an end-effector.
	*
	* @param chId Character name.
	* @param name Bone name.
	* @param endEffector If true, the bone is designated as an end-effector.
	*/
	void setEndEffector( const std::string& chId, const std::string& name, bool endEffector = true );

	/**
	* Gets the set of end-effectors
	* on the specified character.
	*
	* @param chId Character name.
	*/
	const std::set<std::string>& getEndEffectorSet( const std::string& chId ) const;

	/**
	* Gets the number of raw animation sets on the specified character.
	*/
	unsigned int getNumEndEffectors( const std::string& chId ) const;

	/**
	* Adds a set of raw animations to the specified character.
	*
	* @param chId Character name.
	* @param animSet Raw animation set.
	*/
	void addRawAnimationSet( const std::string& chId, AnimationSetPtr animSet );

	/**
	* Adds a set of raw animations to the specified character.
	*
	* @param chId Character name.
	*/
	AnimationSetPtr addRawAnimationSet( const std::string& chId );

	/**
	* Removes a set of raw animations from the specified character.
	*
	* @param chId Character name.
	* @param name Animation set name.
	*/
	void removeRawAnimationSet( const std::string& chId, const std::string& name );

	/**
	* Removes all raw animations from the specified character.
	*
	* @param chId Character name.
	*/
	void removeAllRawAnimationSets( const std::string& chId );

	/**
	* Removes the specified animation clip from its animation set.
	*/
	void removeRawAnimation( const std::string& chId, const std::string& animSetName, const std::string& animName );

	/**
	* Checks if the specified raw animation set exists
	* on the specified character.
	*
	* @param chId Character name.
	* @param name Animation set name.
	* @return true if the animation set exists, false otherwise.
	*/
	bool hasRawAnimationSet( const std::string& chId, const std::string& name ) const;

	/**
	* Gets a raw animation set from the specified character.
	*
	* @param chId Character name.
	* @param name Animation set name.
	*/
	zh::AnimationSetPtr getRawAnimationSet( const std::string& chId, const string& name ) const;

	/**
	* Gets an iterator over the map of raw animations
	* on the specified character.
	*
	* @param chId Character name.
	*/
	AnimationSetIterator getRawAnimationSetIterator( const std::string& chId );

	/**
	* Gets a const iterator over the map of raw animations
	* on the specified character.
	*
	* @param chId Character name.
	*/
	AnimationSetConstIterator getRawAnimationSetConstIterator( const std::string& chId ) const;

	/**
	* Gets the number of raw animation sets on the specified character.
	*/
	unsigned int getNumRawAnimationSets( const std::string& chId ) const;

	/**
	* Adds an animation index to the specified character.
	*
	* @param chId Character name.
	* @param animIndex Animation index.
	*/
	void addAnimationIndex( const std::string& chId, AnimationIndexPtr animIndex );

	/**
	* Adds an animation index to the specified character.
	*
	* @param chId Character name.
	*/
	AnimationIndexPtr addAnimationIndex( const std::string& chId );

	/**
	* Removes an animation index from the specified character.
	*
	* @param chId Character name.
	* @param name Animation index name.
	*/
	void removeAnimationIndex( const std::string& chId, const std::string& name );

	/**
	* Removes all animation indexes from the specified character.
	*
	* @param chId Character name.
	*/
	void removeAllAnimationIndexes( const std::string& chId );

	/**
	* Checks if an specified animation index exists on the specified character.
	*
	* @param chId Character name.
	* @param name Animation index name.
	* @return true if the animation index exists, false otherwise.
	*/
	bool hasAnimationIndex( const std::string& chId, const std::string& name ) const;

	/**
	* Gets an animation index from the specified character.
	*
	* @param chId Character name.
	* @param name Animation index name.
	* @return Animation index.
	*/
	AnimationIndexPtr getAnimationIndex( const std::string& chId, const string& name ) const;

	/**
	* Gets an iterator over the map of animation indexes
	* on the specified character.
	*
	* @param chId Character name.
	* @return Animation index iterator.
	*/
	AnimationIndexIterator getAnimationIndexIterator( const std::string& chId );

	/**
	* Gets a const iterator over the map of animation indexes
	* on the specified character.
	*
	* @param chId Character name.
	* @return Animation index iterator.
	*/
	AnimationIndexConstIterator getAnimationIndexConstIterator( const std::string& chId ) const;

	/**
	* Gets the number of animation indexes on the specified character.
	*/
	unsigned int getNumAnimationIndexes( const std::string& chId ) const;

	/**
	* Creates a new animation set on the specified character.
	*
	* @param chId Character name.
	* @return Animation set.
	*/
	AnimationSetPtr createAnimationSet( const std::string& chId );

	/**
	* Adds an animation set to the specified character.
	*/
	void addAnimationSet( const std::string& chId, AnimationSetPtr animSet );

	/**
	* Adds an animation set to the specified character.
	*/
	AnimationSetPtr addAnimationSet( const std::string& chId );

	/**
	* Removes an animation set from the specified character.
	*
	* @param chId Character name.
	* @param name Animation set name.
	*/
	void removeAnimationSet( const std::string& chId, const std::string& name );

	/**
	* Removes all animation sets from the specified character.
	*/
	void removeAllAnimationSets( const std::string& chId );

	/**
	* Removes the specified animation clip from its animation set.
	*/
	void removeAnimation( const std::string& chId, const std::string& animSetName, const std::string& animName );

	/**
	* Removes the specified animation space from its animation set.
	*/
	void removeAnimationSpace( const std::string& chId, const std::string& animSetName, const std::string& animSpaceName );

	/**
	* Removes all animation clips and animation spaces
	* from the specified animation set.
	*/
	void removeAllAnimations( const std::string& chId, const std::string& animSetName );

	/**
	* Checks if the specified animation set exists
	* on the specified character.
	*
	* @param chId Character name.
	* @param name Animation set name.
	* @return true if the animation set exists, false otherwise.
	*/
	bool hasAnimationSet( const std::string& chId, const std::string& name ) const;

	/**
	* Gets an animation set on the specified character.
	*
	* @param chId Character name.
	* @param name Animation set name.
	*/
	AnimationSetPtr getAnimationSet( const std::string& chId, const string& name ) const;

	/**
	* Gets an iterator over the map of animation sets
	* on the specified character.
	*/
	AnimationSetIterator getAnimationSetIterator( const std::string& chId );

	/**
	* Gets a const iterator over the map of animation sets
	* on the specified character.
	*/
	AnimationSetConstIterator getAnimationSetConstIterator( const std::string& chId ) const;

	/**
	* Gets the number of animation sets on the specified character.
	*/
	unsigned int getNumAnimationSets( const std::string& chId ) const;

	/**
	* Creates a new animation tree on the specified character.
	*/
	AnimationTreePtr createAnimationTree( const std::string& chId );

	/**
	* Adds an animation tree to the specified character.
	*/
	void addAnimationTree( const std::string& chId, AnimationTreePtr animTree );

	/**
	* Adds an animation tree to the specified character.
	*/
	AnimationTreePtr addAnimationTree( const std::string& chId );

	/**
	* Removes an animation tree from the specified character.
	*
	* @param name Animation tree name.
	*/
	void removeAnimationTree( const std::string& chId, const std::string& name );

	/**
	* Removes all animation trees from the specified character.
	*/
	void removeAllAnimationTrees( const std::string& chId );

	/**
	* Checks if the specified animation tree exists
	* on the specified character.
	*
	* @param name Animation tree name.
	* @return true if the animation tree exists, false otherwise.
	*/
	bool hasAnimationTree( const std::string& chId, const std::string& name ) const;

	/**
	* Gets an animation tree on the specified character.
	*
	* @param name Animation tree name.
	*/
	AnimationTreePtr getAnimationTree( const std::string& chId, const string& name ) const;

	/**
	* Gets an iterator over the map of animation trees
	* on the specified character.
	*/
	AnimationTreeIterator getAnimationTreeIterator( const std::string& chId );

	/**
	* Gets a const iterator over the map of animation trees
	* on the specified character.
	*/
	AnimationTreeConstIterator getAnimationTreeConstIterator( const std::string& chId ) const;

	/**
	* Gets the number of animation trees on the specified character.
	*/
	unsigned int getNumAnimationTrees( const std::string& chId ) const;

	/**
	* Selects the specified animation tree for viewing and editing.
	*
	* @param name Animation tree name.
	*/
	AnimationTreeInstance* selectAnimationTree( const std::string& name = zhIntAnimTree_Name );

	/**
	* Gets the currently selected animation tree.
	*/
	AnimationTreeInstance* getCurrentAnimationTree() const;

	/**
	* Gets the internal animation tree used
	* for viewing animations.
	*/
	AnimationTreeInstance* getInternalAnimationTree() const;

	/**
	* Selects the specified animation for playback and editing.
	*
	* @param Animation name.
	*/
	AnimationNode* selectAnimation( const std::string& animSetName, const std::string& animName );

	/**
	* Stops and unselects currently selected animation.
	*/
	void stopAnimation();

	/**
	* Applies a specific animation pose (animation at specified time).
	*
	* @remark Disable animation updates prior to applying animation
	* by setting setAnimationEnabled(false).
	*/
	void applyAnimation( const std::string& animSetName, const std::string& animName, float time );

	/**
	* Gets the currently selected animation node.
	*
	* @param Pointer to the animation node.
	*/
	zh::AnimationNode* getCurrentAnimationNode() const;

	/**
	* Adds nodes corresponding to contents of an animation set
	* to the animation tree and attaches them to a parent node
	* if specified.
	*/
	void _addAnimsToAnimTree( AnimationSetPtr animSet, AnimationTreePtr animTree, AnimationNode* parent = NULL );

	/**
	* Removes nodes corresponding to contents of an animation set
	* from the animation tree.
	*/
	void _removeAnimsFromAnimTree( AnimationSetPtr animSet, AnimationTreePtr animTree );

	/**
	* Resizes the render window.
	*/
	void resize( unsigned int width, unsigned int height );

	/**
	* If true, animation is enabled (updates are sent to visage7 system).
	*/
	bool getAnimationEnabled() const { return mAnimEnabled; }

	/**
	* If true, animation is enabled (updates are sent to visage7 system).
	*/
	void setAnimationEnabled( bool enabled = true ) { mAnimEnabled = enabled; }

	/**
	* OGRE calls this method at the beginning of a new frame.
	*/
	bool frameStarted( const FrameEvent& evt );

	/**
	* OGRE calls this method at the end of the current frame.
	*/
	bool frameEnded( const FrameEvent& evt );

	/**
	* This method is called on application initialization.
	*/
	bool OnInit();

	/**
	* This method is called when there are no messages left to process
	* (extremely often, so this is where we put the render loop).
	*/
	void OnIdle( wxIdleEvent& evt );

protected:

	bool init( wxWindow* wnd = NULL ); ///< Initializes OGRE and visage7.
	bool initOgreCore( wxWindow* renderWnd = NULL ); ///< Initializes OGRE system core.
	bool createSceneManager(); ///< Creates a scene manager.
	bool createCamera(); ///< Creates a camera.
	bool createViewports(); ///< Creates viewports.
	bool createFrameListener(); ///< Sets up a frame listener.
	bool loadResourceLocations(); ///< Loads resource locations from resources.cfg.
	bool createScene(); ///< Creates the default scene.
	bool destroyScene(); ///< Destroys the default scene.
	bool initVisage7(); ///< Initializes visage7 system.

	DECLARE_EVENT_TABLE()

	AnimationStudioFrame* mFrmMain;

	Root *mOgreRoot; ///< OGRE system.
	Camera* mCam; ///< Camera.
	SceneManager* mSceneMgr; ///< Scene manager.
	RenderWindow* mRenderWnd; ///< OGRE render window.

	bool mAnimEnabled; ///< Updates to visage7 system disabled.

	//
	// Current project content:
	//

	std::string mProjName; ///< Project name.
	std::map<std::string, Character*> mChars; ///< visage7 characters by name.
	std::map<std::string, std::string> mScenes; ///< OGRE scene/mesh files by character name.
	std::map<std::string, std::string> mEntities; ///< OGRE entities by character name.
	std::map<std::string, std::set<std::string> > mEndEffectors; ///< End-effectors by character name.
	std::map<std::string, std::map<std::string, AnimationSetPtr> > mRawAnims; ///< Raw animation sets by name by character name.
	std::map<std::string, std::map<std::string, AnimationIndexPtr> > mAnimIndexes; ///< Animation indexes by name by character name.
	std::map<std::string, std::map<std::string, AnimationSetPtr> > mAnimSets; ///< Animation sets by name by character name.
	std::map<std::string, std::map<std::string, AnimationTreePtr> > mAnimTrees; ///< Animation trees by name by character name.

	//
	// Currently selected resources:
	//

	Character* mChar; ///< Currently selected character.
	zh::AnimationNode* mAnim; ///< Currently selected animation.
	zh::AnimationTreeInstance* mAnimTree; ///< Currently selected animation tree.

	//
	// Resource dirty flags:
	//

	bool mDirty; ///< If true, current project needs to be saved.
	std::set<AnimationSetPtr> mDirtyAnims; ///< Animation sets that need to be saved.
	std::set<AnimationIndexPtr> mDirtyAnimIndexes; ///< Animation indexes that need to be saved.
	std::set<AnimationTreePtr> mDirtyAnimTrees; ///< Animation indexes that need to be saved.

};

extern AnimationStudioApp* gApp;

#endif // __AnimationStudioApp_h__
