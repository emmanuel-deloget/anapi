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

#include <sys/time.h>
#include "anapi_ticker.h"

namespace anapi
{

	namespace
	{
		unsigned long long get_current_time()
		{
			struct timeval tv;
			gettimeofday(&tv, NULL);
			return (unsigned long long)(tv.tv_usec) + (unsigned long long)(tv.tv_sec) * 1000000ULL;
		}
	}

	void ticker::update()
	{
		m_last = m_current;

		m_current = get_current_time();

		if (!m_start) {
			m_start = m_last = m_current;
		}
	}

	void ticker::pause()
	{
		m_ptime = get_current_time();
	}

	void ticker::resume()
	{
		if (m_ptime && m_start) {
			tick_type rtime = get_current_time();
			tick_type offs = rtime - m_ptime;
			m_offset += offs;
			m_current = rtime;
			m_ptime = 0;
		}
	}

}
