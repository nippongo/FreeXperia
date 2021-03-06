ifeq ($(TARGET_BOOTLOADER_BOARD_NAME),es209ra)

ATHEROS_TOOL_PATH := $(call my-dir)/android_athr_tools
ATHEROS_MODULE_PATH := $(call my-dir)/ar6k_sdk

ifeq ($(BOARD_HAVE_BLUETOOTH),true)
include $(ATHEROS_TOOL_PATH)/athbtfilter/Android.mk
endif

include $(ATHEROS_TOOL_PATH)/wmiconfig/Android.mk
include $(ATHEROS_TOOL_PATH)/wlan_tool/Android.mk
include $(ATHEROS_MODULE_PATH)/Android.mk

endif
