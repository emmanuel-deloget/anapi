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

#include <stdexcept>
#include <unistd.h>
#include <android/looper.h>

#include "anapi_message_dispatcher.h"
#include "anapi_events.h"
#include "anapi_log.h"
#include "anapi_app.h"

namespace anapi
{
	const int MSG_ID_SYSTEM = 0x16;
	const int MSG_ID_INPUT = 0x08;

	namespace
	{
		const int AINPUT_SOURCE_EXTERNAL = 0x300 | AINPUT_SOURCE_CLASS_BUTTON;

		inline bool is_key_message(AInputEvent *ie)
		{
			int source = AInputEvent_getSource(ie);
			return source == AINPUT_SOURCE_KEYBOARD;
		}

		inline bool is_motion_message(AInputEvent *ie)
		{
			int source = AInputEvent_getSource(ie);
			return source == AINPUT_SOURCE_TOUCHSCREEN;
		}

		inline bool is_dpad_message(AInputEvent *ie)
		{
			int source = AInputEvent_getSource(ie);
			return source == AINPUT_SOURCE_DPAD;
		}

		inline bool is_external_event(AInputEvent *ie)
		{
			int source = AInputEvent_getSource(ie);
			return source == AINPUT_SOURCE_EXTERNAL;
		}
	}

	message_dispatcher::message_dispatcher()
	: m_looper(NULL)
	, m_iqueue(NULL)
	, m_ticker()
	, m_has_focus(false)
	{
		int fds[2];

		if (pipe(fds) < 0)
			throw std::runtime_error("cannot create the internal dispatcher pipe");
		m_readfd = fds[0];
		m_writefd = fds[1];
	}

	message_dispatcher::~message_dispatcher()
	{
		scoped_lock lock(m_mutex);
		reset_queue();
		if (m_looper) {
			ALooper_removeFd(m_looper, m_readfd);
			ALooper_release(m_looper);
		}
		close(m_writefd);
		close(m_readfd);
		LOGV("%s:%d> message_dispatcher(%p) is destroyed\n", __FILE__, __LINE__, this);
	}

	void message_dispatcher::prepare()
	{
		scoped_lock lock(m_mutex);
		m_looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
		if (!m_looper)
			throw std::runtime_error("cannot create the internal looper");
		ALooper_addFd(m_looper, m_readfd, MSG_ID_SYSTEM, ALOOPER_EVENT_INPUT, NULL, NULL);
		if (m_iqueue) {
			attach_queue(m_iqueue);
		}
	}

	void message_dispatcher::attach_queue(AInputQueue *q)
	{
		m_iqueue = q;
		if (m_looper) {
			LOGI("%s:%d> attaching looper %p to queue %p\n", __FILE__, __LINE__, m_looper, m_iqueue);
			AInputQueue_attachLooper(m_iqueue, m_looper, MSG_ID_INPUT, NULL, NULL);
		}
	}

	void message_dispatcher::setup_queue(AInputQueue *q)
	{
		scoped_lock lock(m_mutex);
		reset_queue_locked();

		if (q)
			attach_queue(q);
	}

	void message_dispatcher::reset_queue_locked()
	{
		if (m_iqueue) {
			AInputQueue_detachLooper(m_iqueue);
			LOGI("%s:%d> detaching looper from queue %p\n", __FILE__, __LINE__, m_iqueue);
		}
	}

	void message_dispatcher::reset_queue()
	{
		scoped_lock lock(m_mutex);
		reset_queue_locked();
	}

	bool message_dispatcher::dispatch_message(app& the_app, const app_window& w, bool& shall_quit)
	{
		shall_quit = false;

		if (true) {
			scoped_lock lock(m_mutex);
			if (!m_iqueue || !m_looper)
				return false;
		}

		int fd, events, ident;
		void *data;

		ident = ALooper_pollOnce(m_ticker.paused() ? -1 : 0, &fd, &events, &data);

		if (ident == MSG_ID_SYSTEM) {
			system_event se;
			read(fd, &se, sizeof(se));
			if (se & system_event::system) {
				return fire_on_sysevent(the_app, se, shall_quit);
			} else if (se & system_event::window) {
				return fire_on_winevent(the_app, se, w);
			}
		} else if (ident == MSG_ID_INPUT) {
			scoped_lock lock(m_mutex);
			AInputEvent *ie = NULL;
			if (AInputQueue_getEvent(m_iqueue, &ie) < 0)
				return false;
			if (AInputQueue_preDispatchEvent(m_iqueue, ie))
				return true;
			bool handled;
			if (is_key_message(ie)) {
				handled = fire_on_key(the_app, ie);
			} else if (is_motion_message(ie)) {
				handled = fire_on_motion(the_app, ie);
			} else if (is_dpad_message(ie)) {
				handled = fire_on_dpad(the_app, ie);
			} else if (is_external_event(ie)) {
				handled = fire_on_external(the_app, ie);
			}
			AInputQueue_finishEvent(m_iqueue, ie, handled ? 1 : 0);
			return handled;
		}

		return false;
	}

