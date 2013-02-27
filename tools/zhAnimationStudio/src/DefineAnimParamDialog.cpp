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

#include "DefineAnimParamDialog.h"
#include "AnimationStudioApp.h"
#include "AnimationStudioFrame.h"
#include "OgreWindow.h"

DefineAnimParamDialog::DefineAnimParamDialog( wxWindow* parent, wxWindowID id, Model* mdl, AnimationSpace* animSpace )
: wxDialog( parent, id, "Define Parametrization", wxDefaultPosition, wxSize( 450, 510 ), wxDEFAULT_DIALOG_STYLE ),
mMdl(mdl), mAnimSpace(animSpace)
{
	zhAssert( mdl != NULL && mdl->getSkeleton()->getNumBones() > 0 );
	zhAssert( animSpace != NULL && animSpace->getNumBaseAnimations() > 0 );

	new wxStaticBox( this, wxID_ANY, "Parametrization settings",
		wxPoint( 10, 10 ), wxSize( 420, 80 ) );

	new wxStaticText( this, ID_cbParamClass, "Parametrization type:",
		wxPoint( 20, 30 ), wxSize( 120, 20 ) );
	wxArrayString pc_choices;
	pc_choices.push_back( "DenseSampling" );
	mCbParamClass = new wxComboBox( this, wxID_ANY, "DenseSampling",
		wxPoint( 140, 30 ), wxSize( 120, 20 ), pc_choices, wxCB_READONLY );
	
	new wxStaticText( this, wxID_ANY, "Max. extrapolation:",
		wxPoint( 20, 60 ), wxSize( 120, 20 ) );
	mTxtMaxExtrap = new wxTextCtrl( this, wxID_ANY, "0.15",
		wxPoint( 140, 60 ), wxSize( 40, 20 ) );

	new wxStaticText( this, wxID_ANY, "Min. sample distance:",
		wxPoint( 200, 60 ), wxSize( 120, 20 ) );
	mTxtMinSampleDist = new wxTextCtrl( this, wxID_ANY, "0.01",
		wxPoint( 320, 60 ), wxSize( 40, 20 ) );

	new wxStaticBox( this, wxID_ANY, "Parameter specification",
		wxPoint( 10, 100 ), wxSize( 420, 330 ) );
	
	new wxStaticText( this, wxID_ANY, "Parameters:",
		wxPoint( 20, 120 ), wxSize( 120, 20 ) );
	mLbParams = new wxListBox( this, ID_lbParams,
		wxPoint( 20, 140 ), wxSize( 160, 240 ),
		0, 0, wxLB_SINGLE|wxLB_SORT );

	mBtnAdd = new wxButton( this, ID_btnAdd, "Add",
		wxPoint( 20, 390 ), wxSize( 65, 25 ) );
	mBtnRemove = new wxButton( this, ID_btnRemove, "Remove",
		wxPoint( 95, 390 ), wxSize( 65, 25 ) );

	new wxStaticText( this, wxID_ANY, "Parameter name:",
		wxPoint( 200, 140 ), wxSize( 90, 20 ) );
	mTxtName = new wxTextCtrl( this, ID_txtName, "",
		wxPoint( 290, 140 ), wxSize( 120, 20 ), wxTE_PROCESS_ENTER );

	new wxStaticText( this, wxID_ANY, "Base animation:",
		wxPoint( 200, 170 ), wxSize( 80, 20 ) );
	wxArrayString ba_choices;
	AnimationSpace::AnimationConstIterator bai = mAnimSpace->getBaseAnimationConstIterator();
	while( !bai.end() )
	{
		zh::Animation* banim = bai.next();
		ba_choices.push_back( banim->getName() );
	}
	mCbBaseAnim = new wxComboBox( this, ID_cbBaseAnim, ba_choices.size() > 0 ? ba_choices[0] : wxEmptyString,
		wxPoint( 290, 170 ), wxSize( 120, 20 ), ba_choices, wxCB_READONLY );

	new wxStaticText( this, wxID_ANY, "Time:",
		wxPoint( 200, 200 ), wxSize( 40, 20 ) );
	mTxtTime = new wxTextCtrl( this, ID_txtTime, "00.00",
		wxPoint( 250, 200 ), wxSize( 60, 20 ), wxTE_PROCESS_ENTER );
	new wxStaticText( this, wxID_ANY, "/",
		wxPoint( 310, 200 ), wxSize( 10, 20 ) );
	mStxLength = new wxStaticText( this, wxID_ANY, "00.00",
		wxPoint( 320, 200 ), wxSize( 60, 20 ) );
	new wxStaticText( this, wxID_ANY, "s",
		wxPoint( 380, 200 ), wxSize( 10, 20 ) );

	new wxStaticText( this, wxID_ANY, "Bone:",
		wxPoint( 200, 230 ), wxSize( 80, 20 ) );
	wxArrayString bone_choices;
	zh::Skeleton::BoneConstIterator bone_i = mMdl->getSkeleton()->getBoneConstIterator();
	while( !bone_i.end() )
	{
		zh::Bone* bone = bone_i.next();
		bone_choices.push_back( bone->getName() );
	}
	mCbBone = new wxComboBox( this, ID_cbBone, bone_choices.size() > 0 ? bone_choices[0] : wxEmptyString,
		wxPoint( 290, 230 ), wxSize( 120, 20 ), bone_choices, wxCB_READONLY );

	new wxStaticText( this, wxID_ANY, "Superior bone:",
		wxPoint( 200, 260 ), wxSize( 80, 20 ) );
	mCbSuperBone = new wxComboBox( this, ID_cbSuperBone, bone_choices.size() > 0 ? bone_choices[0] : wxEmptyString,
		wxPoint( 290, 260 ), wxSize( 120, 20 ), bone_choices, wxCB_READONLY );

	new wxStaticText( this, wxID_ANY, "Transf. type:",
		wxPoint( 200, 290 ), wxSize( 80, 20 ) );
	wxArrayString transf_choices;
	transf_choices.push_back( "Translation" );
	transf_choices.push_back( "Rotation" );
	mCbTransfType = new wxComboBox( this, ID_cbTransfType, transf_choices[0],
		wxPoint( 290, 290 ), wxSize( 120, 20 ), transf_choices, wxCB_READONLY );

	new wxStaticText( this, wxID_ANY, "Axis:",
		wxPoint( 200, 320 ), wxSize( 80, 20 ) );
	wxArrayString axis_choices;
	axis_choices.push_back("x");
	axis_choices.push_back("y");
	axis_choices.push_back("z");
	mCbAxis = new wxComboBox( this, ID_cbAxis, axis_choices[0],
		wxPoint( 290, 320 ), wxSize( 40, 20 ), axis_choices, wxCB_READONLY );

	// create OK button
	new wxButton( this, wxID_OK, _("&OK"),  wxPoint( 450/2 - 60, 440 ), wxSize( 75, 30 ) );

	// create Cancel button
	new wxButton( this, wxID_CANCEL, _("&Cancel"),  wxPoint( 450/2 + 40, 440 ), wxSize( 75, 30 ) );

	// disable animation updates
	gApp->setAnimationEnabled(false);

	_updateCtrlEnabledStatus();
}

