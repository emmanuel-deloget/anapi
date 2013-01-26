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

#ifndef anapi_app_h_included
#define anapi_app_h_included

#include <stdexcept>
#include "anapi_sync_primitives.h"
#include "anapi_app_activity.h"
#include "anapi_app_window.h"
#include "anapi_ticker.h"
#include "anapi_events.h"

namespace anapi
{

	class app
	{
	private:
		static mutex m_app_mutex;
		static app* m_app;

	public:
		app()
		{
			scoped_lock lock(m_app_mutex);
			if (m_app)
				throw std::runtime_error("an android::app instance already exists");
			m_app = this;
		}
		virtual ~app()
		{
			scoped_lock lock(m_app_mutex);
			m_app = NULL;
		}

		static app& get_instance()
		{
			scoped_lock lock(m_app_mutex);
			if (!m_app)
				throw std::runtime_error("no android::app exists");
			return *m_app;
		}

		// This function is called when no event is to
		// be processed.
		virtual void on_idle(const ticker& ticker) { }

		// --------------- Events callbacks -------------------------
		// Most of the time, these functions shall return true
		// when a body is provided. There are some exceptions,
		// most notably the on_key_down/on_key_up functions might
		// return false when kcode == AKEYCODE_BACK

		// System events
		virtual bool on_pause() { return false; }
		virtual bool on_resume() { return false; }
		virtual bool on_start(const app_activity& activity) { return false; }
		virtual bool on_stop() { return false; }
		virtual bool on_quit() { return false; }
		virtual bool on_gained_focus() { return false; }
		virtual bool on_lost_focus() { return false; }
		virtual bool on_rect_changed(const rectangle& r) { return false; }

		// External events
		virtual bool on_menu(const button_event& be) { return false; }
		virtual bool on_volume_change(int diff) { return false; }
		virtual bool on_call() { return false; }
		virtual bool on_endcall() { return false; }

		// Window events
		virtual bool on_window_created(const app_window& w) { return false; }
		virtual bool on_window_destroyed(const app_window& w) { return false; }
		virtual bool on_window_exposed(const app_window& w) { return false; }
		virtual bool on_window_resized(const app_window& w) { return false; }

		// Motion events
		virtual bool on_motion(const motion_event& mevent) { return false; }

		// Keyboard events
		virtual bool on_key(const key_event& ke) { return false; }

		// Dpad events
		virtual bool on_dpad_up(const button_event& be) { return false; }
		virtual bool on_dpad_down(const button_event& be) { return false; }
		virtual bool on_dpad_left(const button_event& be) { return false; }
		virtual bool on_dpad_right(const button_event& be) { return false; }
		virtual bool on_dpad_center(const button_event& be) { return false; }

	private:
		app(const app&) = delete;
		app(app&&) = delete;
		app& operator=(const app&) = delete;
		app& operator=(app&&) = delete;
	};

}

#endif // anapi_app_h_included
