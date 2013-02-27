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

#include "AnimationBlenderPropGrid.h"
#include "AnimationStudioApp.h"
#include "boost/lexical_cast.hpp"

AnimationBlenderPropGrid::AnimationBlenderPropGrid( wxWindow* parent, AnimationBlender* animNode )
: AnimationNodePropGrid( parent, animNode )
{
	AnimationBlender* anim_node = static_cast<AnimationBlender*>(mAnimNode);

	AppendCategory( "AnimationBlender", wxPG_LABEL );
	
	//
	// TODO: properties
	// - weights as wxArrayString
	// - params as wxArrayString
	// - animationSet as wxString
	// - animationSpace as wxString
	// - useBlendCurves as wxBool
	// - annotationsEnabled as wxBool
	// - plantConstraintControllers as wxArrayString (where each element is a BoneName:BoneControllerName pair)
	//

	wxArrayString weights, params, plant_const_constr;
	if( anim_node->getNumChildren() > 0 )
	{
		zh::AnimationNode::ChildConstIterator iter = anim_node->getChildConstIterator();
		for( unsigned int i = 0; i < anim_node->getNumChildren(); ++i )
		{
			AnimationNode* node = iter.next();
			wxString w_str = wxString(node->getName());
			//wxString p_str = wxString(node->getName());
			w_str += ":";
			//p_str += ":";
			w_str += wxString::Format(wxT("%f"), anim_node->getWeight(node->getId()) );
			//p_str += wxString::Format(wxT("%f"), anim_node->getParams()(node->getId()) );
			weights.Add(w_str);
			//params.Add(p_str);
		}
	}
	if( anim_node->getParams().size() > 0 )
	{
		zh::Vector p( anim_node->getParams().size(), 0 );
		p = anim_node->getParams();
		for( unsigned int j = 0; j < p.size(); ++j )
		{
			wxString p_str = wxString::Format(wxT("%f"), p[j]);
			params.Add(p_str);
		}
	}
	AnimationBlender::PlantConstrControllerConstIterator pcc_it = anim_node->getPlantConstrControllerConstIterator();
	while ( !pcc_it.end() )
	{
		BoneIKController* pcc = pcc_it.next();
		std::string bc_name = pcc->getName();
		std::string bone_name = gApp->getCurrentCharacter()->getModelController()->getModel()->getSkeleton()->getBone(pcc->getBoneId())->getName();
		std::ostringstream oss;
		oss << bc_name;
		oss << ":";
		oss << bone_name;
		wxString pcc_str(oss.str());
		plant_const_constr.Add(pcc_str);
	}

	Append( new wxArrayStringProperty( "weights", wxPG_LABEL, weights ) );
	Append( new wxArrayStringProperty( "params", wxPG_LABEL, params ) );
	if ( anim_node->getAnimationSet() != NULL )
		Append( new wxStringProperty( "animationSet", wxPG_LABEL, anim_node->getAnimationSet()->getName() ) );
	if ( anim_node->getAnimationSpace() != NULL )
		Append( new wxStringProperty( "animationSpace", wxPG_LABEL, anim_node->getAnimationSpace()->getName() ) );
	Append( new wxBoolProperty( "useBlendCurves", wxPG_LABEL, anim_node->getUseBlendCurves() ) );
	Append( new wxArrayStringProperty( "plantConstraintControllers", wxPG_LABEL, plant_const_constr ) );
}

AnimationBlender* AnimationBlenderPropGrid::getAnimationBlender() const
{
	return static_cast<AnimationBlender*>(mAnimNode);
}