DefineAnimParamDialog::~DefineAnimParamDialog()
{
	// enable animation updates
	gApp->setAnimationEnabled(true);
	
	// hide coord. axes
	gApp->getAppFrame()->getOgreWindow()->show3DAxesOnBone( "", false );
}

void DefineAnimParamDialog::OnComboBox_ParamClass( wxCommandEvent& evt )
{
}

void DefineAnimParamDialog::OnListBox_Params( wxCommandEvent& evt )
{
	_updateCtrlEnabledStatus();

	mCurParam = _findParamSpec( mLbParams->GetStringSelection().c_str() );
	AnimationParamSpec pspec = mParamSpecs[mCurParam];
	_updateParamSpecCtrls(pspec);

	// show coord. axes
	mMdl->getSkeleton()->resetToInitialPose();
	mParamSpecs[mCurParam].getBaseAnimation()->apply( mMdl, 0, 1 );
	zh::Bone* sbone = mMdl->getSkeleton()->getBone( mParamSpecs[mCurParam].getSuperBoneId() );
	gApp->getAppFrame()->getOgreWindow()->show3DAxesOnBone( sbone->getName() );

	_applyAnim();

	// TODO: render base sample point set
}

void DefineAnimParamDialog::OnButton_Add( wxCommandEvent& evt )
{
	std::string param_name = _generateParamName();

	AnimationParamSpec pspec = AnimationParamSpec( param_name,
		mAnimSpace->getBaseAnimation(0), 0,
		mMdl->getSkeleton()->getRoot()->getId(), mMdl->getSkeleton()->getRoot()->getId(),
		AnimationParamSpec::TransfType_Translation, AnimationParamSpec::Axis_x );
	mParamSpecs.push_back(pspec);

	mLbParams->Insert( param_name, mLbParams->GetStrings().size() );
	mLbParams->Select( mLbParams->GetStrings().size() - 1 );
	mCurParam = _findParamSpec( mLbParams->GetStringSelection().c_str() );

	_updateCtrlEnabledStatus();
	_updateParamSpecCtrls(pspec);
}

