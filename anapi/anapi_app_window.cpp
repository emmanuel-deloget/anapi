#include "anapi/anapi_app_window.h"

namespace anapi
{

	int app_window::set_buffer_geometry(unsigned int width, unsigned int height, int format) const
	{
		if (m_window) {
			return ANativeWindow_setBuffersGeometry(m_window, width, height, format);
		}
		return -1;
	}

}
