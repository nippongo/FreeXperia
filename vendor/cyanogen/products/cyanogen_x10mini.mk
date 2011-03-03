# Inherit AOSP device configuration for passion.
$(call inherit-product, device/se/x10mini/device_x10mini.mk)

# Inherit some common cyanogenmod stuff.
$(call inherit-product, vendor/cyanogen/products/common.mk)

# Include GSM stuff
$(call inherit-product, vendor/cyanogen/products/gsm.mk)

#
# Setup device specific product configuration.
#
PRODUCT_NAME := cyanogen_x10mini
PRODUCT_BRAND := SonyEricsson
PRODUCT_DEVICE := x10mini
PRODUCT_MODEL := X10mini
PRODUCT_MANUFACTURER := SonyEricsson
PRODUCT_BUILD_PROP_OVERRIDES += BUILD_ID=FRG83 BUILD_DISPLAY_ID=FRG83 BUILD_FINGERPRINT=google/passion/passion/mahimahi:2.2.1/FRG83/60505:user/release-keys PRIVATE_BUILD_DESC="passion-user 2.2.1 FRG83 60505 release-keys"

PRODUCT_SPECIFIC_DEFINES += TARGET_PRELINKER_MAP=$(TOP)/device/se/x10mini/prelink-linux-arm-x10mini.map

PRODUCT_PACKAGES += ADWLauncher

# Add ROMManager build property
PRODUCT_PROPERTY_OVERRIDES += \
    ro.config.ringtone=Innovation.mp3 \
    ro.config.notification_sound=Color.mp3 \
    ro.config.alarm_alert=Light.mp3 \
    ro.config.cal_notification=Vector.mp3 \
    ro.config.msg_notification=Ascend.mp3



# Enable Windows Media
WITH_WINDOWS_MEDIA := true

#
# Set ro.modversion
#
PRODUCT_PROPERTY_OVERRIDES += \
	ro.modversion=MiniCM-1.0.0-Alpha4

# CyanogenMod specific product packages
PRODUCT_PACKAGES += \
    CMParts \
    CMPartsHelper \
    FileManager \
    CMWallpapers \
    DSPManager \
    Superuser \
    SoundRecorder \
    Gallery \
    LatinIME

# Copy over the changelog to the device
PRODUCT_COPY_FILES += \
    vendor/cyanogen/CHANGELOG.mkdn:system/etc/CHANGELOG-CM.txt

# Common CM overlay
PRODUCT_PACKAGE_OVERLAYS += vendor/cyanogen/overlay/floyo
PRODUCT_PACKAGE_OVERLAYS += vendor/cyanogen/overlay/common

# Bring in some audio files
include frameworks/base/data/sounds/AudioPackageHtc.mk

PRODUCT_COPY_FILES += \
    vendor/cyanogen/prebuilt/common/bin/backuptool.sh:system/bin/backuptool.sh \
    vendor/cyanogen/prebuilt/common/bin/verify_cache_partition_size.sh:system/bin/verify_cache_partition_size.sh \
    vendor/cyanogen/prebuilt/common/etc/resolv.conf:system/etc/resolv.conf \
    vendor/cyanogen/prebuilt/common/etc/sysctl.conf:system/etc/sysctl.conf \
    vendor/cyanogen/prebuilt/common/etc/terminfo/l/linux:system/etc/terminfo/l/linux \
    vendor/cyanogen/prebuilt/common/etc/terminfo/u/unknown:system/etc/terminfo/u/unknown \
    vendor/cyanogen/prebuilt/common/etc/profile:system/etc/profile \
    vendor/cyanogen/prebuilt/common/etc/init.local.rc:system/etc/init.local.rc \
    vendor/cyanogen/prebuilt/common/etc/init.d/00banner:system/etc/init.d/00banner \
    vendor/cyanogen/prebuilt/common/etc/init.d/01sysctl:system/etc/init.d/01sysctl \
    vendor/cyanogen/prebuilt/common/etc/init.d/03firstboot:system/etc/init.d/03firstboot \
    vendor/cyanogen/prebuilt/common/etc/init.d/05mountsd:system/etc/init.d/05mountsd \
    vendor/cyanogen/prebuilt/common/etc/init.d/10apps2sd:system/etc/init.d/10apps2sd \
    vendor/cyanogen/prebuilt/common/etc/init.d/11fsready:system/etc/init.d/11fsready \
    vendor/cyanogen/prebuilt/common/etc/init.d/12compcache:system/etc/init.d/12compcache \
    vendor/cyanogen/prebuilt/common/etc/init.d/20userinit:system/etc/init.d/20userinit \
    vendor/cyanogen/prebuilt/common/bin/handle_compcache:system/bin/handle_compcache \
    vendor/cyanogen/prebuilt/common/bin/compcache:system/bin/compcache \
    vendor/cyanogen/prebuilt/common/bin/fix_permissions:system/bin/fix_permissions \
    vendor/cyanogen/prebuilt/common/bin/sysinit:system/bin/sysinit \
    vendor/cyanogen/prebuilt/common/bin/xrecovery.tar:system/bin/xrecovery.tar \
    vendor/cyanogen/prebuilt/common/bin/chargemon:system/bin/chargemon \
    vendor/cyanogen/prebuilt/common/bin/busybox:system/bin/busybox \
    vendor/cyanogen/prebuilt/common/xbin/htop:system/xbin/htop \
    vendor/cyanogen/prebuilt/common/xbin/irssi:system/xbin/irssi \
    vendor/cyanogen/prebuilt/common/xbin/lsof:system/xbin/lsof \
    vendor/cyanogen/prebuilt/common/xbin/powertop:system/xbin/powertop \
    vendor/cyanogen/prebuilt/common/xbin/openvpn-up.sh:system/xbin/openvpn-up.sh

