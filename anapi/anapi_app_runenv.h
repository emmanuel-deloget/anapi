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

#ifndef anapi_app_runenv_h_included
#define anapi_app_runenv_h_included

#include <android/native_activity.h>

namespace anapi
{

	class app_runenv
	{
	private:
		ANativeActivity *m_activity;

	public:
		app_runenv()
		: m_activity(NULL)
		{ }
		~app_runenv()
		{ }

		app_runenv(ANativeActivity *a)
		: m_activity(a)
		{ }

		app_runenv(const app_runenv& other)
		: m_activity(other.m_activity)
		{ }

		app_runenv& operator=(const app_runenv& other)
		{
			m_activity = other.m_activity;
			return *this;
		}

		std::string external_data_path() const
		{ return std::string(m_activity->externalDataPath); }

		std::string internal_data_path() const
		{ return std::string(m_activity->internalDataPath); }

	private:
		app_runenv(app_runenv&&) = delete;
		app_runenv& operator=(app_runenv&&) = delete;
	};

}

#endif // anapi_app_runenv_h_included