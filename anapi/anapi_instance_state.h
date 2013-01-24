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

#ifndef anapi_instance_state_h_included
#define anapi_instance_state_h_included

#include <vector>
#include <utility>

namespace anapi
{

	class instance_state
	{
	private:
		std::vector<char> m_data;

	public:
		instance_state(char *d, std::size_t len)
		: m_data(d, d + len)
		{ }

		instance_state() = default;
		instance_state(const instance_state& other) = default;
		instance_state(instance_state&& other) = default;
		~instance_state() = default;
		instance_state& operator=(const instance_state& other) = default;
		instance_state& operator=(instance_state&& other) = default;

		void *data()
		{ return reinterpret_cast<void*>(&m_data.front()); }

		std::size_t length() const
		{ return m_data.size(); }
	};

}

#endif // anapi_instance_state_h_included
