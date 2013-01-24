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

#ifndef anapi_ticker_h_included
#define anapi_ticker_h_included

namespace anapi
{

	class ticker
	{
	public:
		typedef unsigned long long tick_type;

	private:
		tick_type m_start;
		tick_type m_current;
		tick_type m_last;
		tick_type m_offset;
		tick_type m_ptime;

	public:
		ticker()
		: m_start(0)
		, m_current(0)
		, m_last(0)
		, m_offset(0)
		, m_ptime(0)
		{ }
		~ticker()
		{ }

		ticker(const ticker& other)
		: m_start(other.m_start)
		, m_current(other.m_current)
		, m_last(other.m_last)
		, m_offset(other.m_offset)
		, m_ptime(other.m_ptime)
		{ }

		ticker& operator=(const ticker& other)
		{
			m_start = other.m_start;
			m_current = other.m_current;
			m_last = other.m_last;
			m_offset = other.m_offset;
			m_ptime = other.m_ptime;
			return *this;
		}

		void update();

		tick_type ticks() const
		{ return m_current - (m_start + m_offset); }

		tick_type laps() const
		{ return m_current - m_last; }

		void pause();
		void resume();

		bool paused() const { return m_ptime ? true : false; }

	private:
		ticker(ticker&&) = delete;
		ticker& operator=(ticker&&) = delete;
	};

}

#endif // anapi_ticker_h_included
