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

#include "ProjectViewWindow.h"
#include "AnimationStudioApp.h"
#include "AnimationStudioFrame.h"
#include "OgreWindow.h"
//#include "NewResourceDialog.h"

class wxTreeProjectData : public wxTreeItemData
{
};

/*class wxTreeSkeletonsData : public wxTreeItemData
{
};*/

class wxTreeSkeletonData : public wxTreeItemData
{

public:

	wxTreeSkeletonData( zh::Skeleton* skel ) : mSkel(skel) { }
	zh::Skeleton* getSkeleton() const { return mSkel; }

private:

	zh::Skeleton* mSkel;

};

/*class wxTreeAnimIndexesData : public wxTreeItemData
{
};

class wxTreeAnimIndexData : public wxTreeItemData
{

public:

	wxTreeAnimIndexData( AnimationIndexPtr animIndex ) : mAnimIndex(animIndex) { }
	AnimationIndexPtr getAnimationIndex() const { return mAnimIndex; }

private:

	AnimationIndexPtr mAnimIndex;

};

class wxTreeAnimSetsData : public wxTreeItemData
{
};

class wxTreeAnimSetData : public wxTreeItemData
{

public:

	wxTreeAnimSetData( AnimationSetPtr animSet ) : mAnimSet(animSet) { }
	AnimationSetPtr getAnimationSet() const { return mAnimSet; }

private:

	AnimationSetPtr mAnimSet;

};

class wxTreeAnimsData : public wxTreeItemData
{
};*/

class wxTreeAnimData : public wxTreeItemData
{

public:

	wxTreeAnimData( zh::Animation* anim ) : mAnim(anim) { }
	zh::Animation* getAnimation() const { return mAnim; }

private:

	zh::Animation* mAnim;

};

/*class wxTreeAnimSpacesData : public wxTreeItemData
{
};

class wxTreeAnimSpaceData : public wxTreeItemData
{

public:

	wxTreeAnimSpaceData( AnimationSpace* animSpace ) : mAnimSpace(animSpace) { }
	AnimationSpace* getAnimationSpace() const { return mAnimSpace; }

private:

	AnimationSpace* mAnimSpace;

};*/

ProjectViewWindow::ProjectViewWindow( wxWindow* parent, wxWindowID id )
: wxWindow( parent, id, wxDefaultPosition, wxDefaultSize )
{
	trcProjTree = new wxTreeCtrl( this, ID_trcProjTree, wxDefaultPosition, GetSize(),
		wxTR_HAS_BUTTONS | wxTR_SINGLE );
	
	refresh();
}

