

# Introduction #

If you use the standard distribution of anapi++ (.tar.gz or .tar.bz2), just decompress it in your jni/ directory. This will create a directory named anapi++-x.y.z/ where x.y.z is the version of the distribution. This directory contains the following sub-directories :

| **directory** | **description** |
|:--------------|:----------------|
| anapi/ | contains the source code of anapi++ |
| docs/ | contains various documents in txt format |
| samples | contains a few anapi++ samples |

# Writing your _Android.mk_ file #

After the definition of `LOCAL_PATH` but before the call to the `CLEAR_VARS` script, you have to define the `ANAPI_SUB_PATH` variable and to include the _Anapi.mk_ makefile. This is done by adding the following lines to your _Android.mk_ file:

```
LOCAL_PATH := $(call my-dir)

ANAPI_SUB_PATH := anapi++-x.y.z
include $(LOCAL_PATH)/$(ANAPI_SUB_PATH)/Anapi.mk

include $(CLEAR_VARS)
```

Once this is setup, the build system will compile the static anapi++ library as part of your project - but it will not link your native activity against it. To force the link, you shall setup the following variables:

```
LOCAL_STATIC_LIBRARIES	:= $(ANAPI_STATIC_LIBRARIES)
LOCAL_CFLAGS		:= $(ANAPI_ADDITIONAL_CFLAGS)
LOCAL_LDFLAGS		:= $(ANAPI_ADDITIONAL_LDFLAGS)
LOCAL_LDLIBS		:= $(ANAPI_ADDITIONAL_LDLIBS)
LOCAL_CPP_FEATURES	+= $(ANAPI_ADDITIONAL_CPP_FEATURES)
```

## Example ##

With the following directory architecture:

  * AndroidManifest.xml
  * jni/
    * anapi++-x.y.z
      * anapi/
      * docs/
      * samples/
    * src1.cpp
    * src2.cpp
    * src2.cpp

We get:

```
LOCAL_PATH := $(call my-dir)

ANAPI_SUB_PATH := anapi++-x.y.z
include $(LOCAL_PATH)/$(LOCAL_SUB_PATH)/Anapi.mk

include $(CLEAR_VARS)

LOCAL_MODULE            := my-module
LOCAL_SRC_FILES         := src1.cpp src2.cpp src3.cpp

LOCAL_STATIC_LIBRARIES	:= $(ANAPI_STATIC_LIBRARY)
LOCAL_CFLAGS		:= $(ANAPI_ADDITIONAL_CFLAGS)
LOCAL_LDFLAGS		:= $(ANAPI_ADDITIONAL_LDFLAGS)
LOCAL_LDLIBS		:= $(ANAPI_ADDITIONAL_LDLIBS)
LOCAL_CPP_FEATURES	+= $(ANAPI_ADDITIONAL_CPP_FEATURES)

include $(BUILD_SHARED_LIBRARY)
```

# Adding an Application.mk file #

To successfully build an anapi++ project, you must include an _Application.mk_ file to your project. This file will tell the build system to

  * use a specific compiler to compile your code
  * link your code to a specific runtime

All anapi++ project **must** use the _g++4.7_ compiler and must link to the _gnustl\_static_ runtime. This resulat can achieved by adding the following lines to your _Application.mk_ file:

```
APP_STL := gnustl_static
NDK_TOOLCHAIN_VERSION := 4.7
```

If you can afford to limit yourself to a high-end ARM plateform, we suggest you to also add the following line (which will force the use of the ARMv7+NEON ABI).

```
APP_ABI := armeabi-v7a
```

# Including anapi++ headers #

The setup which is described above allows you to #include anapi++ headers by using:

```
#include "anapi/anapi_app.h"
```

Be aware that you 'must' use double quotes (otherwise the build system may not find the anapi++ headers).

All anapi++ entities (classes, enumerations, public functions...) are stored in the `anapi` namespace.

