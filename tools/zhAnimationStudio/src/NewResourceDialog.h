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

#ifndef __NewResourceDialog_h__
#define __NewResourceDialog_h__

#include "AnimationStudio.h"

class NewResourceDialog : public wxDialog
{

public:

	enum ResourceType
	{
		ResourceType_AnimSet,
		ResourceType_AnimIndex,
		ResourceType_AnimTree
	};

	NewResourceDialog( wxWindow* parent, wxWindowID id, ResourceType resType );

	/**
	* Gets the resource type.
	*/
	ResourceType getResourceType() const { return mResType; }

	/**
	* Gets the resource name.
	*/
	std::string getName() const { return mTxtName->GetValue().c_str(); }

	/**
	* Gets the resource directory.
	*/
	std::string getDirectory() const { return mLbDir->GetStringSelection().c_str(); }

	DECLARE_EVENT_TABLE()

private:

	ResourceType mResType;
	wxTextCtrl* mTxtName;
	wxListBox* mLbDir;

};

#endif // __BuildIndexDialog_h__
