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

#ifndef anapi_app_activity_h_included
#define anapi_app_activity_h_included

#include <android/native_activity.h>

namespace anapi
{

	// should match the enum definition of window flags
	enum struct window_flag : int
	{
		lock_while_screen_on = 1 << 0,
		dim_behind = 1 << 1,
		blur_behind = 1 << 2,
		not_focusable = 1 << 3,
		not_touchable = 1 << 4,
		not_touch_modal = 1 << 5,
		touchable_when_waking = 1 << 6,
		keep_screen_on = 1 << 7,
		layout_in_screen = 1 << 8,
		layout_no_limits = 1 << 9,
		fullscreen = 1 << 10,
		force_not_fullscreen = 1 << 11,
		dither = 1 << 12,
		secure = 1 << 13,
		scaled = 1 << 14,
		ignore_cheek_press = 1 << 15,
		layout_inset_decor = 1 << 16,
		alt_focusable_im = 1 << 17,
		watch_outside_touch = 1 << 18,
		show_when_locked = 1 << 19,
		show_wallpaper = 1 << 20,
		turn_screen_on = 1 << 21,
		dismiss_keyguard = 1 << 22
	};

	inline window_flag operator|(const window_flag& a, const window_flag& b)
	{
		return (window_flag)((int)a | (int)b);
	}

	inline window_flag operator&(const window_flag& a, const window_flag& b)
	{
		return (window_flag)((int)a & (int)b);
	}

	class app_activity
	{
	private:
		ANativeActivity *m_activity;

	public:
		app_activity()
		: m_activity(NULL)
		{ }
		~app_activity()
		{ }

		app_activity(ANativeActivity *a)
		: m_activity(a)
		{ }

		app_activity(const app_activity& other)
		: m_activity(other.m_activity)
		{ }

		app_activity& operator=(const app_activity& other)
		{
			m_activity = other.m_activity;
			return *this;
		}

		void add_window_flags(const window_flag& flags) const;
		void remove_window_flags(const window_flag& flags) const;
		void finish() const;

	private:
		app_activity(app_activity&&) = delete;
		app_activity& operator=(app_activity&&) = delete;
	};

}

#endif // anapi_app_activity_h_included
