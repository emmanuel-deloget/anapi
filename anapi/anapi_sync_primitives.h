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

#ifndef anapi_sync_primitives_h_included
#define anapi_sync_primitives_h_included

#include <pthread.h>

namespace anapi
{

	class mutex
	{
	private:
		mutable pthread_mutex_t m_mutex;

	public:
		mutex()
		{ pthread_mutex_init(&m_mutex, NULL); }
		~mutex()
		{ pthread_mutex_destroy(&m_mutex); }

		void lock() const
		{ pthread_mutex_lock(&m_mutex); }
		void unlock() const
		{ pthread_mutex_unlock(&m_mutex); }

		pthread_mutex_t *native_ptr()
		{ return &m_mutex; }

	private:
		mutex(const mutex&) = delete;
		mutex& operator=(const mutex&) = delete;
		mutex(mutex&& other) = delete;
		mutex& operator=(mutex&& other) = delete;
	};

	class scoped_lock
	{
	private:
		const mutex& m_mutex;

	public:
		scoped_lock(const mutex& m)
		: m_mutex(m)
		{ m_mutex.lock(); }
		~scoped_lock()
		{ m_mutex.unlock(); }

	private:
		scoped_lock(const scoped_lock&) = delete;
		scoped_lock(scoped_lock&&) = delete;
		scoped_lock& operator=(const scoped_lock&) = delete;
		scoped_lock& operator=(scoped_lock&&) = delete;
	};

	class condition
	{
	private:
		pthread_cond_t m_cond;
		bool m_value;
		mutex m_mutex;

	public:
		condition()
		: m_value(false)
		{ pthread_cond_init(&m_cond, NULL); }
		~condition()
		{ pthread_cond_destroy(&m_cond); }

		void wait_up()
		{
			scoped_lock lock(m_mutex);
			while (!m_value) {
				pthread_cond_wait(&m_cond, m_mutex.native_ptr());
			}
		}

		void wait_up_down()
		{
			scoped_lock lock(m_mutex);
			while (!m_value) {
				pthread_cond_wait(&m_cond, m_mutex.native_ptr());
			}
			m_value = false;
		}

		void wait_down()
		{
			scoped_lock lock(m_mutex);
			while (m_value) {
				pthread_cond_wait(&m_cond, m_mutex.native_ptr());
			}
		}

		void up()
		{
			scoped_lock lock(m_mutex);
			m_value = true;
			pthread_cond_broadcast(&m_cond);
		}

		void down()
		{
			scoped_lock lock(m_mutex);
			m_value = false;
			pthread_cond_broadcast(&m_cond);
		}

		explicit operator bool() const
		{
			scoped_lock lock(m_mutex);
			return m_value;
		}

		bool state() const
		{
			scoped_lock lock(m_mutex);
			return m_value;
		}

	private:
		condition(const condition&) = delete;
		condition(condition&&) = delete;
		condition& operator=(const condition&) = delete;
		condition& operator=(condition&&) = delete;
	};

}

#endif // anapi_sync_primitives_h_included
