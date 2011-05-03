PRODUCT_SPECIFIC_DEFINES += TARGET_PRELINKER_MAP=$(TOP)/device/se/x10/prelink-linux-arm-x10.map

# These is the hardware-specific overlay, which points to the location
# of hardware-specific resource overrides, typically the frameworks and
# application settings that are stored in resourced.
DEVICE_PACKAGE_OVERLAYS := device/se/x10/overlay

# These are the hardware-specific configuration files
PRODUCT_COPY_FILES := \
	device/se/x10/prebuilt/vold.fstab:system/etc/vold.fstab \
	device/se/x10/prebuilt/egl.cfg:system/lib/egl/egl.cfg

# Init files
PRODUCT_COPY_FILES += \
	device/se/x10/init.es209ra.rc:root/init.es209ra.rc 

# Prebuilt kl keymaps
PRODUCT_COPY_FILES += \
	device/se/x10/es209ra_handset.kl:system/usr/keylayout/es209ra_handset.kl \
	device/se/x10/es209ra_keypad.kl:system/usr/keylayout/es209ra_keypad.kl 

# These are the hardware-specific features
PRODUCT_COPY_FILES += \
	frameworks/base/data/etc/handheld_core_hardware.xml:system/etc/permissions/handheld_core_hardware.xml \
	frameworks/base/data/etc/android.hardware.camera.flash-autofocus.xml:system/etc/permissions/android.hardware.camera.flash-autofocus.xml \
	frameworks/base/data/etc/android.hardware.camera.front.xml:system/etc/permissions/android.hardware.camera.front.xml \
	frameworks/base/data/etc/android.hardware.telephony.gsm.xml:system/etc/permissions/android.hardware.telephony.gsm.xml \
	frameworks/base/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml \
	frameworks/base/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
	frameworks/base/data/etc/android.hardware.sensor.proximity.xml:system/etc/permissions/android.hardware.sensor.proximity.xml \
	frameworks/base/data/etc/android.hardware.sensor.light.xml:system/etc/permissions/android.hardware.sensor.light.xml \
	frameworks/base/data/etc/android.hardware.sensor.gyroscope.xml:system/etc/permissions/android.hardware.sensor.gyroscope.xml \
	frameworks/base/data/etc/android.hardware.touchscreen.multitouch.jazzhand.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.jazzhand.xml \
	frameworks/base/data/etc/android.hardware.nfc.xml:system/etc/permissions/android.hardware.nfc.xml \
	frameworks/base/data/etc/android.software.sip.voip.xml:system/etc/permissions/android.software.sip.voip.xml \
	packages/wallpapers/LivePicker/android.software.live_wallpaper.xml:system/etc/permissions/android.software.live_wallpaper.xml

# The OpenGL ES API level that is natively supported by this device.
# This is a 16.16 fixed point number
PRODUCT_PROPERTY_OVERRIDES := \
    ro.opengles.version=131072


# Screen density is actually considered a locale (since it is taken into account
# the the build-time selection of resources). The product definitions including
# this file must pay attention to the fact that the first entry in the final
# PRODUCT_LOCALES expansion must not be a density.
PRODUCT_LOCALES := hdpi



ifeq ($(TARGET_PREBUILT_KERNEL),)
LOCAL_KERNEL := device/se/x10/kernel
else
LOCAL_KERNEL := $(TARGET_PREBUILT_KERNEL)
endif

PRODUCT_COPY_FILES += \
    $(LOCAL_KERNEL):kernel

PRODUCT_PACKAGES += \
    gps.qsd8k \
    lights.es209ra \
    sensors.es209ra \
    gralloc.es209ra \
    copybit.es209ra \
    libOmxCore \
    libOmxVdec \
    libOmxVidEnc \
    libmm-omxcore \
    AndroidTerm \
    CMParts \
    CMStats \
    DSPManager \
    libcyanogen-dsp \
    Pacman \
    SETorch

PRODUCT_PROPERTY_OVERRIDES += \
    dalvik.vm.heapsize=32m \
    persisit.sys.vm.heapsize=32m \
    ro.media.dec.jpeg.memcap=20000000\
    ro.com.google.locationfeatures=1 \
    ro.com.google.networklocation=1

## OMX Video decoder 
#PRODUCT_COPY_FILES += \
#    device/se/x10/prebuilt/libOmxVdec.so:system/lib/libOmxVdec.so


