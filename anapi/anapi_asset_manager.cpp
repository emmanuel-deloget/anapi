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
}