PRODUCT_COPY_FILES += \
    vendor/cyanogen/proprietary/HTC_IME_lo15.apk:./system/app/HTC_IME_lo15.apk \
    vendor/cyanogen/proprietary/Clicker.apk:./system/app/Clicker.apk \
    vendor/cyanogen/proprietary/libt99.so:./system/lib/libt99.so \
    vendor/cyanogen/proprietary/Facebook.apk:./system/app/Facebook.apk \
    vendor/cyanogen/proprietary/YouTube.apk:./system/app/YouTube.apk \
    vendor/cyanogen/proprietary/GenieWidget.apk:./system/app/GenieWidget.apk \
    vendor/cyanogen/proprietary/Gmail.apk:./system/app/Gmail.apk \
    vendor/cyanogen/proprietary/GoogleBackupTransport.apk:./system/app/GoogleBackupTransport.apk \
    vendor/cyanogen/proprietary/GoogleCalendarSyncAdapter.apk:./system/app/GoogleCalendarSyncAdapter.apk \
    vendor/cyanogen/proprietary/GoogleContactsSyncAdapter.apk:./system/app/GoogleContactsSyncAdapter.apk \
    vendor/cyanogen/proprietary/GoogleFeedback.apk:./system/app/GoogleFeedback.apk \
    vendor/cyanogen/proprietary/GooglePartnerSetup.apk:./system/app/GooglePartnerSetup.apk \
    vendor/cyanogen/proprietary/GoogleQuickSearchBox.apk:./system/app/GoogleQuickSearchBox.apk \
    vendor/cyanogen/proprietary/GoogleServicesFramework.apk:./system/app/GoogleServicesFramework.apk \
    vendor/cyanogen/proprietary/Maps.apk:./system/app/Maps.apk \
    vendor/cyanogen/proprietary/MarketUpdater.apk:./system/app/MarketUpdater.apk \
    vendor/cyanogen/proprietary/MediaUploader.apk:./system/app/MediaUploader.apk \
    vendor/cyanogen/proprietary/NetworkLocation.apk:./system/app/NetworkLocation.apk \
    vendor/cyanogen/proprietary/OneTimeInitializer.apk:./system/app/OneTimeInitializer.apk \
    vendor/cyanogen/proprietary/PassionQuickOffice.apk:./system/app/PassionQuickOffice.apk \
    vendor/cyanogen/proprietary/SetupWizard.apk:./system/app/SetupWizard.apk \
    vendor/cyanogen/proprietary/Talk.apk:./system/app/Talk.apk \
    vendor/cyanogen/proprietary/Twitter.apk:./system/app/Twitter.apk \
    vendor/cyanogen/proprietary/Vending.apk:./system/app/Vending.apk \
    vendor/cyanogen/proprietary/kickback.apk:./system/app/kickback.apk \
    vendor/cyanogen/proprietary/soundback.apk:./system/app/soundback.apk \
    vendor/cyanogen/proprietary/talkback.apk:./system/app/talkback.apk \
    vendor/cyanogen/proprietary/com.google.android.maps.xml:./system/etc/permissions/com.google.android.maps.xml \
    vendor/cyanogen/proprietary/features.xml:./system/etc/permissions/features.xml \
    vendor/cyanogen/proprietary/com.google.android.maps.jar:./system/framework/com.google.android.maps.jar \
    vendor/cyanogen/proprietary/libspeech.so:./system/lib/libspeech.so \
    vendor/cyanogen/proprietary/libimageutils.so:./system/lib/libimageutils.so \
    vendor/cyanogen/proprietary/libvoicesearch.so:./system/lib/libvoicesearch.so \
    vendor/cyanogen/proprietary/libzxing.so:./system/lib/libzxing.so

#
# Copy passion specific prebuilt files
#
PRODUCT_COPY_FILES +=  \
    vendor/cyanogen/prebuilt/mdpi/media/bootanimation.zip:system/media/bootanimation.zip
