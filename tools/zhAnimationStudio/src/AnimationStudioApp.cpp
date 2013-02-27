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

#include "AnimationStudioApp.h"
#include "AnimationStudioFrame.h"
#include "ProjectViewWindow.h"
#include "PropertiesWindow.h"
#include "AnimTreeWindow.h"
#include "OgreWindow.h"
#include "NewResourceDialog.h"
#include "DotSceneLoader.h"
#include "zhOgreModelController.h"
// TODO: remove this?
#include <boost/tokenizer.hpp>
//
// TODO: remove this
#include "AnimationNodePropGrid.h"
#include "AnimationTransitionBlenderPropGrid.h"
//

AnimationStudioApp* gApp = NULL;

AnimationStudioApp::AnimationStudioApp()
: mOgreRoot(NULL), mCam(NULL), mSceneMgr(NULL), mRenderWnd(NULL), mAnimEnabled(true),
mProjName("Untitled"), mChar(NULL), mAnim(NULL), mAnimTree(NULL)
{
}

AnimationStudioApp::~AnimationStudioApp()
{
	if( mOgreRoot != NULL ) delete mOgreRoot;
}

AnimationStudioFrame* AnimationStudioApp::getAppFrame() const
{
	return mFrmMain;
}

Camera* AnimationStudioApp::getCamera() const
{
	return mCam;
}

SceneManager* AnimationStudioApp::getSceneManager() const
{
	return mSceneMgr;
}

bool AnimationStudioApp::loadProject()
{
	// TODO: prompt the user to save changes
	unloadProject();

	// TODO: implement .vproj file loader
	// until then, use this hard-coded project
	setProjectName("visage7_test");

	// add characters
	addCharacter( zhAnimationSystem->createCharacter( "Jeanette", "OgreCharacterTemplate" ) );
	addCharacter( zhAnimationSystem->createCharacter( "Ninja", "OgreCharacterTemplate" ) );

	// add OGRE resource directories
	mOgreRoot->addResourceLocation( "../../samples/data/OGRE/models", "FileSystem", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME );
	mOgreRoot->addResourceLocation( "../../samples/data/OGRE/scenes", "FileSystem", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME );
	mOgreRoot->addResourceLocation( "../../samples/data/OGRE/materials/scripts", "FileSystem", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME );
	mOgreRoot->addResourceLocation( "../../samples/data/OGRE/materials/programs", "FileSystem", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME );
	mOgreRoot->addResourceLocation( "../../samples/data/OGRE/materials/textures", "FileSystem", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME );

	// add visage7 resource directories
	zhAnimationSystem->getAnimationManager()->addResourceDirectory( "../../samples/data/animations/Jeanette" );
	zhAnimationSystem->getAnimationManager()->addResourceDirectory( "../../samples/data/animations/Ninja" );
	zhAnimationSearchSystem->getAnimationIndexManager()->addResourceDirectory( "../../samples/data/animindexes/Jeanette" );
	zhAnimationSystem->getAnimationTreeManager()->addResourceDirectory( "../../samples/data/animtrees" );

	// set resources for character Jeanette
	setSceneFilename( "Jeanette", "jeanette.scene" );
	setEntityName( "Jeanette", "body" );
	AnimationSetPtr anim_set = AnimationSetPtr::DynamicCast<zh::Resource>( zhAnimationSystem->getAnimationManager()->createResource( 1, "AS_JeanetteRaw" ) );
	zhAnimationSystem->getAnimationManager()->loadResource( 1, "AS_JeanetteRaw.va", true );
	addRawAnimationSet( "Jeanette", anim_set ); // TODO: disabled to reduce loading time, enable if needed
	AnimationIndexPtr anim_index = AnimationIndexPtr::DynamicCast<zh::Resource>( zhAnimationSearchSystem->getAnimationIndexManager()->createResource( 0, "AI_JeanetteWalk1" ) );
	addAnimationIndex( "Jeanette", anim_index ); // TODO: disabled to reduce loading time, enable if needed
	zhAnimationSearchSystem->getAnimationIndexManager()->loadResource( 0, "AI_JeanetteWalk1.vai", true );
	anim_index = AnimationIndexPtr::DynamicCast<zh::Resource>( zhAnimationSearchSystem->getAnimationIndexManager()->createResource( 1, "AI_JeanetteReachLeft1" ) );
	addAnimationIndex( "Jeanette", anim_index ); // TODO: disabled to reduce loading time, enable if needed
	zhAnimationSearchSystem->getAnimationIndexManager()->loadResource( 1, "AI_JeanetteReachLeft1.vai", true );
	anim_index = AnimationIndexPtr::DynamicCast<zh::Resource>( zhAnimationSearchSystem->getAnimationIndexManager()->createResource( 2, "AI_JeanetteReachRight1" ) );
	addAnimationIndex( "Jeanette", anim_index ); // TODO: disabled to reduce loading time, enable if needed
	zhAnimationSearchSystem->getAnimationIndexManager()->loadResource( 2, "AI_JeanetteReachRight1.vai", true );
	anim_set = AnimationSetPtr::DynamicCast<zh::Resource>( zhAnimationSystem->getAnimationManager()->createResource( 3, "AS_Jeanette" ) );
	addAnimationSet( "Jeanette", anim_set );
	zhAnimationSystem->getAnimationManager()->loadResource( 3, "AS_Jeanette.va", true );
	AnimationTreePtr anim_tree;
	//anim_tree = AnimationTreePtr::DynamicCast<AnimationTree>( zhAnimationSystem->getAnimationTreeManager()->createResource( 0, "AT_Jeanette" );
	//zhAnimationSystem->getAnimationTreeManager()->loadResource( 0, "AT_Jeanette.vat", true );
	// TODO: remove this
	// fill up anim. tree
	//
	//addAnimationTree( "Jeanette", anim_tree );

	// set resources for character Ninja
	setSceneFilename( "Ninja", "ninja.scene" );
	setEntityName( "Ninja", "Ninja" );
	anim_set = AnimationSetPtr::DynamicCast<zh::Resource>( zhAnimationSystem->getAnimationManager()->createResource( 2, "AS_Ninja" ) );
	addAnimationSet( "Ninja", anim_set );
	zhAnimationSystem->getAnimationManager()->loadResource( 2, "AS_Ninja.va", true );
	anim_tree = AnimationTreePtr::DynamicCast<zh::Resource>( zhAnimationSystem->getAnimationTreeManager()->createResource( 1, "AT_Ninja" ) );
	zhAnimationSystem->getAnimationTreeManager()->loadResource( 1, "AT_Ninja.vat", true );
	// TODO: remove this
	// fill up anim. tree
	zhAnimationSystem->getAnimationManager()->loadResource(2);
	AnimationSampler* node = NULL;
	AnimationTransitionBlender* root = static_cast<AnimationTransitionBlender*>( anim_tree->createNode( AnimationTransitionBlender::ClassId(), 1000, "Root" ) );
	anim_tree->setRoot(root);
	AnimationSet::AnimationConstIterator anim_i = anim_set->getAnimationConstIterator();
	while( !anim_i.end() )
	{
		zh::Animation* anim = anim_i.next();
		node = static_cast<AnimationSampler*>(
			anim_tree->createNode( AnimationSampler::ClassId(), anim->getId(), anim->getName() )
			);
		node->setAnimation( anim->getAnimationSet(), anim->getId() );
		root->addChild(node);
	}
	if( node != NULL )
	{
		root->setDefaultNode( "Idle1" );
		root->setDefaultTransitionLength(0);
	}
	BoneTransformController* bc = static_cast<BoneTransformController*>( 
		anim_tree->createBoneController( BoneTransformController::ClassId(), 100, "BC" ) );
	anim_tree->setFirst(bc);
	zhAnimationSystem->getAnimationManager()->unloadResource(2);
	//
	addAnimationTree( "Ninja", anim_tree );

	// define end-effectors for character Jeanette
	setEndEffector( "Jeanette", "Bip01 L Foot" );
	setEndEffector( "Jeanette", "Bip01 R Foot" );

	// select initial character
	selectCharacter( "Jeanette" );
	
	// update view settings
	// TODO
	//

	mFrmMain->getProjectViewWindow()->refresh();

	return true;
}