## RIL related stuff 
PRODUCT_COPY_FILES += \
    vendor/se/x10/proprietary/bin/port-bridge:system/bin/port-bridge \
    vendor/se/x10/proprietary/bin/qmuxd:system/bin/qmuxd \
    vendor/se/x10/proprietary/lib/libauth.so:system/lib/libauth.so \
    vendor/se/x10/proprietary/lib/libcm.so:system/lib/libcm.so \
    vendor/se/x10/proprietary/lib/libdiag.so:system/lib/libdiag.so \
    vendor/se/x10/proprietary/lib/libdll.so:system/lib/libdll.so \
    vendor/se/x10/proprietary/lib/libdsm.so:system/lib/libdsm.so \
    vendor/se/x10/proprietary/lib/libdss.so:system/lib/libdss.so \
    vendor/se/x10/proprietary/lib/libgsdi_exp.so:system/lib/libgsdi_exp.so \
    vendor/se/x10/proprietary/lib/libgstk_exp.so:system/lib/libgstk_exp.so \
    vendor/se/x10/proprietary/lib/libmmgsdilib.so:system/lib/libmmgsdilib.so \
    vendor/se/x10/proprietary/lib/libnv.so:system/lib/libnv.so \
    vendor/se/x10/proprietary/lib/liboem_rapi.so:system/lib/liboem_rapi.so \
    vendor/se/x10/proprietary/lib/liboncrpc.so:system/lib/liboncrpc.so \
    vendor/se/x10/proprietary/lib/libpbmlib.so:system/lib/libpbmlib.so \
    vendor/se/x10/proprietary/lib/libqmi.so:system/lib/libqmi.so \
    vendor/se/x10/proprietary/lib/libqueue.so:system/lib/libqueue.so \
    vendor/se/x10/proprietary/lib/libril-qc-1.so:system/lib/libril-qc-1.so \
    vendor/se/x10/proprietary/lib/libwms.so:system/lib/libwms.so \
    vendor/se/x10/proprietary/lib/libwmsts.so:system/lib/libwmsts.so


## Camera proprietaries
PRODUCT_COPY_FILES += \
    vendor/se/x10/proprietary/lib/liboemcamera.so:system/lib/liboemcamera.so \
    vendor/se/x10/proprietary/lib/libmmjpeg.so:system/lib/libmmjpeg.so \
    vendor/se/x10/proprietary/lib/libmmipl.so:system/lib/libmmipl.so \
    vendor/se/x10/proprietary/etc/firmware/yamato_pfp.fw:system/etc/firmware/yamato_pfp.fw \
    vendor/se/x10/proprietary/etc/firmware/yamato_pm4.fw:system/etc/firmware/yamato_pm4.fw \
    vendor/se/x10/proprietary/etc/firmware/camfirm.bin:system/etc/firmware/camfirm.bin 


## Atheros AR6002 firmware
PRODUCT_COPY_FILES += \
    device/se/x10/prebuilt/ar6000.ko:system/lib/modules/ar6000.ko \
    vendor/se/x10/proprietary/bin/eeprom.AR6002:system/bin/eeprom.AR6002 \
    vendor/se/x10/proprietary/bin/wlan_mac:system/bin/wlan_mac \
    vendor/se/x10/proprietary/bin/wlan_tool:system/bin/wlan_tool \
    vendor/se/x10/proprietary/bin/bmiloader:system/bin/bmiloader \
    vendor/se/x10/proprietary/bin/eeprom.AR6002:system/bin/eeprom.AR6002 \
    vendor/se/x10/proprietary/bin/recEvent:system/bin/recEvent \
    vendor/se/x10/proprietary/bin/wmiconfig:system/bin/wmiconfig 
    

## BT proprietary
PRODUCT_COPY_FILES += \
    vendor/se/x10/proprietary/bin/hci_qcomm_init:system/bin/hci_qcomm_init \
    device/se/x10/prebuilt/init.bt.sh:system/etc/init.bt.sh 


