# Inherit AOSP device configuration for passion.
$(call inherit-product, device/se/x8/device_x8.mk)

# Inherit some common cyanogenmod stuff.
$(call inherit-product, vendor/cyanogen/products/common_full.mk)

# Include GSM stuff
$(call inherit-product, vendor/cyanogen/products/gsm.mk)

#
# Setup device specific product configuration.
#
PRODUCT_NAME := cyanogen_x8
PRODUCT_BRAND := google
PRODUCT_DEVICE := x8
PRODUCT_MODEL := X8
PRODUCT_MANUFACTURER := SE
PRODUCT_BUILD_PROP_OVERRIDES += PRODUCT_NAME=x8 BUILD_ID=GRI40 BUILD_DISPLAY_ID=GRI40 BUILD_FINGERPRINT=google/x8/x8:2.3.3/GRI40/102588:user/release-keys PRIVATE_BUILD_DESC="x8-user 2.3.3 GRI40 102588 release-keys"

# Build kernel
PRODUCT_SPECIFIC_DEFINES += TARGET_PREBUILT_KERNEL=
PRODUCT_SPECIFIC_DEFINES += TARGET_KERNEL_DIR=kernel-msm
PRODUCT_SPECIFIC_DEFINES += TARGET_KERNEL_CONFIG=cyanogen_X8_defconfig
PRODUCT_SPECIFIC_DEFINES += TARGET_PRELINKER_MAP=$(TOP)/device/se/x8/prelink-linux-arm-x8.map

# Extra x8 overlay
PRODUCT_PACKAGE_OVERLAYS += vendor/cyanogen/overlay/x8

# Add the Torch app
#PRODUCT_PACKAGES += Torch

# TI FM radio
$(call inherit-product, vendor/cyanogen/products/ti_fm_radio.mk)

# Extra RIL settings
PRODUCT_PROPERTY_OVERRIDES += \
    ro.ril.enable.managed.roaming=1 \
    ro.ril.oem.nosim.ecclist=911,112,999,000,08,118,120,122,110,119,995 \
    ro.ril.emc.mode=2

#
# Set ro.modversion
#
ifdef CYANOGEN_NIGHTLY
    PRODUCT_PROPERTY_OVERRIDES += \
        ro.modversion=CyanogenMod-7-$(shell date +%m%d%Y)-NIGHTLY-X8
else
    ifdef CYANOGEN_RELEASE
        PRODUCT_PROPERTY_OVERRIDES += \
            ro.modversion=CyanogenMod-7.0.2-X8
    else
        PRODUCT_PROPERTY_OVERRIDES += \
            ro.modversion=CyanogenMod-7.0.2-X8-J8
    endif
endif

#
# Copy x8 specific prebuilt files
#
#PRODUCT_COPY_FILES +=  \
#    vendor/cyanogen/prebuilt/hdpi/media/bootanimation.zip:system/media/bootanimation.zip
