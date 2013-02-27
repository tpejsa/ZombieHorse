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

#include "BoneControllerPropGrid.h"
#include "AnimationStudioApp.h"

BoneControllerPropGrid::BoneControllerPropGrid( wxWindow* parent, BoneController* bc )
: wxPropertyGrid( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize ), mBoneCtrl(bc), mOrigCtrl(NULL)
{
	zhAssert( bc != NULL );

	// get the original bone controller from anim. tree resource
	AnimationTreeInstance* atree = dynamic_cast<AnimationTreeInstance*>( mBoneCtrl->getAnimationTree() );
	if( atree != NULL )
		mOrigCtrl = atree->getAnimationTree()->getBoneController( mBoneCtrl->getId() );

	//
	// TODO: properties
	// - name as wxString
	// - bone as wxString
	// - enabled as wxBool
	// - responseTime as wxFloat
	//

	AppendCategory( "BoneController", wxPG_LABEL );
	Append( new wxIntProperty( "id", wxPG_LABEL, bc->getId() ) );
	Append( new wxStringProperty( "name", wxPG_LABEL, bc->getName() ) );
	Append( new wxStringProperty( "bone", wxPG_LABEL, gApp->getCurrentCharacter()->getModelController()->getModel()->getSkeleton()->getBone(bc->getBoneId())->getName() ) );
	Append( new wxBoolProperty( "enabled", wxPG_LABEL, bc->getEnabled() ) );
	Append( new wxFloatProperty( "responseTime", wxPG_LABEL, bc->getResponseTime() ) );
	
	zhAssert( gApp->getCurrentCharacter() != NULL );
}

BoneController* BoneControllerPropGrid::getBoneController() const
{
	return mBoneCtrl;
}

void BoneControllerPropGrid::OnPGChanged( wxPropertyGridEvent& evt )
{
	if( evt.GetPropertyName() == "id" )
	{
		// validate property value
		unsigned short id = evt.GetProperty()->GetValue().GetInteger();
		if( mBoneCtrl->getAnimationTree()->hasBoneController(id) )
		{
			// duplicate node ID
			evt.GetProperty()->SetValueFromInt( mBoneCtrl->getId() ); // reset value
			return;
		}
	}
	else if( evt.GetPropertyName() == "name" )
	{
		// validate property value
		std::string name = evt.GetProperty()->GetValueAsString().c_str();
		if( mBoneCtrl->getAnimationTree()->hasBoneController(name) )
		{
			// duplicate node name
			evt.GetProperty()->SetValueFromString( mBoneCtrl->getName() ); // reset value
			return;
		}

		mBoneCtrl->setName(name);
		if( mOrigCtrl != NULL )
			mOrigCtrl->setName(name);
	}
	else if( evt.GetPropertyName() == "bone" )
	{
		// validate property value
		std::string bone_str = evt.GetProperty()->GetValueAsString().c_str();
		zh::Bone *bone = gApp->getCurrentCharacter()->getModelController()->getModel()->getSkeleton()->getBone(bone_str);
		if(  bone == NULL )
		{
			return;
		}
		mBoneCtrl->setBoneId( bone->getId() );
		if( mOrigCtrl != NULL )
			mOrigCtrl->setBoneId( bone->getId() );
	}
	else if( evt.GetPropertyName() == "enabled" )
	{
		bool enabled = evt.GetProperty()->GetValue().GetBool();
		mBoneCtrl->setEnabled(enabled);
		if( mOrigCtrl != NULL )
			mOrigCtrl->setEnabled(enabled);
	}
	else if( evt.GetPropertyName() == "responseTime" )
	{
		float response_time = (float)evt.GetProperty()->GetValue().GetDouble();
		mBoneCtrl->setResponseTime(response_time);
		if( mOrigCtrl != NULL )
			mOrigCtrl->setResponseTime(response_time);
	}

}

BEGIN_EVENT_TABLE( BoneControllerPropGrid, wxPropertyGrid )
	EVT_PG_CHANGED( wxID_ANY, OnPGChanged )
END_EVENT_TABLE()