void ProjectViewWindow::refresh()
{
	// Refresh project tree view

	// Clear current view
	trcProjTree->DeleteAllItems();

	// Add root
	wxTreeItemId trc_root = trcProjTree->AddRoot( "Project Files", -1, -1, new wxTreeProjectData() );

	// Add skeletons
	wxTreeItemId trc_skels = trcProjTree->AppendItem( trc_root, "Skeletons", -1, -1, NULL );
	AnimationSystem::SkeletonConstIterator skel_i = zhAnimationSystem->getSkeletonConstIterator();
	while( !skel_i.end() )
	{
		zh::Skeleton* skel = skel_i.next();

		wxTreeItemId trc_ch = trcProjTree->AppendItem( trc_skels, skel->getName(), -1, -1,
			new wxTreeSkeletonData(skel) );
	}

	// Add animation indexes
	/*wxTreeItemId trc_animindexes = trcProjTree->AppendItem( trc_root, "Animation Indexes", -1, -1,
		new wxTreeAnimIndexesData() );
	AnimationStudioApp::AnimationIndexConstIterator aii = gApp->getAnimationIndexConstIterator( ch->getId() );
	while( !aii.end() )
	{
		AnimationIndexPtr anim_index = aii.next();

		wxTreeItemId trc_ai = trcProjTree->AppendItem( trc_animindexes, anim_index->getName(), -1, -1,
			new wxTreeAnimIndexData(anim_index) );
	}*/

	// Add animations
	wxTreeItemId trc_anims = trcProjTree->AppendItem( trc_root, "Animations", -1, -1, NULL );
	zh::ResourceManager::ResourceConstIterator asi =
		zhAnimationSystem->getAnimationManager()->getResourceConstIterator();
	while( !asi.end() )
	{
		AnimationSetPtr anim_set = AnimationSetPtr::DynamicCast<zh::Resource>(asi.next());

		// Add animation clips
		AnimationSet::AnimationConstIterator anim_i = anim_set->getAnimationConstIterator();
		while( !anim_i.end() )
		{
			zh::Animation* anim = anim_i.next();
			trcProjTree->AppendItem( trc_anims, anim->getFullName(), -1, -1,
				new wxTreeAnimData(anim) );
		}

		// also add animation spaces
		/*wxTreeItemId trc_panims = trcProjTree->AppendItem( trc_as, "Animation Spaces", -1, -1,
			new wxTreeAnimSpacesData() );
		AnimationSet::AnimationSpaceConstIterator panim_i = anim_set->getAnimationSpaceConstIterator();
		while( !panim_i.end() )
		{
			AnimationSpace* anim_space = panim_i.next();
			trcProjTree->AppendItem( trc_panims, anim_space->getName(), -1, -1,
				new wxTreeAnimSpaceData(anim_space) );
		}*/
	}

	trcProjTree->Expand(trc_root);
	trcProjTree->Expand(trc_skels);
	trcProjTree->Expand(trc_anims);
}

bool ProjectViewWindow::Show( bool show )
{
	return wxWindow::Show(show);
}

void ProjectViewWindow::OnSize( wxSizeEvent& evt )
{
	trcProjTree->SetSize( GetSize() );
}

void ProjectViewWindow::OnDrag_ProjTree( wxTreeEvent& evt )
{
	wxTreeItemData* base_data = trcProjTree->GetItemData( evt.GetItem() );
	if( base_data == NULL )
		return;

	wxTreeAnimData* anim_data = dynamic_cast<wxTreeAnimData*>(base_data);
	if( anim_data == NULL ) return;

	// Drag and drop the animation?
	wxTextDataObject dndanim_data( anim_data->getAnimation()->getFullName() );
	wxDropSource dndanim_src(this);
	dndanim_src.SetData(dndanim_data);
	dndanim_src.DoDragDrop(TRUE);
}

void ProjectViewWindow::OnDblClick_ProjTree( wxTreeEvent& evt )
{
	wxTreeItemData* base_data = trcProjTree->GetItemData( evt.GetItem() );
	if( base_data == NULL )
		return;

	wxTreeSkeletonData* char_data;
	wxTreeAnimData* banim_data;

	if( ( char_data = dynamic_cast<wxTreeSkeletonData*>(base_data) ) != NULL )
	{
		gApp->selectSkeleton( char_data->getSkeleton()->getName() );
	}
	else if( ( banim_data = dynamic_cast<wxTreeAnimData*>(base_data) ) != NULL )
	{
		gApp->selectAnimation( banim_data->getAnimation()->getFullName() );
	}
}