## Adreno 200 files
PRODUCT_COPY_FILES += \
    vendor/se/x10/proprietary/lib/libgsl.so:system/lib/libgsl.so \
    vendor/se/x10/proprietary/lib/egl/libGLESv1_CM_adreno200.so:system/lib/egl/libGLESv1_CM_adreno200.so \
    vendor/se/x10/proprietary/lib/egl/libq3dtools_adreno200.so:system/lib/egl/libq3dtools_adreno200.so \
    vendor/se/x10/proprietary/lib/egl/libEGL_adreno200.so:system/lib/egl/libEGL_adreno200.so \
    vendor/se/x10/proprietary/lib/egl/libGLESv2_adreno200.so:system/lib/egl/libGLESv2_adreno200.so 



## Other libraries and proprietary binaries
PRODUCT_COPY_FILES += \
    device/se/x10/prebuilt/chargemon:system/bin/chargemon \
    device/se/x10/prebuilt/model:system/recovery/model \
    device/se/x10/prebuilt/sh:system/recovery/sh \
    device/se/x10/prebuilt/recovery.tar.bz2:system/recovery/recovery.tar.bz2 \
    device/se/x10/prebuilt/hw_config.sh:system/etc/hw_config.sh \
    vendor/se/x10/proprietary/etc/sensors.conf:system/etc/sensors.conf \
    vendor/se/x10/proprietary/bin/akmd2:system/bin/akmd2 \
    vendor/se/x10/proprietary/etc/DualMicControl.txt:system/etc/DualMicControl.txt 

#offline charging animation
PRODUCT_COPY_FILES += \
    vendor/se/x10/proprietary/bin/chargemon:system/bin/charger \
    vendor/se/x10/proprietary/bin/updatemiscta:system/bin/updatemiscta \
    vendor/se/x10/proprietary/lib/libmiscta.so:system/lib/libmiscta.so \
    vendor/se/x10/proprietary/etc/semc/chargemon/anim1.rle:system/etc/semc/chargemon/anim1.rle \
    vendor/se/x10/proprietary/etc/semc/chargemon/anim2.rle:system/etc/semc/chargemon/anim2.rle \
    vendor/se/x10/proprietary/etc/semc/chargemon/anim3.rle:system/etc/semc/chargemon/anim3.rle \
    vendor/se/x10/proprietary/etc/semc/chargemon/anim4.rle:system/etc/semc/chargemon/anim4.rle \
    vendor/se/x10/proprietary/etc/semc/chargemon/anim5.rle:system/etc/semc/chargemon/anim5.rle \
    vendor/se/x10/proprietary/etc/semc/chargemon/anim6.rle:system/etc/semc/chargemon/anim6.rle \
    vendor/se/x10/proprietary/etc/semc/chargemon/anim7.rle:system/etc/semc/chargemon/anim7.rle \
    vendor/se/x10/proprietary/etc/semc/chargemon/anim8.rle:system/etc/semc/chargemon/anim8.rle 

#various fixes
PRODUCT_COPY_FILES += \
    device/se/x10/media_profiles.xml:system/etc/media_profiles.xml \
    device/se/x10/vold.fstab:system/etc/vold.fstab \
    device/se/x10/prebuilt/gps.conf:system/etc/gps.conf \
    device/se/x10/prebuilt/wpa_supplicant.conf:system/etc/wifi/wpa_supplicant.conf \
    vendor/se/x10/proprietary/etc/dhcpcd/dhcpcd-hooks/01-test:system/etc/dhcpcd/dhcpcd-hooks/01-test \
    vendor/se/x10/proprietary/etc/dhcpcd/dhcpcd-run-hooks:system/etc/dhcpcd/dhcpcd-run-hooks \
    vendor/se/x10/proprietary/etc/dhcpcd/dhcpcd.conf:system/etc/dhcpcd/dhcpcd.conf 


#testing kernel
PRODUCT_COPY_FILES += \
    device/se/x10/prebuilt/boot.img:system/kernel/boot.img \
    device/se/x10/prebuilt/miniloader:system/kernel/miniloader \
    device/se/x10/prebuilt/splboot.ko:system/kernel/splboot.ko \

#FreeXperia BootLogo
PRODUCT_COPY_FILES += \
    device/se/x10/prebuilt/bootanimation.zip:system/media/bootanimation.zip \
    device/se/x10/prebuilt/Superuser.apk:system/app/Superuser.apk



# See comment at the top of this file. This is where the other
# half of the device-specific product definition file takes care
# of the aspects that require proprietary drivers that aren't
# commonly available
$(call inherit-product-if-exists, vendor/se/x10/device-vendor.mk)
