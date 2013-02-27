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

#include "AnimationTransitionBlenderPropGrid.h"
#include "boost/tokenizer.hpp"

AnimationTransitionBlenderPropGrid::AnimationTransitionBlenderPropGrid( wxWindow* parent, AnimationTransitionBlender* animNode )
: AnimationNodePropGrid( parent, animNode )
{
	AnimationTransitionBlender* anim_node = static_cast<AnimationTransitionBlender*>(mAnimNode);

	AppendCategory( "AnimationTransitionBlender", wxPG_LABEL );
	
	wxArrayString trans_queue;
	const AnimationTransitionBlender::TransitionQueue& tq = anim_node->getTransitionQueue();
	for( unsigned int trans_i = 0; trans_i < tq.size(); ++trans_i )
	{
		std::string trans_str;
		_writeTransitionStr( tq._Get_container()[trans_i], trans_str );
		trans_queue.Add(trans_str);
	}

	Append( new wxArrayStringProperty( "transitionQueue", wxPG_LABEL, trans_queue ) );

	Append( new wxFloatProperty( "defaultTransitionLength", wxPG_LABEL, anim_node->getDefaultTransitionLength() ) );
	Append( new wxStringProperty( "defaultNode", wxPG_LABEL, anim_node->getDefaultNode() != NULL ? anim_node->getDefaultNode()->getName() : "" ) );
}

AnimationTransitionBlender* AnimationTransitionBlenderPropGrid::getAnimationTransitionBlender() const
{
	return static_cast<AnimationTransitionBlender*>(mAnimNode);
}

void AnimationTransitionBlenderPropGrid::OnPGChanged( wxPropertyGridEvent& evt )
{
	AnimationNodePropGrid::OnPGChanged(evt);

	AnimationTransitionBlender* anim_node = static_cast<AnimationTransitionBlender*>(mAnimNode),
		*orig_node = static_cast<AnimationTransitionBlender*>(mOrigNode);

	if( evt.GetPropertyName() == "transitionQueue" )
	{
		anim_node->removeAllTransitions();
		if( orig_node != NULL )
			orig_node->removeAllTransitions();

		wxArrayString trans_queue = evt.GetProperty()->GetValue().GetArrayString();
		for( size_t trans_i = 0; trans_i < trans_queue.size(); ++trans_i )
		{
			AnimationTransitionBlender::Transition trans( 0, 0, 0, anim_node, zh::Vector() );
			if( !_parseTransitionStr( trans_queue[trans_i].c_str(), trans ) )
			{
				trans_queue.erase( trans_queue.begin() + trans_i );
				--trans_i;
				continue;
			}
			
			anim_node->addTransition(trans);
			if( orig_node != NULL )
				orig_node->addTransition(trans);
		}

		// invalid transitions may have been erased, refresh control
		evt.GetProperty()->SetValue( wxVariant(trans_queue) );
	}
	else if( evt.GetPropertyName() == "defaultTransitionLength" )
	{
		float length = (float)evt.GetProperty()->GetValue().GetDouble();
		anim_node->setDefaultTransitionLength(length);
		if( orig_node != NULL )
			orig_node->setDefaultTransitionLength(length);
	}
	else if( evt.GetPropertyName() == "defaultNode" )
	{
		// validate property value
		std::string name = evt.GetProperty()->GetValueAsString().c_str();
		if( !anim_node->hasChild(name) )
		{
			// non-existent child
			evt.GetProperty()->SetValueFromString(""); // reset value
			name = "";
		}

		anim_node->setDefaultNode(name);
		if( orig_node != NULL )
			orig_node->setDefaultNode(name);
	}
}

bool AnimationTransitionBlenderPropGrid::_parseTransitionStr( const std::string& transStr,
															AnimationTransitionBlender::Transition& trans ) const
{
	float st, et, tt;
	AnimationNode* tnode;

	boost::char_separator<char> sep1( ";" );
	boost::tokenizer< boost::char_separator<char> > trans_tok( transStr, sep1 );
	boost::tokenizer< boost::char_separator<char> >::const_iterator wtok_i = trans_tok.begin();

	// parse trans. start time
	if( wtok_i == trans_tok.end() )
		return false;
	st = fromString<float>(*wtok_i);
	++wtok_i;

	// parse trans. end time
	if( wtok_i == trans_tok.end() )
		return false;
	et = fromString<float>(*wtok_i);
	++wtok_i;

	// parse trans. target node
	if( wtok_i == trans_tok.end() )
		return false;
	std::string tnodestr = *wtok_i;
	if( !mAnimNode->hasChild(tnodestr) )
		return false;
	tnode = mAnimNode->getChild(tnodestr);
	++wtok_i;

	Vector tparams( tnode->getNumChildren() > 0 ? tnode->getNumChildren() : 1, 1.f );
	AnimationBlender* tbnode = dynamic_cast<AnimationBlender*>(tnode);
	if( tbnode != NULL && tbnode->getAnimationSpace() != NULL &&
		tbnode->getAnimationSpace()->hasParametrization() &&
		tbnode->getAnimationSpace()->getParametrization()->getNumParams() > 0 )
	{
		tparams = Vector( tbnode->getParams().size() );
	}

	// parse trans. target params
	if( wtok_i == trans_tok.end() )
		return false;
	std::string tparamstr = *wtok_i;
	if( tparamstr.size() > 0 && tparamstr[0] == '(' && tparamstr[ tparamstr.size()-1 ] == ')' )
	{
		tparamstr = tparamstr.substr( 1, tparamstr.size() - 2 );
		boost::char_separator<char> sep2( "," );
		boost::tokenizer< boost::char_separator<char> > tparam_tok( tparamstr, sep2 );
		boost::tokenizer< boost::char_separator<char> >::const_iterator wtok_j = tparam_tok.begin();
		
		unsigned int tparam_i = 0;
		for( boost::tokenizer< boost::char_separator<char> >::const_iterator wtok_j = tparam_tok.begin();
			wtok_j != tparam_tok.end() && tparam_i < tparams.size(); ++wtok_j, ++tparam_i )
		{
			tparams[tparam_i] = fromString<float>(*wtok_j);
		}
	}
	else
	{
		return false;
	}
	++wtok_i;

	// parse trans. target time
	if( wtok_i == trans_tok.end() )
		return false;
	tt = fromString<float>(*wtok_i);
	++wtok_i;

	trans = AnimationTransitionBlender::Transition( st, et, tt, tnode, tparams );

	return true;
}

void AnimationTransitionBlenderPropGrid::_writeTransitionStr( const AnimationTransitionBlender::Transition& trans,
															std::string& transStr ) const
{
	std::ostringstream oss;
	oss << trans.getStartTime();
	oss << ";";
	oss << trans.getEndTime();
	oss << ";";
	oss << trans.getTargetNode()->getName();
	oss << ";";
	oss << "(";
	for( unsigned int vi = 0; vi < trans.getTargetParams().size(); ++vi )
	{
		oss << trans.getTargetParams().get(vi);
		if( vi + 1 < trans.getTargetParams().size() )
			oss << ",";
	}
	oss << ")";
	oss << ";";
	oss << trans.getTargetTime();
	transStr = oss.str();
}

BEGIN_EVENT_TABLE( AnimationTransitionBlenderPropGrid, AnimationNodePropGrid )
	EVT_PG_CHANGED( wxID_ANY, OnPGChanged )
END_EVENT_TABLE()