void ProjectViewWindow::OnRightClick_ProjTree( wxTreeEvent& evt )
{
	wxTreeItemData* base_data = trcProjTree->GetItemData( evt.GetItem() );
	if( base_data == NULL )
		return;

	wxTreeProjectData* proj_data;
	//wxTreeSkeletonsData* chars_data;
	wxTreeSkeletonData* char_data;
	/*wxTreeAnimIndexesData* animindexes_data;
	wxTreeAnimIndexData* animindex_data;
	wxTreeAnimSetsData* anims_data;
	wxTreeAnimSetData* animset_data;
	wxTreeAnimsData* banims_data;*/
	wxTreeAnimData* anim_data;
	/*wxTreeAnimSpacesData* panims_data;
	wxTreeAnimSpaceData* panim_data;*/

	// determine the correct popup menu to display
	if( ( proj_data = dynamic_cast<wxTreeProjectData*>(base_data) ) != NULL )
	{
		// display Project popup menu
	}
	/*else if( ( chars_data = dynamic_cast<wxTreeSkeletonsData*>(base_data) ) != NULL )
	{
		// display Characters popup menu
	}*/
	else if( ( char_data = dynamic_cast<wxTreeSkeletonData*>(base_data) ) != NULL )
	{
		// display Character popup menu

		wxMenu menu;
		menu.SetClientData(char_data);
		menu.Append( ID_mnSkeletonSelect, "Select" );
		menu.Append( ID_mnSkeletonRemove, "Remove" );
		PopupMenu(&menu);
	}
	/*else if( ( animindexes_data = dynamic_cast<wxTreeAnimIndexesData*>(base_data) ) != NULL )
	{
		// display Animation Indexes popup menu

		wxMenu menu;
		menu.SetClientData(animindexes_data);
		menu.Append( ID_mnAnimIndexesBuildIndex, "Build Index" );
		menu.Append( ID_mnAnimIndexesAddIndex, "Add Index" );
		menu.Append( ID_mnAnimIndexesClearIndexes, "Clear Indexes" );
		menu.AppendSeparator();
		menu.Append( ID_mnAnimIndexesViewMatchWeb, "View Match Web" );
		menu.Append( ID_mnAnimIndexesSearch, "Search" );
		PopupMenu(&menu);
	}
	else if( ( animindex_data = dynamic_cast<wxTreeAnimIndexData*>(base_data) ) != NULL )
	{
		// display Animation Index popup menu

		wxMenu menu;
		menu.SetClientData(animindex_data);
		menu.Append( ID_mnAnimIndexRemove, "Remove" );
		menu.Append( ID_mnAnimIndexRename, "Rename" );
		PopupMenu(&menu);
	}
	else if( ( animset_data = dynamic_cast<wxTreeAnimSetData*>(base_data) ) != NULL )
	{
		// display Animation Set popup menu
	}
	else if( ( banims_data = dynamic_cast<wxTreeAnimsData*>(base_data) ) != NULL )
	{
		// display Base Animations popup menu
	}*/
	else if( ( anim_data = dynamic_cast<wxTreeAnimData*>(base_data) ) != NULL )
	{
		// display Base Animation popup menu

		wxMenu menu;
		menu.SetClientData(anim_data);
		menu.Append( ID_mnAnimSelect, "Select" );
		menu.Append( ID_mnAnimRemove, "Remove" );
		menu.Append( ID_mnAnimRename, "Rename" );
		PopupMenu(&menu);
	}
	/*else if( ( panims_data = dynamic_cast<wxTreeAnimSpacesData*>(base_data) ) != NULL )
	{
		// display Animation Spaces popup menu
	}
	else if( ( panim_data = dynamic_cast<wxTreeAnimSpaceData*>(base_data) ) != NULL )
	{
		// display Animation Space popup menu

		wxMenu menu;
		menu.SetClientData(panim_data);
		menu.Append( ID_mnAnimSpaceSelect, "Select" );
		menu.Append( ID_mnAnimSpaceRemove, "Remove" );
		menu.Append( ID_mnAnimSpaceRename, "Rename" );
		menu.AppendSeparator();
		menu.Append( ID_mnAnimSpaceDefineParam, "Define Parametrization" );
		menu.Append( ID_mnAnimSpaceMatchAnnots, "Match Annotations" );
		menu.Append( ID_mnAnimSpaceBuildBlendCurves, "Build Blend Curves" );
		PopupMenu(&menu);
	}*/
}

void ProjectViewWindow::OnMenu_SkeletonSelect( wxCommandEvent& evt )
{
	wxTreeSkeletonData* skel_data = static_cast<wxTreeSkeletonData*>(
		static_cast<wxMenu*>( evt.GetEventObject() )->GetClientData()
		);

	// TODO: warn the user in case of unsaved changes made to the current character
	// and its resources
	
	gApp->selectSkeleton( skel_data->getSkeleton()->getName() );
}

