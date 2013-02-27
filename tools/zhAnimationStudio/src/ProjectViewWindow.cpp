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
#include "NewResourceDialog.h"
// TODO: remove this
#include "DefineAnimParamDialog.h"
//

class wxTreeProjectData : public wxTreeItemData
{
};

class wxTreeCharacterData : public wxTreeItemData
{

public:

	wxTreeCharacterData( Character* character ) : mChar(character) { }
	Character* getCharacter() const { return mChar; }

private:

	Character* mChar;

};

class wxTreeRawAnimSetsData : public wxTreeItemData
{
};

class wxTreeRawAnimSetData : public wxTreeItemData
{

public:

	wxTreeRawAnimSetData( AnimationSetPtr animSet ) : mAnimSet(animSet) { }
	AnimationSetPtr getAnimationSet() const { return mAnimSet; }

private:

	AnimationSetPtr mAnimSet;

};

class wxTreeRawAnimData : public wxTreeItemData
{

public:

	wxTreeRawAnimData( zh::Animation* anim ) : mAnim(anim) { }
	zh::Animation* getAnimation() const { return mAnim; }

private:

	zh::Animation* mAnim;

};

class wxTreeAnimIndexesData : public wxTreeItemData
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

class wxTreeBaseAnimsData : public wxTreeItemData
{
};

class wxTreeBaseAnimData : public wxTreeItemData
{

public:

	wxTreeBaseAnimData( zh::Animation* anim ) : mAnim(anim) { }
	zh::Animation* getAnimation() const { return mAnim; }

private:

	zh::Animation* mAnim;

};

class wxTreeAnimSpacesData : public wxTreeItemData
{
};

class wxTreeAnimSpaceData : public wxTreeItemData
{

public:

	wxTreeAnimSpaceData( AnimationSpace* animSpace ) : mAnimSpace(animSpace) { }
	AnimationSpace* getAnimationSpace() const { return mAnimSpace; }

private:

	AnimationSpace* mAnimSpace;

};

class wxTreeAnimTreesData : public wxTreeItemData
{
};

class wxTreeAnimTreeData : public wxTreeItemData
{

public:

	wxTreeAnimTreeData( AnimationTreePtr animTree ) : mAnimTree(animTree) { }
	AnimationTreePtr getAnimationTree() const { return mAnimTree; }

private:

	AnimationTreePtr mAnimTree;

};

ProjectViewWindow::ProjectViewWindow( wxWindow* parent, wxWindowID id )
: wxWindow( parent, id, wxDefaultPosition, wxDefaultSize )
{
	trcProjTree = new wxTreeCtrl( this, ID_trcProjTree, wxDefaultPosition, GetSize(),
		wxTR_HAS_BUTTONS | wxTR_SINGLE );
	refresh();
}

