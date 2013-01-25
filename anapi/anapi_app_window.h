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

#ifndef anapi_app_window_h_included
#define anapi_app_window_h_included

#include <android/native_window.h>

namespace anapi
{

	class app_window
	{
	private:
		ANativeWindow *m_window;

	public:
		app_window(ANativeWindow *w)
		: m_window(w)
		{ }
		~app_window()
		{ }

		app_window(const app_window& other)
		: m_window(other.m_window)
		{ }

		app_window& operator=(const app_window& other)
		{
			m_window = other.m_window;
			return *this;
		}

		int set_buffer_geometry(unsigned int width, unsigned int height, int format) const;

		ANativeWindow* native_handle() const
		{ return m_window; }

	private:
		app_window(app_window&&) = delete;
		app_window& operator=(app_window&&) = delete;
	};

}

#endif // anapi_app_window_h_included
