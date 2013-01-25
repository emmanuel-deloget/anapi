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

ANAPI_PATH := $(LOCAL_PATH)/$(ANAPI_SUB_PATH)

ANAPI_ADDITIONAL_CFLAGS := -std=c++11
ANAPI_ADDITIONAL_LDFLAGS := -u ANativeActivity_onCreate
ANAPI_ADDITIONAL_LDLIBS := -llog -landroid
ANAPI_ADDITIONAL_CPP_FEATURES := exceptions rtti

ANAPI_ADDITIONAL_CFLAGS += -I $(ANAPI_PATH)

include $(ANAPI_PATH)/anapi/Source.mk
