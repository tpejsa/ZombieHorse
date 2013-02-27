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

#ifndef __BuildAnimSpaceDialog_h__
#define __BuildAnimSpaceDialog_h__

#include "AnimationStudio.h"

class BuildAnimSpaceDialog : public wxDialog
{

public:

	BuildAnimSpaceDialog( wxWindow* parent, wxWindowID id );

	/**
	* Gets the animation space name.
	*/
	std::string getName() const { return mTxtName->GetValue().c_str(); }

	/**
	* Gets whether or not timewarp and alignment curves should be
	* built for blending.
	*/
	bool getBuildBlendCurves() const { return mChkBuildBlendCurves->IsChecked(); }

	/**
	* Gets whether or not annotation matches should be automatically computed
	* for the base animations.
	*/
	bool getMatchAnnots() const { return mChkMatchAnnots->IsChecked(); }

	/**
	* Gets the desired knot spacing for blend curves.
	*/
	unsigned int getKnotSpacing() const { return fromString<unsigned int>( mTxtKnotSpacing->GetValue().c_str() ); }

	DECLARE_EVENT_TABLE()

private:

	wxTextCtrl *mTxtName, *mTxtKnotSpacing;
	wxCheckBox *mChkBuildBlendCurves, *mChkMatchAnnots;

};

#endif // __BuildAnimSpaceDialog_h__
