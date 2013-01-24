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

#include <sys/time.h>
#include "anapi_native_activity.h"
#include "anapi_app_window.h"
#include "anapi_events.h"
#include "anapi_log.h"

namespace anapi
{

	struct callback_set
	{
		static void on_configuration_changed(ANativeActivity* activity);
		static void on_content_rect_changed(ANativeActivity* activity, const ARect *r);
		static void *on_save_instance_state(ANativeActivity* activity, size_t *size);
		static void on_start(ANativeActivity* activity);
		static void on_stop(ANativeActivity* activity);
		static void on_resume(ANativeActivity* activity);
		static void on_pause(ANativeActivity* activity);
		static void on_destroy(ANativeActivity* activity);
		static void on_window_focus_changed(ANativeActivity* activity, int has_focus);
		static void on_window_created(ANativeActivity* activity, ANativeWindow* window);
		static void on_window_resized(ANativeActivity* activity, ANativeWindow* window);
		static void on_window_redraw_needed(ANativeActivity* activity, ANativeWindow* window);
		static void on_window_destroyed(ANativeActivity* activity, ANativeWindow* window);
		static void on_input_queue_created(ANativeActivity *activity, AInputQueue *q);
		static void on_input_queue_destroyed(ANativeActivity *activity, AInputQueue *q);
	};

	void callback_set::on_configuration_changed(ANativeActivity* activity)
	{
		native_activity *a = reinterpret_cast<native_activity*>(activity->instance);
		if (a) {
			a->on_configuration_changed();
		}
	}

	void callback_set::on_content_rect_changed(ANativeActivity* activity, const ARect *r)
	{
		native_activity *a = reinterpret_cast<native_activity*>(activity->instance);
		if (a) {
			a->on_content_rect_changed(r);
		}
	}

	void *callback_set::on_save_instance_state(ANativeActivity* activity, size_t *size)
	{
		native_activity *a = reinterpret_cast<native_activity*>(activity->instance);
		if (a) {
			return a->on_save_instance_state(size);
		}
		return NULL;
	}

	void callback_set::on_start(ANativeActivity* activity)
	{
		native_activity *a = reinterpret_cast<native_activity*>(activity->instance);
		if (a) {
			a->on_start();
		}
	}

	void callback_set::on_stop(ANativeActivity* activity)
	{
		native_activity *a = reinterpret_cast<native_activity*>(activity->instance);
		if (a) {
			a->on_stop();
		}
	}

	void callback_set::on_resume(ANativeActivity* activity)
	{
		native_activity *a = reinterpret_cast<native_activity*>(activity->instance);
		if (a) {
			a->on_resume();
		}
	}

	void callback_set::on_pause(ANativeActivity* activity)
	{
		native_activity *a = reinterpret_cast<native_activity*>(activity->instance);
		if (a) {
			a->on_pause();
		}
	}

	void callback_set::on_destroy(ANativeActivity* activity)
	{
		native_activity *a = reinterpret_cast<native_activity*>(activity->instance);
		if (a) {
			a->on_destroy();
		}
	}

	void callback_set::on_window_focus_changed(ANativeActivity* activity, int has_focus)
	{
		native_activity *a = reinterpret_cast<native_activity*>(activity->instance);
		if (a) {
			a->on_window_focus_changed(has_focus);
		}
	}

	void callback_set::on_window_created(ANativeActivity* activity, ANativeWindow* window)
	{
		native_activity *a = reinterpret_cast<native_activity*>(activity->instance);
		if (a) {
			a->on_window_created(window);
		}
	}

	void callback_set::on_window_resized(ANativeActivity* activity, ANativeWindow* window)
	{
		native_activity *a = reinterpret_cast<native_activity*>(activity->instance);
		if (a) {
			a->on_window_resized(window);
		}
	}

	void callback_set::on_window_redraw_needed(ANativeActivity* activity, ANativeWindow* window)
	{
		native_activity *a = reinterpret_cast<native_activity*>(activity->instance);
		if (a) {
			a->on_window_redraw_needed(window);
		}
	}

