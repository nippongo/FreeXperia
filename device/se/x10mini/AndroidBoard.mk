LOCAL_PATH := $(call my-dir)

file := $(TARGET_OUT_KEYLAYOUT)/x10mini-keypad.kl
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/x10mini-keypad.kl | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_ROOT_OUT)/init.x10mini.rc
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/init.x10mini.rc | $(ACP)
	$(transform-prebuilt-to-target)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := x10mini-keypad.kcm
include $(BUILD_KEY_CHAR_MAP)

file := $(TARGET_OUT_KEYLAYOUT)/h2w_headset.kl
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/h2w_headset.kl | $(ACP)
	$(transform-prebuilt-to-target)

include $(CLEAR_VARS)
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE := vold.fstab
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

# the system properties for each device, loaded by init
file := $(TARGET_OUT)/build.x10mini.prop
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/build.x10mini.prop | $(ACP)
	$(transform-prebuilt-to-target)

-include vendor/se/x10mini/AndroidBoardVendor.mk
