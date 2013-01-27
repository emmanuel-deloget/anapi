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

#ifndef egl_context_h_included
#define egl_context_h_included

#include <EGL/egl.h>
#include "anapi/anapi_app_window.h"

namespace egl
{

	class context
	{
	private:
	    EGLDisplay m_display;
	    EGLSurface m_surface;
	    EGLContext m_context;
	    int m_width;
	    int m_height;

	    EGLDisplay release_display()
	    { EGLDisplay tmp = m_display; m_display = EGL_NO_DISPLAY; return tmp; }
	    EGLSurface release_surface()
	    { EGLSurface tmp = m_surface; m_surface = EGL_NO_SURFACE; return tmp; }
	    EGLContext release_context()
	    { EGLContext tmp = m_context; m_context = EGL_NO_CONTEXT; return tmp; }

	    context(EGLDisplay d, EGLSurface s, EGLContext c)
	    : m_display(d)
	    , m_surface(s)
	    , m_context(c)
	    { }

	    bool valid() const
	    { return m_display != EGL_NO_DISPLAY && m_surface != EGL_NO_SURFACE && m_context != EGL_NO_CONTEXT; }

	public:
		context()
		: m_display(EGL_NO_DISPLAY)
		, m_surface(EGL_NO_SURFACE)
		, m_context(EGL_NO_CONTEXT)
		{ }

		~context();

		context(context&& other);
		context& operator=(context&&);

		bool allocate(const anapi::app_window& wnd);
		void liberate();

		void present();

		int width() const
		{ return m_width; }

		int height() const
		{ return m_height; }

		explicit operator bool() const
		{ return valid(); }

	private:
		context(const context&) = delete;
		context& operator=(const context&) = delete;
	};

}

#endif // egl_context_h_included
