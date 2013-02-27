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

#include "AnimationNodePropGrid.h"
#include "AnimationStudioApp.h"

AnimationNodePropGrid::AnimationNodePropGrid( wxWindow* parent, AnimationNode* animNode )
: wxPropertyGrid( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize ), mAnimNode(animNode), mOrigNode(NULL)
{
	zhAssert( animNode != NULL );

	// get the original anim. node from anim. tree resource
	AnimationTreeInstance* atree = dynamic_cast<AnimationTreeInstance*>( mAnimNode->getAnimationTree() );
	if( atree != NULL )
		mOrigNode = atree->getAnimationTree()->getNode( mAnimNode->getId() );

	AppendCategory( "General", wxPG_LABEL );
	Append( new wxIntProperty( "id", wxPG_LABEL, animNode->getId() ) );
	DisableProperty("id");
	Append( new wxStringProperty( "name", wxPG_LABEL, animNode->getName() ) );
	AppendCategory( "AnimationNode", wxPG_LABEL );
	Append( new wxStringProperty( "mainChild", wxPG_LABEL,
		animNode->getMainChild() != NULL ? animNode->getMainChild()->getName() : "" ) );
	Append( new wxBoolProperty( "playing", wxPG_LABEL, animNode->getPlaying() ) );
	Append( new wxBoolProperty( "paused", wxPG_LABEL, animNode->getPaused() ) );
	Append( new wxFloatProperty( "playTime", wxPG_LABEL, animNode->getPlayTime() ) );
	Append( new wxFloatProperty( "normalizedPlayTime", wxPG_LABEL, animNode->getNormalizedPlayTime() ) );
	Append( new wxFloatProperty( "playRate", wxPG_LABEL, animNode->getPlayRate() ) );
	Append( new wxFloatProperty( "playLength", wxPG_LABEL, animNode->getPlayLength() ) );
	DisableProperty("playLength");
	Append( new wxBoolProperty( "annotationsEnabled", wxPG_LABEL, animNode->getAnnotationsEnabled() ) );
	Append( new wxStringProperty( "origin", wxPG_LABEL, toString<Model::Situation>( animNode->getOrigin() ) ) );
	
	zhAssert( gApp->getCurrentCharacter() != NULL );
	zh::Skeleton* skel = gApp->getCurrentCharacter()->getModelController()->getModel()->getSkeleton();
	wxArrayString bone_mask;
	zh::Skeleton::BoneConstIterator bone_i = skel->getBoneConstIterator();
	while( !bone_i.end() )
	{
		zh::Bone* bone = bone_i.next();
		if( animNode->isBoneMasked( bone->getId() ) )
			bone_mask.Add( bone->getName() );
	}

	Append( new wxArrayStringProperty( "boneMask", wxPG_LABEL, bone_mask ) );
}

AnimationNode* AnimationNodePropGrid::getAnimationNode() const
{
	return mAnimNode;
}