void DefineAnimParamDialog::OnButton_Remove( wxCommandEvent& evt )
{
	std::string param_name = mLbParams->GetStringSelection().c_str();
	if( param_name != "" )
	{
		// delete param. from param. list control
		mLbParams->Delete( mLbParams->GetSelection() );
		// delete param. spec.
		unsigned int param_i = _findParamSpec(param_name);
		mParamSpecs.erase( mParamSpecs.begin() + param_i );

		_updateCtrlEnabledStatus();
	}
}

void DefineAnimParamDialog::OnTextEnter_Name( wxCommandEvent& evt )
{
	std::string param_name = mTxtName->GetValue().c_str();
	if( _findParamSpec(param_name) < mParamSpecs.size() )
	{
		wxMessageBox( "Parameter by that name already specified.",
			"Duplicate Name", wxOK|wxICON_ERROR );
		mTxtName->SetValue( mParamSpecs[mCurParam].getParamName() );
		return;
	}

	mParamSpecs[mCurParam].setParamName(param_name);

	_updateParamList();
}

void DefineAnimParamDialog::OnComboBox_BaseAnim( wxCommandEvent& evt )
{
	std::string anim_name = mCbBaseAnim->GetStringSelection().c_str();
	zh::Animation* anim = NULL;
	for( unsigned int anim_i = 0; anim_i < mAnimSpace->getNumBaseAnimations(); ++anim_i )
	{
		anim = mAnimSpace->getBaseAnimation(anim_i);
		if( anim_name == anim->getName() )
			break;
	}

	mParamSpecs[mCurParam].setBaseAnimation(anim);
	mStxLength->SetLabel( toString<float>( anim->getLength() ) );

	float time = fromString<float>( mTxtTime->GetValue().c_str() );
	if( time > anim->getLength() )
		time = anim->getLength();

	mTxtTime->SetValue( toString<float>(time) );
	mParamSpecs[mCurParam].setTime(time);

	_applyAnim();

	// TODO: render base sample point set
}

void DefineAnimParamDialog::OnTextEnter_Time( wxCommandEvent& evt )
{
	float time = fromString<float>( mTxtTime->GetValue().c_str() );
	if( time < 0 )
		time = 0;
	else if( time > mParamSpecs[mCurParam].getBaseAnimation()->getLength() )
		time = mParamSpecs[mCurParam].getBaseAnimation()->getLength();
	
	mTxtTime->SetValue( toString<float>(time) );
	mParamSpecs[mCurParam].setTime(time);

	_applyAnim();

	// TODO: render base sample point set
}

