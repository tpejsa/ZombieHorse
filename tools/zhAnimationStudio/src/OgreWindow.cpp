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

#include "OgreWindow.h"
#include "AnimationStudioApp.h"
#include "zhOgreModelController.h";

OgreWindow::OgreWindow( wxWindow *parent, wxWindowID id )
: wxWindow( parent, id, parent->GetPosition(), parent->GetSize(), wxBORDER_SIMPLE, "OgreWindow" ),
mFPS(0)
{
}

float OgreWindow::getFrameRate() const
{
	return mFPS;
}

void OgreWindow::showSkeleton( bool show )
{
}

void OgreWindow::showGround( bool show )
{
	Ogre::SceneNode* sn = gApp->getSceneManager()->getSceneNode( "Ground" );
	if( sn != NULL )
		sn->setVisible(show);
}

void OgreWindow::showSkybox( bool show )
{
	if(show)
		gApp->getSceneManager()->setSkyBox( true, "Samples/VisageSkyBox", 2000.f );
	else
		gApp->getSceneManager()->setSkyBox( false, "Samples/VisageSkyBox", 2000.f );
}

void OgreWindow::show3DAxesOnBone( const Ogre::String& boneName, bool show )
{
	Ogre::SceneManager* scenemgr = gApp->getSceneManager();
	Ogre::SkeletonInstance* oskel = NULL;

	if( !show || scenemgr->hasManualObject( "AxisSystem" ) )
	{
		// delete existing axes
		if( scenemgr->hasManualObject( "AxisSystem" ) )
		{
			scenemgr->destroySceneNode( "AxisSystem" );
			scenemgr->destroyManualObject( "AxisSystem" );
		}

		if( !show )
		{
			return;
		}
	}
	
	// find specified bone
	OgreModelController* omc = static_cast<OgreModelController*>( gApp->getCurrentCharacter()->getModelController() );
	OgreModelController::EntityConstIterator oei = omc->getEntityConstIterator();
	while( !oei.end() )
	{
		Ogre::Entity* oent = oei.next();
		if( oent->hasSkeleton() )
		{
			oskel = oent->getSkeleton();
			zhAssert( oskel->hasBone(boneName) );

			break;
		}
	}

	zhAssert( oskel != NULL );

	// create axes in the scene
	ManualObject* axesobj = scenemgr->createManualObject( "AxisSystem" );
	SceneNode* axesnode = scenemgr->getRootSceneNode()->createChildSceneNode( "AxisSystem" );

	// create material for the axes
	MaterialPtr axesmat = MaterialManager::getSingleton().getByName( "AxisSystemMat" );
	if( axesmat.isNull() )
	{
		axesmat = MaterialManager::getSingleton().create( "AxisSystemMat", "General" );
		axesmat->setReceiveShadows(false);
		axesmat->getTechnique(0)->setLightingEnabled(false);
		axesmat->getTechnique(0)->setDepthCheckEnabled(false);
		axesmat->getTechnique(0)->setDepthWriteEnabled(false);
	}

	// create geometry for the axes
	axesobj->begin( "AxisSystemMat", Ogre::RenderOperation::OT_LINE_LIST );
	// red - x
	// green - y
	// blue - z
	axesobj->position( 0, 0, 0 );
	axesobj->colour( 1, 0, 0 );
	axesobj->position( 1, 0, 0 );
	axesobj->colour( 1, 0, 0 );
	axesobj->position( 0, 0, 0 );
	axesobj->colour( 0, 1, 0 );
	axesobj->position( 0, 1, 0 );
	axesobj->colour( 0, 1, 0 );
	axesobj->position( 0, 0, 0 );
	axesobj->colour( 0, 0, 1 );
	axesobj->position( 0, 0, 1 );
	axesobj->colour( 0, 0, 1 );
	axesobj->end();

	axesnode->attachObject(axesobj);
	axesobj->setRenderQueueGroup(RENDER_QUEUE_OVERLAY);

	Ogre::Bone* obone = oskel->getBone(boneName);

	// position and orient the axes
	axesnode->setPosition( obone->_getDerivedPosition() );
	axesnode->setOrientation( obone->_getDerivedOrientation() );
}

void OgreWindow::createPointSet( const Ogre::String& name, const std::vector<Ogre::Vector3>& points,
								const Ogre::ColourValue& pColor, int pSize )
{
	Ogre::SceneManager* scenemgr = gApp->getSceneManager();
	
	zhAssert( !scenemgr->hasEntity(name) );

	// create the point set in the scene
	ManualObject* psobj = scenemgr->createManualObject(name);
	SceneNode* psnode = scenemgr->getRootSceneNode()->createChildSceneNode(name);

	// create material for the point set
	MaterialPtr psmat = MaterialManager::getSingleton().create( name, "General" );
	psmat->setReceiveShadows(false);
	psmat->getTechnique(0)->setLightingEnabled(false);
	psmat->getTechnique(0)->setDepthCheckEnabled(false);
	psmat->getTechnique(0)->setDepthWriteEnabled(false);
	psmat->getTechnique(0)->setPointSize(pSize);
	
	// create geometry for the point set
	psobj->begin( name, Ogre::RenderOperation::OT_POINT_LIST );
	for( unsigned int pti = 0; pti < points.size(); ++pti )
	{
		psobj->position( points[pti] );
		psobj->colour(pColor);
	}
	psobj->end();

	psnode->attachObject(psobj);
	psobj->setRenderQueueGroup(RENDER_QUEUE_OVERLAY);
}

