/* Copyright (C) 2009 Mobile Sorcery AB

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License, version 2, as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.
*/

#ifndef _MAUI_SCREEN_H_
#define _MAUI_SCREEN_H_

#include <MAUI/Widget.h>
#include <MAUtil/Environment.h>

namespace MAUI {

	using namespace MAUtil;

	/** A Screen is a logically distinct part of a MAUI interface.
	  * MAUI applications typically consist of a number of screens
	  * that the user can navigate between.
	  **/

	class Screen : public KeyListener, PointerListener {
	public:
		Screen();
		/** Causes this screen to be shown and all other screens in
		  * the application to be hidden.
		  **/
		virtual void show();
		/** Sets the main widget for this screen. The widget will
		  * be resized to match the screen resolution of the device **/
		void setMain(Widget* main);
		/** Returns the main widget of this screen **/
		Widget* getMain();
		/** keylistener implementation to overload when inheriting 
		  * your own screens.
		  **/
		virtual void keyPressEvent(int keyCode) {}
		/** keylistener implementation to overload when inheriting 
		  * your own screens.
		  **/
		virtual void keyReleaseEvent(int keyCode) {}
		/** pointerlistener implementation to overload when inheriting 
		  * your own screens.
		  **/
		virtual void pointerPressEvent(MAPoint2d point) {}
		/** pointerlistener implementation to overload when inheriting 
		  * your own screens.
		  **/
		virtual void pointerReleaseEvent(MAPoint2d point) {}
		/** pointerlistener implementation to overload when inheriting 
		  * your own screens.
		  **/
		virtual void pointerMoveEvent(MAPoint2d point) {}

		/**
		* Does not delete the main Widget.
		*/
		~Screen();
	protected:
		virtual void hide();
		static Screen* currentScreen;
		Widget* main;
	};

}

#endif	//_MAUI_SCREEN_H_