void ProjectViewWindow::refresh()
{
	// refresh project tree view

	// clear current view
	trcProjTree->DeleteAllItems();

	wxTreeItemId trc_curch;

	// add root
	wxTreeItemId trc_root = trcProjTree->AddRoot( gApp->getProjectName(), -1, -1, new wxTreeProjectData() );

	// add characters
	AnimationStudioApp::CharacterConstIterator chi = gApp->getCharacterConstIterator();
	while( !chi.end() )
	{
		Character* ch = chi.next();

		wxTreeItemId trc_ch = trcProjTree->AppendItem( trc_root, ch->getId(), -1, -1,
			new wxTreeCharacterData(ch) );

		trc_curch = trc_ch;
		
		if( gApp->getCurrentCharacter() == NULL ||
			gApp->getCurrentCharacter() != ch )
			continue;
		else
			trc_curch = trc_ch;

		// add raw animation sets
		wxTreeItemId trc_ranims = trcProjTree->AppendItem( trc_ch, "Raw Animations", -1, -1,
			new wxTreeRawAnimSetsData() );
		AnimationStudioApp::AnimationSetConstIterator rasi = gApp->getRawAnimationSetConstIterator( ch->getId() );
		while( !rasi.end() )
		{
			AnimationSetPtr ras = rasi.next();

			wxTreeItemId trc_ra = trcProjTree->AppendItem( trc_ranims, ras->getName(), -1, -1,
				new wxTreeRawAnimSetData(ras) );

			// add raw animations
			AnimationSet::AnimationConstIterator rai = ras->getAnimationConstIterator();
			while( !rai.end() )
			{
				zh::Animation* anim = rai.next();
				trcProjTree->AppendItem( trc_ra, anim->getName(), -1, -1,
					new wxTreeRawAnimData(anim) );
			}
		}

		// add animation indexes
		wxTreeItemId trc_animindexes = trcProjTree->AppendItem( trc_ch, "Animation Indexes", -1, -1,
			new wxTreeAnimIndexesData() );
		AnimationStudioApp::AnimationIndexConstIterator aii = gApp->getAnimationIndexConstIterator( ch->getId() );
		while( !aii.end() )
		{
			AnimationIndexPtr anim_index = aii.next();

			wxTreeItemId trc_ai = trcProjTree->AppendItem( trc_animindexes, anim_index->getName(), -1, -1,
				new wxTreeAnimIndexData(anim_index) );
		}

		// add animation sets
		wxTreeItemId trc_anims = trcProjTree->AppendItem( trc_ch, "Animations", -1, -1,
			new wxTreeAnimSetsData() );
		AnimationStudioApp::AnimationSetConstIterator asi = gApp->getAnimationSetConstIterator( ch->getId() );
		while( !asi.end() )
		{
			AnimationSetPtr anim_set = asi.next();

			wxTreeItemId trc_as = trcProjTree->AppendItem( trc_anims, anim_set->getName(), -1, -1,
				new wxTreeAnimSetData(anim_set) );
			
			// add base animations
			wxTreeItemId trc_banims = trcProjTree->AppendItem( trc_as, "Base Animations", -1, -1,
				new wxTreeBaseAnimsData() );
			AnimationSet::AnimationConstIterator anim_i = anim_set->getAnimationConstIterator();
			while( !anim_i.end() )
			{
				zh::Animation* anim = anim_i.next();
				trcProjTree->AppendItem( trc_banims, anim->getName(), -1, -1,
					new wxTreeBaseAnimData(anim) );
			}

			// also add animation spaces
			wxTreeItemId trc_panims = trcProjTree->AppendItem( trc_as, "Animation Spaces", -1, -1,
				new wxTreeAnimSpacesData() );
			AnimationSet::AnimationSpaceConstIterator panim_i = anim_set->getAnimationSpaceConstIterator();
			while( !panim_i.end() )
			{
				AnimationSpace* anim_space = panim_i.next();
				trcProjTree->AppendItem( trc_panims, anim_space->getName(), -1, -1,
					new wxTreeAnimSpaceData(anim_space) );
			}
		}

		// add animation trees
		wxTreeItemId trc_animtrees = trcProjTree->AppendItem( trc_ch, "Animation Trees", -1, -1,
			new wxTreeAnimTreesData() );
		AnimationStudioApp::AnimationTreeConstIterator ati = gApp->getAnimationTreeConstIterator( ch->getId() );
		while( !ati.end() )
		{
			AnimationTreePtr anim_tree = ati.next();

			wxTreeItemId trc_at = trcProjTree->AppendItem( trc_animtrees, anim_tree->getName(), -1, -1,
				new wxTreeAnimTreeData(anim_tree) );
		}
	}

	// expand subtree
	trcProjTree->Expand(trc_root);
	if( trc_curch.IsOk() )
		trcProjTree->Expand(trc_curch);
}

bool ProjectViewWindow::Show( bool show )
{
	AnimationStudioFrame* frm_app = gApp->getAppFrame();
	if( frm_app != NULL )
	{
		frm_app->GetMenuBar()->Check( ID_mnViewProject, show );
	}

	return wxWindow::Show(show);
}

void ProjectViewWindow::OnSize( wxSizeEvent& evt )
{
	trcProjTree->SetSize( GetSize() );
}

