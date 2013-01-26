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

#include <utility>
#include <EGL/egl.h>

#include "egl_context.h"
#include "anapi/anapi_log.h"

namespace egl
{

	context::context(context&& other)
	: m_display(other.release_display())
	, m_surface(other.release_surface())
	, m_context(other.release_context())
	, m_width(other.width())
	, m_height(other.height())
	{
		LOGV("%s:%d> in %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);
	}

	context::~context()
	{
		liberate();
	}

	void context::liberate()
	{
		if (m_display != EGL_NO_DISPLAY) {
			eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
			if (m_context != EGL_NO_CONTEXT)
				eglDestroyContext(m_display, m_context);
			if (m_surface != EGL_NO_SURFACE)
				eglDestroySurface(m_display, m_surface);
			eglTerminate(m_display);
		}
	}

	context& context::operator=(context&& other)
	{
		std::swap(m_display, other.m_display);
		std::swap(m_surface, other.m_surface);
		std::swap(m_context, other.m_context);
		return *this;
	}

	void context::present()
	{
		if (m_display != EGL_NO_DISPLAY && m_surface != EGL_NO_SURFACE)
			eglSwapBuffers(m_display, m_surface);
	}

	// this is more or less awful... no error check, no
	// flexibility, and so on... awful, I tell you.
	bool context::allocate(const anapi::app_window& window)
	{
	    const EGLint attribs[] = {
	            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
	            EGL_BLUE_SIZE, 8,
	            EGL_GREEN_SIZE, 8,
	            EGL_RED_SIZE, 8,
	            EGL_NONE
	    };

	    EGLint format;
	    EGLint numConfigs;
	    EGLConfig config;

	    liberate();

		m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
		eglInitialize(m_display, 0, 0);
		eglChooseConfig(m_display, attribs, &config, 1, &numConfigs);
		eglGetConfigAttrib(m_display, config, EGL_NATIVE_VISUAL_ID, &format);

		window.set_buffer_geometry(0, 0, format);

	    m_surface = eglCreateWindowSurface(m_display, config, window.native_handle(), NULL);
	    m_context = eglCreateContext(m_display, config, NULL, NULL);

	    if (eglMakeCurrent(m_display, m_surface, m_surface, m_context) == EGL_FALSE) {
	        return false;
	    }

	    eglQuerySurface(m_display, m_surface, EGL_WIDTH, &m_width);
	    eglQuerySurface(m_display, m_surface, EGL_HEIGHT, &m_height);

	    return true;
	}

}
