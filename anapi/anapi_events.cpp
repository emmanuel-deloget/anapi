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

#include "anapi_events.h"

namespace anapi
{

#define GSEN_CASE(v) case v: return #v

	std::string get_sys_event_name(const system_event& se)
	{
		switch (se) {
		GSEN_CASE(system_event::start);
		GSEN_CASE(system_event::stop);
		GSEN_CASE(system_event::quit);
		GSEN_CASE(system_event::resume);
		GSEN_CASE(system_event::pause);
		GSEN_CASE(system_event::gained_focus);
		GSEN_CASE(system_event::lost_focus);
		GSEN_CASE(system_event::rect_changed);
		GSEN_CASE(system_event::window_created);
		GSEN_CASE(system_event::window_destroyed);
		GSEN_CASE(system_event::window_exposed);
		GSEN_CASE(system_event::window_resized);
		default:
			break;
		}
		return "(unknown system event)";
	}

#undef GSEN_CASE

}