void ProjectViewWindow::OnRightClick_ProjTree( wxTreeEvent& evt )
{
	wxTreeItemData* base_data = trcProjTree->GetItemData( evt.GetItem() );

	if( base_data == NULL )
		return;

	wxTreeProjectData* proj_data;
	wxTreeCharacterData* char_data;
	wxTreeRawAnimSetsData* ranims_data;
	wxTreeRawAnimSetData* ranimset_data;
	wxTreeRawAnimData* ranim_data;
	wxTreeAnimIndexesData* animindexes_data;
	wxTreeAnimIndexData* animindex_data;
	wxTreeAnimSetsData* anims_data;
	wxTreeAnimSetData* animset_data;
	wxTreeBaseAnimsData* banims_data;
	wxTreeBaseAnimData* banim_data;
	wxTreeAnimSpacesData* panims_data;
	wxTreeAnimSpaceData* panim_data;
	wxTreeAnimTreesData* animtrees_data;
	wxTreeAnimTreeData* animtree_data;

	// determine the correct popup menu to display
	if( ( proj_data = dynamic_cast<wxTreeProjectData*>(base_data) ) != NULL )
	{
		// display Project popup menu
	}
	else if( ( char_data = dynamic_cast<wxTreeCharacterData*>(base_data) ) != NULL )
	{
		// display Character popup menu

		wxMenu menu;
		menu.SetClientData(char_data);
		menu.Append( ID_mnCharacterSelect, "Select" );
		menu.Append( ID_mnCharacterRemove, "Remove" );
		menu.Append( ID_mnCharacterRename, "Rename" );
		menu.AppendSeparator();
		menu.Append( ID_mnCharacterSave, "Save" );
		PopupMenu(&menu);
	}
	else if( ( ranims_data = dynamic_cast<wxTreeRawAnimSetsData*>(base_data) ) != NULL )
	{
		// display Raw Animations popup menu

		wxMenu menu;
		menu.SetClientData(ranims_data);
		menu.Append( ID_mnRawAnimSetsAdd, "Add Anim. Set" );
		menu.Append( ID_mnRawAnimSetsClear, "Clear All Anim. Sets" );
		menu.AppendSeparator();
		menu.Append( ID_mnRawAnimSetsBuildMotionGraph, "Build Motion Graph" );
		menu.Append( ID_mnRawAnimSetsViewMotionGraph, "View Motion Graph" );
		PopupMenu(&menu);
	}
	else if( ( ranimset_data = dynamic_cast<wxTreeRawAnimSetData*>(base_data) ) != NULL )
	{
		// display Raw Animation Set popup menu
	}
	else if( ( ranim_data = dynamic_cast<wxTreeRawAnimData*>(base_data) ) != NULL )
	{
		// display Raw Animation popup menu

		wxMenu menu;
		menu.SetClientData(ranim_data);
		menu.Append( ID_mnRawAnimSelect, "Select" );
		menu.Append( ID_mnRawAnimRemove, "Remove" );
		menu.Append( ID_mnRawAnimRename, "Rename" );
		PopupMenu(&menu);
	}
	else if( ( animindexes_data = dynamic_cast<wxTreeAnimIndexesData*>(base_data) ) != NULL )
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
	else if( ( anims_data = dynamic_cast<wxTreeAnimSetsData*>(base_data) ) != NULL )
	{
		// display Animations popup menu

		wxMenu menu;
		menu.SetClientData(ranims_data);
		menu.Append( ID_mnAnimSetsCreate, "Create Anim. Set" );
		menu.Append( ID_mnAnimSetsAdd, "Add Anim. Set" );
		menu.Append( ID_mnAnimSetsClear, "Clear All Anim. Sets" );
		menu.AppendSeparator();
		menu.Append( ID_mnAnimSetsBuildTransitions, "Build Transitions" );
		menu.Append( ID_mnAnimSetsViewMotionGraph2, "View Motion Graph" );
		PopupMenu(&menu);
	}
	else if( ( animset_data = dynamic_cast<wxTreeAnimSetData*>(base_data) ) != NULL )
	{
		// display Animation Set popup menu
	}
	else if( ( banims_data = dynamic_cast<wxTreeBaseAnimsData*>(base_data) ) != NULL )
	{
		// display Base Animations popup menu
	}
	else if( ( banim_data = dynamic_cast<wxTreeBaseAnimData*>(base_data) ) != NULL )
	{
		// display Base Animation popup menu

		wxMenu menu;
		menu.SetClientData(banim_data);
		menu.Append( ID_mnBaseAnimSelect, "Select" );
		menu.Append( ID_mnBaseAnimRemove, "Remove" );
		menu.Append( ID_mnBaseAnimRename, "Rename" );
		PopupMenu(&menu);
	}
	else if( ( panims_data = dynamic_cast<wxTreeAnimSpacesData*>(base_data) ) != NULL )
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
	}
	else if( ( animtrees_data = dynamic_cast<wxTreeAnimTreesData*>(base_data) ) != NULL )
	{
		// display Animation Trees popup menu

		wxMenu menu;
		menu.SetClientData(animtrees_data);
		menu.Append( ID_mnAnimTreesCreate, "Create Anim. Tree" );
		menu.Append( ID_mnAnimTreesAdd, "Add Anim. Tree" );
		menu.Append( ID_mnAnimTreesClear, "Clear Anim. Trees" );
		PopupMenu(&menu);
	}
	else if( ( animtree_data = dynamic_cast<wxTreeAnimTreeData*>(base_data) ) != NULL )
	{
		// display Animation Tree popup menu

		wxMenu menu;
		menu.SetClientData(animtree_data);
		menu.Append( ID_mnAnimTreeSelect, "Select" );
		menu.Append( ID_mnAnimTreeRemove, "Remove" );
		menu.Append( ID_mnAnimTreeRename, "Rename" );
		PopupMenu(&menu);
	}
}