	void message_dispatcher::fire_sys_event(const system_event& se)
	{
		write(m_writefd, &se, sizeof(se));
		if (se & system_event::window) {
			// we must wait for the event to be handled
			m_wndevent.wait_set_and_reset();
		}
	}

	void message_dispatcher::fire_rect_changed(const system_event& se, const ARect *r)
	{
		write(m_writefd, &se, sizeof(se));
		write(m_writefd, r, sizeof(ARect));
	}

	void message_dispatcher::fire_idle(app& the_app)
	{
		if (!m_ticker.paused() && m_has_focus) {
			m_ticker.update();
			the_app.on_idle(m_ticker);
		}
	}

	bool message_dispatcher::fire_on_sysevent(app& the_app, const system_event& se, bool& shall_quit)
	{
		LOGI("%s:%d> firing system event %s\n", __FILE__, __LINE__,
				get_sys_event_name(se).c_str());

		switch (se) {
		case system_event::stop:
			return the_app.on_stop();
		case system_event::start:
			return the_app.on_start();
		case system_event::gained_focus:
			m_has_focus = true;
			return the_app.on_gained_focus();
		case system_event::lost_focus:
			m_has_focus = false;
			return the_app.on_lost_focus();
		case system_event::pause: {
				bool h = the_app.on_pause();
				m_ticker.pause();
				return h;
			}
		case system_event::resume:
			m_ticker.resume();
			return the_app.on_resume();
		case system_event::quit: {
				bool handled = the_app.on_quit();
				shall_quit = true;
				return handled;
			}
		case system_event::rect_changed: {
				rectangle r;
				read(m_readfd, &r, sizeof(r));
				return the_app.on_rect_changed(r);
			}
		default:
			break;
		}
		return false;
	}

	bool message_dispatcher::fire_on_winevent(app& the_app, const system_event& se, const app_window& w)
	{
		LOGI("%s:%d> firing window event %s\n", __FILE__, __LINE__,
				get_sys_event_name(se).c_str());

		bool h = false;

		switch (se) {
		case system_event::window_created:
			h = the_app.on_window_created(w);
			break;
		case system_event::window_destroyed:
			h = the_app.on_window_destroyed(w);
			break;
		case system_event::window_exposed:
			h = the_app.on_window_exposed(w);
			break;
		case system_event::window_resized:
			h = the_app.on_window_resized(w);
			break;
		default:
			break;
		}
		m_wndevent.set();
		return h;
	}

	bool message_dispatcher::fire_on_key(app& the_app, AInputEvent *ie)
	{
		return the_app.on_key(key_event(ie));
	}

	bool message_dispatcher::fire_on_motion(app& the_app, AInputEvent *ie)
	{
		return the_app.on_motion(motion_event(ie));
	}

	bool message_dispatcher::fire_on_dpad(app& the_app, AInputEvent *ie)
	{
		int kcode = AKeyEvent_getKeyCode(ie);
		int type = AKeyEvent_getAction(ie);

		switch (kcode) {
		case AKEYCODE_DPAD_UP:
			return the_app.on_dpad_up(type == AKEY_EVENT_ACTION_DOWN ? button_event::down : button_event::up);
		case AKEYCODE_DPAD_DOWN:
			return the_app.on_dpad_down(type == AKEY_EVENT_ACTION_DOWN ? button_event::down : button_event::up);
		case AKEYCODE_DPAD_LEFT:
			return the_app.on_dpad_left(type == AKEY_EVENT_ACTION_DOWN ? button_event::down : button_event::up);
		case AKEYCODE_DPAD_RIGHT:
			return the_app.on_dpad_right(type == AKEY_EVENT_ACTION_DOWN ? button_event::down : button_event::up);
		case AKEYCODE_DPAD_CENTER:
			return the_app.on_dpad_center(type == AKEY_EVENT_ACTION_DOWN ? button_event::down : button_event::up);
		default:
			break;
		}

		LOGE("%s:%d> unknown dpad keycode %d\n", __FILE__, __LINE__, kcode);
		return false;
	}

	bool message_dispatcher::fire_on_external(app& the_app, AInputEvent *ie)
	{
		int kcode = AKeyEvent_getKeyCode(ie);

		if (kcode >= AKEYCODE_DPAD_UP && kcode <= AKEYCODE_DPAD_CENTER) {
			return fire_on_dpad(the_app, ie);
		} if (kcode == AKEYCODE_CALL) {
			return the_app.on_call();
		} else if (kcode == AKEYCODE_ENDCALL) {
			return the_app.on_endcall();
		} else if (kcode == AKEYCODE_VOLUME_UP || kcode == AKEYCODE_VOLUME_DOWN) {
			return the_app.on_volume_change(kcode == AKEYCODE_VOLUME_UP ? 1 : -1);
		} else if (kcode == AKEYCODE_MENU) {
			int action = AKeyEvent_getAction(ie);
			button_event be = button_event::down;
			if (action != AKEY_EVENT_ACTION_DOWN)
				be = button_event::up;
			return the_app.on_menu(be);
		}

		// I should handle some other "keys", but I'm a bit lazy to write the code...
		LOGE("%s:%d> unknown external function %d\n", __FILE__, __LINE__, kcode);
		return false;
	}
}

