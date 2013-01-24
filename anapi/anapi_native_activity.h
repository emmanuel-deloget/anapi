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

#ifndef anapi_native_activity_h_included
#define anapi_native_activity_h_included

#include <android/native_activity.h>
#include <android/native_window.h>

#include "anapi_instance_state.h"
#include "anapi_message_dispatcher.h"
#include "anapi_app.h"

namespace anapi
{

	class native_activity
	{
	private:
		ANativeActivity *m_activity;
		instance_state m_istate;
		message_dispatcher m_dispatcher;
		condition m_started;

	public:
		native_activity(ANativeActivity *a, const instance_state& istate);
		~native_activity();

	private:
		native_activity(const native_activity&) = delete;
		native_activity(native_activity&&) = delete;
		native_activity& operator=(const native_activity&) = delete;
		native_activity& operator=(native_activity&&) = delete;

	private:
		friend struct callback_set;
		void setup_callbacks();
		void reset_callbacks();

		void on_configuration_changed();
		void on_content_rect_changed(const ARect *r);
		void *on_save_instance_state(size_t *size);
		void on_start();
		void on_stop();
		void on_resume();
		void on_pause();
		void on_destroy();
		void on_window_focus_changed(int has_focus);
		void on_window_created(ANativeWindow* window);
		void on_window_resized(ANativeWindow* window);
		void on_window_redraw_needed(ANativeWindow* window);
		void on_window_destroyed(ANativeWindow* window);
		void on_input_queue_created(AInputQueue *q);
		void on_input_queue_destroyed(AInputQueue *q);

	public:
		void prepare_msg_dispatcher();

		void started();
		void wait_started();
		bool dispatch_message(app& the_app, bool& shall_quit);
		void dispatch_idle(app& the_app);

		ANativeActivity *ptr()
		{ return m_activity; }
	};

}

#endif // anapi_native_activity_h_included