void ProjectViewWindow::OnMenu_ProjectSave( wxCommandEvent& evt )
{
	gApp->getAppFrame()->OnMenu_FileSaveProject(evt);
}

void ProjectViewWindow::OnMenu_ProjectSettings( wxCommandEvent& evt )
{
}

void ProjectViewWindow::OnMenu_ProjectCreateCharacter( wxCommandEvent& evt )
{
}

void ProjectViewWindow::OnMenu_ProjectClearCharacters( wxCommandEvent& evt )
{
}

void ProjectViewWindow::OnMenu_CharacterSelect( wxCommandEvent& evt )
{
	wxTreeCharacterData* char_data = static_cast<wxTreeCharacterData*>(
		static_cast<wxMenu*>( evt.GetEventObject() )->GetClientData()
		);

	// TODO: warn the user in case of unsaved changes made to the current character
	// and its resources
	
	gApp->selectCharacter( char_data->getCharacter()->getId() );
	refresh();
}

void ProjectViewWindow::OnMenu_CharacterSave( wxCommandEvent& evt )
{
}

void ProjectViewWindow::OnMenu_CharacterRemove( wxCommandEvent& evt )
{
}

void ProjectViewWindow::OnMenu_CharacterRename( wxCommandEvent& evt )
{
}

void ProjectViewWindow::OnMenu_RawAnimSetsAdd( wxCommandEvent& evt )
{
}

void ProjectViewWindow::OnMenu_RawAnimSetsClear( wxCommandEvent& evt )
{
}

void ProjectViewWindow::OnMenu_RawAnimSetsBuildMotionGraph( wxCommandEvent& evt )
{
	gApp->getAppFrame()->OnMenu_ToolsBuildMotionGraph(evt);
}

void ProjectViewWindow::OnMenu_RawAnimSetsViewMotionGraph( wxCommandEvent& evt )
{
	gApp->getAppFrame()->OnMenu_ToolsViewMotionGraph(evt);
}

void ProjectViewWindow::OnMenu_RawAnimSetRemove( wxCommandEvent& evt )
{
}

void ProjectViewWindow::OnMenu_RawAnimSetRename( wxCommandEvent& evt )
{
}

void ProjectViewWindow::OnMenu_RawAnimSetClear( wxCommandEvent& evt )
{
}

void ProjectViewWindow::OnMenu_RawAnimSelect( wxCommandEvent& evt )
{
	wxTreeRawAnimData* ranim_data = static_cast<wxTreeRawAnimData*>(
		static_cast<wxMenu*>( evt.GetEventObject() )->GetClientData()
		);
	
	zh::Animation* anim = ranim_data->getAnimation();
	gApp->selectAnimation( anim->getAnimationSet()->getName(), anim->getName() );
}

