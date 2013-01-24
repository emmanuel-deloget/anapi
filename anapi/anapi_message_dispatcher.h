// *********************************************************************
//
// anapi++ -- a C++ interface for the Android NDK
//
//  Copyright (C) 2013 Emmanuel Deloget
//
//  This software is provided 'as-is', without any express or implied
//  warranty.  In no event will the authors be held liable for any
//  damages arising from the use of this software.
//
//  Permission is granted to anyone to use this software for any
//  purpose, including commercial applications, and to alter it and
//  redistribute it freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must
//     not claim that you wrote the original software. If you use this
//     software in a product, an acknowledgment in the product
//     documentation would be appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must
//     not be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source
//     distribution.
//
//  Emmanuel Deloget logout@free.fr
//
// *********************************************************************

#ifndef anapi_message_dispatcher_h_included
#define anapi_message_dispatcher_h_included

#include <vector>
#include <android/looper.h>
#include <android/input.h>
#include "anapi_app_window.h"
#include "anapi_sync_primitives.h"
#include "anapi_ticker.h"
#include "anapi_app.h"

namespace anapi
{

	class message_dispatcher
	{
	private:
		mutex m_mutex;
		int m_readfd;
		int m_writefd;
		ALooper *m_looper;
		AInputQueue *m_iqueue;
		ticker m_ticker;
		condition m_wndevent;
		bool m_has_focus;

	public:
		message_dispatcher();
		~message_dispatcher();

	private:
		message_dispatcher(const message_dispatcher&) = delete;
		message_dispatcher(message_dispatcher&&) = delete;
		message_dispatcher& operator=(const message_dispatcher&) = delete;
		message_dispatcher& operator=(message_dispatcher&&) = delete;

	private:
		void reset_queue_locked();
		void attach_queue(AInputQueue *q);
		bool fire_on_sysevent(app& the_app, const system_event& sysevent, bool& shall_quit);
		bool fire_on_winevent(app& the_app, const system_event& sysevent, const app_window& w);
		bool fire_on_key(app& the_app, AInputEvent *ie);
		bool fire_on_motion(app& the_app, AInputEvent *ie);
		bool fire_on_dpad(app& the_app, AInputEvent *ie);
		bool fire_on_external(app& the_app, AInputEvent *ie);

	public:
		void prepare();
		void setup_queue(AInputQueue *q);
		void reset_queue();
		void fire_sys_event(const system_event& se);
		void fire_rect_changed(const system_event& se, const ARect *r);
		void fire_idle(app& the_app);
		bool dispatch_message(app& the_app, const app_window& w, bool& shall_quit);
	};

}

#endif // anapi_message_dispatcher_h_included
