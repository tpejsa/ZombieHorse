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

#include "BoneTransformControllerPropGrid.h"

BoneTransformControllerPropGrid::BoneTransformControllerPropGrid( wxWindow* parent, BoneTransformController* bc )
: BoneControllerPropGrid( parent, bc )
{
	BoneTransformController* ctrl = static_cast<BoneTransformController*>(mBoneCtrl);

	AppendCategory( "BoneTransformController", wxPG_LABEL );
	
	//
	// TODO: properties
	// - translation as wxString (of the form x y z)
	// - rotation as wxString (of the form yaw,pitch,roll or ax;ay;az;phi)
	// - scale as wxString (of the form x y z)
	// - transformMode as wxEnum (possible values: RelToInitial, RelToCurrent, RelToTarget)
	//

	zh::Vector3 trans = ctrl->getTranslation();
	std::ostringstream oss_t;
	oss_t << trans.x;
	oss_t << " ";
	oss_t << trans.y;
	oss_t << " ";
	oss_t << trans.z;
	wxString trans_str(oss_t.str());
	Append( new wxStringProperty( "translation", wxPG_LABEL, trans_str ) );

	zh::Quat rot = ctrl->getRotation();
	std::ostringstream oss_r;
	oss_r << rot.x;
	oss_r << " ";
	oss_r << rot.y;
	oss_r << " ";
	oss_r << rot.z;
	wxString rot_str(oss_r.str());
	Append( new wxStringProperty( "rotation", wxPG_LABEL, rot_str ) );

	zh::Vector3 scale = ctrl->getScale();
	std::ostringstream oss_s;
	oss_s << scale.x;
	oss_s << " ";
	oss_s << scale.y;
	oss_s << " ";
	oss_s << scale.z;
	wxString scale_str(oss_s.str());
	Append( new wxStringProperty( "scale", wxPG_LABEL, scale_str ) );
	
	wxArrayString trans_mode;
	trans_mode.Add("RelToInitial");
	trans_mode.Add("RelToCurrent");
	trans_mode.Add("RelToTarget");
	Append( new wxEnumProperty( "transformMode", wxPG_LABEL, trans_mode ) );
}

BoneTransformController* BoneTransformControllerPropGrid::getBoneTransformController() const
{
	return static_cast<BoneTransformController*>(mBoneCtrl);
}

void BoneTransformControllerPropGrid::OnPGChanged( wxPropertyGridEvent& evt )
{
	BoneControllerPropGrid::OnPGChanged(evt);

	BoneTransformController* ctrl = static_cast<BoneTransformController*>(mBoneCtrl),
		*orig_node = static_cast<BoneTransformController*>(mOrigCtrl);

	if( evt.GetPropertyName() == "translation" )
	{
		wxString translation = evt.GetProperty()->GetValue().GetString();
		std::string trans_str = std::string( translation.mb_str() );
		zh::Vector3 trans_vec;
		if ( !parseVector3(trans_str, trans_vec) )
		{
			evt.GetProperty()->SetValueFromString("0 0 0");
			return;
		}
		
		ctrl->setTranslation(trans_vec);
		if( orig_node != NULL )
			orig_node->setTranslation(trans_vec);
	}
	else if( evt.GetPropertyName() == "rotation" )
	{
		wxString rotation = evt.GetProperty()->GetValue().GetString();
		std::string rot_str = std::string( rotation.mb_str() );
		zh::Vector3 rot_vec;
		if ( !parseVector3(rot_str, rot_vec) )
		{
			evt.GetProperty()->SetValueFromString("0 0 0");
			return;
		}
		
		ctrl->setRotation(zh::Quat(rot_vec,0));
		if( orig_node != NULL )
			orig_node->setRotation(zh::Quat(rot_vec,0));
	}
	else if( evt.GetPropertyName() == "scale" )
	{
		wxString scale = evt.GetProperty()->GetValue().GetString();
		std::string scale_str = std::string( scale.mb_str() );
		zh::Vector3 scale_vec;
		if ( !parseVector3(scale_str, scale_vec) )
		{
			evt.GetProperty()->SetValueFromString("0 0 0");
			return;
		}
		
		ctrl->setScale(scale_vec);
		if( orig_node != NULL )
			orig_node->setScale(scale_vec);
	}
	else if( evt.GetPropertyName() == "transformMode" )
	{
		if( evt.GetPropertyValueAsString() == "RelToCurrent" )
		{
			ctrl->setTransformMode( zh::BoneTransformMode_RelToCurrent );
			if( orig_node != NULL )
				orig_node->setTransformMode( zh::BoneTransformMode_RelToCurrent );
		}
		else if( evt.GetPropertyValueAsString() == "RelToInitial" )
		{
			ctrl->setTransformMode( zh::BoneTransformMode_RelToInitial );
			if( orig_node != NULL )
				orig_node->setTransformMode( zh::BoneTransformMode_RelToInitial );
		}
		else if( evt.GetPropertyValueAsString() == "RelToTarget" )
		{
			ctrl->setTransformMode( zh::BoneTransformMode_RelToTarget );
			if( orig_node != NULL )
				orig_node->setTransformMode( zh::BoneTransformMode_RelToTarget );
		}
	}
}

bool BoneTransformControllerPropGrid::parseVector3( const std::string& zhtr, zh::Vector3& v )
{
	std::istringstream iss(zhtr);
	if( iss.eof() )
		return false;
	iss >> v.x;
	if( iss.eof() )
		return false;
	iss >> v.y;
	if( iss.eof() )
		return false;
	iss >> v.z;

	return true;
}

BEGIN_EVENT_TABLE( BoneTransformControllerPropGrid, BoneControllerPropGrid )
	EVT_PG_CHANGED( wxID_ANY, OnPGChanged )
END_EVENT_TABLE()