void ProjectViewWindow::OnMenu_RawAnimRemove( wxCommandEvent& evt )
{
	wxTreeRawAnimData* ranim_data = static_cast<wxTreeRawAnimData*>(
		static_cast<wxMenu*>( evt.GetEventObject() )->GetClientData()
		);
	
	zh::Animation* anim = ranim_data->getAnimation();
	gApp->removeAnimation( gApp->getCurrentCharacter()->getId(), anim->getAnimationSet()->getName(), anim->getName() );
	trcProjTree->Delete( ranim_data->GetId() );
}

void ProjectViewWindow::OnMenu_RawAnimRename( wxCommandEvent& evt )
{
}

void ProjectViewWindow::OnMenu_AnimIndexesBuildIndex( wxCommandEvent& evt )
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

void ProjectViewWindow::OnMenu_AnimSetsCreate( wxCommandEvent& evt )
{
}

void ProjectViewWindow::OnMenu_AnimSetsAdd( wxCommandEvent& evt )
{
}

void ProjectViewWindow::OnMenu_AnimSetsClear( wxCommandEvent& evt )
{
}

void ProjectViewWindow::OnMenu_AnimSetsBuildTransitions( wxCommandEvent& evt )
{
	// TODO: make sure a valid animation is selected!
	gApp->getAppFrame()->OnMenu_ToolsBuildTransitions(evt);
}

void ProjectViewWindow::OnMenu_AnimSetsViewMotionGraph2( wxCommandEvent& evt )
{
	gApp->getAppFrame()->OnMenu_ToolsViewMotionGraph2(evt);
}

void ProjectViewWindow::OnMenu_AnimSetRemove( wxCommandEvent& evt )
{
}

void ProjectViewWindow::OnMenu_AnimSetRename( wxCommandEvent& evt )
{
}

void ProjectViewWindow::OnMenu_AnimSetClear( wxCommandEvent& evt )
{
}

void ProjectViewWindow::OnMenu_BaseAnimSelect( wxCommandEvent& evt )
{
	wxTreeBaseAnimData* banim_data = static_cast<wxTreeBaseAnimData*>(
		static_cast<wxMenu*>( evt.GetEventObject() )->GetClientData()
		);
	
	zh::Animation* anim = banim_data->getAnimation();
	gApp->selectAnimation( anim->getAnimationSet()->getName(), anim->getName() );
}

void ProjectViewWindow::OnMenu_BaseAnimRemove( wxCommandEvent& evt )
{
}

void ProjectViewWindow::OnMenu_BaseAnimRename( wxCommandEvent& evt )
{
}

void ProjectViewWindow::OnMenu_AnimSpacesBuildAnimSpace( wxCommandEvent& evt )
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

	/* TODO: remove this
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

		gApp->getAppFrame()->getOgreWindow()->deletePointSet( "BaseSamples" );
		gApp->getAppFrame()->getOgreWindow()->createPointSet( "BaseSamples", pts, Ogre::ColourValue( 1, 0, 1 ), 12 );
		gApp->getAppFrame()->getOgreWindow()->showPointSet( "BaseSamples" );
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

		gApp->getAppFrame()->getOgreWindow()->deletePointSet( "DenseSamples" );
		gApp->getAppFrame()->getOgreWindow()->createPointSet( "DenseSamples", pts, Ogre::ColourValue( 0, 1, 0 ), 5 );
		gApp->getAppFrame()->getOgreWindow()->showPointSet( "DenseSamples" );
		pts.clear();

		zh::Vector3 ptpos;
		ptpos = zh::Vector3( 40, 70, 25 );
		ptpos = ptpos * bscal;
		ptpos = ptpos.rotate( borient );
		ptpos = bpos + ptpos;
		pts.push_back( zhOgreVector3(ptpos) );

		gApp->getAppFrame()->getOgreWindow()->deletePointSet( "TargetPoint" );
		gApp->getAppFrame()->getOgreWindow()->createPointSet( "TargetPoint", pts, Ogre::ColourValue( 1, 0, 0 ), 12 );
		gApp->getAppFrame()->getOgreWindow()->showPointSet( "TargetPoint" );
		pts.clear();
	}
	*/

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
	
	Character* ch = gApp->getCurrentCharacter();
	Model* mdl = ch->getModelController()->getModel();
	zh::AnimationSpace* anim_space = panim_data->getAnimationSpace();

	DefineAnimParamDialog dlg( this, wxID_ANY, mdl, anim_space );
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
	/*std::vector<AnimationParamSpec> pspecs;
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
		);*/
	zhAnimationSearchSystem->parametrizeAnimSpace( mdl, anim_space, dlg.getParamSpecs() );

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
}

