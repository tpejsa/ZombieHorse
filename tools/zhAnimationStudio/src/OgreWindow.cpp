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
mOutSkel(NULL), mRenderSkel(NULL), mCamFocus(zhCamera_Focus),
mRaySceneQuery(NULL), mFPS(0)
{
}

bool OgreWindow::init()
{
	mRaySceneQuery = gApp->getSceneManager()->createRayQuery(Ogre::Ray());

	return true;
}

float OgreWindow::getFrameRate() const
{
	return mFPS;
}

void OgreWindow::showSkeleton( bool show )
{
	if( mRenderSkel == NULL )
		return;

	mRenderSkel->setVisible(show);
}

void OgreWindow::showGround( bool show )
{
	Ogre::SceneManager* scene_mgr = gApp->getSceneManager();
	if( scene_mgr->hasSceneNode("Ground") )
	{
		scene_mgr->getSceneNode( "Ground" )->setVisible(show);
		return;
	}

	// Setup ground
	ManualObject* ground = scene_mgr->createManualObject("Ground");
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
	scene_mgr->getRootSceneNode()->createChildSceneNode("Ground")->attachObject(ground);
}

void OgreWindow::showSkybox( bool show )
{
	if(show)
		gApp->getSceneManager()->setSkyBox( true, "Samples/VisageSkyBox", 2000.f );
	else
		gApp->getSceneManager()->setSkyBox( false, "Samples/VisageSkyBox", 2000.f );
}

void OgreWindow::showCoordAxesOnBones( bool show )
{
	zh::Skeleton* skel = zhAnimationSystem->getOutputSkeleton();
	if( skel == NULL )
		return;

	zh::Skeleton::BoneConstIterator bone_i = skel->getBoneConstIterator();
	while( !bone_i.end() )
		showCoordAxesOnBone( bone_i.next()->getName(), show );
}

