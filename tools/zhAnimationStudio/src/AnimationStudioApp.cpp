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
#include "NewResourceDialog.h"
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
	if( !zhAnimationSystem->hasSkeleton(name) )
	{
		// Can't find skeleton by that name
		wxMessageBox( "Unable to find skeleton " + name + ".",
			"Error", wxOK | wxICON_EXCLAMATION );
		return NULL;
	}

	zhAnimationSystem->stopAnimation();
	zhAnimationSystem->setOutputSkeleton(name);
	zh::Skeleton* skel = zhAnimationSystem->getOutputSkeleton();

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

zh::Animation* AnimationStudioApp::selectAnimation( const string& anim )
{
	if( zhAnimationSystem->getOutputSkeleton() == NULL )
	{
		if( zhAnimationSystem->getNumSkeletons() > 0 )
			selectSkeleton( zhAnimationSystem->getSkeletonConstIterator().next()->getName() );
		else
			return NULL;
	}

	zhAnimationSystem->stopAnimation();
	mCurAnim = zhAnimationSystem->getAnimation(anim);

	// Position camera so that the whole motion is visible
	float minx, maxx, miny, maxy, minz, maxz;
	mCurAnim->computeAnimationBounds( minx, maxx, miny, maxy, minz, maxz );
	// TODO

	// Refresh joint traces
	traceJointPaths( std::set<std::string>(), false );
	if( !mTracedJoints.empty() )
		traceJointPaths(mTracedJoints);
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

		for( std::set<std::string>::const_iterator jmi = boneNames.begin();
			jmi != boneNames.end(); ++jmi )
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

		for( std::set<std::string>::const_iterator jmi = boneNames.begin();
			jmi != boneNames.end(); ++jmi )
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

void AnimationStudioApp::useConstFrameRate( bool useConstFR )
{
	mRenderTimer->Stop();
	zh::Animation* cur_anim = getCurrentAnimation();
	if( useConstFR && cur_anim != NULL && cur_anim->getFrameRate() >= 20 )
		mRenderTimer->Start( 1000.f/cur_anim->getFrameRate() );
}

bool AnimationStudioApp::frameStarted( const FrameEvent& evt )
{
	if( mCurAnim != mTracedJointAnim )
	{
		traceJointPaths( std::set<std::string>(), false ); // disable joint traces when no anim. selected

		if( mCurAnim != NULL && !mTracedJoints.empty() )
			// Animation changed, joint traces must be refreshed
			traceJointPaths( mTracedJoints);
	}

	if( mAnimEnabled )
		// Update the ZombieHorse system
		zhAnimationSystem->update( evt.timeSinceLastFrame );

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
		!createScene() )
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
	for( boost::filesystem::recursive_directory_iterator end, dir("../../samples/data/animations"); dir != end; ++dir )
	{
		std::string path = (*dir).path().string();
		std::transform( path.begin(), path.end(), path.begin(), ::tolower );
		if( path.substr(path.size()-4) ==  ".bvh" || path.substr(path.size()-4) ==  ".zha" )
			zhAnimationSystem->loadAnimationSet(path);
	}

	// Set default output skeleton
	zh::Skeleton* out_skel = zhAnimationSystem->getOutputSkeleton();
	if( out_skel != NULL )
		mFrmMain->getOgreWindow()->setRenderSkeleton(out_skel);
	//displayJointMarkers( std::set<std::string>() );

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
	zhAnimationSearchSystem->init();

	return true;
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
