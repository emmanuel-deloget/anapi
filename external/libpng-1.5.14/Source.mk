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

LOCAL_MODULE		:=	png
LOCAL_SRC_FILES		:=	\
	$(ANAPI_SUB_PATH)/libpng/png.c \
	$(ANAPI_SUB_PATH)/libpng/pngerror.c \
	$(ANAPI_SUB_PATH)/libpng/pngget.c \
	$(ANAPI_SUB_PATH)/libpng/pngmem.c \
	$(ANAPI_SUB_PATH)/libpng/pngpread.c \
	$(ANAPI_SUB_PATH)/libpng/pngread.c \
	$(ANAPI_SUB_PATH)/libpng/pngrio.c \
	$(ANAPI_SUB_PATH)/libpng/pngrtran.c \
	$(ANAPI_SUB_PATH)/libpng/pngrutil.c \
	$(ANAPI_SUB_PATH)/libpng/pngset.c \
	$(ANAPI_SUB_PATH)/libpng/pngtrans.c \
	$(ANAPI_SUB_PATH)/libpng/pngwio.c \
	$(ANAPI_SUB_PATH)/libpng/pngwrite.c \
	$(ANAPI_SUB_PATH)/libpng/pngwtran.c \
	$(ANAPI_SUB_PATH)/libpng/pngwutil.c

LOCAL_CFLAGS		:=	-std=c99
LOCAL_CPP_FEATURES 	+=	$(ANAPI_ADDITIONAL_CPP_FEATURES)

ANAPI_STATIC_LIBRARIES += $(LOCAL_MODULE)

include $(BUILD_STATIC_LIBRARY)
