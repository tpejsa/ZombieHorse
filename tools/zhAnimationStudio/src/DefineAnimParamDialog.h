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

#ifndef __DefineAnimParamDialog_h__
#define __DefineAnimParamDialog_h__

#include "AnimationStudio.h"

enum
{
	ID_cbParamClass,
	ID_lbParams,
	ID_btnAdd,
	ID_btnRemove,
	ID_txtName,
	ID_cbBaseAnim,
	ID_txtTime,
	ID_cbBone,
	ID_cbSuperBone,
	ID_cbTransfType,
	ID_cbAxis
};

class DefineAnimParamDialog : public wxDialog
{

public:

	DefineAnimParamDialog( wxWindow* parent, wxWindowID id, zh::Skeleton* skel, AnimationSpace* animSpace );

	~DefineAnimParamDialog();

	/**
	* Gets the parametrization class.
	*/
	AnimationParamClass getParamClass() const { return AnimationParam_DenseSampling; }

	/**
	* Gets the maximum extrapolation for dense sampling parametrization.
	*/
	float getMaxExtrapolation() const { return fromString<float>( mTxtMaxExtrap->GetValue().c_str() ); }

	/**
	* Gets the minimum distance between dense samples.
	*/
	float getMinSampleDistance() const { return fromString<float>( mTxtMinSampleDist->GetValue().c_str() ); }

	/**
	* Gets the parameter specifications.
	*/
	const std::vector<AnimationParamSpec>& getParamSpecs() const { return mParamSpecs; }

	void OnComboBox_ParamClass( wxCommandEvent& evt );
	void OnListBox_Params( wxCommandEvent& evt );
	void OnButton_Add( wxCommandEvent& evt );
	void OnButton_Remove( wxCommandEvent& evt );
	void OnTextEnter_Name( wxCommandEvent& evt );
	void OnComboBox_BaseAnim( wxCommandEvent& evt );
	void OnTextEnter_Time( wxCommandEvent& evt );
	void OnComboBox_Bone( wxCommandEvent& evt );
	void OnComboBox_SuperBone( wxCommandEvent& evt );
	void OnComboBox_TransfType( wxCommandEvent& evt );
	void OnComboBox_Axis( wxCommandEvent& evt );

	DECLARE_EVENT_TABLE()

private:

	void _updateCtrlEnabledStatus();
	unsigned int _findParamSpec( const std::string& paramName ) const;
	void _updateParamList();
	void _updateParamSpecCtrls( const AnimationParamSpec& paramSpec );
	std::string _generateParamName() const;
	void _applyAnim();

	zh::Skeleton* mSkel;
	AnimationSpace* mAnimSpace;
	std::vector<AnimationParamSpec> mParamSpecs;
	unsigned int mCurParam;

	wxComboBox* mCbParamClass;
	wxTextCtrl* mTxtMaxExtrap, *mTxtMinSampleDist;
	wxListBox* mLbParams;
	wxButton* mBtnAdd;
	wxButton* mBtnRemove;
	wxTextCtrl* mTxtName;
	wxComboBox* mCbBaseAnim;
	wxTextCtrl* mTxtTime;
	wxStaticText* mStxLength;
	wxComboBox* mCbBone, *mCbSuperBone, *mCbTransfType, *mCbAxis;

};

#endif // __DefineAnimParamDialog_h__
