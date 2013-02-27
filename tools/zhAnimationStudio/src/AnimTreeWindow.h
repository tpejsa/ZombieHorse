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

#ifndef __AnimTreeWindow_h__
#define __AnimTreeWindow_h__

#include "AnimationStudio.h"

#include "connex.h"
#include "connexspace.h"
#include "connexnode.h"
#include "AnimTreeConnexSpace.h"

#include "wx/spinbutt.h"
#include "wx/app.h"
#include "wx/frame.h"
#include <vector>

enum
{
	ID_connexWindow,
	ID_wndAnimTreeConnexSpaceScroll
};

class wxConnexSpace;
class AnimTreeConnexSpace;

/**
* Animation annotations management window.
*/
class AnimTreeWindow : public wxWindow
{

protected:

	AnimTreeConnexSpace* connex_space;

public:
	
	wxScrolledWindow* mWndAnimTreeConnexSpaceScroll;
	
	/**
	* Constructor.
	*/
	AnimTreeWindow( wxWindow *parent, wxWindowID id );

	/**
	* Initializes the animation tree editor
	* with the currently selected anim. tree.
	*/
	void initEditor();

	/**
	* Deinitializes the animation tree editor.
	*/
	void deinitEditor();

	/**
	* true if editor is initialized with the current anim. tree,
	* otherwise false.
	*/
	bool isEditorActive() const;

	bool Show( bool show = true );

	void OnSize( wxSizeEvent& evt );

	void OnMouseEvent( wxMouseEvent& evt );

	DECLARE_EVENT_TABLE()

private:

	bool mActive;

};

#endif // __AnimTreeWindow_h__
