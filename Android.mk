LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := ProcessZombieThreadBlocked.cpp
LOCAL_STATIC_LIBRARIES += \
	libcutils \
	libutils \
	liblog

LOCAL_MODULE := prcoess_zombie_thread_blocked
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
