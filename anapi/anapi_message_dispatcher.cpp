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
#include <android/native_window.h>

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

	event_result message_dispatcher::dispatch_message(app& the_app)
	{
		if (true) {
			scoped_lock lock(m_mutex);
			if (!m_iqueue || !m_looper)
				return event_result::unhandled;
		}

		int fd, events, ident;
		void *data;

		ident = ALooper_pollOnce(m_ticker.paused() ? -1 : 0, &fd, &events, &data);

		if (ident == MSG_ID_SYSTEM) {
			system_event se;
			read(fd, &se, sizeof(se));
			if (se & system_event::synchronous) {
				return fire_on_syncevent(the_app, se);
			} else {
				return fire_on_asyncevent(the_app, se);
			}
		} else if (ident == MSG_ID_INPUT) {
			scoped_lock lock(m_mutex);
			AInputEvent *ie = NULL;
			if (AInputQueue_getEvent(m_iqueue, &ie) < 0)
				return event_result::unhandled;
			if (AInputQueue_preDispatchEvent(m_iqueue, ie))
				return event_result::handled;
			event_result result;
			if (is_key_message(ie)) {
				result = fire_on_key(the_app, ie);
			} else if (is_motion_message(ie)) {
				result = fire_on_motion(the_app, ie);
			} else if (is_dpad_message(ie)) {
				result = fire_on_dpad(the_app, ie);
			} else if (is_external_event(ie)) {
				result = fire_on_external(the_app, ie);
			}
			AInputQueue_finishEvent(m_iqueue, ie, result == event_result::handled ? 1 : 0);
			return result;
		}

		return event_result::unhandled;
	}

	void message_dispatcher::send_idle(app& the_app)
	{
		if (!m_ticker.paused() && m_has_focus) {
			m_ticker.update();
			the_app.on_idle(m_ticker);
		}
	}

	event_result message_dispatcher::fire_on_asyncevent(app& the_app, const system_event& se)
	{
		LOGI("%s:%d> firing %s\n", __FILE__, __LINE__, get_sys_event_name(se).c_str());

		switch (se) {
		case system_event::stop:
			return the_app.on_stop() ? event_result::handled : event_result::unhandled;
		case system_event::start: {
			return the_app.on_start() ? event_result::handled : event_result::unhandled;
			}
		case system_event::gained_focus:
			m_has_focus = true;
			return the_app.on_gained_focus() ? event_result::handled : event_result::unhandled;
		case system_event::lost_focus:
			m_has_focus = false;
			return the_app.on_lost_focus() ? event_result::handled : event_result::unhandled;
		case system_event::pause: {
				bool h = the_app.on_pause();
				m_ticker.pause();
				return h ? event_result::handled : event_result::unhandled;
			}
		case system_event::resume:
			m_ticker.resume();
			return the_app.on_resume() ? event_result::handled : event_result::unhandled;
		case system_event::quit: {
				the_app.on_quit();
				return event_result::shall_quit;
			}
		case system_event::rect_changed: {
				rectangle r;
				read(m_readfd, &r, sizeof(r));
				return the_app.on_rect_changed(r) ? event_result::handled : event_result::unhandled;
			}
		default:
			break;
		}
		return event_result::unhandled;
	}

	event_result message_dispatcher::fire_on_syncevent(app& the_app, const system_event& se)
	{
		ANativeWindow *wnd;

		read(m_readfd, &wnd, sizeof(wnd));

		LOGI("%s:%d> firing %s, wnd = %p\n", __FILE__, __LINE__, get_sys_event_name(se).c_str(), wnd);

		bool h = false;

		switch (se) {
		case system_event::window_created:
			h = the_app.on_window_created(app_window(wnd));
			break;
		case system_event::window_destroyed:
			h = the_app.on_window_destroyed(app_window(wnd));
			break;
		case system_event::window_exposed:
			h = the_app.on_window_exposed(app_window(wnd));
			break;
		case system_event::window_resized:
			h = the_app.on_window_resized(app_window(wnd));
			break;
		default:
			break;
		}
		m_syncevent.set();
		return h ? event_result::handled : event_result::unhandled;
	}

	event_result message_dispatcher::fire_on_key(app& the_app, AInputEvent *ie)
	{
		if (the_app.on_key(key_event(ie)))
			return event_result::handled;
		return event_result::unhandled;
	}

	event_result message_dispatcher::fire_on_motion(app& the_app, AInputEvent *ie)
	{
		if (the_app.on_motion(motion_event(ie)))
			return event_result::handled;
		return event_result::unhandled;
	}

	event_result message_dispatcher::fire_on_dpad(app& the_app, AInputEvent *ie)
	{
		int kcode = AKeyEvent_getKeyCode(ie);
		int type = AKeyEvent_getAction(ie);
		bool h = false;

		switch (kcode) {
		case AKEYCODE_DPAD_UP:
			h = the_app.on_dpad_up(type == AKEY_EVENT_ACTION_DOWN ? button_event::down : button_event::up);
			break;
		case AKEYCODE_DPAD_DOWN:
			h = the_app.on_dpad_down(type == AKEY_EVENT_ACTION_DOWN ? button_event::down : button_event::up);
			break;
		case AKEYCODE_DPAD_LEFT:
			h = the_app.on_dpad_left(type == AKEY_EVENT_ACTION_DOWN ? button_event::down : button_event::up);
			break;
		case AKEYCODE_DPAD_RIGHT:
			h = the_app.on_dpad_right(type == AKEY_EVENT_ACTION_DOWN ? button_event::down : button_event::up);
			break;
		case AKEYCODE_DPAD_CENTER:
			h = the_app.on_dpad_center(type == AKEY_EVENT_ACTION_DOWN ? button_event::down : button_event::up);
			break;
		default:
			break;
		}

		return h ? event_result::handled : event_result::unhandled;
	}

	event_result message_dispatcher::fire_on_external(app& the_app, AInputEvent *ie)
	{
		int kcode = AKeyEvent_getKeyCode(ie);

		if (kcode >= AKEYCODE_DPAD_UP && kcode <= AKEYCODE_DPAD_CENTER) {
			return fire_on_dpad(the_app, ie);
		} if (kcode == AKEYCODE_CALL) {
			if (the_app.on_call())
				return event_result::handled;
			return event_result::unhandled;
		} else if (kcode == AKEYCODE_ENDCALL) {
			if (the_app.on_endcall())
				return event_result::handled;
			return event_result::unhandled;
		} else if (kcode == AKEYCODE_VOLUME_UP || kcode == AKEYCODE_VOLUME_DOWN) {
			if (the_app.on_volume_change(kcode == AKEYCODE_VOLUME_UP ? 1 : -1))
				return event_result::handled;
			return event_result::unhandled;
		} else if (kcode == AKEYCODE_MENU) {
			int action = AKeyEvent_getAction(ie);
			button_event be = button_event::down;
			if (action != AKEY_EVENT_ACTION_DOWN)
				be = button_event::up;
			if (the_app.on_menu(be))
				return event_result::handled;
			return event_result::unhandled;
		}

		// I should handle some other "keys", but I'm a bit lazy to write the code...
		LOGE("%s:%d> unknown external function %d\n", __FILE__, __LINE__, kcode);
		return event_result::unhandled;
	}
}

