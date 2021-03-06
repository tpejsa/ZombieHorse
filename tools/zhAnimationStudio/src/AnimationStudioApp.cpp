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
#include "OgreWindow.h"
//#include "NewResourceDialog.h"
#include "DotSceneLoader.h"
#include "zhOgreModelController.h"

#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp> // TODO: remove this?

AnimationStudioApp* gApp = NULL;

void RenderTimer::Notify()
{
	zh::Animation* cur_anim = gApp->getCurrentAnimation();
	if( cur_anim == NULL || cur_anim->getFrameRate() < 20 )
		Stop();
	else
		gApp->getOgreRoot()->renderOneFrame();
}

AnimationStudioApp::AnimationStudioApp()
: mOgreRoot(NULL), mCam(NULL), mSceneMgr(NULL), mRenderWnd(NULL),
mCurAnim(NULL), mAnimEnabled(true), mTracedJointAnim(NULL)
{
	mRenderTimer = new RenderTimer();
	mFrameTime = 1.f/60.f;
	mParamSkelEditor = new ParamSkeletonEditor();
}

AnimationStudioApp::~AnimationStudioApp()
{
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

zh::Skeleton* AnimationStudioApp::selectSkeleton( const string& name )
{
	if( !zhAnimationSystem->hasSkeleton(name) || name == "Environment" )
	{
		// Can't find skeleton by that name
		wxMessageBox( "Unable to find skeleton " + name + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return NULL;
	}

	// Reset all motion and environment visualization
	displayJointMarkers( std::set<std::string>(), false );
	traceJointPaths( std::set<std::string>(), false );

	zhAnimationSystem->stopAnimation();
	zhAnimationSystem->setOutputSkeleton(name);
	zh::Skeleton* skel = zhAnimationSystem->getOutputSkeleton();
	
	mParamSkelEditor->init(skel);
	displayJointMarkers( std::set<std::string>(), false );
	mFrmMain->getOgreWindow()->setRenderSkeleton(skel);
	mFrmMain->refresh();

	return skel;
}

void AnimationStudioApp::removeSkeleton( const string& name )
{
	// TODO: remove skeleton

	// Update window contents
	mFrmMain->refresh();
}

ParamSkeletonEditor* AnimationStudioApp::getParamSkeletonEditor() const
{
	return mParamSkelEditor;
}

zh::Animation* AnimationStudioApp::selectAnimation( const string& animName )
{
	// Write out environment to a file
	zh::Animation* anim = getCurrentAnimation();
	zh::Skeleton* env = zhAnimationSystem->getEnvironment();
	if( anim != NULL && env->getNumBones() >= 2 )
	{
		std::string path = anim->getAnimationSet()->getPath() + ".env";
		std::ofstream ofs(path);
		zh::Skeleton::BoneConstIterator obj_i = env->getBoneConstIterator();
		while( obj_i.hasMore() )
		{
			zh::Bone* obj = obj_i.next();
			if( obj->getName() == "Root" )
				continue;
			zh::Vector3 pos = obj->getWorldPosition();
			ofs << toString<zh::Vector3>(pos) << std::endl;
		}
	}

	// Reset all motion and environment visualization
	displayJointMarkers( std::set<std::string>(), false );
	traceJointPaths( std::set<std::string>(), false );
	deleteAllEnvironmentObjects();

	if( zhAnimationSystem->getOutputSkeleton() == NULL )
	{
		if( zhAnimationSystem->getNumSkeletons() > 0 )
			selectSkeleton( zhAnimationSystem->getSkeletonConstIterator().next()->getName() );
		else
			return NULL;
	}

	zhAnimationSystem->stopAnimation();
	mCurAnim = zhAnimationSystem->getAnimation(animName);
	useConstFrameRate();

	// Position camera so that the whole motion is visible
	float minx, maxx, miny, maxy, minz, maxz;
	mCurAnim->computeAnimationBounds( minx, maxx, miny, maxy, minz, maxz );
	// TODO

	// Load the environment associated with the animation
	std::ifstream ifs( mCurAnim->getAnimationSet()->getPath() + ".env" );
	unsigned short obj_id = 1;
	while( ifs.good() && !ifs.eof() )
	{
		char line[4097];
		ifs.getline( &line[0], 4096 );
		std::string pos_str(line);
		if( pos_str.length() <= 1 )
			break;
		zh::Vector3 pos = fromString<zh::Vector3>(pos_str);
		createEnvironmentObject( toString<unsigned short>(obj_id++), pos );
	}

	return mCurAnim;
}

void AnimationStudioApp::deselectAnimation()
{
	mCurAnim = NULL;
}

zh::Animation* AnimationStudioApp::getCurrentAnimation() const
{
	return mCurAnim;
}

void AnimationStudioApp::displayJointMarkers( const std::set<std::string>& boneNames, bool enable )
{
	zh::Skeleton* skel = zhAnimationSystem->getOutputSkeleton();
	if( skel == NULL )
		return;
	std::set<std::string> bone_names = boneNames;
	
	if( bone_names.empty() )
	{
		zh::Skeleton::BoneConstIterator bone_i = skel->getBoneConstIterator();
		while( bone_i.hasMore() )
			bone_names.insert( bone_i.next()->getName() );
	}

	if(!enable)
	{
		std::set<std::string> results;
		std::set_difference( mJointsWithMarkers.begin(), mJointsWithMarkers.end(),
			bone_names.begin(), bone_names.end(), std::inserter( results, results.end() ) );
		mJointsWithMarkers = results;

		for( std::set<std::string>::const_iterator jmi = bone_names.begin();
			jmi != bone_names.end(); ++jmi )
			mFrmMain->getOgreWindow()->deletePrettyObject("Marker"+*jmi);
	}
	else
	{
		std::set_union( mJointsWithMarkers.begin(), mJointsWithMarkers.end(),
			bone_names.begin(), bone_names.end(),
			std::inserter( mJointsWithMarkers, mJointsWithMarkers.end() ) );

		// Joint marker points
		std::vector<Ogre::Vector3> pts;
		pts.push_back(Ogre::Vector3(0,0,0));

		for( std::set<std::string>::const_iterator jmi = bone_names.begin();
			jmi != bone_names.end(); ++jmi )
		{
			mFrmMain->getOgreWindow()->createPrettyObject(
				"Marker"+*jmi, Ogre::RenderOperation::OT_POINT_LIST, pts,
				zhSkeleton_MarkerColor, zhSkeleton_MarkerSize, RENDER_QUEUE_OVERLAY, *jmi );
		}
	}
}

void AnimationStudioApp::displayJointMarker( const std::string& boneName, bool enable )
{
	std::set<std::string> bone_names;
	bone_names.insert(boneName);
	displayJointMarkers( bone_names, enable );
}

bool AnimationStudioApp::hasJointMarker( const std::string& boneName ) const
{
	return mJointsWithMarkers.count(boneName) > 0;
}

void AnimationStudioApp::traceJointPaths( const std::set<std::string>& boneNames, bool enable )
{
	zh::Skeleton* skel = zhAnimationSystem->getOutputSkeleton();
	if( skel == NULL )
		return;
	std::set<std::string> bone_names = boneNames;
	
	if( bone_names.empty() )
	{
		zh::Skeleton::BoneConstIterator bone_i = skel->getBoneConstIterator();
		while( bone_i.hasMore() )
			bone_names.insert( bone_i.next()->getName() );
	}

	if(!enable)
	{
		std::set<std::string> results;
		std::set_difference( mTracedJoints.begin(), mTracedJoints.end(),
			bone_names.begin(), bone_names.end(), std::inserter( results, results.end() ) );
		mTracedJoints = results;
		mTracedJointAnim = NULL;

		for( std::set<std::string>::const_iterator jmi = bone_names.begin();
			jmi != bone_names.end(); ++jmi )
			mFrmMain->getOgreWindow()->deletePrettyObject("Trace"+*jmi);
	}
	else
	{
		if( getCurrentAnimation() == NULL )
			return;
		else
			mTracedJointAnim = getCurrentAnimation();

		std::set_union( mTracedJoints.begin(), mTracedJoints.end(),
			bone_names.begin(), bone_names.end(),
			std::inserter( mTracedJoints, mTracedJoints.end() ) );

		for( std::set<std::string>::const_iterator jmi = bone_names.begin();
			jmi != bone_names.end(); ++jmi )
		{
			std::vector<Ogre::Vector3> path;
			_computeJointTracePath( *jmi, path );

			mFrmMain->getOgreWindow()->createPrettyObject(
				"Trace"+*jmi, Ogre::RenderOperation::OT_LINE_STRIP, path,
				zhSkeleton_TraceColor, zhSkeleton_MarkerSize, RENDER_QUEUE_WORLD_GEOMETRY_2 );
		}
	}
}

void AnimationStudioApp::traceJointPath( const std::string& boneName, bool enable )
{
	std::set<std::string> bone_names;
	bone_names.insert(boneName);
	traceJointPaths( bone_names, enable );
}

bool AnimationStudioApp::hasJointTrace( const std::string& boneName ) const
{
	return mTracedJoints.count(boneName) > 0;
}

zh::Bone* AnimationStudioApp::createEnvironmentObject(
	const std::string& name, const zh::Vector3& pos )
{
	// Create the bone representing the "object"
	zh::Skeleton* env = zhAnimationSystem->getEnvironment();
	unsigned short obj_id = 0;
	while( env->hasBone(obj_id) ) ++obj_id;
	zh::Bone* obj = env->createBone( obj_id, name );
	env->getRoot()->addChild(obj);
	obj->setInitialPosition(pos);
	obj->resetToInitialPose();

	// Show the object in the scene
	std::vector<Ogre::Vector3> pts;
	pts.push_back( zhOgreVector3(pos) );
	mFrmMain->getOgreWindow()->createPrettyObject( "EnvObj"+obj->getName(),
		Ogre::RenderOperation::OT_POINT_LIST, pts,
		zhSkeleton_EnvObjColor, zhSkeleton_MarkerSize );

	return obj;
}

void AnimationStudioApp::deleteEnvironmentObject( const std::string& name )
{
	// Delete the bone representing the "object"
	zh::Skeleton* env = zhAnimationSystem->getEnvironment();
	if( !env->hasBone(name) ) return;
	env->deleteBone(name);

	// Remove object from the scene
	mFrmMain->getOgreWindow()->deletePrettyObject( "EnvObj"+name );
}

void AnimationStudioApp::deleteAllEnvironmentObjects()
{
	zh::Skeleton* env = zhAnimationSystem->getEnvironment();
	zh::Skeleton::BoneIterator obj_i = env->getBoneIterator();
	std::vector<std::string> objnames;
	while( obj_i.hasMore() )
	{
		zh::Bone* obj = obj_i.next();
		if( obj->getName() != "Root" )
			objnames.push_back(obj->getName());
	}

	for( std::vector<std::string>::iterator name_i = objnames.begin();
		name_i != objnames.end(); ++name_i )
		deleteEnvironmentObject(*name_i);
}

void AnimationStudioApp::useConstFrameRate( bool useConstFR )
{
	mRenderTimer->Stop();
	zh::Animation* cur_anim = getCurrentAnimation();
	if(useConstFR)
	{
		if( cur_anim != NULL && cur_anim->getFrameRate() >= 20 )
			mFrameTime = 1.f/cur_anim->getFrameRate();
		else
			mFrameTime = 1.f/60.f;
		mRenderTimer->Start(1000.f*mFrameTime);
	}
}

bool AnimationStudioApp::frameStarted( const FrameEvent& evt )
{
	if( mCurAnim == NULL )
		traceJointPaths( std::set<std::string>(), false ); // disable joint traces when no anim. selected

	if( mAnimEnabled )
	{
		// Update the ZombieHorse system
		zhAnimationSystem->update(evt.timeSinceLastFrame);
	}

	// Apply updated pose to the skeleton in viewport
	mFrmMain->getOgreWindow()->updateRenderSkeletonPose( zhAnimationSystem->getOutputSkeleton() );

	return true;
}

bool AnimationStudioApp::frameEnded( const FrameEvent& evt )
{
	return true;
}

void AnimationStudioApp::resize( unsigned int width, unsigned int height )
{
	if( mRenderWnd )
		mRenderWnd->windowMovedOrResized();
}

int AnimationStudioApp::FilterEvent( wxEvent& evt )
{
	if( evt.GetEventType() == wxEVT_KEY_DOWN )
	{
		int kc = ((wxKeyEvent&)evt).GetKeyCode();
		if( kc == WXK_F9 )
		{
			// Toggle root IK
			_toggleIK( RootIKSolver::ClassId() );
			return true;
		}
		else if( kc == WXK_F10 )
		{
			// Toggle posture IK
			_toggleIK( PostureIKSolver::ClassId() );
			return true;
		}
		else if( kc == WXK_F11 )
		{
			// Toggle limb IK
			_toggleIK( LimbIKSolver::ClassId() );
			return true;
		}
		else if( kc == WXK_F12 )
		{
			// Toggle prediction
			zh::Animation* anim = zhAnimationSystem->getCurrentAnimation();
			if( anim != NULL )
			{
				AnimationNode* node = zhAnimationSystem->getAnimationTree()->getNode( anim->getFullName() );
				AnimationAdaptor* adapt = node->getAdaptor();
				if( node->getAdaptor() != NULL )
				{
					if( adapt->getPredictionWeight() > 0 )
						adapt->setPredictionWeight(0);
					else
						adapt->setPredictionWeight(0.15f);
				}
			}
			return true;
		}
    }
 
    return -1;
}

bool AnimationStudioApp::OnInit()
{
	gApp = this;

	// create main application window
	mFrmMain = new AnimationStudioFrame( ID_frmMain, "Animation Studio", wxPoint( 0, 0 ),
		wxSize( 1280, 800 ) );
	if( mFrmMain == NULL ) return false;
	mFrmMain->Show();
	wxInitAllImageHandlers();
	SetTopWindow(mFrmMain);

	// initialize OGRE and ZombieHorse
	if( !init( mFrmMain->getOgreWindow() ) )
		return false;

	return true;
}

int AnimationStudioApp::OnExit()
{
	destroyScene();
	if( mOgreRoot != NULL ) delete mOgreRoot;

	return wxApp::OnExit();
}

void AnimationStudioApp::OnIdle( wxIdleEvent& evt )
{
	if( mRenderTimer->IsRunning() )
		// Rendering at constant framerate, skip this
		return;

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
		!createScene() ||
		!mFrmMain->getOgreWindow()->init() )
		return false;

	if( !initZombieHorse() )
		return false;

	// Add OGRE resource directories
	mOgreRoot->addResourceLocation( "../../samples/data/OGRE/models", "FileSystem", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME );
	mOgreRoot->addResourceLocation( "../../samples/data/OGRE/scenes", "FileSystem", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME );
	mOgreRoot->addResourceLocation( "../../samples/data/OGRE/materials/scripts", "FileSystem", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME );
	mOgreRoot->addResourceLocation( "../../samples/data/OGRE/materials/programs", "FileSystem", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME );
	mOgreRoot->addResourceLocation( "../../samples/data/OGRE/materials/textures", "FileSystem", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME );

	// Load ZombieHorse resources
	boost::filesystem::recursive_directory_iterator end;
	for( boost::filesystem::recursive_directory_iterator dir("../../samples/data/animations");
		dir != end; ++dir )
	{
		boost::filesystem::directory_entry bdir = (*dir);
		boost::filesystem::path bpath = bdir.path();
		std::string path = bpath.string();
		std::transform( path.begin(), path.end(), path.begin(), ::tolower );
		if( path.substr(path.size()-4) ==  ".bvh" || path.substr(path.size()-4) ==  ".zha" )
			zhAnimationSystem->loadAnimationSet(path);
	}

	// Auto-tag all skeletons and create IK solvers on them
	// TODO: one day we'll have a config file or script that specifies these things
	AnimationSystem::SkeletonIterator skel_i = zhAnimationSystem->getSkeletonIterator();
	while( skel_i.hasMore() )
	{
		zh::Skeleton* skel = skel_i.next();
		// TODO: fix auto-tagging in BVHLoader...
		skel->_removeAllBoneTags();
		if( skel->hasBone("0_Hips") ) skel->getBone("0_Hips")->tag(BT_Root);
		if( skel->hasBone("13_lowerback") ) skel->getBone("13_lowerback")->tag(BT_LowerBack);
		if( skel->hasBone("15_Chest") ) skel->getBone("15_Chest")->tag(BT_Chest);
		if( skel->hasBone("2_LeftHip") ) skel->getBone("2_LeftHip")->tag(BT_LHip);
		if( skel->hasBone("3_LeftKnee") ) skel->getBone("3_LeftKnee")->tag(BT_LKnee);
		if( skel->hasBone("4_LeftAnkle") ) skel->getBone("4_LeftAnkle")->tag(BT_LAnkle);
		if( skel->hasBone("8_RightHip") ) skel->getBone("8_RightHip")->tag(BT_RHip);
		if( skel->hasBone("9_RightKnee") ) skel->getBone("9_RightKnee")->tag(BT_RKnee);
		if( skel->hasBone("10_RightAnkle") ) skel->getBone("10_RightAnkle")->tag(BT_RAnkle);
		if( skel->hasBone("21_LeftShoulder") ) skel->getBone("21_LeftShoulder")->tag(BT_LShoulder);
		if( skel->hasBone("22_LeftElbow") ) skel->getBone("22_LeftElbow")->tag(BT_LElbow);
		if( skel->hasBone("23_LeftWrist") ) skel->getBone("23_LeftWrist")->tag(BT_LWrist);
		if( skel->hasBone("30_RightShoulder") ) skel->getBone("30_RightShoulder")->tag(BT_RShoulder);
		if( skel->hasBone("31_RightElbow") ) skel->getBone("31_RightElbow")->tag(BT_RElbow);
		if( skel->hasBone("32_RightWrist") ) skel->getBone("32_RightWrist")->tag(BT_RWrist);
		if( skel->hasBone("20_LeftCollar") ) skel->getBone("20_LeftCollar")->tag(BT_LCollar);
		if( skel->hasBone("29_RightCollar") ) skel->getBone("29_RightCollar")->tag(BT_RCollar);
		if( skel->hasBone("1_LHipJoint") ) skel->getBone("1_LHipJoint")->tag(BT_LHipJoint);
		if( skel->hasBone("7_RHipJoint") ) skel->getBone("7_RHipJoint")->tag(BT_RHipJoint);
		//
		zhAnimationSystem->createIKSolversOnSkeleton( skel->getName() );
	}

	zh::Skeleton* out_skel = zhAnimationSystem->getOutputSkeleton();
	if( out_skel == NULL )
	{
		// No skeletons or animations to be found
		wxMessageBox( "No animations found in the project directory. This application will now exit.",
			"Warning", wxOK | wxICON_WARNING );
		ExitMainLoop();
	}

	// Create user-editable skeleton and set it as default
	zh::Skeleton* skel = zhAnimationSystem->createSkeleton("TestFigure");
	out_skel->_clone(skel);
	zhAnimationSystem->createIKSolversOnSkeleton( skel->getName() );
	zhAnimationSystem->setOutputSkeleton( skel->getName() );
	mFrmMain->getOgreWindow()->setRenderSkeleton(skel);
	mParamSkelEditor->init(skel);

	// Update window contents
	mFrmMain->refresh();

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
	mCam->setNearClipDistance(0.01f);
	mCam->setFarClipDistance(3000.f);
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
	mFrmMain->getOgreWindow()->showGround();
	mFrmMain->getOgreWindow()->showSkybox();

	// Setup lighting
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

bool AnimationStudioApp::initZombieHorse()
{
	zhAnimationSystem->init();
	zhAnimationDatabaseSystem->init();

	return true;
}

void AnimationStudioApp::_toggleIK( unsigned long solverClassId )
{
	zh::Skeleton* skel = zhAnimationSystem->getOutputSkeleton();
	if( skel != NULL && skel->getNumIKSolvers() > 0 )
	{
		zh::Skeleton::IKSolverIterator solver_i = skel->getIKSolverIterator();
		while( solver_i.hasMore() )
		{
			IKSolver* solver = solver_i.next();
			if( solver->getClassId() == solverClassId )
				solver->setEnabled( !solver->getEnabled() );
		}
	}
}

void AnimationStudioApp::_computeJointTracePath( const std::string& boneName,
	std::vector<Ogre::Vector3>& path )
{
	zh::Skeleton* skel = zhAnimationSystem->getOutputSkeleton();
	zhAssert( skel != NULL );
	zhAssert( mTracedJointAnim != NULL );
	
	float dt = 1/60.f;
	for( float t = 0; t <= mTracedJointAnim->getLength(); t += dt )
	{
		skel->resetToInitialPose();
		mTracedJointAnim->apply( skel, t, 1, 1, zh::Animation::EmptyBoneMask );
		zh::Vector3 pt = skel->getBone(boneName)->getWorldPosition();
		path.push_back( zhOgreVector3(pt) );
	}
}

BEGIN_EVENT_TABLE( AnimationStudioApp, wxApp )
	EVT_IDLE( AnimationStudioApp::OnIdle )
END_EVENT_TABLE()

IMPLEMENT_APP(AnimationStudioApp)