void OgreWindow::showCoordAxesOnBone( const Ogre::String& boneName, bool show )
{
	Ogre::SceneManager* scene_mgr = gApp->getSceneManager();
	Ogre::SkeletonInstance* oskel = NULL;

	if( !show || scene_mgr->hasManualObject( "AxisSystem" ) )
	{
		// delete existing axes
		if( scene_mgr->hasManualObject( "AxisSystem" ) )
		{
			scene_mgr->destroySceneNode( "AxisSystem" );
			scene_mgr->destroyManualObject( "AxisSystem" );
		}

		if( !show )
		{
			return;
		}
	}

	zhAssert( oskel != NULL );

	// create axes in the scene
	ManualObject* axesobj = scene_mgr->createManualObject( "AxisSystem" );
	SceneNode* axesnode = scene_mgr->getRootSceneNode()->createChildSceneNode( "AxisSystem" );

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

void OgreWindow::setRenderSkeleton( zh::Skeleton* skel )
{
	if( skel == NULL ) return;
	skel->resetToInitialPose(); // skeleton needs to be in initial pose
	mOutSkel = skel;
	Ogre::SceneManager* scene_mgr = gApp->getSceneManager();

	if( mRenderSkel != NULL )
	{
		// Delete existing skeleton
		_destroyRenderSkeleton(mRenderSkel);
		scene_mgr->getRootSceneNode()->removeAndDestroyChild( mRenderSkel->getName() );
	}

	MaterialPtr bone_mat = MaterialManager::getSingleton().getByName( "BoneMat" );
	if( bone_mat.isNull() )
	{
		// Create material for the bones
		bone_mat = MaterialManager::getSingleton().create( "BoneMat", "General" );
		bone_mat->setReceiveShadows(false);
		bone_mat->setAmbient(zhSkeleton_BoneColor);
		bone_mat->setDiffuse(zhSkeleton_BoneColor);
	}

	mRenderSkel = _createRenderSkeleton( skel->getRoot(), scene_mgr->getRootSceneNode(), NULL );
	zh::Skeleton::BoneIterator bone_i = skel->getBoneIterator();
	zh::Bone* bone = bone_i.next();
	gApp->getCamera()->setPosition( gApp->getCamera()->getPosition() + Ogre::Vector3(bone->getWorldPosition().x,bone->getWorldPosition().y,bone->getWorldPosition().z) );
	// Make sure skeleton's feet are touching the ground
	/*zh::Skeleton::BoneIterator bone_i = skel->getBoneIterator();
	float feet_height = FLT_MAX;
	while( bone_i.hasMore() )
	{
		zh::Bone* bone = bone_i.next();
		if( bone->getWorldPosition().y < feet_height )
			feet_height = bone->getWorldPosition().y;
	}
	scene_mgr->getSceneNode("Ground")->translate(0,feet_height,0);
	gApp->getCamera()->setPosition( gApp->getCamera()->getPosition() + Ogre::Vector3(0,feet_height,0) );
	mCamFocus += Ogre::Vector3(0,feet_height,0);*/
}

void OgreWindow::updateRenderSkeletonPose( zh::Skeleton* skel )
{
	zhAssert( mOutSkel == skel );
	if( skel == NULL ) return;
	Ogre::SceneManager* scene_mgr = gApp->getSceneManager();

	zh::Skeleton::BoneConstIterator bone_ci = skel->getBoneConstIterator();
	while( bone_ci.hasMore() )
	{
		zh::Bone* bone = bone_ci.next();
		Ogre::SceneNode* rbone = scene_mgr->getSceneNode(bone->getName());
		if(rbone == NULL ) continue;

		rbone->setPosition( zhOgreVector3(bone->getPosition()) );
		rbone->setOrientation( zhOgreQuat(bone->getOrientation()) );
		rbone->setScale( zhOgreVector3(bone->getScale()) );
		if( bone->getParent() != NULL )
		{
			Ogre::SceneNode* parent = rbone->getParentSceneNode();
			Ogre::SceneNode* parent_obj = (Ogre::SceneNode*)parent->getChild( parent->getName()+"Obj" );
			parent_obj->setPosition( 0.5f*rbone->getPosition() );
			parent_obj->setOrientation( Ogre::Vector3::UNIT_Y.getRotationTo(rbone->getPosition()) );
			parent_obj->setScale( 1, 0.5f*rbone->getPosition().length()/zhSkeleton_BoneSize, 1 );
		}
	}
}

const Ogre::String& OgreWindow::getSelectedBone() const
{
	return mSelBone;
}

void OgreWindow::createPrettyObject( const Ogre::String& name,
	Ogre::RenderOperation::OperationType objType, const std::vector<Ogre::Vector3>& points,
	const Ogre::ColourValue& pColor, int pSize,
	Ogre::uint8 renderGroup, const Ogre::String& attachToNode )
{
	Ogre::SceneManager* scene_mgr = gApp->getSceneManager();
	zhAssert( !scene_mgr->hasEntity(name) );

	// Create material for the pretty object
	MaterialPtr psmat = MaterialManager::getSingleton().create( name, "General" );
	psmat->setReceiveShadows(false);
	psmat->getTechnique(0)->setLightingEnabled(false);
	psmat->getTechnique(0)->setDepthCheckEnabled(false);
	psmat->getTechnique(0)->setDepthWriteEnabled(false);
	psmat->getTechnique(0)->setPointSize(pSize);
	
	// Create the pretty object in the scene
	deletePrettyObject(name); // delete any existing object with that name
	ManualObject* psobj = scene_mgr->createManualObject(name);
	psobj->begin( name, objType );
	for( unsigned int pti = 0; pti < points.size(); ++pti )
	{
		psobj->position( points[pti] );
		psobj->colour(pColor);
	}
	psobj->end();
	psobj->setRenderQueueGroup(renderGroup);

	// Place the pretty object in the scene
	SceneNode* psnode = attachToNode != "" && scene_mgr->hasSceneNode(attachToNode) ?
		scene_mgr->getSceneNode(attachToNode)
		: scene_mgr->getRootSceneNode()->createChildSceneNode(name);
	psnode->attachObject(psobj);
}

void OgreWindow::deletePrettyObject( const Ogre::String& name )
{
	Ogre::SceneManager* scenemgr = gApp->getSceneManager();

	if( scenemgr->hasManualObject(name) )
	{
		if( scenemgr->hasSceneNode(name) )
			scenemgr->destroySceneNode(name);
		scenemgr->destroyManualObject(name);
		MaterialManager::getSingleton().remove(name);
	}
}

bool OgreWindow::hasPrettyObject( const Ogre::String& name ) const
{
	Ogre::SceneManager* scenemgr = gApp->getSceneManager();

	return scenemgr->hasManualObject(name);
}

void OgreWindow::showPrettyObject( const Ogre::String& name, bool show )
{
	Ogre::SceneManager* scenemgr = gApp->getSceneManager();
	zhAssert( scenemgr->hasManualObject(name) );

	scenemgr->getManualObject(name)->getParentSceneNode()->setVisible(show);
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

	Ogre::Camera* cam = gApp->getCamera();
	if( cam == NULL )
		return;

	// Compute mouse position and offset
	float pos_x, pos_y;
	float off_x, off_y;
	long x, y, dx = 0, dy = 0;
	int w, h;
	this->GetSize( &w, &h );
	x = evt.GetX();
	y = evt.GetY();
	if( x0 > -1 && y0 > -1 )
	{
		dx = x - x0;
		dy = y - y0;
	}
	pos_x = ((float)x)/w;
	pos_y = ((float)y)/h;
	off_x = ((float)dx)/w;
	off_y = ((float)dy)/h;
	x0 = x;
	y0 = y;

	if( evt.LeftIsDown() )
	{
		if( ::wxGetKeyState(WXK_ALT) )
		{
			// Zoom camera
			Ogre::Vector3 cam_pos = gApp->getCamera()->getPosition();
			cam_pos = cam_pos + gApp->getCamera()->getDirection().normalisedCopy() * (-off_y) * zhCamera_TransSpeed * 3.f;
			gApp->getCamera()->setPosition(cam_pos);
			gApp->getCamera()->lookAt(mCamFocus);
		}
		else
		{
			// Rotate camera
			Ogre::Vector3 cam_pos = gApp->getCamera()->getPosition();
			cam_pos = Ogre::Quaternion( Radian( off_x * Math::PI * zhCamera_RotSpeed ), Ogre::Vector3::UNIT_Y ) * cam_pos;
			cam_pos = Ogre::Quaternion( - Radian( off_y * Math::PI * zhCamera_RotSpeed ),
				Ogre::Vector3::UNIT_Y.crossProduct( gApp->getCamera()->getDirection() ) ) * cam_pos;
			cam->setPosition(cam_pos);
			cam->lookAt(mCamFocus);
		}
	}
	else if( evt.RightIsDown() )
	{
		// Pan camera
		Ogre::Vector3 cam_pos = gApp->getCamera()->getPosition();
		Ogre::Vector3 off = gApp->getCamera()->getOrientation() * ( Ogre::Vector3(off_x,-off_y,0) * zhCamera_TransSpeed );
		cam_pos += off;
		mCamFocus += off;
		cam->setPosition(cam_pos);
		cam->lookAt(mCamFocus);
	}
	else
	{
		x0 = -1;
		y0 = -1;
	}

	// Is the mouse pointing at any interesting objects?
	Ogre::Ray ray = cam->getCameraToViewportRay( pos_x, pos_y );
	mRaySceneQuery->setRay(ray);
	Ogre::RaySceneQueryResult& result = mRaySceneQuery->execute();
	Ogre::RaySceneQueryResult::iterator res_i = result.begin();
	while( res_i != result.end() )
	{
		MovableObject* obj = res_i->movable;
		++res_i;

		/*zh::Skeleton* skel = zhAnimationSystem->getOutputSkeleton();
		if( skel != NULL && skel->hasBone( obj->getName() )
			highlightObject( obj->getName() );*/
	}
}

void OgreWindow::OnMouseLeftDown( wxMouseEvent& evt )
{
	Ogre::Camera* cam = gApp->getCamera();
	if( cam == NULL )
		return;

	// Compute mouse position
	float pos_x, pos_y;
	long x, y;
	int w, h;
	this->GetSize( &w, &h );
	x = evt.GetX();
	y = evt.GetY();
	pos_x = ((float)x)/w;
	pos_y = ((float)y)/h;

	// Is the mouse pointing at any interesting objects?
	Ogre::Ray ray = cam->getCameraToViewportRay( pos_x, pos_y );
	mRaySceneQuery->setRay(ray);
	Ogre::RaySceneQueryResult& result = mRaySceneQuery->execute();
	Ogre::RaySceneQueryResult::iterator res_i = result.begin();
	while( res_i != result.end() )
	{
		MovableObject* obj = res_i->movable;
		++res_i;

		zh::Skeleton* skel = zhAnimationSystem->getOutputSkeleton();
		if( skel != NULL && skel->hasBone( obj->getName() ) )
		{
			// User has clicked on a bone
			if( ::wxGetKeyState(WXK_SHIFT) )
			{
				// Create an environment object where user is pointing
				zh::Bone* bone = skel->getBone(obj->getName());
				zh::Vector3 pos = bone->getWorldPosition();
				unsigned short obj_id = 1;
				while( zhAnimationSystem->getEnvironment()->hasBone( toString<unsigned short>(obj_id) ) ) ++obj_id;
				gApp->createEnvironmentObject( toString<unsigned short>(obj_id), pos );
			}
			else
			{
				// Just select the bone
				mSelBone = obj->getName();
			}
		}
	}
}

void OgreWindow::OnSize( wxSizeEvent& evt )
{
	gApp->resize( evt.GetSize().GetWidth(), evt.GetSize().GetHeight() );
}

Ogre::SceneNode* OgreWindow::_createRenderSkeleton( zh::Bone* bone,
	Ogre::SceneNode* renderParent, Ogre::SceneNode* parentObj )
{
	SceneManager* scene_mgr = gApp->getSceneManager();

	SceneNode* rbone = renderParent->createChildSceneNode( bone->getName() );
	rbone->setPosition( zhOgreVector3(bone->getInitialPosition()) );
	rbone->setScale( zhOgreVector3(bone->getInitialScale()) );
	SceneNode* rbone_obj = rbone->createChildSceneNode( rbone->getName() + "Obj" );
	if( scene_mgr->hasManualObject(bone->getName()) )
		scene_mgr->destroyManualObject(bone->getName());
	rbone_obj->attachObject( _createBox( bone->getName(), "BoneMat", zhSkeleton_BoneSize ) );

	if( parentObj != NULL && bone->getParent()->getNumChildren() <= 1 )
	{
		parentObj->setPosition( 0.5f*rbone->getPosition() );
		parentObj->setOrientation( Ogre::Vector3::UNIT_Y.getRotationTo(rbone->getPosition()) );
		parentObj->setScale( 1, 0.5f*rbone->getPosition().length()/zhSkeleton_BoneSize, 1 );
	}

	zh::Bone::ChildIterator child_i = bone->getChildIterator();
	while( child_i.hasMore() )
	{
		zh::Bone* child = child_i.next();
		_createRenderSkeleton( child, rbone, rbone_obj );
	}

	if( bone->getNumChildren() <= 0 )
		rbone_obj->scale( 0.5f, 0.5f, 0.5f );

	return rbone;
}

void OgreWindow::_destroyRenderSkeleton( Ogre::SceneNode* bone )
{
	SceneManager* scene_mgr = gApp->getSceneManager();

	// Destroy all objects attached to this bone
	while( bone->numAttachedObjects() > 0 )
	{
		Ogre::MovableObject* obj = bone->getAttachedObject(0);
		bone->detachObject(obj);
		scene_mgr->destroyMovableObject(obj);
	}

	// Do the same for children, too
	Ogre::SceneNode::ChildNodeIterator bone_i = bone->getChildIterator();
	while( bone_i.hasMoreElements() )
		_destroyRenderSkeleton( static_cast<Ogre::SceneNode*>( bone_i.getNext() ) );
}

ManualObject* OgreWindow::_createBox( const Ogre::String& name, const Ogre::String& matName, float size )
{
	Ogre::SceneManager* scenemgr = gApp->getSceneManager();

	// Create bone objects in the scene
	ManualObject* box = scenemgr->createManualObject(name);
	box->begin( matName, Ogre::RenderOperation::OT_TRIANGLE_LIST );
	//bone_obj->begin( "BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST );
	// bottom
	box->position( -size, -size, -size );
	box->normal( 0.f, -1.f, 0.f );
	box->position( size, -size, -size );
	box->normal( 0.f, -1.f, 0.f );
	box->position( -size, -size, size );
	box->normal( 0.f, -1.f, 0.f );
	box->position( -size, -size, size );
	box->normal( 0.f, -1.f, 0.f );
	box->position( size, -size, -size );
	box->normal( 0.f, -1.f, 0.f );
	box->position( size, -size, size );
	box->normal( 0.f, -1.f, 0.f );
	// top
	box->position( -size, size, -size );
	box->normal( 0.f, 1.f, 0.f );
	box->position( -size, size, size );
	box->normal( 0.f, 1.f, 0.f );
	box->position( size, size, -size );
	box->normal( 0.f, 1.f, 0.f );
	box->position( -size, size, size );
	box->normal( 0.f, 1.f, 0.f );
	box->position( size, size, size );
	box->normal( 0.f, 1.f, 0.f );
	box->position( size, size, -size );
	box->normal( 0.f, 1.f, 0.f );
	// front
	box->position( -size, -size, -size );
	box->normal( 0.f, 0.f, -1.f );
	box->position( -size, size, -size );
	box->normal( 0.f, 0.f, -1.f );
	box->position( size, -size, -size );
	box->normal( 0.f, 0.f, -1.f );
	box->position( -size, size, -size );
	box->normal( 0.f, 0.f, -1.f );
	box->position( size, size, -size );
	box->normal( 0.f, 0.f, -1.f );
	box->position( size, -size, -size );
	box->normal( 0.f, 0.f, -1.f );
	// back
	box->position( -size, -size, size );
	box->normal( 0.f, 0.f, 1.f );
	box->position( size, -size, size );
	box->normal( 0.f, 0.f, 1.f );
	box->position( -size, size, size );
	box->normal( 0.f, 0.f, 1.f );
	box->position( -size, size, size );
	box->normal( 0.f, 0.f, 1.f );
	box->position( size, -size, size );
	box->normal( 0.f, 0.f, 1.f );
	box->position( size, size, size );
	box->normal( 0.f, 0.f, 1.f );
	// left
	box->position( -size, -size, size );
	box->normal( -1.f, 0.f, 0.f );
	box->position( -size, size, size );
	box->normal( -1.f, 0.f, 0.f );
	box->position( -size, -size, -size );
	box->normal( -1.f, 0.f, 0.f );
	box->position( -size, size, size );
	box->normal( -1.f, 0.f, 0.f );
	box->position( -size, size, -size );
	box->normal( -1.f, 0.f, 0.f );
	box->position( -size, -size, -size );
	box->normal( -1.f, 0.f, 0.f );
	// right
	box->position( size, -size, size );
	box->normal( 1.f, 0.f, 0.f );
	box->position( size, -size, -size );
	box->normal( 1.f, 0.f, 0.f );
	box->position( size, size, size );
	box->normal( 1.f, 0.f, 0.f );
	box->position( size, size, size );
	box->normal( 1.f, 0.f, 0.f );
	box->position( size, -size, -size );
	box->normal( 1.f, 0.f, 0.f );
	box->position( size, size, -size );
	box->normal( 1.f, 0.f, 0.f );
	// end...
	box->end();

	return box;
}

BEGIN_EVENT_TABLE( OgreWindow, wxWindow )
	EVT_MOTION( OgreWindow::OnMouseMove )
	EVT_LEFT_DOWN( OgreWindow::OnMouseLeftDown )
	EVT_SIZE( OgreWindow::OnSize )
END_EVENT_TABLE()
