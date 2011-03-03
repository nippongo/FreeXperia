# Inherit AOSP device configuration for x8.
$(call inherit-product, device/se/x8/device_x8.mk)

# Inherit some common cyanogenmod stuff.
$(call inherit-product, vendor/cyanogen/products/common_full.mk)

# Include GSM stuff
$(call inherit-product, vendor/cyanogen/products/gsm.mk)

#
# Setup device specific product configuration.
#
PRODUCT_NAME := cyanogen_x8
PRODUCT_BRAND := SonyEricsson
PRODUCT_DEVICE := x8
PRODUCT_MODEL := X8
PRODUCT_MANUFACTURER := SonyEricsson
PRODUCT_BUILD_PROP_OVERRIDES += PRODUCT_NAME=SonyEricsson_x8 BUILD_ID=FRG83 BUILD_DISPLAY_ID=GRH78C BUILD_FINGERPRINT=google/passion/passion/mahimahi:2.2.1/FRG83/60505:user/release-keys PRIVATE_BUILD_DESC="passion-user 2.2.1 FRG83 60505 release-keys"

PRODUCT_SPECIFIC_DEFINES += TARGET_PRELINKER_MAP=$(TOP)/device/se/x8/prelink-linux-arm-x8.map

#
# Set ro.modversion
#
ifdef CYANOGEN_NIGHTLY
    PRODUCT_PROPERTY_OVERRIDES += \
        ro.modversion=CyanogenMod-7-$(shell date +%m%d%Y)-NIGHTLY-x8
else
    ifdef CYANOGEN_RELEASE
        PRODUCT_PROPERTY_OVERRIDES += \
            ro.modversion=CyanogenMod-7.0.0-RC1-x8
    else
        PRODUCT_PROPERTY_OVERRIDES += \
            ro.modversion=CyanogenMod-7.0.0-RC1-x8-J003
    endif
endif

PRODUCT_PROPERTY_OVERRIDES += \
ro.media.enc.file.format=3gp,mp4
ro.media.enc.vid.codec=m4v,h263
ro.media.enc.vid.h263.width=176,352
ro.media.enc.vid.h263.height=144,288
ro.media.enc.vid.h263.bps=64000,800000
ro.media.enc.vid.h263.fps=1,30
ro.media.enc.vid.m4v.width=176,352
ro.media.enc.vid.m4v.height=144,288
ro.media.enc.vid.m4v.bps=64000,800000
ro.media.enc.vid.m4v.fps=1,30
ro.media.dec.aud.wma.enabled=1
ro.media.dec.vid.wmv.enabled=1
settings.display.autobacklight=1
media.stagefright.enable-player=true
media.stagefright.enable-meta=true
media.stagefright.enable-scan=true
media.stagefright.enable-http=true
keyguard.no_require_sim=true
ro.sf.lcd_density=160
dalvik.vm.execution-mode=int:jit
dalvik.vm.heapsize=32m



#
# Copy specific prebuilt files
#
PRODUCT_COPY_FILES +=  \
    vendor/cyanogen/prebuilt/mdpi/media/bootanimation.zip:system/media/bootanimation.zip