void ProjectViewWindow::OnMenu_SkeletonRemove( wxCommandEvent& evt )
{
}

/*void ProjectViewWindow::OnMenu_AnimIndexesBuildIndex( wxCommandEvent& evt )
{
	gApp->getAppFrame()->OnMenu_ToolsBuildAnimIndex(evt);
}

void ProjectViewWindow::OnMenu_AnimIndexesAddIndex( wxCommandEvent& evt )
{
}

void ProjectViewWindow::OnMenu_AnimIndexesClearIndexes( wxCommandEvent& evt )
{
}

void ProjectViewWindow::OnMenu_AnimIndexesViewMatchWeb( wxCommandEvent& evt )
{
	gApp->getAppFrame()->OnMenu_ToolsViewMatchWeb(evt);
}

void ProjectViewWindow::OnMenu_AnimIndexesSearch( wxCommandEvent& evt )
{
	gApp->getAppFrame()->OnMenu_ToolsSearchAnimIndex(evt);
}

void ProjectViewWindow::OnMenu_AnimIndexRemove( wxCommandEvent& evt )
{
}

void ProjectViewWindow::OnMenu_AnimIndexRename( wxCommandEvent& evt )
{
}

void ProjectViewWindow::OnMenu_AnimSetRemove( wxCommandEvent& evt )
{
}

void ProjectViewWindow::OnMenu_AnimSetRename( wxCommandEvent& evt )
{
}

void ProjectViewWindow::OnMenu_AnimSetClear( wxCommandEvent& evt )
{
}*/

void ProjectViewWindow::OnMenu_AnimSelect( wxCommandEvent& evt )
{
	wxTreeAnimData* banim_data = static_cast<wxTreeAnimData*>(
		static_cast<wxMenu*>( evt.GetEventObject() )->GetClientData()
		);
	
	zh::Animation* anim = banim_data->getAnimation();
	gApp->selectAnimation( anim->getFullName() );
}

void ProjectViewWindow::OnMenu_AnimRemove( wxCommandEvent& evt )
{
}

void ProjectViewWindow::OnMenu_AnimRename( wxCommandEvent& evt )
{
}