bool AnimationStudioApp::saveProject()
{
	// TODO: save project info
	// TODO: implement resource dirty flags

	if( mChar == NULL )
		return true;
	std::string chid = mChar->getId();

	// serialize raw animations
	AnimationSetConstIterator rai = getRawAnimationSetConstIterator(chid);
	while( !rai.end() )
	{
		AnimationSetPtr anim_set = rai.next();
		zhAnimationSystem->getAnimationManager()->serializeResource( anim_set->getId() );
	}

	// serialize animation indexes
	AnimationIndexConstIterator aii = getAnimationIndexConstIterator(chid);
	while( !aii.end() )
	{
		AnimationIndexPtr anim_index = aii.next();
		zhAnimationSearchSystem->getAnimationIndexManager()->serializeResource( anim_index->getId() );
	}

	// serialize animations
	AnimationSetConstIterator ai = getAnimationSetConstIterator(chid);
	while( !ai.end() )
	{
		AnimationSetPtr anim_set = ai.next();
		zhAnimationSystem->getAnimationManager()->serializeResource( anim_set->getId() );
	}

	// serialize animations
	AnimationTreeConstIterator ati = getAnimationTreeConstIterator(chid);
	while( !ati.end() )
	{
		AnimationTreePtr anim_tree = ati.next();
		zhAnimationSystem->getAnimationTreeManager()->serializeResource( anim_tree->getId() );
	}

	return true;
}

bool AnimationStudioApp::saveProjectAs()
{
	return true;
}

void AnimationStudioApp::unloadProject()
{
	destroyScene();
	deleteAllCharacters();
}

const std::string& AnimationStudioApp::getProjectName() const
{
	return mProjName;
}

void AnimationStudioApp::setProjectName( const std::string& name )
{
	mProjName = name;
}

void AnimationStudioApp::addCharacter( Character* ch )
{
	zhAssert( ch != NULL );

	ch->setEnabled(false);
	mChars[ ch->getId() ] = ch;
	mScenes[ ch->getId() ] = "";
	mEndEffectors[ ch->getId() ] = std::set<std::string>();
}

Character* AnimationStudioApp::createCharacter()
{
	// TODO
	// - pop dialog for defining character properties (name, .scene/.mesh file, initial anim. set)
	// - create character in the system
	// - add character to mChars
	// - don't forget to add scene name and empty end-effector set
	// - create initial anim. set in the system
	// - add anim. set to mAnimSets
	// - disable the character
	// - if this is the first character, select it

	return NULL;
}

void AnimationStudioApp::deleteCharacter( const std::string& name )
{
	stopAnimation();

	// TODO
	// - if this character is selected, select another one (or none)
	// - delete all character resources from the system
	// - remove all character resources from project
	// - remove character from mChars
	// - delete character from the system
}

void AnimationStudioApp::deleteAllCharacters()
{
	while( mChars.size() > 0 )
		deleteCharacter( mChars.begin()->second->getId() );
}

bool AnimationStudioApp::hasCharacter( const std::string& name ) const
{
	return mChars.count(name) > 0;
}

Character* AnimationStudioApp::getCharacter( const std::string& name ) const
{
	std::map<std::string, Character*>::const_iterator chi = mChars.find(name);
	
	if( chi != mChars.end() )
		return chi->second;

	return NULL;
}

AnimationStudioApp::CharacterIterator AnimationStudioApp::getCharacterIterator()
{
	return CharacterIterator(mChars);
}

AnimationStudioApp::CharacterConstIterator AnimationStudioApp::getCharacterConstIterator() const
{
	return CharacterConstIterator(mChars);
}

unsigned int AnimationStudioApp::getNumCharacters() const
{
	return mChars.size();
}

