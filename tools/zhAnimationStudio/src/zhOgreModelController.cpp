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

#include "zhOgreModelController.h"

/*SceneManager* OgreModelController::getSceneManager() const
{
	return mSceneMgr;
}

void OgreModelController::setSceneManager( SceneManager* sceneMgr )
{
	mSceneMgr = sceneMgr;
}

void OgreModelController::addEntity( Entity* ent )
{
	zhAssert( ent != NULL );

	mEntities.push_back(ent);
}

void OgreModelController::removeEntity( unsigned int index )
{
	zhAssert( index < getNumEntities() );

	mEntities.erase( mEntities.begin() + index );
}

void OgreModelController::removeAllEntities()
{
	mEntities.clear();
}

Entity* OgreModelController::getEntity( unsigned int index ) const
{
	zhAssert( index < getNumEntities() );

	return mEntities[index];
}

unsigned int OgreModelController::getNumEntities() const
{
	return mEntities.size();
}

OgreModelController::EntityIterator OgreModelController::getEntityIterator()
{
	return EntityIterator( mEntities );
}

OgreModelController::EntityConstIterator OgreModelController::getEntityConstIterator() const
{
	return EntityConstIterator( mEntities );
}

void OgreModelController::init()
{
	zh::Skeleton* skel = NULL;

	// reset the model
	mMdl->getSkeleton()->deleteAllBones();
	mMdl->deleteAllMeshes();

	// for each entity, create a mesh (+ morph targets if there are any),
	// also create a skeleton for the first OGRE skeleton found
	EntityIterator ei = getEntityIterator();
	unsigned short id = 1;
	while( !ei.end() )
	{
		Entity* ent = ei.next();

		// create a mesh
		if( !mMdl->hasMesh( ent->getName() ) )
		{
			zh::Mesh* mesh = mMdl->createMesh( id++, ent->getName() );

			for( unsigned int mti = 0; mti < ent->getMesh()->getPoseCount(); ++mti )
				mesh->createMorphTarget( ent->getMesh()->getPose(mti)->getName() );
		}

		// create a skeleton
		skel = mMdl->getSkeleton();
		mSkeleton = ent->getSkeleton();
		if( skel->getNumBones() <= 0 && mSkeleton != NULL )
		{
			// first create bones and set tranformations
			Ogre::Skeleton::BoneIterator obi = mSkeleton->getBoneIterator();
			while( obi.hasMoreElements() )
			{
				Ogre::Bone* ob = obi.getNext();
				ob->setManuallyControlled(true); // allows ZombieHorse to manipulate bones
				
				zh::Bone* vb = skel->createBone( ob->getHandle(), ob->getName() );
				vb->setInitialPosition( zhVisageVector3( ob->getInitialPosition() ) );
				vb->setInitialOrientation( zhVisageQuat( ob->getInitialOrientation() ) );
				vb->setInitialScale( zhVisageVector3( ob->getInitialScale() ) );
				vb->resetToInitialPose();

				// TODO: remove this
				// create bone marker
				//if( ob->getName().find_first_of( "Bip" ) == Ogre::String::npos )
					//continue;
				//ManualObject* marker = mSceneMgr->createManualObject( ob->getName() );
				//marker->begin( "Samples/BoneMarker", RenderOperation::OT_POINT_LIST );
				//marker->position( 0, 0, 0 );
				//marker->colour( 0, 1, 0 );
				//marker->end();
				//SceneNode* marker_node = mSceneMgr->getRootSceneNode()->createChildSceneNode( ob->getName() );
				//marker_node->attachObject(marker);
				//marker_node->setPosition( zhOgreVector3( vb->getWorldPosition() ) );
				//
			}

			// next, set up the hierarchy
			obi = mSkeleton->getBoneIterator();
			while( obi.hasMoreElements() )
			{
				Ogre::Bone* ob = obi.getNext();
				zh::Bone* vb = skel->getBone( ob->getName() );

				Ogre::Bone::ChildNodeIterator bci = ob->getChildIterator();
				while( bci.hasMoreElements() )
				{
					Node* obc = bci.getNext();

					vb->addChild( skel->getBone( obc->getName() ) );
				}
			}
			skel->getRoot(); // precompute root
		}
	}

	if( mSkeleton == NULL )
		return;

	// delete animations from OGRE skeleton
	for( unsigned int ai = 0; ai < mSkeleton->getNumAnimations(); ++ai )
	{
		Ogre::Animation* oanim = mSkeleton->getAnimation(ai);
		oanim->destroyAllTracks();
	}
}

void OgreModelController::update( float dt )
{
	zh::Skeleton* skel = mMdl->getSkeleton();

	if( mSkeleton != NULL )
	{
		// reset OGRE skeleton back to initial pose
		mSkeleton->reset(true);

		// copy bone transformations to OGRE skeleton
		zh::Skeleton::BoneConstIterator bone_i = skel->getBoneConstIterator();
		while( !bone_i.end() )
		{
			zh::Bone* bone = bone_i.next();
			Ogre::Bone* ob = mSkeleton->getBone( ( unsigned short )bone->getId()  );			

			if( ob == NULL )
				continue;

			ob->setPosition( zhOgreVector3( bone->getPosition() ) );
			ob->setOrientation( zhOgreQuat( bone->getOrientation() ) );
			ob->setScale( zhOgreVector3( bone->getScale() ) );

			// update marker position
			//SceneNode* marker_node = mSceneMgr->getSceneNode( ob->getName() );
			//if( marker_node != NULL )
				//marker_node->setPosition( zhOgreVector3( bone->getWorldPosition() ) );
		}
	}

	// copy morph target influences to OGRE entities
	// TODO:
}*/
