LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := google-play-games
LOCAL_MODULE_FILENAME := gpg
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libgpg.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../include/android/gpg
include $(PREBUILT_STATIC_LIBRARY)
