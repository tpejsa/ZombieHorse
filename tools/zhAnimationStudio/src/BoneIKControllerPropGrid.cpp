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

#include "BoneIKControllerPropGrid.h"

BoneIKControllerPropGrid::BoneIKControllerPropGrid( wxWindow* parent, BoneIKController* bc )
: BoneControllerPropGrid( parent, bc )
{
	BoneIKController* ctrl = static_cast<BoneIKController*>(mBoneCtrl);

	AppendCategory( "BoneIKController", wxPG_LABEL );
	
	//
	// TODO: properties
	// - targetPosition as wxString (of the form x y z)
	// - offsetRotation as wxString (of the form x y z)
	// - IKSolver as wxEnum (possible values: CCD)
	//

	zh::Vector3 target_pos = ctrl->getTargetPosition();
	std::ostringstream oss_p;
	oss_p << target_pos.x;
	oss_p << " ";
	oss_p << target_pos.y;
	oss_p << " ";
	oss_p << target_pos.z;
	std::string pos = oss_p.str();
	wxString target_pos_str(pos);
	Append( new wxStringProperty( "targetPosition", wxPG_LABEL, target_pos_str ) );

	zh::Quat offset_rot = ctrl->getOffsetRotation();
	std::ostringstream oss_r;
	oss_r << offset_rot.x;
	oss_r << " ";
	oss_r << offset_rot.y;
	oss_r << " ";
	oss_r << offset_rot.z;
	std::string rot = oss_r.str();
	wxString offset_rot_str(rot);
	Append( new wxStringProperty( "offsetRotation", wxPG_LABEL, offset_rot_str ) );
	
	wxArrayString ik_solver;
	ik_solver.Add("CCD");
	Append( new wxEnumProperty( "IKSolver", wxPG_LABEL, ik_solver ) );
	
}

BoneIKController* BoneIKControllerPropGrid::getBoneIKController() const
{
	return static_cast<BoneIKController*>(mBoneCtrl);
}

void BoneIKControllerPropGrid::OnPGChanged( wxPropertyGridEvent& evt )
{
	BoneControllerPropGrid::OnPGChanged(evt);

	BoneIKController* ctrl = static_cast<BoneIKController*>(mBoneCtrl),
		*orig_node = static_cast<BoneIKController*>(mOrigCtrl);

	if( evt.GetPropertyName() == "targetPosition" )
	{
		wxString target_pos = evt.GetProperty()->GetValue().GetString();
		std::string target_pos_str = std::string( target_pos.mb_str() );
		zh::Vector3 target_pos_vec;
		if ( !parseVector3(target_pos_str, target_pos_vec) )
		{
			evt.GetProperty()->SetValueFromString("0 0 0");
			return;
		}
		
		ctrl->setTargetPosition(target_pos_vec);
		if( orig_node != NULL )
			orig_node->setTargetPosition(target_pos_vec);
	}
	else if( evt.GetPropertyName() == "offsetRotation" )
	{
		wxString offset_rotation = evt.GetProperty()->GetValue().GetString();
		std::string offset_rot_str = std::string( offset_rotation.mb_str() );
		zh::Vector3 offset_rot_vec;
		if ( !parseVector3(offset_rot_str, offset_rot_vec) )
		{
			evt.GetProperty()->SetValueFromString("0 0 0");
			return;
		}
		
		ctrl->setOffsetRotation(zh::Quat(offset_rot_vec,0));
		if( orig_node != NULL )
			orig_node->setOffsetRotation(zh::Quat(offset_rot_vec,0));
	}
	else if( evt.GetPropertyName() == "IKSolver" )
	{
		if( evt.GetPropertyValueAsString() == "CCD" )
		{
			ctrl->setIKSolver( zh::IKSolver_CCD );
			if( orig_node != NULL )
				orig_node->setIKSolver( zh::IKSolver_CCD );
		}
	}
}

bool BoneIKControllerPropGrid::parseVector3( const std::string& zhtr, zh::Vector3& v )
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

BEGIN_EVENT_TABLE( BoneIKControllerPropGrid, BoneControllerPropGrid )
	EVT_PG_CHANGED( wxID_ANY, OnPGChanged )
END_EVENT_TABLE()
