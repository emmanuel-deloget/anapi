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

#ifndef anapi_asset_manager_h_included
#define anapi_asset_manager_h_included

#include <string>
#include <android/native_activity.h>
#include <android/asset_manager.h>

namespace anapi
{

	enum struct seek_position
	{ from_start, from_cur, from_end };

	class asset
	{
	private:
		struct description
		{
			int fd;
			off_t start;
			off_t length;
		};
	private:
		JNIEnv *m_jni;
		AAssetManager *m_am;
		description m_desc;

		asset(JNIEnv *jni, AAssetManager *am);
		void close();

		friend class asset_manager;

	public:
		asset();
		~asset();

		asset(asset&& other);
		asset& operator=(asset&& other);

		bool open(const std::string& name);
		ssize_t read(void *p, size_t length) const;

		bool seek(off_t offset, const seek_position& whence = seek_position::from_start) const;
		bool copy_to(const std::string& out);

	private:
		asset(const asset& other) = delete;
		asset& operator=(const asset& other) = delete;
	};

	class asset_dir
	{
	private:
		JNIEnv *m_jni;
		AAssetManager *m_am;
		AAssetDir *m_dir;

		friend class asset_manager;

		asset_dir(JNIEnv *jni, AAssetManager *am);
		void close();

	public:
		asset_dir();
		~asset_dir();

		bool open(const std::string& dirname);
		void rewind() const;
		std::string get_next_file() const;

		asset_dir(asset_dir&& other);
		asset_dir& operator=(asset_dir&& other);
	};

	class asset_manager
	{
	private:
		JNIEnv *m_jni;
		AAssetManager *m_am;

	public:
		asset_manager();
		asset_manager(JNIEnv *jni, AAssetManager *am);
		~asset_manager();

		asset_manager(const asset_manager& other);
		asset_manager& operator=(const asset_manager& other);
		asset_manager(asset_manager&&);
		asset_manager& operator=(asset_manager&&);

		asset make_asset() const;
		asset_dir make_asset_dir() const;
	};

}

#endif // anapi_asset_manager_h_included
