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

#include "AnimationSamplerPropGrid.h"
#include "AnimationStudioApp.h"

AnimationSamplerPropGrid::AnimationSamplerPropGrid( wxWindow* parent, AnimationSampler* animNode )
: AnimationNodePropGrid( parent, animNode )
{
	AnimationSampler* anim_node = static_cast<AnimationSampler*>(mAnimNode);

	AppendCategory( "AnimationSampler", wxPG_LABEL );
	
	//
	// TODO: properties
	// - animationSet as wxString
	// - animation as wxString
	// - annotationsEnabled as wxBool
	//

	Append( new wxStringProperty( "animationSet", wxPG_LABEL, anim_node->getAnimationSet()->getName() ) );
	Append( new wxStringProperty( "animation", wxPG_LABEL, anim_node->getAnimation()->getName() ) );

}

AnimationSampler* AnimationSamplerPropGrid::getAnimationSampler() const
{
	return static_cast<AnimationSampler*>(mAnimNode);
}

void AnimationSamplerPropGrid::OnPGChanged( wxPropertyGridEvent& evt )
{
	AnimationNodePropGrid::OnPGChanged(evt);

	AnimationSampler* anim_node = static_cast<AnimationSampler*>(mAnimNode),
		*orig_node = static_cast<AnimationSampler*>(mOrigNode);

	if( evt.GetPropertyName() == "animationSet" )
	{
		std::string anim_set_str = evt.GetProperty()->GetValueAsString().c_str();
		AnimationSetPtr anim_set = gApp->getAnimationSet( gApp->getCurrentCharacter()->getId(), anim_set_str );
		if(  anim_set == NULL  || !anim_set->getNumAnimations() )
		{
			evt.GetProperty()->SetValueFromString(anim_node->getAnimationSet()->getName()); // reset value
			return;
		}
		AnimationSet::AnimationConstIterator iter = anim_set->getAnimationConstIterator();
		zh::Animation *anim = iter.next();
		anim_node->setAnimation( anim_set, anim->getId() );
		if( orig_node != NULL )
			orig_node->setAnimation( anim_set, anim->getId() );
	}
	else if( evt.GetPropertyName() == "animation" )
	{
		std::string anim_str = evt.GetProperty()->GetValueAsString().c_str();
		if( !anim_node->getAnimationSet()->hasAnimation(anim_str) )
		{
			evt.GetProperty()->SetValueFromString(anim_node->getAnimation()->getName()); // reset value
			return;
		}
		anim_node->setAnimation( anim_node->getAnimationSet(), anim_node->getAnimationSet()->getAnimation(anim_str)->getId() );
		if( orig_node != NULL )
			orig_node->setAnimation( anim_node->getAnimationSet(), anim_node->getAnimationSet()->getAnimation(anim_str)->getId() );
	}
}

BEGIN_EVENT_TABLE( AnimationSamplerPropGrid, AnimationNodePropGrid )
	EVT_PG_CHANGED( wxID_ANY, OnPGChanged )
END_EVENT_TABLE()