void AnimationNodePropGrid::OnPGChanged( wxPropertyGridEvent& evt )
{
	/*if( evt.GetPropertyName() == "id" )
	{
		// validate property value
		unsigned short id = evt.GetProperty()->GetValue().GetInteger();
		if( mAnimNode->getAnimationTree()->hasNode(id) )
		{
			// duplicate node ID
			evt.GetProperty()->SetValueFromInt( mAnimNode->getId() ); // reset value
			return;
		}
	}
	else */if( evt.GetPropertyName() == "name" )
	{
		// validate property value
		std::string name = evt.GetProperty()->GetValueAsString().c_str();
		if( mAnimNode->getAnimationTree()->hasNode(name) )
		{
			// duplicate node name
			evt.GetProperty()->SetValueFromString( mAnimNode->getName() ); // reset value
			return;
		}

		mAnimNode->setName(name);
		if( mOrigNode != NULL )
			mOrigNode->setName(name);
	}
	else if( evt.GetPropertyName() == "mainChild" )
	{
		// validate property value
		std::string name = evt.GetProperty()->GetValueAsString().c_str();
		if( !mAnimNode->hasChild(name) )
		{
			// non-existent child
			evt.GetProperty()->SetValueFromString(""); // reset value
			name = "";
		}

		mAnimNode->setMainChild(name);
		if( mOrigNode != NULL )
			mOrigNode->setMainChild(name);
	}
	else if( evt.GetPropertyName() == "playing" )
	{
		bool playing = evt.GetProperty()->GetValue().GetBool();
		mAnimNode->setPlaying(playing);
		if( mOrigNode != NULL )
			mOrigNode->setPlaying(playing);
	}
	else if( evt.GetPropertyName() == "paused" )
	{
		bool paused = evt.GetProperty()->GetValue().GetBool();
		mAnimNode->setPaused(paused);
		if( mOrigNode != NULL )
			mOrigNode->setPaused(paused);
	}
	else if( evt.GetPropertyName() == "playTime" )
	{
		float play_time = (float)evt.GetProperty()->GetValue().GetDouble();
		mAnimNode->setPlayTime(play_time);
		if( mOrigNode != NULL )
			mOrigNode->setPlayTime(play_time);
	}
	else if( evt.GetPropertyName() == "normalizedPlayTime" )
	{
		float play_time = (float)evt.GetProperty()->GetValue().GetDouble();
		mAnimNode->setNormalizedPlayTime(play_time);
		if( mOrigNode != NULL )
			mOrigNode->setNormalizedPlayTime(play_time);
	}
	else if( evt.GetPropertyName() == "playRate" )
	{
		float play_rate = (float)evt.GetProperty()->GetValue().GetDouble();
		mAnimNode->setPlayRate(play_rate);
		if( mOrigNode != NULL )
			mOrigNode->setPlayRate(play_rate);
	}
	else if( evt.GetPropertyName() == "annotationsEnabled" )
	{
		bool annot_enabled = evt.GetProperty()->GetValue().GetBool();
		mAnimNode->setAnnotationsEnabled(annot_enabled);
		if( mOrigNode != NULL )
			mOrigNode->setAnnotationsEnabled(annot_enabled);
	}
	else if( evt.GetPropertyName() == "origin" )
	{
		std::string orig_str = evt.GetProperty()->GetValueAsString().c_str();
		Model::Situation orig = fromString<Model::Situation>(orig_str);
		mAnimNode->setOrigin(orig);
		if( mOrigNode != NULL )
			mOrigNode->setOrigin(orig);
	}
	else if( evt.GetPropertyName() == "boneMask" )
	{
		zhAssert( gApp->getCurrentCharacter() != NULL );
		zh::Skeleton* skel = gApp->getCurrentCharacter()->getModelController()->getModel()->getSkeleton();
		zh::Skeleton::BoneConstIterator bone_i = skel->getBoneConstIterator();

		mAnimNode->unmaskAllBones();
		if( mOrigNode != NULL )
			mOrigNode->unmaskAllBones();

		wxArrayString bone_mask = evt.GetProperty()->GetValue().GetArrayString();
		for( size_t bone_i = 0; bone_i < bone_mask.Count(); ++bone_i )
		{
			std::string jname = bone_mask[bone_i].c_str();
			if( !skel->hasBone(jname) )
			{
				// non-existent bone specified, erase it
				bone_mask.erase( bone_mask.begin() + bone_i );
				--bone_i;
			}
			else
			{
				mAnimNode->maskBone( skel->getBone(jname)->getId() );
				if( mOrigNode != NULL )
					mOrigNode->maskBone( skel->getBone(jname)->getId() );
			}
		}

		// non-existent bones may have been erased, refresh control
		evt.GetProperty()->SetValue( wxVariant(bone_mask) );
	}

	_refreshPlaybackProps();
}

void AnimationNodePropGrid::_refreshPlaybackProps()
{
	// refresh playback properties
	GetProperty("playing")->SetValue( wxVariant( mAnimNode->getPlaying() ) );
	GetProperty("paused")->SetValue( wxVariant( mAnimNode->getPaused() ) );
	GetProperty("playTime")->SetValue( wxVariant( mAnimNode->getPlayTime() ) );
	GetProperty("normalizedPlayTime")->SetValue( wxVariant( mAnimNode->getNormalizedPlayTime() ) );
	GetProperty("playRate")->SetValue( wxVariant( mAnimNode->getPlayRate() ) );
	GetProperty("playLength")->SetValue( wxVariant( mAnimNode->getPlayLength() ) );
}

BEGIN_EVENT_TABLE( AnimationNodePropGrid, wxPropertyGrid )
	EVT_PG_CHANGED( wxID_ANY, OnPGChanged )
END_EVENT_TABLE()