	void callback_set::on_window_destroyed(ANativeActivity* activity, ANativeWindow* window)
	{
		native_activity *a = reinterpret_cast<native_activity*>(activity->instance);
		if (a) {
			a->on_window_destroyed(window);
		}
	}

	void callback_set::on_input_queue_created(ANativeActivity *activity, AInputQueue *q)
	{
		native_activity *a = reinterpret_cast<native_activity*>(activity->instance);
		if (a) {
			a->on_input_queue_created(q);
		}
	}

	void callback_set::on_input_queue_destroyed(ANativeActivity *activity, AInputQueue *q)
	{
		native_activity *a = reinterpret_cast<native_activity*>(activity->instance);
		if (a) {
			a->on_input_queue_destroyed(q);
		}
	}

	native_activity::native_activity(ANativeActivity *a, const instance_state& istate)
	: m_activity(a)
	, m_istate(istate)
	, m_dispatcher()
	, m_window(NULL)
	{
		LOGV("%s:%d> native_activity(%p) is created\n", __FILE__, __LINE__, this);
		setup_callbacks();
	}

	native_activity::~native_activity()
	{
		m_activity->instance = NULL;
		reset_callbacks();
		LOGV("%s:%d> native_activity(%p) is destroyed\n", __FILE__, __LINE__, this);
	}

	void native_activity::started()
	{
		m_started.set();
	}

	void native_activity::wait_started()
	{
		m_started.wait_set();
	}

	bool native_activity::dispatch_message(app& the_app, bool& shall_quit)
	{
		return m_dispatcher.dispatch_message(the_app, app_window(m_window), shall_quit);
	}

	void native_activity::prepare_msg_dispatcher()
	{
		m_dispatcher.prepare();
	}

	void native_activity::setup_callbacks()
	{
		m_activity->callbacks->onStart = callback_set::on_start;
		m_activity->callbacks->onStop = callback_set::on_stop;
		m_activity->callbacks->onPause = callback_set::on_pause;
		m_activity->callbacks->onResume = callback_set::on_resume;
		m_activity->callbacks->onDestroy = callback_set::on_destroy;
		m_activity->callbacks->onWindowFocusChanged = callback_set::on_window_focus_changed;
		m_activity->callbacks->onNativeWindowCreated = callback_set::on_window_created;
		m_activity->callbacks->onNativeWindowDestroyed = callback_set::on_window_destroyed;
		m_activity->callbacks->onNativeWindowRedrawNeeded = callback_set::on_window_redraw_needed;
		m_activity->callbacks->onNativeWindowResized = callback_set::on_window_resized;
		m_activity->callbacks->onSaveInstanceState = callback_set::on_save_instance_state;
		m_activity->callbacks->onConfigurationChanged = callback_set::on_configuration_changed;
		m_activity->callbacks->onContentRectChanged = callback_set::on_content_rect_changed;
		m_activity->callbacks->onInputQueueCreated = callback_set::on_input_queue_created;
		m_activity->callbacks->onInputQueueDestroyed = callback_set::on_input_queue_destroyed;
	}

	void native_activity::reset_callbacks()
	{
		m_activity->callbacks->onStart = NULL;
		m_activity->callbacks->onStop = NULL;
		m_activity->callbacks->onPause = NULL;
		m_activity->callbacks->onResume = NULL;
		m_activity->callbacks->onDestroy = NULL;
		m_activity->callbacks->onWindowFocusChanged = NULL;
		m_activity->callbacks->onNativeWindowCreated = NULL;
		m_activity->callbacks->onNativeWindowDestroyed = NULL;
		m_activity->callbacks->onNativeWindowRedrawNeeded = NULL;
		m_activity->callbacks->onNativeWindowResized = NULL;
		m_activity->callbacks->onSaveInstanceState = NULL;
		m_activity->callbacks->onConfigurationChanged = NULL;
		m_activity->callbacks->onContentRectChanged = NULL;
		m_activity->callbacks->onInputQueueCreated = NULL;
		m_activity->callbacks->onInputQueueDestroyed = NULL;
	}