void DefineAnimParamDialog::OnComboBox_Bone( wxCommandEvent& evt )
{
	zh::Bone* bone = mMdl->getSkeleton()->getBone( mCbBone->GetValue().c_str() );
	zh::Bone* sbone = mMdl->getSkeleton()->getBone( mCbSuperBone->GetValue().c_str() );
	zh::Bone* pbone = bone;
	bool super = false;
	
	// check if the superior bone is really the superior bone
	while(true)
	{
		if( pbone == sbone )
		{
			super = true;
			break;
		}

		if( pbone == mMdl->getSkeleton()->getRoot() )
			break;

		pbone = pbone->getParent();
	}

	if( !super )
	{
		mCbSuperBone->SetValue( mMdl->getSkeleton()->getRoot()->getName() );
		mParamSpecs[mCurParam].setSuperiorBoneId( mMdl->getSkeleton()->getRoot()->getId() );
	}

	mParamSpecs[mCurParam].setBoneId( bone->getId() );

	// TODO: render base sample point set
}

void DefineAnimParamDialog::OnComboBox_SuperBone( wxCommandEvent& evt )
{
	zh::Bone* bone = mMdl->getSkeleton()->getBone( mCbBone->GetValue().c_str() );
	zh::Bone* sbone = mMdl->getSkeleton()->getBone( mCbSuperBone->GetValue().c_str() );
	zh::Bone* pbone = bone;
	bool super = false;
	
	// check if the superior bone is really the superior bone
	while(true)
	{
		if( pbone == sbone )
		{
			super = true;
			break;
		}

		if( pbone == mMdl->getSkeleton()->getRoot() )
			break;

		pbone = pbone->getParent();
	}

	if( !super )
	{
		mCbBone->SetValue( sbone->getName() );
		mParamSpecs[mCurParam].setSuperiorBoneId( sbone->getId() );
	}

	mParamSpecs[mCurParam].setSuperiorBoneId( sbone->getId() );

	// show coord. axes
	mMdl->getSkeleton()->resetToInitialPose();
	mParamSpecs[mCurParam].getBaseAnimation()->apply( mMdl, 0, 1 );
	sbone = mMdl->getSkeleton()->getBone( mParamSpecs[mCurParam].getSuperBoneId() );
	gApp->getAppFrame()->getOgreWindow()->show3DAxesOnBone( sbone->getName() );

	_applyAnim();

	// TODO: render base sample point set
}

void DefineAnimParamDialog::OnComboBox_TransfType( wxCommandEvent& evt )
{
	std::string transf_str = mCbTransfType->GetValue().c_str();
	AnimationParamSpec::TransfType transf = AnimationParamSpec::TransfType_Translation;
	if( transf_str == "Translation" )
		transf = AnimationParamSpec::TransfType_Translation;
	else // if( transf_str == "Rotation" )
		transf = AnimationParamSpec::TransfType_Rotation;

	mParamSpecs[mCurParam].setTransfType(transf);

	// TODO: render base sample point set
}

void DefineAnimParamDialog::OnComboBox_Axis( wxCommandEvent& evt )
{
	std::string axis_str = mCbAxis->GetValue().c_str();
	AnimationParamSpec::Axis axis = AnimationParamSpec::Axis_x;
	if( axis_str == "x" )
		axis = AnimationParamSpec::Axis_x;
	else if( axis_str == "y" )
		axis = AnimationParamSpec::Axis_y;
	else // if( axis_str == "z" )
		axis = AnimationParamSpec::Axis_z;

	mParamSpecs[mCurParam].setAxis(axis);

	// TODO: render base sample point set
}

void DefineAnimParamDialog::_updateCtrlEnabledStatus()
{
	if( mLbParams->GetStringSelection() == "" )
	{
		mBtnRemove->Disable();

		mTxtName->Disable();
		mCbBaseAnim->Disable();
		mTxtTime->Disable();
		mStxLength->Disable();
		mCbBone->Disable();
		mCbSuperBone->Disable();
		mCbTransfType->Disable();
		mCbAxis->Disable();
	}
	else
	{
		mBtnRemove->Enable();

		mTxtName->Enable();
		mCbBaseAnim->Enable();
		mTxtTime->Enable();
		mStxLength->Enable();
		mCbBone->Enable();
		mCbSuperBone->Enable();
		mCbTransfType->Enable();
		mCbAxis->Enable();
	}
}

unsigned int DefineAnimParamDialog::_findParamSpec( const std::string& paramName ) const
{
	for( unsigned int param_i = 0; param_i < mParamSpecs.size(); ++param_i )
	{
		if( mParamSpecs[param_i].getParamName() == paramName )
			return param_i;
	}

	return UINT_MAX;
}

