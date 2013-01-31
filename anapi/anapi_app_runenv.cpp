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

#include <string>
#include <stdexcept>
#include "anapi_app_runenv.h"
#include "anapi_log.h"

namespace anapi
{

	bool app_runenv::extract_assets(bool external)
	{
		try {
			std::string outdir = external ? m_activity->externalDataPath : m_activity->internalDataPath;
			// make sure we have an ending slash
			outdir += "/";
			asset_manager am(m_jni, m_activity->assetManager);
			asset_dir dir = am.make_asset_dir();
			if (!dir.open(""))
				throw std::runtime_error("extract: opening ./ failed");
			std::string fname;
			while (!(fname = dir.get_next_file()).empty()) {
				LOGV("%s:%d> processing file %s, copy to %s%s\n", __FILE__, __LINE__,
						fname.c_str(), outdir.c_str(), fname.c_str());
				asset a = am.make_asset();
				if (!a.open(fname)) {
					LOGW("%s:%d> failed to open %s\n", __FILE__, __LINE__, fname.c_str());
					continue;
				}
				a.copy_to(outdir + fname);
			}
		} catch (std::exception& e) {
			LOGE("%s:%d> exception caught while extracting assets: %s\n", __FILE__, __LINE__, e.what());
		}
		return false;
	}

	bool app_runenv::extract_one_asset(const std::string& name, bool external)
	{
		try {
			std::string outdir = external ? m_activity->externalDataPath : m_activity->internalDataPath;
			// make sure we have an ending slash
			outdir += "/";
			asset_manager am(m_jni, m_activity->assetManager);
			asset a = am.make_asset();
			if (!a.open(name))
				throw std::runtime_error("failed to open " + name);
			if (!a.copy_to(outdir + name))
				throw std::runtime_error("failed to copy " + name);
			return true;
		} catch (std::exception& e) {
			LOGE("%s:%d> exception caught while extracting assets: %s\n", __FILE__, __LINE__, e.what());
		}
		return false;
	}

}
