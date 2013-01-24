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

#ifndef anapi_events_h_included
#define anapi_events_h_included

#include <string>
#include <android/input.h>
#include <android/keycodes.h>

namespace anapi
{

	enum struct system_event : unsigned int
	{
		start, stop, quit, pause, resume, gained_focus, lost_focus, rect_changed,
		synchronous = 0x10000, window_created, window_resized, window_exposed, window_destroyed
	};

	inline int operator&(const system_event& sea, const system_event& seb)
	{
		int a = (int)sea;
		int b = (int)seb;
		return a & b;
	}

	enum struct button_event : unsigned int
	{
		down, up
	};

	std::string get_sys_event_name(const system_event& se);

	class motion_event
	{
	private:
		AInputEvent *m_ie;

	public:
		motion_event(AInputEvent *ie)
		: m_ie(ie)
		{ }
		~motion_event()
		{ }
		motion_event(const motion_event& other)
		: m_ie(other.m_ie)
		{ }
		motion_event& operator=(const motion_event& other)
		{ m_ie = other.m_ie; return *this; }

		int action() const
		{ return AMotionEvent_getAction(m_ie); }

		int rawx(size_t idx) const
		{ return AMotionEvent_getRawX(m_ie, idx); }
		int rawy(size_t idx) const
		{ return AMotionEvent_getRawY(m_ie, idx); }

		size_t pointer_count() const
		{ return AMotionEvent_getPointerCount(m_ie); }

	private:
		motion_event(motion_event&&) = delete;
		motion_event& operator=(motion_event&&) = delete;
	};

	class key_event
	{
	private:
		AInputEvent *m_ie;

	public:
		key_event(AInputEvent *ie)
		:m_ie(ie)
		{ }
		~key_event()
		{ }
		key_event(const key_event& other)
		: m_ie(other.m_ie)
		{ }
		key_event& operator=(const key_event& other)
		{ m_ie = other.m_ie; return *this; }

		button_event action() const
		{
			if (AKeyEvent_getAction(m_ie) == AKEY_EVENT_ACTION_DOWN)
				return button_event::down;
			return button_event::up;
		}

		int keycode() const
		{ return AKeyEvent_getKeyCode(m_ie); }

		bool is_back() const
		{ return keycode() == AKEYCODE_BACK; }

	private:
		key_event(key_event&&) = delete;
		key_event& operator=(key_event&&) = delete;
	};

	struct rectangle
	{
		int left;
		int top;
		int right;
		int bottom;
	};

}

#endif // anapi_events_h_included