void AnimationBlenderPropGrid::OnPGChanged( wxPropertyGridEvent& evt )
{
	AnimationNodePropGrid::OnPGChanged(evt);

	AnimationBlender* anim_node = static_cast<AnimationBlender*>(mAnimNode),
		*orig_node = static_cast<AnimationBlender*>(mOrigNode);

	if ( evt.GetPropertyName() == "weights" )
	{
		wxArrayString weights = evt.GetProperty()->GetValue().GetArrayString();
		zh::Vector w(weights.size(),0);
		for( size_t weights_i = 0; weights_i < weights.size(); ++weights_i )
		{
			std::string weight_str = std::string( weights[weights_i].mb_str() );
			std::string anim_node_name, weight;
			parseStrings( weight_str, anim_node_name, weight );
			float weight_val = boost::lexical_cast<float>( weight );
		anim_node->setWeight( anim_node_name, weight_val );
		if( orig_node != NULL )
			orig_node->setWeight( anim_node_name, weight_val );
		}
		evt.GetProperty()->SetValue( wxVariant(weights) );
	}
	else if ( evt.GetPropertyName() == "params" )
	{
		wxArrayString params = evt.GetProperty()->GetValue().GetArrayString();
		zh::Vector p(params.size(),0);
		for( size_t params_i = 0; params_i < params.size(); ++params_i )
		{
			std::string params_str = std::string( params[params_i].mb_str() );
			std::istringstream stream (params_str);
			float param_val;
			stream >> param_val;
			p[params_i] = param_val;
		}
		anim_node->setParams( p );
		if( orig_node != NULL )
			orig_node->setParams( p );

		evt.GetProperty()->SetValue( wxVariant(params) );
	}
	else if( evt.GetPropertyName() == "animationSet" )
	{
		std::string anim_set_str = evt.GetProperty()->GetValueAsString().c_str();
		AnimationSetPtr anim_set = gApp->getAnimationSet( gApp->getCurrentCharacter()->getId(), anim_set_str );
		if(  anim_set == NULL  || !anim_set->getNumAnimations() )
		{
			evt.GetProperty()->SetValueFromString( anim_node->getAnimationSet()->getName() ); // reset value
			return;
		}
		anim_node->setAnimationSpace( anim_set, anim_node->getAnimationSpaceId() );
		if( orig_node != NULL )
			orig_node->setAnimationSpace( anim_set, anim_node->getAnimationSpaceId() );
	}
	else if( evt.GetPropertyName() == "animationSpace" )
	{
		std::string anim_space_str = evt.GetProperty()->GetValueAsString().c_str();
		AnimationSpace *anim_space = anim_node->getAnimationSet()->getAnimationSpace(anim_space_str);
		if( anim_space == NULL  || !anim_space->getNumBaseAnimations() )
		{
			evt.GetProperty()->SetValueFromString( anim_node->getAnimationSpace()->getName() ); // reset value
			return;
		}
		anim_node->setAnimationSpace( anim_node->getAnimationSet(), anim_space->getId() );
		if( orig_node != NULL )
			orig_node->setAnimationSpace( anim_node->getAnimationSet(), anim_space->getId() );
	}
	else if( evt.GetPropertyName() == "useBlendCurves" )
	{
		bool reg_curves = evt.GetProperty()->GetValue().GetBool();		
		anim_node->setUseBlendCurves(reg_curves);
		if( orig_node != NULL )
			orig_node->setUseBlendCurves(reg_curves);
	}
	else if( evt.GetPropertyName() == "plantConstraintControllers" )
	{
		wxArrayString pcc = evt.GetProperty()->GetValue().GetArrayString();
		for( size_t pcc_i = 0; pcc_i < pcc.size(); ++pcc_i )
		{
			std::string pcc_str = std::string( pcc[pcc_i].mb_str() );
			std::string bone_name, bc_name;
			//if ( !parseStrings(pcc_str, bone_name, bc_name) );
			if( pcc_str.find(":", 0) != std::string::npos )
			{
				size_t  pos = pcc_str.find(":", 0);
				bone_name = pcc_str.substr(0, pos);   
				pcc_str.erase(0, pos + 1);   
				bc_name = pcc_str;
			}
			else
			{
				pcc.erase( pcc.begin() + pcc_i );
				--pcc_i;
				continue;
			}

			zh::Bone* bone = gApp->getCurrentCharacter()->getModelController()->getModel()->getSkeleton()->getBone(bone_name);
			if ( bone == NULL || anim_node->getPlantConstrController( bone->getId() ) == NULL ) //kriv nacin za dobiti bc?
			{
				return;
			}
		
			anim_node->setPlantConstrController( bone->getId(), anim_node->getPlantConstrController( bone->getId() ) );
			if( orig_node != NULL )
				orig_node->setPlantConstrController( bone->getId(), anim_node->getPlantConstrController( bone->getId() ) );

		}
		// invalid transitions may have been erased, refresh control
		evt.GetProperty()->SetValue( wxVariant(pcc) );
	}
}

bool AnimationBlenderPropGrid::parseStrings( const std::string& istr, std::string& string1, std::string& string2 )
{
	if( istr.find(":", 0) != std::string::npos )
	{
		size_t  pos = istr.find(":", 0);
		string1 = istr.substr(0, pos);   
		//istr.erase(0, pos + 1);   
		string2 = istr.substr(pos + 1, istr.size() );
	}
	else
	{
		return false;
	}
	return true;
}

BEGIN_EVENT_TABLE( AnimationBlenderPropGrid, AnimationNodePropGrid )
	EVT_PG_CHANGED( wxID_ANY, OnPGChanged )
END_EVENT_TABLE()
