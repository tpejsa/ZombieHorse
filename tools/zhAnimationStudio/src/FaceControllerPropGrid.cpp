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

#include "FaceControllerPropGrid.h"

FaceControllerPropGrid::FaceControllerPropGrid( wxWindow* parent, FaceController* animNode )
: AnimationNodePropGrid( parent, animNode )
{
	FaceController* anim_node = static_cast<FaceController*>(mAnimNode);

	AppendCategory( "FaceController", wxPG_LABEL );
	
	//
	// TODO: properties
	// - morphTargetWeights as wxArrayString of value pairs MTName;MTWeight
	//

	wxArrayString morph_target_weights;
	const zh::Vector vec = anim_node->getMorphTargetWeights();
	for( unsigned int vec_i = 0; vec_i < vec.size(); ++vec_i )
	{
		std::string morph_str;
		//morph_str = vec[vec_i];
		//morph_target_weights.Add(morph_str);
	}
	Append( new wxArrayStringProperty( "morphTargetWeights", wxPG_LABEL, morph_target_weights ) );
}

FaceController* FaceControllerPropGrid::getFaceController() const
{
	return static_cast<FaceController*>(mAnimNode);
}

void FaceControllerPropGrid::OnPGChanged( wxPropertyGridEvent& evt )
{
	AnimationNodePropGrid::OnPGChanged(evt);

	FaceController* anim_node = static_cast<FaceController*>(mAnimNode),
		*orig_node = static_cast<FaceController*>(mOrigNode);
}

BEGIN_EVENT_TABLE( FaceControllerPropGrid, AnimationNodePropGrid )
	EVT_PG_CHANGED( wxID_ANY, OnPGChanged )
END_EVENT_TABLE()
