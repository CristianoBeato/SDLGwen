/*
===========================================================================
GWEN

Copyright (c) 2010 Facepunch Studios
Copyright (c) 2017-2018 Cristiano Beato

MIT License

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===========================================================================
*/

#include "precompiled.h"
#pragma hdrstop

#include "Gwen/Controls/WindowControl.h"
#include "Gwen/Controls/ImagePanel.h"
#include "Gwen/Controls/Label.h"
#include "Gwen/Controls/Modal.h"


using namespace Gwen;
using namespace Gwen::Controls;
using namespace Gwen::ControlsInternal;


GWEN_CONTROL_CONSTRUCTOR( WindowControl )
{
	m_Modal = NULL;
	m_bDeleteOnClose = false;
	m_TitleBar = new Dragger( this );
	m_TitleBar->SetHeight( 24 );
	m_TitleBar->SetPadding( Padding( 0, 0, 0, 0 ) );
	m_TitleBar->SetMargin( Margin( 0, 0, 0, 4 ) );
	m_TitleBar->SetTarget( this );
	m_TitleBar->Dock( Pos::Top );
	m_Title = new Label( m_TitleBar );
	m_Title->SetAlignment( Pos::Left | Pos::CenterV );
	m_Title->SetText( "Window" );
	m_Title->Dock( Pos::Fill );
	m_Title->SetPadding( Padding( 8, 0, 0, 0 ) );
	m_Title->SetTextColor( GetSkin()->Colors.Window.TitleInactive );
	m_CloseButton = new Gwen::Controls::WindowCloseButton( m_TitleBar );
	m_CloseButton->SetText( "" );
	m_CloseButton->Dock( Pos::Right );
	m_CloseButton->onPress.Add( this, &WindowControl::CloseButtonPressed );
	m_CloseButton->SetTabable( false );
	m_CloseButton->SetName( "closeButton" );
	m_CloseButton->SetWindow( this );
	//Create a blank content control, dock it to the top - Should this be a ScrollControl?
	m_InnerPanel = new Base( this );
	m_InnerPanel->Dock( Pos::Fill );
	GetResizer( 8 )->Hide();
	BringToFront();
	SetTabable( false );
	Focus();
	SetMinimumSize( Gwen::Point( 100, 40 ) );
	SetClampMovement( true );
	SetKeyboardInputEnabled( false );
}


WindowControl::~WindowControl()
{
	DestroyModal();
}

void WindowControl::MakeModal( bool bDrawBackground )
{
	if ( m_Modal ) { return; }

	m_Modal = new ControlsInternal::Modal( GetCanvas() );
	SetParent( m_Modal );
	m_Modal->SetShouldDrawBackground( bDrawBackground );
}

void WindowControl::DestroyModal()
{
	if ( !m_Modal ) { return; }

	// Really should be restoring our parent here.. but we don't know who it is.
	// Assume it's the canvas.
	SetParent( GetCanvas() );
	m_Modal->DelayedDelete();
	m_Modal = NULL;
}

bool WindowControl::IsOnTop()
{
	for ( Base::List::reverse_iterator iter = GetParent()->Children.rbegin(); iter != GetParent()->Children.rend(); ++iter )
	{
		WindowControl* pWindow = gwen_cast<WindowControl> ( *iter );

		if ( !pWindow )
		{ continue; }

		if ( pWindow == this )
		{ return true; }

		return false;
	}

	return false;
}

void WindowControl::Render( Skin::Base* skin )
{
	bool bHasFocus = IsOnTop();

	if ( bHasFocus )
	{ m_Title->SetTextColor( GetSkin()->Colors.Window.TitleActive ); }
	else
	{ m_Title->SetTextColor( GetSkin()->Colors.Window.TitleInactive ); }

	skin->DrawWindow( this, m_TitleBar->Bottom(), bHasFocus );
}

void WindowControl::RenderUnder( Skin::Base* skin )
{
	BaseClass::RenderUnder( skin );
	skin->DrawShadow( this );
}

void WindowControl::SetTitle( Gwen::UnicodeString title )
{
	m_Title->SetText( title );
}

void WindowControl::SetClosable( bool closeable )
{
	m_CloseButton->SetHidden( !closeable );
}

void WindowControl::SetHidden( bool hidden )
{
	if ( !hidden )
	{ BringToFront(); }

	BaseClass::SetHidden( hidden );
}

void WindowControl::Touch()
{
	BaseClass::Touch();
	BringToFront();
}

void WindowControl::CloseButtonPressed()
{
	DestroyModal();
	onWindowClosed.Call( this );
	SetHidden( true );

	if ( m_bDeleteOnClose )
	{ DelayedDelete(); }
}


void WindowControl::RenderFocus( Gwen::Skin::Base* /*skin*/ )
{
}