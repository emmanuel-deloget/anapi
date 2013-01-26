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

#include "anapi_app.h"
#include "anapi_log.h"
#include "anapi_sync_primitives.h"

namespace anapi
{

	mutex app::m_app_mutex;
	app *app::m_app = NULL;

	void app::terminate(const app_activity& activity) const
	{
		LOGI("%s:%d> app asked to terminate the activity\n", __FILE__, __LINE__);
		activity.finish();
		m_terminating = true;
	}

}
