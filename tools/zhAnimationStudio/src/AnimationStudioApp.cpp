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

AnimationStudioApp::AnimationStudioApp()
: mOgreRoot(NULL), mCam(NULL), mSceneMgr(NULL), mRenderWnd(NULL), mAnimEnabled(true)
{
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

	return skel;
}

void AnimationStudioApp::removeSkeleton( const string& name )
{
	// TODO: remove skeleton

	mFrmMain->getProjectViewWindow()->refresh();
}

/*void AnimationStudioApp::applyAnimation( const std::string& animSetName, const std::string& animName, float time )
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
}*/

void AnimationStudioApp::resize( unsigned int width, unsigned int height )
{
	if( mRenderWnd )
		mRenderWnd->windowMovedOrResized();
}

bool AnimationStudioApp::frameStarted( const FrameEvent& evt )
{
	if( mAnimEnabled )
		// update ZombieHorse System
		zhAnimationSystem->update( evt.timeSinceLastFrame );

	// Apply updated pose to the skeleton in viewport
	mFrmMain->getOgreWindow()->updateRenderSkeletonPose( zhAnimationSystem->getOutputSkeleton() );

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
	mFrmMain = new AnimationStudioFrame( ID_frmMain, "Animation Studio", wxPoint( 0, 0 ),
		wxSize( 1280, 800 ) );
	if( mFrmMain == NULL ) return false;
	mFrmMain->Show();
	wxInitAllImageHandlers();
	SetTopWindow(mFrmMain);

	// initialize OGRE and ZombieHorse
	if( !init( mFrmMain->getOgreWindow() ) )
		return false;
	//
	//Testing load
	//zhAnimationSystem->loadAnimationSet("141_01.bvh");
	//
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
	//Testing!!!!!
	selectSkeleton(zhAnimationSystem->getOutputSkeleton()->getName());
	//
	mFrmMain->getProjectViewWindow()->refresh();

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

bool AnimationStudioApp::initZombieHorse()
{
	zhAnimationSystem->init();
	zhAnimationSearchSystem->init();

	return true;
}

BEGIN_EVENT_TABLE( AnimationStudioApp, wxApp )
	EVT_IDLE( AnimationStudioApp::OnIdle )
END_EVENT_TABLE()

IMPLEMENT_APP(AnimationStudioApp)
