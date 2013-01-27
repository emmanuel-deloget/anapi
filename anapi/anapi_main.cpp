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

#include <pthread.h>
#include <android/native_activity.h>

#include "anapi_native_activity.h"
#include "anapi_instance_state.h"
#include "anapi_log.h"
#include "anapi_app.h"

namespace anapi
{
	void *main_thread_func(void *p)
	{
		LOGV("%s:%d> entering the main thread[%ld]\n", __FILE__, __LINE__, pthread_self());

		native_activity *a = reinterpret_cast<native_activity*>(p);

		if (a) {
			try {
				// the dispatcher is a per-thread thingy
				// (based upon the Android ALooper object)
				a->prepare_msg_dispatcher();

				app& the_app = app::get_instance();
				the_app.on_create(app_activity(a->ptr()), app_runenv(a->ptr()));

				LOGI("%s:%d> thread is about to be started\n", __FILE__, __LINE__);
				a->started();

				// process messages
				do {
					event_result result = a->dispatch_message(the_app);
					if (result == event_result::shall_quit) {
						break;
					} else if (result == event_result::handled) {
						continue;
					}
					if (!the_app.terminating())
						a->dispatch_idle(the_app);
				} while (true);
			} catch (std::exception& e) {
				LOGE("%s:%d > an exception occurred: '%s'\n", __FILE__, __LINE__, e.what());
			}
		}

		// we don't need the activity anymore
		ANativeActivity *ptr = a->ptr();
		LOGI("%s:%d> the thread is about to end\n", __FILE__, __LINE__);
		delete a;
		LOGI("%s:%d> Finishing activity\n", __FILE__, __LINE__);
		ANativeActivity_finish(ptr);

		return NULL;
	}
}

extern "C" void ANativeActivity_onCreate(ANativeActivity* activity, void* saved_state, size_t state_size)
{
	LOGI("%s:%d> starting the activity %p\n", __FILE__, __LINE__, activity);

	anapi::instance_state state(reinterpret_cast<char*>(saved_state), state_size);
	anapi::native_activity *nact = new anapi::native_activity(activity, state);

	pthread_t thread;
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if (pthread_create(&thread, &attr, anapi::main_thread_func, nact) != 0) {
		LOGE("%s:%d> failed to create the main thread\n", __FILE__, __LINE__);
		return;
	}

	nact->wait_started();
	LOGI("%s:%d> thread is started, we're giving the control back to the VM'\n", __FILE__, __LINE__);

	activity->instance = nact;
}