void ProjectViewWindow::OnMenu_AnimTreesCreate( wxCommandEvent& evt )
{
	Character* ch = gApp->getCurrentCharacter();
	gApp->createAnimationTree( ch->getId() );
	
	refresh();
}

void ProjectViewWindow::OnMenu_AnimTreesAdd( wxCommandEvent& evt )
{
}

void ProjectViewWindow::OnMenu_AnimTreesClear( wxCommandEvent& evt )
{
}

void ProjectViewWindow::OnMenu_AnimTreeSelect( wxCommandEvent& evt )
{
	wxTreeAnimTreeData* animtree_data = static_cast<wxTreeAnimTreeData*>(
		static_cast<wxMenu*>( evt.GetEventObject() )->GetClientData()
		);
	
	AnimationTreePtr anim_tree = animtree_data->getAnimationTree();
	gApp->selectAnimationTree( anim_tree->getName() );
}

void ProjectViewWindow::OnMenu_AnimTreeRemove( wxCommandEvent& evt )
{
}

void ProjectViewWindow::OnMenu_AnimTreeRename( wxCommandEvent& evt )
{
}

BEGIN_EVENT_TABLE( ProjectViewWindow, wxWindow )
	EVT_SIZE( ProjectViewWindow::OnSize )
	EVT_TREE_ITEM_RIGHT_CLICK( ID_trcProjTree, ProjectViewWindow::OnRightClick_ProjTree )
	EVT_MENU( ID_mnProjectSave, ProjectViewWindow::OnMenu_ProjectSave )
	EVT_MENU( ID_mnProjectSettings, ProjectViewWindow::OnMenu_ProjectSettings )
	EVT_MENU( ID_mnProjectCreateCharacter, ProjectViewWindow::OnMenu_ProjectCreateCharacter )
	EVT_MENU( ID_mnProjectClearCharacters, ProjectViewWindow::OnMenu_ProjectClearCharacters )
	EVT_MENU( ID_mnCharacterSelect, ProjectViewWindow::OnMenu_CharacterSelect )
	EVT_MENU( ID_mnCharacterRemove, ProjectViewWindow::OnMenu_CharacterRemove )
	EVT_MENU( ID_mnCharacterRename, ProjectViewWindow::OnMenu_CharacterRename )
	EVT_MENU( ID_mnCharacterSave, ProjectViewWindow::OnMenu_CharacterSave )
	EVT_MENU( ID_mnRawAnimSetsAdd, ProjectViewWindow::OnMenu_RawAnimSetsAdd )
	EVT_MENU( ID_mnRawAnimSetsClear, ProjectViewWindow::OnMenu_RawAnimSetsClear )
	EVT_MENU( ID_mnRawAnimSetsBuildMotionGraph, ProjectViewWindow::OnMenu_RawAnimSetsBuildMotionGraph )
	EVT_MENU( ID_mnRawAnimSetsViewMotionGraph, ProjectViewWindow::OnMenu_RawAnimSetsViewMotionGraph )
	EVT_MENU( ID_mnRawAnimSetRemove, ProjectViewWindow::OnMenu_RawAnimSetRemove )
	EVT_MENU( ID_mnRawAnimSetRename, ProjectViewWindow::OnMenu_RawAnimSetRename )
	EVT_MENU( ID_mnRawAnimSetClear, ProjectViewWindow::OnMenu_RawAnimSetClear )
	EVT_MENU( ID_mnRawAnimSelect, ProjectViewWindow::OnMenu_RawAnimSelect )
	EVT_MENU( ID_mnRawAnimRemove, ProjectViewWindow::OnMenu_RawAnimRemove )
	EVT_MENU( ID_mnRawAnimRename, ProjectViewWindow::OnMenu_RawAnimRename )
	EVT_MENU( ID_mnAnimIndexesBuildIndex, ProjectViewWindow::OnMenu_AnimIndexesBuildIndex )
	EVT_MENU( ID_mnAnimIndexesAddIndex, ProjectViewWindow::OnMenu_AnimIndexesAddIndex )
	EVT_MENU( ID_mnAnimIndexesClearIndexes, ProjectViewWindow::OnMenu_AnimIndexesClearIndexes )
	EVT_MENU( ID_mnAnimIndexesViewMatchWeb, ProjectViewWindow::OnMenu_AnimIndexesViewMatchWeb )
	EVT_MENU( ID_mnAnimIndexesSearch, ProjectViewWindow::OnMenu_AnimIndexesSearch )
	EVT_MENU( ID_mnAnimIndexRemove, ProjectViewWindow::OnMenu_AnimIndexRemove )
	EVT_MENU( ID_mnAnimIndexRename, ProjectViewWindow::OnMenu_AnimIndexRename )
	EVT_MENU( ID_mnAnimSetsCreate, ProjectViewWindow::OnMenu_AnimSetsCreate )
	EVT_MENU( ID_mnAnimSetsAdd, ProjectViewWindow::OnMenu_AnimSetsAdd )
	EVT_MENU( ID_mnAnimSetsClear, ProjectViewWindow::OnMenu_AnimSetsClear )
	EVT_MENU( ID_mnAnimSetsBuildTransitions, ProjectViewWindow::OnMenu_AnimSetsBuildTransitions )
	EVT_MENU( ID_mnAnimSetsViewMotionGraph2, ProjectViewWindow::OnMenu_AnimSetsViewMotionGraph2 )
	EVT_MENU( ID_mnAnimSetRemove, ProjectViewWindow::OnMenu_AnimSetRemove )
	EVT_MENU( ID_mnAnimSetRename, ProjectViewWindow::OnMenu_AnimSetRename )
	EVT_MENU( ID_mnAnimSetClear, ProjectViewWindow::OnMenu_AnimSetClear )
	EVT_MENU( ID_mnBaseAnimSelect, ProjectViewWindow::OnMenu_BaseAnimSelect )
	EVT_MENU( ID_mnBaseAnimRemove, ProjectViewWindow::OnMenu_BaseAnimRemove )
	EVT_MENU( ID_mnBaseAnimRename, ProjectViewWindow::OnMenu_BaseAnimRename )
	EVT_MENU( ID_mnAnimSpacesBuildAnimSpace, ProjectViewWindow::OnMenu_AnimSpacesBuildAnimSpace )
	EVT_MENU( ID_mnAnimSpacesClear, ProjectViewWindow::OnMenu_AnimSpacesClear )
	EVT_MENU( ID_mnAnimSpaceSelect, ProjectViewWindow::OnMenu_AnimSpaceSelect )
	EVT_MENU( ID_mnAnimSpaceRemove, ProjectViewWindow::OnMenu_AnimSpaceRemove )
	EVT_MENU( ID_mnAnimSpaceRename, ProjectViewWindow::OnMenu_AnimSpaceRename )
	EVT_MENU( ID_mnAnimSpaceDefineParam, ProjectViewWindow::OnMenu_AnimSpaceDefineParam )
	EVT_MENU( ID_mnAnimSpaceMatchAnnots, ProjectViewWindow::OnMenu_AnimSpaceMatchAnnots )
	EVT_MENU( ID_mnAnimSpaceBuildBlendCurves, ProjectViewWindow::OnMenu_AnimSpaceBuildBlendCurves )
	EVT_MENU( ID_mnAnimTreesCreate, ProjectViewWindow::OnMenu_AnimTreesCreate )
	EVT_MENU( ID_mnAnimTreesAdd, ProjectViewWindow::OnMenu_AnimTreesAdd )
	EVT_MENU( ID_mnAnimTreesClear, ProjectViewWindow::OnMenu_AnimTreesClear )
	EVT_MENU( ID_mnAnimTreeSelect, ProjectViewWindow::OnMenu_AnimTreeSelect )
	EVT_MENU( ID_mnAnimTreeRemove, ProjectViewWindow::OnMenu_AnimTreeRemove )
	EVT_MENU( ID_mnAnimTreeRename, ProjectViewWindow::OnMenu_AnimTreeRename )
END_EVENT_TABLE()