void OgreWindow::deletePointSet( const Ogre::String& name )
{
	Ogre::SceneManager* scenemgr = gApp->getSceneManager();

	if( scenemgr->hasManualObject(name) )
	{
		scenemgr->destroySceneNode(name);
		scenemgr->destroyManualObject(name);
		MaterialManager::getSingleton().remove(name);
	}
}

bool OgreWindow::hasPointSet( const Ogre::String& name ) const
{
	Ogre::SceneManager* scenemgr = gApp->getSceneManager();

	return scenemgr->hasManualObject(name);
}

void OgreWindow::showPointSet( const Ogre::String& name, bool show )
{
	Ogre::SceneManager* scenemgr = gApp->getSceneManager();

	zhAssert( scenemgr->hasManualObject(name) );

	scenemgr->getSceneNode(name)->setVisible(show);
}

bool OgreWindow::frameStarted( const FrameEvent& evt )
{
	// update FPS
	mFPS = static_cast<float>( 1000.0 / mFPSTimer.getMilliseconds() );
	mFPSTimer.reset();

	Ogre::SceneManager* scene_mgr = gApp->getSceneManager();
	Ogre::Camera* cam = gApp->getCamera();

	// update camera position
	/*Character* ch = gApp->getCurrentCharacter();
	if( ch != NULL )
	{
		zh::Vector3 root_pos = ch->getModelController()->getModel()->getSkeleton()->getRoot()->getWorldPosition();

		// compute new camera position
		Ogre::Vector3 rp, cp0, d0, temp_d, cp;
		float phi;
		rp = zhOgreVector3(root_pos);
		cp0 = cam->getPosition();
		d0 = cam->getDirection().normalisedCopy();
		temp_d = ( rp - cp0 ).normalisedCopy();
		cp = cp0 + ( d0 - temp_d ) * ( rp - cp0 ).length() * Ogre::Math::Sin( d0.angleBetween(temp_d) );

		// reposition camera
		cam->setPosition(cp);
		cam->lookAt(rp);
	}*/

	// update scaling of gizmos
	if( scene_mgr->hasSceneNode( "AxisSystem" ) )
	{
		Ogre::SceneNode* axesnode = scene_mgr->getSceneNode( "AxisSystem" );

		float axesscal = cam->getPosition().distance( axesnode->getPosition() ) / 10.f;
		axesnode->setScale( axesscal, axesscal, axesscal );
	}

	return true;
}

bool OgreWindow::frameEnded( const FrameEvent& evt )
{
	return true;
}

void OgreWindow::OnMouseMove( wxMouseEvent& evt )
{
	static long x0 = -1, y0 = -1;
	long x, y, dx = 0, dy = 0;
	int w, h;

	if( evt.LeftIsDown() )
	{
		// calculate mouse offset and rotate camera

		x = evt.GetX();
		y = evt.GetY();

		if( x0 > -1 && y0 > -1 )
		{
			dx = x - x0;
			dy = y - y0;
		}

		this->GetSize( &w, &h );

		Ogre::Vector3 cam_pos = gApp->getCamera()->getPosition();
		cam_pos = Ogre::Quaternion( Radian( ((float)dx) / w * Math::PI * zhCamera_RotSpeed ), Ogre::Vector3::UNIT_Y ) * cam_pos;
		cam_pos = Ogre::Quaternion( - Radian( ((float)dy) / h * Math::PI * zhCamera_RotSpeed ),
			Ogre::Vector3::UNIT_Y.crossProduct( gApp->getCamera()->getDirection() ) ) * cam_pos;
		gApp->getCamera()->setPosition( cam_pos );
		gApp->getCamera()->lookAt( zhCamera_Focus );

		x0 = x;
		y0 = y;
	}
	else
	{
		x0 = -1;
		y0 = -1;
	}
}

void OgreWindow::OnMouseWheel( wxMouseEvent& evt )
{
	Ogre::Vector3 cam_pos = gApp->getCamera()->getPosition();
	cam_pos = cam_pos + gApp->getCamera()->getDirection().normalisedCopy() * evt.GetWheelRotation() / 10.f;
	gApp->getCamera()->setPosition( cam_pos );
	gApp->getCamera()->lookAt( zhCamera_Focus );
}

void OgreWindow::OnSize( wxSizeEvent& evt )
{
	gApp->resize( evt.GetSize().GetWidth(), evt.GetSize().GetHeight() );
}

BEGIN_EVENT_TABLE( OgreWindow, wxWindow )
	EVT_MOTION( OgreWindow::OnMouseMove )
	EVT_MOUSEWHEEL( OgreWindow::OnMouseWheel )
	EVT_SIZE( OgreWindow::OnSize )
END_EVENT_TABLE()
