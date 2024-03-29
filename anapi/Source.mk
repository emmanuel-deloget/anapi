# *********************************************************************
#
# anapi++ -- a C++ interface for the Android NDK
#
#  Copyright (C) 2013 Emmanuel Deloget
#
#  This software is provided 'as-is', without any express or implied
#  warranty.  In no event will the authors be held liable for any
#  damages arising from the use of this software.
#
#  Permission is granted to anyone to use this software for any
#  purpose, including commercial applications, and to alter it and
#  redistribute it freely, subject to the following restrictions:
#
#  1. The origin of this software must not be misrepresented; you must
#     not claim that you wrote the original software. If you use this
#     software in a product, an acknowledgment in the product
#     documentation would be appreciated but is not required.
#
#  2. Altered source versions must be plainly marked as such, and must
#     not be misrepresented as being the original software.
#
#  3. This notice may not be removed or altered from any source
#     distribution.
#
#  Emmanuel Deloget logout@free.fr
#
# *********************************************************************

include $(CLEAR_VARS)

LOCAL_MODULE		:=	anapi++
LOCAL_SRC_FILES		:=	\
	$(ANAPI_SUB_PATH)/anapi/anapi_main.cpp \
	$(ANAPI_SUB_PATH)/anapi/anapi_native_activity.cpp \
	$(ANAPI_SUB_PATH)/anapi/anapi_message_dispatcher.cpp \
	$(ANAPI_SUB_PATH)/anapi/anapi_events.cpp \
	$(ANAPI_SUB_PATH)/anapi/anapi_app.cpp \
	$(ANAPI_SUB_PATH)/anapi/anapi_ticker.cpp \
	$(ANAPI_SUB_PATH)/anapi/anapi_app_activity.cpp \
	$(ANAPI_SUB_PATH)/anapi/anapi_app_window.cpp \
	$(ANAPI_SUB_PATH)/anapi/anapi_asset_manager.cpp \
	$(ANAPI_SUB_PATH)/anapi/anapi_app_runenv.cpp

LOCAL_CFLAGS		:=	$(ANAPI_ADDITIONAL_CFLAGS) -Wall -Werror
LOCAL_CPP_FEATURES 	+=	$(ANAPI_ADDITIONAL_CPP_FEATURES)

ANAPI_STATIC_LIBRARIES += $(LOCAL_MODULE)

include $(BUILD_STATIC_LIBRARY)
