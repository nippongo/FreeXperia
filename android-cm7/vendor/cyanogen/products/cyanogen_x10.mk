# Inherit AOSP device configuration for x10.
$(call inherit-product, device/se/x10/device_x10.mk)

# Inherit some common cyanogenmod stuff.
$(call inherit-product, vendor/cyanogen/products/common_full.mk)

# Include GSM stuff
$(call inherit-product, vendor/cyanogen/products/gsm.mk)

#
# Setup device specific product configuration.
#
PRODUCT_NAME := cyanogen_x10
PRODUCT_BRAND := google
PRODUCT_DEVICE := x10
PRODUCT_MODEL := X10
PRODUCT_MANUFACTURER := SE
PRODUCT_BUILD_PROP_OVERRIDES += PRODUCT_NAME=x10 BUILD_ID=GRI40 BUILD_DISPLAY_ID=GRI40 BUILD_FINGERPRINT=google/x10/x10:2.3.3/GRI40/102588:user/release-keys PRIVATE_BUILD_DESC="x10-user 2.3.3 GRI40 102588 release-keys"

# Build kernel
PRODUCT_SPECIFIC_DEFINES += TARGET_PREBUILT_KERNEL=
PRODUCT_SPECIFIC_DEFINES += TARGET_KERNEL_DIR=kernel-msm
PRODUCT_SPECIFIC_DEFINES += TARGET_KERNEL_CONFIG=cyanogen_es209ra_defconfig
PRODUCT_SPECIFIC_DEFINES += TARGET_PRELINKER_MAP=$(TOP)/device/se/x10/prelink-linux-arm-x10.map

# Extra x10 overlay
PRODUCT_PACKAGE_OVERLAYS += vendor/cyanogen/overlay/x10

# Add the Torch app
PRODUCT_PACKAGES += SETorch


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
        ro.modversion=CyanogenMod-7-$(shell date +%m%d%Y)-NIGHTLY-X10
else
    ifdef CYANOGEN_RELEASE
        PRODUCT_PROPERTY_OVERRIDES += \
            ro.modversion=CyanogenMod-7.0.2-X10
    else
        PRODUCT_PROPERTY_OVERRIDES += \
            ro.modversion=CyanogenMod-7.0.2-X10-J018
    endif
endif

#
# Copy x10 specific prebuilt files
#
#PRODUCT_COPY_FILES +=  \
#    vendor/cyanogen/prebuilt/hdpi/media/bootanimation.zip:system/media/bootanimation.zip