void DefineAnimParamDialog::_updateParamList()
{
	for( unsigned int param_i = 0; param_i < mParamSpecs.size(); ++param_i )
	{
		mLbParams->SetString( param_i, mParamSpecs[param_i].getParamName() );
	}
}

void DefineAnimParamDialog::_updateParamSpecCtrls( const AnimationParamSpec& paramSpec )
{
	std::string transf = paramSpec.getTransfType() == AnimationParamSpec::TransfType_Translation ? "Translation" : "Rotation";
	std::string axis = "x";
	if( paramSpec.getAxis() == AnimationParamSpec::Axis_x )
		axis = "x";
	else if( paramSpec.getAxis() == AnimationParamSpec::Axis_y )
		axis = "y";
	else // if( paramSpec.getAxis() == AnimationParamSpec::Axis_z )
		axis = "z";

	// update param spec. fields
	mTxtName->SetValue( paramSpec.getParamName() );
	mCbBaseAnim->SetValue( paramSpec.getBaseAnimation()->getName() );
	mTxtTime->SetValue( toString<float>( paramSpec.getTime() ) );
	mStxLength->SetLabel( toString<float>( paramSpec.getBaseAnimation()->getLength() ) );
	mCbBone->SetValue( mMdl->getSkeleton()->getBone( paramSpec.getBoneId() )->getName() );
	mCbSuperBone->SetValue( mMdl->getSkeleton()->getBone( paramSpec.getSuperBoneId() )->getName() );
	mCbTransfType->SetValue(transf);
	mCbAxis->SetValue(axis);
}

std::string DefineAnimParamDialog::_generateParamName() const
{
	std::string param_name = "";
	unsigned int param_i = 0;
	while( _findParamSpec( "NewParam" + toString<unsigned int>(param_i) ) < mParamSpecs.size() )
		++param_i;
	param_name = "NewParam" + toString<unsigned int>(param_i);

	return param_name;
}

void DefineAnimParamDialog::_applyAnim()
{
	zh::Animation* anim = mParamSpecs[mCurParam].getBaseAnimation();
	float time = fromString<float>( mTxtTime->GetValue().c_str() );

	// render character at specified time
	gApp->applyAnimation( anim->getAnimationSet()->getName(), anim->getName(), time );

	// show coord. axes
	mMdl->getSkeleton()->resetToInitialPose();
	zh::Bone* sbone = mMdl->getSkeleton()->getBone( mParamSpecs[mCurParam].getSuperBoneId() );
	gApp->getAppFrame()->getOgreWindow()->show3DAxesOnBone( sbone->getName() );
}

BEGIN_EVENT_TABLE( DefineAnimParamDialog, wxDialog )
	EVT_COMBOBOX( ID_cbParamClass, DefineAnimParamDialog::OnComboBox_ParamClass )
	EVT_LISTBOX( ID_lbParams, DefineAnimParamDialog::OnListBox_Params )
	EVT_BUTTON( ID_btnAdd, DefineAnimParamDialog::OnButton_Add )
	EVT_BUTTON( ID_btnRemove, DefineAnimParamDialog::OnButton_Remove )
	EVT_TEXT_ENTER(	ID_txtName, DefineAnimParamDialog::OnTextEnter_Name )
	EVT_COMBOBOX( ID_cbBaseAnim, DefineAnimParamDialog::OnComboBox_BaseAnim )
	EVT_TEXT_ENTER( ID_txtTime, DefineAnimParamDialog::OnTextEnter_Time )
	EVT_COMBOBOX( ID_cbBone, DefineAnimParamDialog::OnComboBox_Bone )
	EVT_COMBOBOX( ID_cbSuperBone, DefineAnimParamDialog::OnComboBox_SuperBone )
	EVT_COMBOBOX( ID_cbTransfType, DefineAnimParamDialog::OnComboBox_TransfType )
	EVT_COMBOBOX( ID_cbAxis, DefineAnimParamDialog::OnComboBox_Axis )
END_EVENT_TABLE()