Character* AnimationStudioApp::selectCharacter( const string& name )
{
	if( mChar != NULL && mChar->getId() == name )
	{
		// character already selected
		return mChar;
	}

	if( !hasCharacter(name) && name != "" )
	{
		// can't find a character by that name
		wxMessageBox( "Unable to find character " + name + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return NULL;
	}

	// reset scene
	stopAnimation();
	destroyScene();
	createScene();

	if( mChar != NULL )
	{
		mChar->setEnabled(false);

		std::string old_name = mChar->getId();

		// remove internal anim. tree instance
		mChar->getAnimationController()->removeAnimationTree(zhIntAnimTree_Name);

		// remove model entities
		OgreModelController* omc = static_cast<OgreModelController*>( mChar->getModelController() );
		omc->removeAllEntities();

		// unload character resources
		for( std::map<std::string, AnimationTreePtr>::iterator ati = mAnimTrees[old_name].begin(); ati != mAnimTrees[old_name].end(); ++ati )
		{
			zhAnimationSystem->getAnimationTreeManager()->unloadResource( ati->second->getId() );
		}
		for( std::map<std::string, AnimationSetPtr>::iterator asi = mAnimSets[old_name].begin(); asi != mAnimSets[old_name].end(); ++asi )
		{
			zhAnimationSystem->getAnimationManager()->unloadResource( asi->second->getId() );
		}
		for( std::map<std::string, AnimationIndexPtr>::iterator aii = mAnimIndexes[old_name].begin(); aii != mAnimIndexes[old_name].end(); ++aii )
		{
			zhAnimationSearchSystem->getAnimationIndexManager()->unloadResource( aii->second->getId() );
		}
		for( std::map<std::string, AnimationSetPtr>::iterator rai = mRawAnims[old_name].begin(); rai != mRawAnims[old_name].end(); ++rai )
		{
			zhAnimationSystem->getAnimationManager()->unloadResource( rai->second->getId() );
		}
	}

	// delete internal animation tree
	zhAnimationSystem->getAnimationTreeManager()->deleteResource(zhIntAnimTree_Name);

	if( name == "" )
	{
		// don't select a new character

		mChar = NULL;
		mAnim = NULL;
		mAnimTree = NULL;

		return mChar;
	}

	// load OGRE scene
	DotSceneLoader* loader = new DotSceneLoader();
	try
	{
		loader->parseDotScene( mScenes[name], "General", mSceneMgr, mSceneMgr->getRootSceneNode() );
	}
	catch(...)
	{
		wxMessageBox( "Unable to load scene. Did you remember to set your resource directories correctly?",
			"Error", wxOK | wxICON_EXCLAMATION );
		delete loader;

		mChar = NULL;
		mAnim = NULL;
		mAnimTree = NULL;

		return mChar;
	}
	delete loader;

	// check if entity is loaded
	std::string ent_name = getEntityName(name);
	if( !mSceneMgr->hasEntity(ent_name) )
	{
		wxMessageBox( "Unable to load scene. Entity " + ent_name + " failed to load.",
			"Error", wxOK | wxICON_EXCLAMATION );
		
		mChar = NULL;
		mAnim = NULL;
		mAnimTree = NULL;

		return mChar;
	}

	/* TODO: remove this
	mSceneMgr->getEntity( "body" )->getAnimationState( "WalkForward2" )->setEnabled(true);
	return true;
	*/

	// select, init. and enable the character
	mChar = getCharacter(name);
	OgreModelController* omc = static_cast<OgreModelController*>( mChar->getModelController() );
	omc->setSceneManager(mSceneMgr);
	omc->addEntity( mSceneMgr->getEntity( getEntityName(name) ) );
	omc->init();
	mChar->setEnabled(true);

	// load character resources
	for( std::map<std::string, AnimationSetPtr>::iterator rai = mRawAnims[name].begin(); rai != mRawAnims[name].end(); ++rai )
	{
		zh::ResourcePtr res = rai->second;
		if( res->getState() == ResourceState_Unloaded && !zhAnimationSystem->getAnimationManager()->loadResource( res->getId() ) )
		{
			// TODO: warn user that resource failed to load
		}
	}
	for( std::map<std::string, AnimationIndexPtr>::iterator aii = mAnimIndexes[name].begin(); aii != mAnimIndexes[name].end(); ++aii )
	{
		zh::ResourcePtr res = aii->second;
		if( res->getState() == ResourceState_Unloaded && !zhAnimationSearchSystem->getAnimationIndexManager()->loadResource( res->getId() ) )
		{
			// TODO
		}
	}
	for( std::map<std::string, AnimationSetPtr>::iterator asi = mAnimSets[name].begin(); asi != mAnimSets[name].end(); ++asi )
	{
		zh::ResourcePtr res = asi->second;
		if( res->getState() == ResourceState_Unloaded && !zhAnimationSystem->getAnimationManager()->loadResource( res->getId() ) )
		{
			// TODO
		}
	}
	for( std::map<std::string, AnimationTreePtr>::iterator ati = mAnimTrees[name].begin(); ati != mAnimTrees[name].end(); ++ati )
	{
		zh::ResourcePtr res = ati->second;
		if( res->getState() == ResourceState_Unloaded && !zhAnimationSystem->getAnimationTreeManager()->loadResource( res->getId() ) )
		{
			// TODO
		}
	}

	// create internal animation tree
	unsigned long atid = 0;
	while( zhAnimationSystem->getAnimationTreeManager()->hasResource(atid) ) ++atid;
	AnimationTreePtr atree_int = AnimationTreePtr::DynamicCast<zh::Resource>( zhAnimationSystem->getAnimationTreeManager()->createResource( atid, zhIntAnimTree_Name ) );
	AnimationTransitionBlender* root = static_cast<AnimationTransitionBlender*>(
		atree_int->createNode( AnimationTransitionBlender::ClassId(), 0, "Root" ) );
	atree_int->setRoot(root);

	std::vector<zh::Bone*> ebones; // end-effectors
	zh::Skeleton* skel = mChar->getModelController()->getModel()->getSkeleton();
	const std::set<std::string>& eeset = getEndEffectorSet( mChar->getId() );
	for( std::set<std::string>::const_iterator eei = eeset.begin(); eei != eeset.end(); ++eei )
		if( skel->hasBone(*eei) )
			ebones.push_back( skel->getBone(*eei) );
	
	// create bone controllers for plant constr. enforcement
	for( unsigned int ebone_i = 0; ebone_i < ebones.size(); ++ebone_i )
	{
		zh::Bone* ebone = ebones[ebone_i];
		BoneIKController* ebc = static_cast<BoneIKController*>( atree_int->createBoneController( BoneIKController::ClassId(), ebone_i, "IK_" + ebone->getName() ) );
		ebc->setBoneId( ebone->getId() );
		ebc->setIKSolver(IKSolver_CCD);
		ebc->setEnabled(false);

		if( atree_int->getFirst() == NULL )
			atree_int->setFirst(ebc);
		else
			atree_int->getLast()->setNext(ebc);

		/* TODO: remove this
		if( ebone->getName() == "Bip01 L Foot" )
		{
			zh::Vector3 pos( 0, 0, 0 );
			ebc->setTargetPosition(pos);
			ebc->setActive(true);

			std::vector<Ogre::Vector3> pts;
			pts.push_back( zhOgreVector3(pos) );
			getAppFrame()->getOgreWindow()->deletePointSet( "IKTarget" );
			getAppFrame()->getOgreWindow()->createPointSet( "IKTarget", pts, Ogre::ColourValue( 1, 0, 0 ), 10 );
		}
		*/
	}

	// fill internal anim. tree with nodes
	for( std::map<std::string, AnimationSetPtr>::iterator rai = mRawAnims[name].begin(); rai != mRawAnims[name].end(); ++rai )
	{
		AnimationSetPtr anim_set = rai->second;
		_addAnimsToAnimTree( anim_set, atree_int, root );
	}
	for( std::map<std::string, AnimationSetPtr>::iterator asi = mAnimSets[name].begin(); asi != mAnimSets[name].end(); ++asi )
	{
		AnimationSetPtr anim_set = asi->second;
		_addAnimsToAnimTree( anim_set, atree_int, root );
	}

	// add internal anim. tree to the character
	mChar->getAnimationController()->addAnimationTree(atree_int);

	// use the internal anim. tree
	selectAnimationTree(zhIntAnimTree_Name);

	return mChar;
}

Character* AnimationStudioApp::getCurrentCharacter() const
{
	return mChar;
}

const std::string& AnimationStudioApp::getSceneFilename( const std::string& chId ) const
{
	zhAssert( hasCharacter(chId) );
	
	return mScenes.find(chId)->second;
}

void AnimationStudioApp::setSceneFilename( const std::string& chId, const std::string& filename )
{
	zhAssert( hasCharacter(chId) );

	mScenes[chId] = filename;
}

const std::string& AnimationStudioApp::getEntityName( const std::string& chId ) const
{
	zhAssert( hasCharacter(chId) );
	
	return mEntities.find(chId)->second;
}

void AnimationStudioApp::setEntityName( const std::string& chId, const std::string& name )
{
	zhAssert( hasCharacter(chId) );

	mEntities[chId] = name;
}

bool AnimationStudioApp::getEndEffector( const std::string& chId, const std::string& name ) const
{
	if( !hasCharacter(chId) )
	{
		// can't find a character by that name
		wxMessageBox( "Unable to find character " + chId + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return false;
	}

	return mEndEffectors.find(chId)->second.count(name) > 0;
}

void AnimationStudioApp::setEndEffector( const std::string& chId, const std::string& name, bool endEffector )
{
	if( !hasCharacter(chId) )
	{
		// can't find a character by that name
		wxMessageBox( "Unable to find character " + chId + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return;
	}

	if( endEffector )
		mEndEffectors[chId].insert(name);
	else
		mEndEffectors[chId].erase(name);
}

const std::set<std::string>& AnimationStudioApp::getEndEffectorSet( const std::string& chId ) const
{
	zhAssert( hasCharacter(chId) );

	return mEndEffectors.find(chId)->second;
}

void AnimationStudioApp::addRawAnimationSet( const std::string& chId, AnimationSetPtr animSet )
{
	zhAssert( animSet != NULL );

	if( !hasCharacter(chId) )
	{
		// can't find a character by that name
		wxMessageBox( "Unable to find character " + chId + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return;
	}

	stopAnimation();

	// add animation set to project
	mRawAnims[chId].insert( make_pair( animSet->getName(), animSet ) );

	if( mChar != getCharacter(chId) )
		return;

	// remove outdated internal anim. tree
	mChar->getAnimationController()->removeAnimationTree( zhIntAnimTree_Name );

	// add new nodes to internal anim. tree
	AnimationTreePtr atree_int = AnimationTreePtr::DynamicCast<zh::Resource>( zhAnimationSystem->getAnimationTreeManager()->getResource( zhIntAnimTree_Name ) );
	_addAnimsToAnimTree( animSet, atree_int, atree_int->getRoot() );

	// create new internal anim. tree instance
	mChar->getAnimationController()->addAnimationTree(atree_int);
}

AnimationSetPtr AnimationStudioApp::addRawAnimationSet( const std::string& chId )
{
	if( !hasCharacter(chId) )
	{
		// can't find a character by that name
		wxMessageBox( "Unable to find character " + chId + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return NULL;
	}

	stopAnimation();

	// TODO
	// - pop dialog for defining raw anim. set properties (.va/.vab file)
	// - create/load raw anim. set in the system
	// - add raw anim. set to project (mRawAnims)
	// - if the character is selected, update internal anim. tree (see prev. function)

	return NULL;
}

void AnimationStudioApp::removeRawAnimationSet( const std::string& chId, const std::string& name )
{
	if( !hasCharacter(chId) )
	{
		// can't find a character by that name
		wxMessageBox( "Unable to find character " + chId + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return;
	}

	stopAnimation();

	// TODO
	// - if this raw anim. set is referenced in an anim. index, warn the user
	// - remove raw anim. set from mRawAnims
	// - delete raw anim. set from the system
	//    - if unused, delete it
	//    - if used by other characters, unload it
	// - if the character is selected, update internal anim. tree (see prev. function)
}

void AnimationStudioApp::removeAllRawAnimationSets( const std::string& chId )
{
	if( !hasCharacter(chId) )
	{
		// can't find a character by that name
		wxMessageBox( "Unable to find character " + chId + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return;
	}

	while( !mRawAnims[chId].empty() )
		removeRawAnimationSet( chId, mRawAnims[chId].begin()->second->getName() );
}

void AnimationStudioApp::removeRawAnimation( const std::string& chId, const std::string& animSetName, const std::string& animName )
{
	if( !hasCharacter(chId) )
	{
		// can't find a character by that name
		wxMessageBox( "Unable to find character " + chId + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return;
	}

	if( !hasRawAnimationSet( chId, animSetName ) )
	{
		// can't find an animation set by that name
		wxMessageBox( "Unable to find animation set " + animSetName + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return;
	}

	stopAnimation();

	// TODO
	// - if this raw anim. is referenced in an anim. index, warn the user
	// - delete raw anim. from its raw anim. set
	// - if the character is selected, update internal anim. tree (see prev. function)
}

bool AnimationStudioApp::hasRawAnimationSet( const std::string& chId, const std::string& name ) const
{
	if( !hasCharacter(chId) )
	{
		// can't find a character by that name
		wxMessageBox( "Unable to find character " + chId + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return false;
	}

	return mRawAnims.find(chId)->second.count(name) > 0;
}

AnimationSetPtr AnimationStudioApp::getRawAnimationSet( const std::string& chId, const string& name ) const
{
	if( !hasCharacter(chId) )
	{
		// can't find a character by that name
		wxMessageBox( "Unable to find character " + chId + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return NULL;
	}

	std::map<std::string, std::map<std::string, AnimationSetPtr> >::const_iterator chi = mRawAnims.find(chId);
	std::map<std::string, AnimationSetPtr>::const_iterator rai = chi->second.find(name);

	if( rai != chi->second.end() )
		return rai->second;

	return NULL;
}

AnimationStudioApp::AnimationSetIterator AnimationStudioApp::getRawAnimationSetIterator( const std::string& chId )
{
	zhAssert( hasCharacter(chId) );

	return AnimationSetIterator(mRawAnims[chId]);
}

AnimationStudioApp::AnimationSetConstIterator AnimationStudioApp::getRawAnimationSetConstIterator( const std::string& chId ) const
{
	zhAssert( hasCharacter(chId) );

	return AnimationSetConstIterator( mRawAnims.find(chId)->second );
}

unsigned int AnimationStudioApp::getNumRawAnimationSets( const std::string& chId ) const
{
	zhAssert( hasCharacter(chId) );

	return mRawAnims.find(chId)->second.size();
}

void AnimationStudioApp::addAnimationIndex( const std::string& chId, AnimationIndexPtr animIndex )
{
	zhAssert( animIndex != NULL );

	if( !hasCharacter(chId) )
	{
		// can't find a character by that name
		wxMessageBox( "Unable to find character " + chId + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return;
	}

	// add anim. index to project
	mAnimIndexes[chId].insert( make_pair( animIndex->getName(), animIndex ) );
}

AnimationIndexPtr AnimationStudioApp::addAnimationIndex( const std::string& chId )
{
	if( !hasCharacter(chId) )
	{
		// can't find a character by that name
		wxMessageBox( "Unable to find character " + chId + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return NULL;
	}

	// TODO
	// - pop dialog for defining anim. index properties (.vai/.vaib file)
	// - create/load anim. index in the system
	// - add anim. index to project (mAnimIndexes)

	return NULL;
}

void AnimationStudioApp::removeAnimationIndex( const std::string& chId, const std::string& name )
{
	if( !hasCharacter(chId) )
	{
		// can't find a character by that name
		wxMessageBox( "Unable to find character " + chId + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return;
	}

	// TODO
	// - remove anim. index from mAnimIndexes
	// - delete anim. index from the system
	//    - if unused, delete it
	//    - if used by other characters, unload it
}

void AnimationStudioApp::removeAllAnimationIndexes( const std::string& chId )
{
	if( !hasCharacter(chId) )
	{
		// can't find a character by that name
		wxMessageBox( "Unable to find character " + chId + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return;
	}

	while( !mAnimIndexes[chId].empty() )
		removeAnimationIndex( chId, mAnimIndexes[chId].begin()->second->getName() );
}

bool AnimationStudioApp::hasAnimationIndex( const std::string& chId, const std::string& name ) const
{
	if( !hasCharacter(chId) )
	{
		// can't find a character by that name
		wxMessageBox( "Unable to find character " + chId + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return false;
	}

	return mAnimIndexes.find(chId)->second.count(name) > 0;
}

AnimationIndexPtr AnimationStudioApp::getAnimationIndex( const std::string& chId, const string& name ) const
{
	if( !hasCharacter(chId) )
	{
		// can't find a character by that name
		wxMessageBox( "Unable to find character " + chId + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return NULL;
	}

	std::map<std::string, std::map<std::string, AnimationIndexPtr> >::const_iterator chi = mAnimIndexes.find(chId);
	std::map<std::string, AnimationIndexPtr>::const_iterator rai = chi->second.find(name);

	if( rai != chi->second.end() )
		return rai->second;

	return NULL;
}

AnimationStudioApp::AnimationIndexIterator AnimationStudioApp::getAnimationIndexIterator( const std::string& chId )
{
	zhAssert( hasCharacter(chId) );

	return AnimationIndexIterator(mAnimIndexes[chId]);
}

AnimationStudioApp::AnimationIndexConstIterator AnimationStudioApp::getAnimationIndexConstIterator( const std::string& chId ) const
{
	zhAssert( hasCharacter(chId) );

	return AnimationIndexConstIterator( mAnimIndexes.find(chId)->second );
}

unsigned int AnimationStudioApp::getNumAnimationIndexes( const std::string& chId ) const
{
	zhAssert( hasCharacter(chId) );

	return mAnimIndexes.find(chId)->second.size();
}

AnimationSetPtr AnimationStudioApp::createAnimationSet( const std::string& chId )
{
	if( !hasCharacter(chId) )
	{
		// can't find a character by that name
		wxMessageBox( "Unable to find character " + chId + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return NULL;
	}

	// TODO
	// - pop dialog for defining anim. set properties (name, directory)
	// - create anim. set in the system
	// - add anim. set to mAnimSets

	return NULL;
}

void AnimationStudioApp::addAnimationSet( const std::string& chId, AnimationSetPtr animSet )
{
	zhAssert( animSet != NULL );

	if( !hasCharacter(chId) )
	{
		// can't find a character by that name
		wxMessageBox( "Unable to find character " + chId + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return;
	}

	stopAnimation();

	// add animation set to project
	mAnimSets[chId].insert( make_pair( animSet->getName(), animSet ) );

	if( mChar != getCharacter(chId) )
		return;

	// remove outdated internal anim. tree
	mChar->getAnimationController()->removeAnimationTree(zhIntAnimTree_Name);

	// add new nodes to internal anim. tree
	AnimationTreePtr atree_int = AnimationTreePtr::DynamicCast<zh::Resource>( zhAnimationSystem->getAnimationTreeManager()->getResource( zhIntAnimTree_Name ) );
	_addAnimsToAnimTree( animSet, atree_int, atree_int->getRoot() );

	// create new internal anim. tree instance
	mChar->getAnimationController()->addAnimationTree(atree_int);
}

AnimationSetPtr AnimationStudioApp::addAnimationSet( const std::string& chId )
{
	if( !hasCharacter(chId) )
	{
		// can't find a character by that name
		wxMessageBox( "Unable to find character " + chId + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return NULL;
	}

	stopAnimation();

	// TODO
	// - pop dialog for defining anim. set properties (.va/.vab file)
	// - create/load anim. set in the system
	// - add anim. set to project (mAnimSets)
	// - if the character is selected, update internal anim. tree (see prev. function)

	return NULL;
}

void AnimationStudioApp::removeAnimationSet( const std::string& chId, const std::string& name )
{
	if( !hasCharacter(chId) )
	{
		// can't find a character by that name
		wxMessageBox( "Unable to find character " + chId + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return;
	}

	stopAnimation();

	// TODO
	// - if this anim. set is referenced in an anim. tree, warn the user
	// - remove anim. set from mAnimSets
	// - delete anim. set from the system
	//    - if unused, delete it
	//    - if used by other characters, unload it
	// - if the character is selected, update internal anim. tree (see prev. function)
}

void AnimationStudioApp::removeAllAnimationSets( const std::string& chId )
{
	if( !hasCharacter(chId) )
	{
		// can't find a character by that name
		wxMessageBox( "Unable to find character " + chId + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return;
	}

	while( !mAnimSets[chId].empty() )
		removeAnimationSet( chId, mAnimSets[chId].begin()->second->getName() );
}

void AnimationStudioApp::removeAnimation( const std::string& chId, const std::string& animSetName, const std::string& animName )
{
	if( !hasCharacter(chId) )
	{
		// can't find a character by that name
		wxMessageBox( "Unable to find character " + chId + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return;
	}

	if( !hasAnimationSet( chId, animSetName ) )
	{
		// can't find an animation set by that name
		wxMessageBox( "Unable to find animation set " + animSetName + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return;
	}

	stopAnimation();

	// TODO
	// - if this anim. is referenced in an anim. tree, warn the user
	// - delete anim. from its anim. set
	// - if the character is selected, update internal anim. tree (see prev. function)
}

void AnimationStudioApp::removeAnimationSpace( const std::string& chId, const std::string& animSetName, const std::string& animSpaceName )
{
	if( !hasCharacter(chId) )
	{
		// can't find a character by that name
		wxMessageBox( "Unable to find character " + chId + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return;
	}

	if( !hasAnimationSet( chId, animSetName ) )
	{
		// can't find an animation set by that name
		wxMessageBox( "Unable to find animation set " + animSetName + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return;
	}

	stopAnimation();

	// TODO
	// - if this anim. space is referenced in an anim. tree, warn the user
	// - ask the user if base anims should be deleted also (and use removeAnimation for that purpose)
	// - delete anim. space from its anim. set
	// - if the character is selected, update internal anim. tree (see prev. function)
}

void AnimationStudioApp::removeAllAnimations( const std::string& chId, const std::string& animSetName )
{
	if( !hasCharacter(chId) )
	{
		// can't find a character by that name
		wxMessageBox( "Unable to find character " + chId + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return;
	}

	if( !hasAnimationSet( chId, animSetName ) )
	{
		// can't find an animation set by that name
		wxMessageBox( "Unable to find animation set " + animSetName + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return;
	}

	while( mAnimSets[chId][animSetName]->getNumAnimationSpaces() > 0 )
		removeAnimationSpace( chId, animSetName, mAnimSets[chId][animSetName]->getAnimationSpaceIterator().next()->getName() );

	while( mAnimSets[chId][animSetName]->getNumAnimations() > 0 )
		removeAnimation( chId, animSetName, mAnimSets[chId][animSetName]->getAnimationIterator().next()->getName() );
}

bool AnimationStudioApp::hasAnimationSet( const std::string& chId, const std::string& name ) const
{
	if( !hasCharacter(chId) )
	{
		// can't find a character by that name
		wxMessageBox( "Unable to find character " + chId + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return false;
	}

	return mAnimSets.find(chId)->second.count(name) > 0;
}

AnimationSetPtr AnimationStudioApp::getAnimationSet( const std::string& chId, const string& name ) const
{
	if( !hasCharacter(chId) )
	{
		// can't find a character by that name
		wxMessageBox( "Unable to find character " + chId + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return NULL;
	}

	std::map<std::string, std::map<std::string, AnimationSetPtr> >::const_iterator chi = mAnimSets.find(chId);
	std::map<std::string, AnimationSetPtr>::const_iterator rai = chi->second.find(name);

	if( rai != chi->second.end() )
		return rai->second;

	return NULL;
}

AnimationStudioApp::AnimationSetIterator AnimationStudioApp::getAnimationSetIterator( const std::string& chId )
{
	zhAssert( hasCharacter(chId) );

	return AnimationSetIterator(mAnimSets[chId]);
}

AnimationStudioApp::AnimationSetConstIterator AnimationStudioApp::getAnimationSetConstIterator( const std::string& chId ) const
{
	zhAssert( hasCharacter(chId) );

	return AnimationSetConstIterator( mAnimSets.find(chId)->second );
}

unsigned int AnimationStudioApp::getNumAnimationSets( const std::string& chId ) const
{
	zhAssert( hasCharacter(chId) );

	return mAnimSets.find(chId)->second.size();
}

AnimationTreePtr AnimationStudioApp::createAnimationTree( const std::string& chId )
{
	if( !hasCharacter(chId) )
	{
		// can't find a character by that name
		wxMessageBox( "Unable to find character " + chId + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return NULL;
	}

	Character* ch = getCharacter(chId);

	NewResourceDialog dlg( mFrmMain, wxID_ANY, NewResourceDialog::ResourceType_AnimTree );
	if( dlg.ShowModal() != wxID_OK )
	{
		return NULL;
	}

	std::string name = dlg.getName();
	std::string dir = dlg.getDirectory();

	if( zhAnimationSystem->getAnimationTreeManager()->hasResource(name) )
	{
		// duplicate resource name
		wxMessageBox( "Animation tree by the name " + name + " already exists!",
			"Duplicate Name", wxOK|wxICON_ERROR );
		return NULL;
	}

	if( dir == "" )
	{
		// no resource dir.
		wxMessageBox( "Valid resource location not specified.",
			"Invalid Location", wxOK|wxICON_ERROR );
		return NULL;
	}

	// create anim. tree
	unsigned long id = 0;
	while( zhAnimationSystem->getAnimationTreeManager()->hasResource(id) ) ++id;
	AnimationTreePtr anim_tree = AnimationTreePtr::DynamicCast<zh::Resource>( zhAnimationSystem->getAnimationTreeManager()->createResource( id, name ) );
	zhAnimationSystem->getAnimationTreeManager()->serializeResource( id, dir + name + ".vat" );

	// add anim. tree
	addAnimationTree( chId, anim_tree );

	return anim_tree;
}

void AnimationStudioApp::addAnimationTree( const std::string& chId, AnimationTreePtr animTree )
{
	zhAssert( animTree != NULL );

	if( !hasCharacter(chId) )
	{
		// can't find a character by that name
		wxMessageBox( "Unable to find character " + chId + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return;
	}

	// add anim. tree to character
	getCharacter(chId)->getAnimationController()->addAnimationTree(animTree);

	// add anim. tree to project
	mAnimTrees[chId].insert( make_pair( animTree->getName(), animTree ) );

	if( mChar != getCharacter(chId) )
		return;
}

AnimationTreePtr AnimationStudioApp::addAnimationTree( const std::string& chId )
{
	if( !hasCharacter(chId) )
	{
		// can't find a character by that name
		wxMessageBox( "Unable to find character " + chId + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return NULL;
	}

	// TODO
	// - pop dialog for defining anim. tree properties (.va/.vab file)
	// - create/load anim. tree in the system
	// - add anim. tree to character (AnimationController)
	// - add anim. tree to project (mAnimTrees)

	return NULL;
}

void AnimationStudioApp::removeAnimationTree( const std::string& chId, const std::string& name )
{
	if( !hasCharacter(chId) )
	{
		// can't find a character by that name
		wxMessageBox( "Unable to find character " + chId + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return;
	}
	
	stopAnimation();

	// TODO
	// - if this anim. tree is viewed in editor, warn the user
	// - remove anim. tree from project (mAnimTrees)
	// - remove anim. tree from its character (AnimationController)
	// - remove anim. tree from the system
	//    - if unused, delete it
	//    - if used by other characters, unload it
}

void AnimationStudioApp::removeAllAnimationTrees( const std::string& chId )
{
	if( !hasCharacter(chId) )
	{
		// can't find a character by that name
		wxMessageBox( "Unable to find character " + chId + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return;
	}

	while( !mAnimTrees[chId].empty() )
		removeAnimationTree( chId, mAnimTrees[chId].begin()->second->getName() );
}

bool AnimationStudioApp::hasAnimationTree( const std::string& chId, const std::string& name ) const
{
	if( !hasCharacter(chId) )
	{
		// can't find a character by that name
		wxMessageBox( "Unable to find character " + chId + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return false;
	}

	return mAnimTrees.find(chId)->second.count(name) > 0;
}

AnimationTreePtr AnimationStudioApp::getAnimationTree( const std::string& chId, const string& name ) const
{
	if( !hasCharacter(chId) )
	{
		// can't find a character by that name
		wxMessageBox( "Unable to find character " + chId + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return NULL;
	}

	std::map<std::string, std::map<std::string, AnimationTreePtr> >::const_iterator chi = mAnimTrees.find(chId);
	std::map<std::string, AnimationTreePtr>::const_iterator rai = chi->second.find(name);

	if( rai != chi->second.end() )
		return rai->second;

	return NULL;
}

AnimationStudioApp::AnimationTreeIterator AnimationStudioApp::getAnimationTreeIterator( const std::string& chId )
{
	zhAssert( hasCharacter(chId) );

	return AnimationTreeIterator(mAnimTrees[chId]);
}

AnimationStudioApp::AnimationTreeConstIterator AnimationStudioApp::getAnimationTreeConstIterator( const std::string& chId ) const
{
	zhAssert( hasCharacter(chId) );

	return AnimationTreeConstIterator( mAnimTrees.find(chId)->second );
}

unsigned int AnimationStudioApp::getNumAnimationTrees( const std::string& chId ) const
{
	zhAssert( hasCharacter(chId) );

	return mAnimTrees.find(chId)->second.size();
}

AnimationTreeInstance* AnimationStudioApp::selectAnimationTree( const std::string& name )
{
	if( mChar == NULL )
	{
		// no character loaded
		wxMessageBox( "Unable to view animation tree, no character loaded.",
			"Error", wxOK | wxICON_EXCLAMATION );
		return NULL;
	}

	if( !mChar->getAnimationController()->hasAnimationTree(name) )
	{
		// anim. tree not found
		wxMessageBox( "Animation tree " + name + " does not exist.",
			"Error", wxOK | wxICON_EXCLAMATION );
		return NULL;
	}

	mAnimTree = mChar->getAnimationController()->selectAnimationTree(name);
	mAnim = NULL;
	
	if( mAnimTree->getName() != zhIntAnimTree_Name )
		// init. anim. tree editor
		mFrmMain->getAnimTreeWindow()->initEditor();

	return mAnimTree;
}

AnimationTreeInstance* AnimationStudioApp::getCurrentAnimationTree() const
{
	return mAnimTree;
}

AnimationTreeInstance* AnimationStudioApp::getInternalAnimationTree() const
{
	if( mChar == NULL )
	{
		// no character loaded
		wxMessageBox( "Unable to get internal animation tree, no character loaded.",
			"Error", wxOK | wxICON_EXCLAMATION );
		return NULL;
	}

	zhAssert( mChar->getAnimationController()->hasAnimationTree(zhIntAnimTree_Name) );

	return mChar->getAnimationController()->getAnimationTree(zhIntAnimTree_Name);
}

AnimationNode* AnimationStudioApp::selectAnimation( const std::string& animSetName, const std::string& animName )
{
	if( mChar == NULL )
	{
		// no character loaded
		wxMessageBox( "Unable to play animation, no character loaded.",
			"Error", wxOK | wxICON_EXCLAMATION );
		return NULL;
	}

	// make sure internal anim. tree is selected
	if( mAnimTree != getInternalAnimationTree() )
	{
		// deinit. anim. tree editor
		mFrmMain->getAnimTreeWindow()->deinitEditor();

		selectAnimationTree();
	}

	std::string node_name = zhIntNodeName_AnimName(animSetName,animName);
	if( mAnimTree != getInternalAnimationTree() || mAnimTree == NULL || !mAnimTree->hasNode(node_name)  )
	{
		// can't find anim.
		wxMessageBox( "Unable to play animation " + animName + ". Internal animation tree not selected or missing appropriate anim. node.",
			"Error", wxOK | wxICON_EXCLAMATION );
		return NULL;
	}

	AnimationNode* node = mAnimTree->getNode(node_name);
	AnimationTransitionBlender* root = static_cast<AnimationTransitionBlender*>( mAnimTree->getRoot() );

	Model* mdl = mChar->getModelController()->getModel();
	zh::Skeleton* skel = mdl->getSkeleton();
	skel->resetToInitialPose();

	if( hasAnimationSet( mChar->getId(), animSetName ) && root->hasChild( node->getId() ) )
	{
		mAnimTree->setApplyMover(true);
		root->setOrigin( mdl->getSituation() );
	}
	else
	{
		// by default, view original mover channel (if any)
		mAnimTree->setApplyMover(false);
	}

	if( !root->hasChild( node->getId() ) )
	{
		// playing a single base animation

		node = node->getParent();
		AnimationBlender* blend_node = static_cast<AnimationBlender*>(node);
		blend_node->setParamEnabled(false);
		blend_node->setAllWeights(0);
		blend_node->setWeight( zhIntNodeName_AnimName( animSetName, animName ), 1 );
		blend_node->setUseBlendCurves(false);
	}
	else if( node->getClassId() == AnimationBlender::ClassId() )
	{
		// playing a blend

		AnimationBlender* blend_node = static_cast<AnimationBlender*>(node);
		AnimationSpace* anim_space = blend_node->getAnimationSpace();

		// TODO: remove this?
		if( anim_space->hasParametrization() )
		{
			blend_node->setParamEnabled(true);

			// compute mean param. values
			Vector params( anim_space->getParametrization()->getNumParams() );
			for( unsigned int si = 0; si < anim_space->getParametrization()->getNumBaseSamples(); ++si )
				params += anim_space->getParametrization()->getBaseSample(si);
			params /= anim_space->getParametrization()->getNumBaseSamples();
			std::string param_str = "";

			// form param. string
			for( unsigned int param_i = 0; param_i < params.size(); ++param_i )
			{
				param_str += anim_space->getParametrization()->getParam(param_i);
				param_str += "=";
				param_str += toString<float>( params[param_i] );
				
				if( param_i + 1 < params.size() )
					param_str += ";";
			}

			// input param. values
			wxTextEntryDialog dlg( NULL, "Parameter values:", "Input Param. Values", param_str );
			if( dlg.ShowModal() == wxID_OK )
			{
				param_str = dlg.GetValue().c_str();

				// get param. name-value pairs
				boost::char_separator<char> sep( ",;" );
				boost::tokenizer< boost::char_separator<char> > param_tok( param_str, sep );
				unsigned int param_i = 0;
				for( boost::tokenizer< boost::char_separator<char> >::const_iterator wtok_i = param_tok.begin();
					wtok_i != param_tok.end() && param_i < params.size(); ++wtok_i, ++param_i )
				{
					std::string nvpair = *wtok_i;
					if( nvpair.find_first_of('=') >= nvpair.length() - 1 )
						continue;
					
					std::string pn = nvpair.substr( 0, nvpair.find_first_of('=') ),
						pv = nvpair.substr( nvpair.find_first_of('=') + 1 );

					param_i = anim_space->getParametrization()->findParam(pn);
					if( param_i >= anim_space->getParametrization()->getNumParams() )
						continue;

					params[param_i] = fromString<float>(pv);
				}
			}
			else
			{
				return blend_node;
			}

			// set param. values
			blend_node->setParams(params);
		}
		else
		{
			blend_node->setParamEnabled(false);

			// compute mean blend weights
			Vector weights( anim_space->getNumBaseAnimations(), 0 );
			std::string weight_str = "";

			// form weight. string
			for( unsigned int weight_i = 0; weight_i < weights.size(); ++weight_i )
			{
				weight_str += toString<float>( weights[weight_i] );
				
				if( weight_i + 1 < weights.size() )
					weight_str += ";";
			}

			// input weight. values
			wxTextEntryDialog dlg( NULL, "Blend weights:", "Input Blend Weights", weight_str );
			if( dlg.ShowModal() == wxID_OK )
			{
				weight_str = dlg.GetValue().c_str();

				// get weight. name-value pairs
				boost::char_separator<char> sep( ",;" );
				boost::tokenizer< boost::char_separator<char> > weight_tok( weight_str, sep );
				unsigned int weight_i = 0;
				for( boost::tokenizer< boost::char_separator<char> >::const_iterator wtok_i = weight_tok.begin();
					wtok_i != weight_tok.end() && weight_i < weights.size(); ++wtok_i, ++weight_i )
				{
					std::string wstr = *wtok_i;
					weights[weight_i] = fromString<float>(wstr);
				}
			}
			else
			{
				return blend_node;
			}

			// set weight. values
			blend_node->setWeights(weights);
		}
		//

		wxMessageDialog rcdlg( NULL, "Use blend curves?", "Use Blend Curves", wxYES_NO|wxCENTRE );
		// activate blend curves if user specifies
		blend_node->setUseBlendCurves( rcdlg.ShowModal() == wxID_YES ? true : false );

		// activate plant constr. enforcement
		// TODO
	}

	root->setAnnotationsEnabled(false); // to prevent transitions from triggering
	root->setPlaying(false);
	root->removeAllTransitions();
	root->setDefaultNode( node->getId() );
	root->addTransition( node->getId() );
	root->setPlaying(true);

	return mAnim = node;
}

void AnimationStudioApp::stopAnimation()
{
	if( mAnim != NULL )
	{
		if( mAnimTree == getInternalAnimationTree() )
		{
			AnimationTransitionBlender* root = static_cast<AnimationTransitionBlender*>( mAnimTree->getRoot() );
			root->setPlaying(false);
		}

		mAnim->setPlaying(false);
		mAnim = NULL;
	}
}

void AnimationStudioApp::applyAnimation( const std::string& animSetName, const std::string& animName, float time )
{
	if( mChar == NULL )
	{
		// no character loaded
		wxMessageBox( "Unable to apply animation, no character loaded.",
			"Error", wxOK | wxICON_EXCLAMATION );
		return;
	}

	AnimationSetPtr anim_set = getAnimationSet( mChar->getId(), animSetName );
	if( anim_set == NULL || !anim_set->hasAnimation(animName) )
	{
		// animation does not exist
		wxMessageBox( "Unable to find animation " + animSetName + ":" + animName,
			"Error", wxOK | wxICON_EXCLAMATION );
		return;
	}

	zh::Animation* anim = anim_set->getAnimation(animName);
	Model* mdl = mChar->getModelController()->getModel();

	mdl->getSkeleton()->resetToInitialPose();
	anim->apply( mdl, time, 1, 1, zh::Animation::EmptyBoneMask );
	mChar->getModelController()->update(0);
}

AnimationNode* AnimationStudioApp::getCurrentAnimationNode() const
{
	return mAnim;
}

void AnimationStudioApp::_addAnimsToAnimTree( AnimationSetPtr animSet, AnimationTreePtr animTree, AnimationNode* parent )
{
	zhAssert( animSet != NULL );
	zhAssert( animTree != NULL );

	unsigned int node_id = 0;

	_removeAnimsFromAnimTree( animSet, animTree );

	// add animation spaces to the animation tree
	AnimationSet::AnimationSpaceConstIterator asi = animSet->getAnimationSpaceConstIterator();
	while( !asi.end() )
	{
		AnimationSpace* anim_space = asi.next();
		if( animTree->hasNode( zhIntNodeName_AnimPtr(anim_space) ) )
			continue;

		while( animTree->hasNode(node_id) ) ++node_id;
		AnimationBlender* node = static_cast<AnimationBlender*>( animTree->createNode( AnimationBlender::ClassId(), node_id, zhIntNodeName_AnimPtr(anim_space) ) );
		if( parent != NULL ) parent->addChild(node);

		// add base animations to the animation tree
		AnimationSpace::AnimationConstIterator anim_i = anim_space->getBaseAnimationConstIterator();
		while( !anim_i.end() )
		{
			zh::Animation* anim = anim_i.next();
			while( animTree->hasNode(node_id) ) ++node_id;
			AnimationSampler* child = static_cast<AnimationSampler*>( animTree->createNode( AnimationSampler::ClassId(), node_id, zhIntNodeName_AnimPtr(anim) ) );
			node->addChild(child);
			child->setAnimation( anim->getAnimationSet(), anim->getId() );
		}

		node->setAnimationSpace( anim_space->getAnimationSet(), anim_space->getId() );

		// set bone controllers for plant constr. enforcement
		zh::Skeleton* skel = mChar->getModelController()->getModel()->getSkeleton();
		const std::set<std::string>& eeset = getEndEffectorSet( mChar->getId() );
		for( std::set<std::string>::const_iterator eei = eeset.begin(); eei != eeset.end(); ++eei )
		{
			zh::Bone* ebone = skel->getBone(*eei);
			BoneIKController* ebc = static_cast<BoneIKController*>( animTree->getBoneController( "IK_" + ebone->getName() ) );

			if( ebone != NULL && ebc != NULL )
				node->setPlantConstrController( ebone->getId(), ebc );
		}
	}

	// add animations (that aren't part of anim. spaces) to the animation tree
	AnimationSet::AnimationConstIterator anim_i = animSet->getAnimationConstIterator();
	while( !anim_i.end() )
	{
		zh::Animation* anim = anim_i.next();
		if( animTree->hasNode( zhIntNodeName_AnimPtr(anim) ) )
			continue;

		while( animTree->hasNode(node_id) ) ++node_id;
		AnimationSampler* node = static_cast<AnimationSampler*>( animTree->createNode( AnimationSampler::ClassId(), node_id, zhIntNodeName_AnimPtr(anim) ) );
		parent->addChild(node);
		node->setAnimation( anim->getAnimationSet(), anim->getId() );
	}
}

void AnimationStudioApp::_removeAnimsFromAnimTree( AnimationSetPtr animSet, AnimationTreePtr animTree )
{
	zhAssert( animSet != NULL );
	zhAssert( animTree != NULL );

	unsigned int node_id = 0;

	// remove all nodes that refer to this animation set
	AnimationTree::NodeIterator node_i = animTree->getNodeIterator();
	while( node_i.hasMore() )
	{
		AnimationNode* node = node_i.next();
		
		if( node->isClass( AnimationSampler::ClassId() ) )
		{
			// remove animation

			AnimationSampler* snode = static_cast<AnimationSampler*>(node);
			
			if( snode->getAnimationSet() == animSet )
				animTree->deleteNode( node->getId() );
		}
		else if( node->isClass( AnimationBlender::ClassId() ) )
		{
			// remove blend

			AnimationBlender* bnode = static_cast<AnimationBlender*>(node);
			
			if( bnode->getAnimationSet() == animSet )
				animTree->deleteNode( node->getId() );
		}
	}
}

void AnimationStudioApp::resize( unsigned int width, unsigned int height )
{
	if( mRenderWnd )
		mRenderWnd->windowMovedOrResized();
}

bool AnimationStudioApp::frameStarted( const FrameEvent& evt )
{
	if( mAnimEnabled )
	{
		// update visage7 System
		zhAnimationSystem->update( evt.timeSinceLastFrame );
	}

	return true;
}

bool AnimationStudioApp::frameEnded( const FrameEvent& evt )
{
	return true;
}

bool AnimationStudioApp::OnInit()
{
	gApp = this;

	// create main application window
	mFrmMain = new AnimationStudioFrame( ID_frmMain, "Animation Studio 0.1", wxPoint( 0, 0 ),
		wxSize( 1280, 800 ) );
	if( mFrmMain == NULL ) return false;
	mFrmMain->Show();
	wxInitAllImageHandlers();
	SetTopWindow(mFrmMain);

	// initialize OGRE and visage7
	if( !init( mFrmMain->getOgreWindow() ) )
		return false;

	return true;
}

void AnimationStudioApp::OnIdle( wxIdleEvent& evt )
{
	mOgreRoot->renderOneFrame();
	evt.RequestMore();
}

bool AnimationStudioApp::init( wxWindow* wnd )
{
	#ifdef _DEBUG
	mOgreRoot = new Root( "plugins_d.cfg", "ogre_d.cfg", "ogre_d.log" );
	#else
	mOgreRoot = new Root( "plugins.cfg", "ogre.cfg", "ogre.log" );
	#endif

	if( !initOgreCore(wnd) ||
		!createSceneManager() ||
		!createCamera() ||
		!createViewports() )
		return false;
	TextureManager::getSingleton().setDefaultNumMipmaps(5);
	if( !loadResourceLocations() ||
		!createFrameListener() ||
		!createScene() )
		return false;

	if( !initVisage7() )
		return false;

	return true;
}

bool AnimationStudioApp::initOgreCore( wxWindow* renderWnd  )
{
	if( !mOgreRoot->restoreConfig() && !mOgreRoot->showConfigDialog() )
		return false;

	if( renderWnd == NULL )
	{
		this->mRenderWnd = mOgreRoot->initialise(true);
	}
	else
	{
		this->mRenderWnd = mOgreRoot->initialise(false);
		NameValuePairList params;
		params["externalWindowHandle"] = StringConverter::toString( (int)renderWnd->GetHandle() );
		this->mRenderWnd = mOgreRoot->createRenderWindow( "OGRE render window",
			renderWnd->GetRect().GetWidth(),
			renderWnd->GetRect().GetHeight(), false, &params );
	}

	return true;
}

bool AnimationStudioApp::createSceneManager()
{
	mSceneMgr = mOgreRoot->createSceneManager( ST_GENERIC );

	return true;
}

bool AnimationStudioApp::createCamera()
{
	mCam = mSceneMgr->createCamera( "MainCam" );
	mCam->setPosition( zhCamera_Pos );
	mCam->lookAt( zhCamera_Focus );
	mCam->setNearClipDistance(0.05f);
	mCam->setFarClipDistance(5000.f);
	mCam->setAutoAspectRatio(true);

	return true;
}

bool AnimationStudioApp::createViewports()
{
	Viewport* viewport = mRenderWnd->addViewport( mCam );
	viewport->setBackgroundColour( ColourValue( 0, 0, 0 ) );
	mCam->setAspectRatio(
		Real( viewport->getActualWidth() ) / Real( viewport->getActualHeight() ) );

	return true;
}

bool AnimationStudioApp::createFrameListener()
{
	mOgreRoot->addFrameListener(this);
	mOgreRoot->addFrameListener( mFrmMain->getOgreWindow() );

	return true;
}

bool AnimationStudioApp::loadResourceLocations()
{
	ConfigFile res_cfg;
	res_cfg.load( "resources.cfg" );

	ConfigFile::SectionIterator si = res_cfg.getSectionIterator();
	String sec_name, type_name, arch_name;

	while( si.hasMoreElements() )
	{
		sec_name = si.peekNextKey();
		ConfigFile::SettingsMultiMap* settings = si.getNext();
		
		for( ConfigFile::SettingsMultiMap::iterator i = settings->begin();
			i != settings->end(); ++i )
		{
			type_name = i->first;
			arch_name = i->second;

			ResourceGroupManager::getSingleton().addResourceLocation(
				arch_name, type_name, sec_name );
		}
	}

	// initialize resource groups (doesn't actually load all the resources)
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	return true;
}

bool AnimationStudioApp::createScene()
{
	// setup background
	mSceneMgr->setSkyBox( true, "Samples/VisageSkyBox", 2000.f );

	// setup ground
	ManualObject* ground = mSceneMgr->createManualObject( "Ground" );
	ground->begin( "Samples/VisageGround", RenderOperation::OT_TRIANGLE_LIST );
	for( unsigned int i = 0; i < zhGround_Size; ++i )
	{
		for( unsigned int j = 0; j < zhGround_Size; ++j )
		{
			ground->position( -zhGround_Size/2 * zhGround_TileSize + j * zhGround_TileSize,
				0,
				zhGround_Size/2 * zhGround_TileSize - i * zhGround_TileSize );
			ground->textureCoord(0,0);
			
			ground->position( -zhGround_Size/2 * zhGround_TileSize + (j+1) * zhGround_TileSize,
				0,
				zhGround_Size/2 * zhGround_TileSize - (i+1) * zhGround_TileSize );
			ground->textureCoord(1,1);
			
			ground->position( -zhGround_Size/2 * zhGround_TileSize + j * zhGround_TileSize,
				0,
				zhGround_Size/2 * zhGround_TileSize - (i+1) * zhGround_TileSize );
			ground->textureCoord(0,1);

			ground->position( -zhGround_Size/2 * zhGround_TileSize + j * zhGround_TileSize,
				0,
				zhGround_Size/2 * zhGround_TileSize - i * zhGround_TileSize );
			ground->textureCoord(0,0);
			
			ground->position( -zhGround_Size/2 * zhGround_TileSize + (j+1) * zhGround_TileSize,
				0,
				zhGround_Size/2 * zhGround_TileSize - i * zhGround_TileSize );
			ground->textureCoord(1,0);
			
			ground->position( -zhGround_Size/2 * zhGround_TileSize + (j+1) * zhGround_TileSize,
				0,
				zhGround_Size/2 * zhGround_TileSize - (i+1) * zhGround_TileSize );
			ground->textureCoord(1,1);
		}
	}
	ground->end();
	mSceneMgr->getRootSceneNode()->createChildSceneNode( "Ground" )->attachObject(ground);

	// setup lights
	mSceneMgr->setAmbientLight( ColourValue( 0.8f, 0.8f, 0.8f ) );
	Light* sunlight = mSceneMgr->createLight( "Sunlight1" );
	sunlight->setType( Light::LT_DIRECTIONAL );
	sunlight->setDiffuseColour( ColourValue( 1.f, 1.f, 1.f ) );
	sunlight->setSpecularColour( ColourValue( 1.f, 1.f, 1.f ) );
	sunlight->setDirection( Ogre::Vector3( -1.f, 0.f, -1.f ) );
	sunlight = mSceneMgr->createLight( "Sunlight2" );
	sunlight->setType( Light::LT_DIRECTIONAL );
	sunlight->setDiffuseColour( ColourValue( 1.f, 1.f, 1.f ) );
	sunlight->setSpecularColour( ColourValue( 1.f, 1.f, 1.f ) );
	sunlight->setDirection( Ogre::Vector3( -1.f, 0.f, 1.f ) );

	return true;
}

bool AnimationStudioApp::destroyScene()
{
	mSceneMgr->clearScene();

	return true;
}

bool AnimationStudioApp::initVisage7()
{
	zhAnimationSystem->init();
	
	zhRegister_CharacterController( OgreModelController );
	
	CharacterTemplate ch_templ( "OgreCharacterTemplate" );
	ch_templ.addController( "AnimationController" );
	ch_templ.addController( "OgreModelController" );
	std::list<string> order;
	order.push_back( "AnimationController" );
	order.push_back( "OgreModelController" );
	ch_templ.setControllerUpdateOrder( order );
	zhAnimationSystem->addCharacterTemplate( ch_templ );

	zhAnimationSearchSystem->init();

	return true;
}

BEGIN_EVENT_TABLE( AnimationStudioApp, wxApp )
	EVT_IDLE( AnimationStudioApp::OnIdle )
END_EVENT_TABLE()

IMPLEMENT_APP(AnimationStudioApp)
