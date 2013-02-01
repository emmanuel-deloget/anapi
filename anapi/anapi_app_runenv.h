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

#include <string>
#include <android/native_activity.h>
#include "anapi_asset_manager.h"

namespace anapi
{

	class app_runenv
	{
	private:
		ANativeActivity *m_activity;
		JNIEnv *m_jni;

	public:
		app_runenv()
		: m_activity(NULL)
		, m_jni(NULL)
		{ }
		~app_runenv()
		{ }

		app_runenv(ANativeActivity *a, JNIEnv* jni)
		: m_activity(a)
		, m_jni(jni)
		{ }

		app_runenv(const app_runenv& other)
		: m_activity(other.m_activity)
		, m_jni(other.m_jni)
		{ }

		app_runenv& operator=(const app_runenv& other)
		{
			m_activity = other.m_activity;
			m_jni = other.m_jni;
			return *this;
		}

		std::string external_data_path() const
		{ return std::string(m_activity->externalDataPath); }

		std::string internal_data_path() const
		{ return std::string(m_activity->internalDataPath); }

		asset_manager make_asset_manager() const
		{ return asset_manager(m_jni, m_activity->assetManager); }

		bool extract_assets(bool external);
		bool extract_one_asset(const std::string& name, bool external);
		bool enter_internal_data_path() const;

	private:
		app_runenv(app_runenv&&) = delete;
		app_runenv& operator=(app_runenv&&) = delete;
	};

}

#endif // anapi_app_runenv_h_included
