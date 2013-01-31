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

#include <vector>
#include <stdexcept>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <fcntl.h>
#include <unistd.h>

#include "anapi_asset_manager.h"
#include "anapi_log.h"

namespace anapi
{

	// ================= CLASS anapi::asset_manager

	asset_manager::asset_manager()
	: m_jni(NULL)
	, m_am(NULL)
	{ }

	asset_manager::asset_manager(JNIEnv *jni, AAssetManager *am)
	: m_jni(jni)
	, m_am(am)
	{ }

	asset_manager::asset_manager(const asset_manager& other)
	: m_jni(other.m_jni)
	, m_am(other.m_am)
	{ }

	asset_manager::asset_manager(asset_manager&& other)
	: m_jni(other.m_jni)
	, m_am(other.m_am)
	{
		other.m_jni = NULL;
		other.m_am = NULL;
	}

	asset_manager::~asset_manager()
	{ }

	asset_manager& asset_manager::operator=(const asset_manager& other)
	{
		m_jni = other.m_jni;
		m_am = other.m_am;
		return *this;
	}

	asset_manager& asset_manager::operator=(asset_manager&& other)
	{
		m_jni = other.m_jni;
		m_am = other.m_am;
		other.m_jni = NULL;
		other.m_am = NULL;
		return *this;
	}

	asset asset_manager::make_asset() const
	{
		return asset(m_jni, m_am);
	}

	asset_dir asset_manager::make_asset_dir() const
	{
		return asset_dir(m_jni, m_am);
	}

	// ================= CLASS anapi::asset_sir

	asset_dir::asset_dir()
	: m_jni(NULL)
	, m_am(NULL)
	, m_dir(NULL)
	{ }

	asset_dir::~asset_dir()
	{ close(); }

	asset_dir::asset_dir(asset_dir&& other)
	: m_jni(other.m_jni)
	, m_am(other.m_am)
	, m_dir(other.m_dir)
	{
		other.m_jni = NULL;
		other.m_am = NULL;
		other.m_dir = NULL;
	}

	asset_dir& asset_dir::operator=(asset_dir&& other)
	{
		m_jni = other.m_jni;
		m_am = other.m_am;
		m_dir = other.m_dir;
		other.m_jni = NULL;
		other.m_am = NULL;
		other.m_dir = NULL;
		return *this;
	}

	asset_dir::asset_dir(JNIEnv *jni, AAssetManager *am)
	: m_jni(jni)
	, m_am(am)
	, m_dir(NULL)
	{ }

	bool asset_dir::open(const std::string& dirname)
	{
		m_dir = AAssetManager_openDir(m_am, dirname.c_str());
		if (!m_dir)
			return false;
		return true;
	}

	void asset_dir::close()
	{
		if (m_dir)
			AAssetDir_close(m_dir);
	}

	void asset_dir::rewind() const
	{
		if (m_dir)
			AAssetDir_rewind(m_dir);
	}

	// empty file names do not exist, so hitting .empty() == true
	// means we found all the files.
	std::string asset_dir::get_next_file() const
	{
		if (m_dir) {
			const char *f = AAssetDir_getNextFileName(m_dir);
			if (f)
				return std::string(f);
		}
		return std::string();
	}

	// ================= CLASS anapi::asset

	asset::asset()
	: m_jni(NULL)
	, m_am(NULL)
	, m_desc({-1, 0, 0})
	{ }

	asset::asset(JNIEnv *jni, AAssetManager *am)
	: m_jni(jni)
	, m_am(am)
	, m_desc({-1, 0, 0})
	{ }

	asset::asset(asset&& other)
	: m_jni(other.m_jni)
	, m_am(other.m_am)
	, m_desc(other.m_desc)
	{
		other.m_jni = NULL;
		other.m_am = NULL;
	}

	asset& asset::operator=(asset&& other)
	{
		m_jni = other.m_jni;
		m_am = other.m_am;
		m_desc = other.m_desc;
		other.m_jni = NULL;
		other.m_am = NULL;
		other.m_desc = { -1, 0, 0 };
		return *this;
	}

	asset::~asset()
	{ close(); }

	bool asset::open(const std::string& name)
	{
		AAsset *asset = AAssetManager_open(m_am, name.c_str(), AASSET_MODE_UNKNOWN);
		if (!asset)
			return false;
		off_t start, length;
		int fd = AAsset_openFileDescriptor(asset, &start, &length);
		AAsset_close(asset);
		if (fd < 0) {
			return false;
		}
		lseek(fd, start, SEEK_SET);
		m_desc = description{ fd, start, length };
		return true;
	}

	ssize_t asset::read(void *p, size_t length) const
	{
		// need to explicitely tell the scope, otherwise
		// we're searching for an asset::read method and -
		// guess what, it exists.
		return ::read(m_desc.fd, p, length);
	}

	void asset::close()
	{
		if (m_desc.fd >= 0)
			::close(m_desc.fd);
	}

	bool asset::seek(off_t offset, const seek_position& whence) const
	{
		off_t real_offset;
		int real_whence = SEEK_SET;

		switch (whence) {
		case seek_position::from_cur:
			real_offset = offset;
			real_whence = SEEK_CUR;
			break;
		case seek_position::from_end:
			real_offset = m_desc.start + m_desc.length + offset;
			break;
		case seek_position::from_start:
		default:
			real_offset = offset + m_desc.start;
			break;
		}
		if (lseek(m_desc.fd, real_offset, real_whence) < 0)
			return false;
		return true;
	}

	bool asset::copy_to(const std::string& out)
	{
		if (m_desc.fd < 0)
			return false;

		seek(0, seek_position::from_start);
		std::vector<unsigned char> content(m_desc.length);
		if (read(&content.front(), content.size()) != (ssize_t)content.size()) {
			LOGE("failed to read the source file\n");
			return false;
		}

		int fd = ::open(out.c_str(), O_WRONLY|O_TRUNC|O_CREAT, 0644);
		if (fd < 0) {
			LOGE("failed to open the destination file [%s]\n", out.c_str());
			return false;
		}
		::write(fd, &content.front(), content.size());
		::close(fd);
		return true;
	}
}

