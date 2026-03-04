LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

ifeq ($(TARGET_ARCH_ABI), arm64-v8a)
    LOCAL_MODULE := ue4dumper64

	LOCAL_SRC_FILES := ELF64/fix.cpp \
                       kmods.cpp \

    LOCAL_CPP_INCLUDES += $(LOCAL_PATH)
    LOCAL_CPP_INCLUDES += $(LOCAL_PATH)/ELF64

endif

ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)
    LOCAL_MODULE := ue4dumper

	LOCAL_SRC_FILES := ELF/ElfReader.cpp \
                       ELF/ElfRebuilder.cpp \
                       kmods.cpp \

    LOCAL_CPP_INCLUDES += $(LOCAL_PATH)
    LOCAL_CPP_INCLUDES += $(LOCAL_PATH)/ELF

endif

LOCAL_CFLAGS += -Os -ffunction-sections -fdata-sections -fvisibility=hidden -fvisibility-inlines-hidden
LOCAL_CPPFLAGS += -Os -ffunction-sections -fdata-sections -fvisibility=hidden -fvisibility-inlines-hidden
LOCAL_LDFLAGS += -Wl,--gc-sections

LOCAL_LDLIBS += -llog

include $(BUILD_EXECUTABLE)