	void native_activity::dispatch_idle(app& the_app)
	{
		m_dispatcher.fire_idle(the_app);
	}

	void native_activity::on_configuration_changed()
	{
		LOGV("%s:%d> in %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);
	}

	void native_activity::on_content_rect_changed(const ARect *r)
	{
		LOGV("%s:%d> in %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);
		m_dispatcher.send_event(system_event::rect_changed, *r);
	}

	void *native_activity::on_save_instance_state(size_t *size)
	{
		LOGV("%s:%d> in %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);
		return NULL;
	}

	void native_activity::on_start()
	{
		LOGV("%s:%d> in %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);
		m_dispatcher.send_event(system_event::start);
	}

	void native_activity::on_stop()
	{
		LOGV("%s:%d> in %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);
		m_dispatcher.send_event(system_event::stop);
	}

	void native_activity::on_resume()
	{
		LOGV("%s:%d> in %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);
		m_dispatcher.send_event(system_event::resume);
	}

	void native_activity::on_pause()
	{
		LOGV("%s:%d> in %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);
		m_dispatcher.send_event(system_event::pause);
	}

	void native_activity::on_destroy()
	{
		LOGV("%s:%d> in %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);
		m_dispatcher.send_event(system_event::quit);
	}

	void native_activity::on_window_focus_changed(int has_focus)
	{
		LOGV("%s:%d> in %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);
		if (has_focus) {
			m_dispatcher.send_event(system_event::gained_focus);
		} else {
			m_dispatcher.send_event(system_event::lost_focus);
		}
	}

	void native_activity::on_window_created(ANativeWindow* window)
	{
		LOGV("%s:%d> in %s(w=%p), m_window=%p\n", __FILE__, __LINE__,
				__PRETTY_FUNCTION__, window, m_window);
		if (window && m_window) {
			LOGE("%s:%d> on_window_created while m_window != NULL\n", __FILE__, __LINE__);
		}
		m_window = window;
		m_dispatcher.send_sync_event(system_event::window_created, window);
	}

	void native_activity::on_window_resized(ANativeWindow* window)
	{
		LOGV("%s:%d> in %s(w=%p), m_window=%p\n", __FILE__, __LINE__,
				__PRETTY_FUNCTION__, window, m_window);
		if (m_window != window) {
			LOGE("%s:%d> incoherent window object from message (%p!=%p)",
					__FILE__, __LINE__, m_window, window);
			m_window = window;
		}
		m_dispatcher.send_sync_event(system_event::window_resized, window);
	}

	void native_activity::on_window_redraw_needed(ANativeWindow* window)
	{
		LOGV("%s:%d> in %s(w=%p), m_window=%p\n", __FILE__, __LINE__,
				__PRETTY_FUNCTION__, window, m_window);
		if (m_window != window) {
			LOGE("%s:%d> incoherent window object from message (%p!=%p)",
					__FILE__, __LINE__, m_window, window);
			m_window = window;
		}
		m_dispatcher.send_sync_event(system_event::window_exposed, window);
	}

	void native_activity::on_window_destroyed(ANativeWindow* window)
	{
		LOGV("%s:%d> in %s(w=%p), m_window=%p\n", __FILE__, __LINE__,
				__PRETTY_FUNCTION__, window, m_window);
		if (m_window != window) {
			LOGE("%s:%d> incoherent window object from message (%p!=%p)",
					__FILE__, __LINE__, m_window, window);
			m_window = window;
		}
		m_dispatcher.send_sync_event(system_event::window_destroyed, window);
		m_window = NULL;
	}

	void native_activity::on_input_queue_created(AInputQueue *q)
	{
		LOGV("%s:%d> in %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);
		m_dispatcher.setup_queue(q);
	}

	void native_activity::on_input_queue_destroyed(AInputQueue *q)
	{
		LOGV("%s:%d> in %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);
		m_dispatcher.reset_queue();
	}

}
