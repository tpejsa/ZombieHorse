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

#include "zhZHTLoader.h"
#include "zhString.h"
#include "zhFileSystem.h"
#include "zhAnimationSystem.h"
#include "zhAnimationNode.h"
// TODO: remove this when done testing?
/*#include "zhAnimation.h"
#include "zhAnimationSampler.h"
#include "zhAnimationMixer.h"
#include "zhAnimationBlender.h"
#include "zhAnimationTransitionBlender.h"
#include "zhAnimationTree.h"*/
//

namespace zh
{

bool ZHTLoader::tryLoad( ResourcePtr res, const std::string& path )
{
	zhAssert( res != NULL );
	zhAssert( res->getClassId() == Resource_AnimationTree );

	std::string dir, filename, prefix, ext;
	parsePathStr( path, dir, filename, prefix, ext );

	if( ext != "vat" )
		return false;

	return true;
}

bool ZHTLoader::load( ResourcePtr res, const std::string& path )
{
	zhAssert( res != NULL );

	/* TODO: this is all for testing; implement parsing later

	AnimationTreePtr atree = AnimationTreePtr::DynamicCast<Resource>(res);
	AnimationManager* amgr = zhAnimationSystem->getAnimationManager();
	AnimationSampler* def_node = NULL;

	// create an AnimationTransitionBlender root node
	AnimationTransitionBlender* root = static_cast<AnimationTransitionBlender*>( atree->createNode( zhAnimationTransitionBlender_ClassId, 1000, "Root" ) );
	
	// add animations as children of that node
	AnimationManager::ResourceConstIterator ari = amgr->getResourceConstIterator();
	while( !ari.end() )
	{
		AnimationSetPtr animset = AnimationSetPtr::DynamicCast<Resource>( ari.next() );

		AnimationSet::AnimationConstIterator kfai = animset->getAnimationConstIterator();
		while( !kfai.end() )
		{
			Animation* anim = kfai.next();

			AnimationSampler* node = static_cast<AnimationSampler*>(
				atree->createNode( zhAnimationSampler_ClassId, anim->getId(), anim->getName() )
				);
			node->setAnimation( anim->getAnimationSet(), anim->getId() );
			root->addChild(node);

			def_node = node;
		}
	}

	// configure root
	if( def_node != NULL )
	{
		root->setDefaultNode( def_node->getId() );
		root->setDefaultTransitionLength(0);
	}

	// to precompute the root
	atree->getRoot();
	*/

	return true;
}

}