/*void ProjectViewWindow::OnMenu_AnimSpacesBuildAnimSpace( wxCommandEvent& evt )
{
	gApp->getAppFrame()->OnMenu_ToolsBuildAnimSpace(evt);
}

void ProjectViewWindow::OnMenu_AnimSpacesClear( wxCommandEvent& evt )
{
}

void ProjectViewWindow::OnMenu_AnimSpaceSelect( wxCommandEvent& evt )
{
	wxTreeAnimSpaceData* panim_data = static_cast<wxTreeAnimSpaceData*>(
		static_cast<wxMenu*>( evt.GetEventObject() )->GetClientData()
		);
	
	zh::AnimationSpace* anim_space = panim_data->getAnimationSpace();

	// TODO: remove this
	gApp->getAppFrame()->getOgreWindow()->show3DAxesOnBone( "Bip01" );

	zh::Model* mdl = gApp->getCurrentCharacter()->getModelController()->getModel();

	// compute base transf.'s
	mdl->getSkeleton()->resetToInitialPose();
	anim_space->getBaseAnimation(0)->apply( mdl, 0, 1, std::set<unsigned short>() );
	zh::Vector3 bpos = mdl->getSkeleton()->getBone("Bip01")->getWorldPosition();
	zh::Quat borient = mdl->getSkeleton()->getBone("Bip01")->getWorldOrientation();
	zh::Vector3 bscal = mdl->getSkeleton()->getBone("Bip01")->getWorldScale();
	mdl->getSkeleton()->resetToInitialPose();

	// show param. samples
	DenseSamplingParametrization* animparam = static_cast<DenseSamplingParametrization*>( anim_space->getParametrization() );
	if( animparam != NULL )
	{
		std::vector<Ogre::Vector3> pts;

		for( unsigned int bsi = 0; bsi < animparam->getNumBaseSamples(); ++bsi )
		{
			zh::Vector3 ptpos;
			const zh::Vector& params = animparam->getBaseSample(bsi);
			ptpos = zh::Vector3( params[0], params[1], params[2] );
			ptpos = ptpos * bscal;
			ptpos = ptpos.rotate( borient );
			ptpos = bpos + ptpos;
			pts.push_back( zhOgreVector3(ptpos) );
		}

		gApp->getAppFrame()->getOgreWindow()->deletePrettyObject( "BaseSamples" );
		gApp->getAppFrame()->getOgreWindow()->createPrettyObject( "BaseSamples", pts, Ogre::ColourValue( 1, 0, 1 ), 12 );
		gApp->getAppFrame()->getOgreWindow()->showPrettyObject( "BaseSamples" );
		pts.clear();

		for( unsigned int si = 0; si < animparam->getNumSamples(); ++si )
		{
			zh::Vector3 ptpos;
			zh::Vector params( animparam->getNumParams() ),
				weights( animparam->getNumBaseSamples() );
			animparam->getSample( si, params, weights );
			ptpos = zh::Vector3( params[0], params[1], params[2] );
			ptpos = ptpos * bscal;
			ptpos = ptpos.rotate( borient );
			ptpos = bpos + ptpos;
			pts.push_back( zhOgreVector3(ptpos) );
		}

		gApp->getAppFrame()->getOgreWindow()->deletePrettyObject( "DenseSamples" );
		gApp->getAppFrame()->getOgreWindow()->createPrettyObject( "DenseSamples", pts, Ogre::ColourValue( 0, 1, 0 ), 5 );
		gApp->getAppFrame()->getOgreWindow()->showPrettyObject( "DenseSamples" );
		pts.clear();

		zh::Vector3 ptpos;
		ptpos = zh::Vector3( 40, 70, 25 );
		ptpos = ptpos * bscal;
		ptpos = ptpos.rotate( borient );
		ptpos = bpos + ptpos;
		pts.push_back( zhOgreVector3(ptpos) );

		gApp->getAppFrame()->getOgreWindow()->deletePrettyObject( "TargetPoint" );
		gApp->getAppFrame()->getOgreWindow()->createPrettyObject( "TargetPoint", pts, Ogre::ColourValue( 1, 0, 0 ), 12 );
		gApp->getAppFrame()->getOgreWindow()->showPrettyObject( "TargetPoint" );
		pts.clear();
	}

	gApp->selectAnimation( anim_space->getAnimationSet()->getName(), anim_space->getName() );
}

void ProjectViewWindow::OnMenu_AnimSpaceRemove( wxCommandEvent& evt )
{
}

void ProjectViewWindow::OnMenu_AnimSpaceRename( wxCommandEvent& evt )
{
}

void ProjectViewWindow::OnMenu_AnimSpaceDefineParam( wxCommandEvent& evt )
{
	wxTreeAnimSpaceData* panim_data = static_cast<wxTreeAnimSpaceData*>(
		static_cast<wxMenu*>( evt.GetEventObject() )->GetClientData()
		);
	
	zh::Skeleton* skel = zhAnimationSystem->getOutputSkeleton();
	zh::AnimationSpace* anim_space = panim_data->getAnimationSpace();

	DefineAnimParamDialog dlg( this, wxID_ANY, skel, anim_space );
	if( dlg.ShowModal() != wxID_OK || dlg.getParamSpecs().size() <= 0 )
	{
		// user cancelled
		return;
	}

	if( anim_space->hasParametrization() )
		anim_space->deleteParametrization();

	// build parametrization over the animation space
	zhAnimationSearchSystem->setMaxExtrapolation( dlg.getMaxExtrapolation() );
	zhAnimationSearchSystem->setMinSampleDistance( dlg.getMinSampleDistance() );
	std::vector<AnimationParamSpec> pspecs;
	pspecs.push_back( AnimationParamSpec( "posX", anim_space->getBaseAnimation(0),
		0.82f, mdl->getSkeleton()->getBone("Bip01 L Hand")->getId(),
		mdl->getSkeleton()->getBone("Bip01")->getId(),
		AnimationParamSpec::TransfType_Translation, AnimationParamSpec::Axis_x )
		);
	pspecs.push_back( AnimationParamSpec( "posY", anim_space->getBaseAnimation(0),
		0.82f, mdl->getSkeleton()->getBone("Bip01 L Hand")->getId(),
		mdl->getSkeleton()->getBone("Bip01")->getId(),
		AnimationParamSpec::TransfType_Translation, AnimationParamSpec::Axis_y )
		);
	pspecs.push_back( AnimationParamSpec( "posZ", anim_space->getBaseAnimation(0),
		0.82f, mdl->getSkeleton()->getBone("Bip01 L Hand")->getId(),
		mdl->getSkeleton()->getBone("Bip01")->getId(),
		AnimationParamSpec::TransfType_Translation, AnimationParamSpec::Axis_z )
		);
	zhAnimationSearchSystem->parametrizeAnimSpace( skel, anim_space, dlg.getParamSpecs() );

	// TODO: anim. set modified
}

void ProjectViewWindow::OnMenu_AnimSpaceMatchAnnots( wxCommandEvent& evt )
{
	wxTreeAnimSpaceData* panim_data = static_cast<wxTreeAnimSpaceData*>(
		static_cast<wxMenu*>( evt.GetEventObject() )->GetClientData()
		);
	
	zh::AnimationSpace* anim_space = panim_data->getAnimationSpace();
	zhAnimationSearchSystem->computeAnnotMatches(anim_space);

	zhAssert( anim_space->getAnimationSet()->getPath() != "" );
	// TODO: anim. set modified
}

void ProjectViewWindow::OnMenu_AnimSpaceBuildBlendCurves( wxCommandEvent& evt )
{
}*/

