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

#ifndef __BuildIndexDialog_h__
#define __BuildIndexDialog_h__

#include "AnimationStudio.h"

class BuildIndexDialog : public wxDialog
{

public:

	BuildIndexDialog( wxWindow* parent, wxWindowID id );

	/**
	* Gets the animation index name.
	*/
	std::string getName() const { return mTxtName->GetValue().c_str(); }

	/**
	* Gets the animation label filter.
	*/
	std::string getLabelFilter() const { return mTxtLabelFilter->GetValue().c_str(); }

	/**
	* Gets the factor by which animation sample rate should be reduced
	* while building the match web.
	*/
	unsigned int getResampleFactor() const { return fromString<unsigned int>( mTxtResampleFact->GetValue().c_str() ); }

	/**
	* Gets the length of the frame window
	* used in animation comparison.
	*/
	float getWindowLength() const { return fromString<float>( mTxtWndLength->GetValue().c_str() ); }

	/**
	* Gets the distance value threshold for local minima, normalized.
	*/
	float getMinDistance() const { return fromString<float>( mTxtMinDist->GetValue().c_str() ); }

	/**
	* Gets the maximum difference in animation distance allowed
	* for two pairs of frames in a single minima chain.
	*/
	float getMaxDistanceDiff() const { return fromString<float>( mTxtMaxDistDiff->GetValue().c_str() ); }

	/**
	* Gets the minimum length of a minima chain.
	* Shorter chains are discarded.
	*/
	float getMinChainLength() const { return fromString<float>( mTxtMinChainLength->GetValue().c_str() ); }

	/**
	* Gets the maximum length of a bridge between a pair
	* of minima chains.
	*/
	float getMaxBridgeLength() const { return fromString<float>( mTxtMaxBridgeLength->GetValue().c_str() ); }

	DECLARE_EVENT_TABLE()

private:

	wxTextCtrl *mTxtName, *mTxtLabelFilter,
		*mTxtResampleFact, *mTxtWndLength,
		*mTxtMinDist, *mTxtMaxDistDiff,
		*mTxtMinChainLength, *mTxtMaxBridgeLength;

};

#endif // __BuildIndexDialog_h__