BEGIN_EVENT_TABLE( ProjectViewWindow, wxWindow )
	EVT_SIZE( ProjectViewWindow::OnSize )
	EVT_TREE_BEGIN_DRAG( ID_trcProjTree, ProjectViewWindow::OnDrag_ProjTree )
	EVT_TREE_ITEM_ACTIVATED( ID_trcProjTree, ProjectViewWindow::OnDblClick_ProjTree )
	EVT_TREE_ITEM_RIGHT_CLICK( ID_trcProjTree, ProjectViewWindow::OnRightClick_ProjTree )
	EVT_MENU( ID_mnSkeletonSelect, ProjectViewWindow::OnMenu_SkeletonSelect )
	EVT_MENU( ID_mnSkeletonRemove, ProjectViewWindow::OnMenu_SkeletonRemove )
	/*EVT_MENU( ID_mnAnimIndexRemove, ProjectViewWindow::OnMenu_AnimIndexRemove )
	EVT_MENU( ID_mnAnimIndexRename, ProjectViewWindow::OnMenu_AnimIndexRename )
	EVT_MENU( ID_mnAnimSetRemove, ProjectViewWindow::OnMenu_AnimSetRemove )
	EVT_MENU( ID_mnAnimSetRename, ProjectViewWindow::OnMenu_AnimSetRename )
	EVT_MENU( ID_mnAnimSetClear, ProjectViewWindow::OnMenu_AnimSetClear )*/
	EVT_MENU( ID_mnAnimSelect, ProjectViewWindow::OnMenu_AnimSelect )
	EVT_MENU( ID_mnAnimRemove, ProjectViewWindow::OnMenu_AnimRemove )
	EVT_MENU( ID_mnAnimRename, ProjectViewWindow::OnMenu_AnimRename )
	/*EVT_MENU( ID_mnAnimSpaceDefineParam, ProjectViewWindow::OnMenu_AnimSpaceDefineParam )
	EVT_MENU( ID_mnAnimSpaceMatchAnnots, ProjectViewWindow::OnMenu_AnimSpaceMatchAnnots )
	EVT_MENU( ID_mnAnimSpaceBuildBlendCurves, ProjectViewWindow::OnMenu_AnimSpaceBuildBlendCurves )*/
END_EVENT_TABLE()
