# The gps config appropriate for this device
$(call inherit-product, device/common/gps/gps_us_supl.mk)

$(call inherit-product-if-exists, vendor/se/x8/x8-vendor.mk)

DEVICE_PACKAGE_OVERLAYS += device/se/x8/overlay

PRODUCT_PROPERTY_OVERRIDES += \
    dalvik.vm.heapsize=32m \
    persistent.sys.vm.heapsize=32m 
    
# HAL libs and other system binaries
PRODUCT_PACKAGES += \
    gps.shakira \
    gralloc.shakira \
    copybit.shakira \
    lights.shakira \
    libOmxCore \
    libmm-omxcore\
    libaudio

# Live wallpaper packages
PRODUCT_PACKAGES += \
    LiveWallpapersPicker \
    librs_jni
#    sensors.shakira \
#    LiveWallpapers \
#    MagicSmokeWallpapers \
#    VisualizationWallpapers 
        
# Publish that we support the live wallpaper feature.
PRODUCT_COPY_FILES += \
    packages/wallpapers/LivePicker/android.software.live_wallpaper.xml:/system/etc/permissions/android.software.live_wallpaper.xml

## RIL related stuff
PRODUCT_COPY_FILES += \
    vendor/se/x8/proprietary/bin/port-bridge:system/bin/port-bridge \
    vendor/se/x8/proprietary/bin/qmuxd:system/bin/qmuxd \
    vendor/se/x8/proprietary/lib/libauth.so:system/lib/libauth.so \
    vendor/se/x8/proprietary/lib/libcm.so:system/lib/libcm.so \
    vendor/se/x8/proprietary/lib/libdiag.so:system/lib/libdiag.so \
    vendor/se/x8/proprietary/lib/libdll.so:system/lib/libdll.so \
    vendor/se/x8/proprietary/lib/libdss.so:system/lib/libdss.so \
    vendor/se/x8/proprietary/lib/libdsm.so:system/lib/libdsm.so \
    vendor/se/x8/proprietary/lib/libgsdi_exp.so:system/lib/libgsdi_exp.so \
    vendor/se/x8/proprietary/lib/libgstk_exp.so:system/lib/libgstk_exp.so \
    vendor/se/x8/proprietary/lib/libmmgsdilib.so:system/lib/libmmgsdilib.so \
    vendor/se/x8/proprietary/lib/libnv.so:system/lib/libnv.so \
    vendor/se/x8/proprietary/lib/liboem_rapi.so:system/lib/liboem_rapi.so \
    vendor/se/x8/proprietary/lib/liboncrpc.so:system/lib/liboncrpc.so \
    vendor/se/x8/proprietary/lib/libpbmlib.so:system/lib/libpbmlib.so \
    vendor/se/x8/proprietary/lib/libqmi.so:system/lib/libqmi.so \
    vendor/se/x8/proprietary/lib/libqueue.so:system/lib/libqueue.so \
    vendor/se/x8/proprietary/lib/libril-qc-1.so:system/lib/libril-qc-1.so \
    vendor/se/x8/proprietary/lib/libwms.so:system/lib/libwms.so \
    vendor/se/x8/proprietary/lib/libwmsts.so:system/lib/libwmsts.so 
#    vendor/se/x8/proprietary/lib/libril.so:system/lib/libril.so \

## SE Sensors
PRODUCT_COPY_FILES += \
    vendor/se/x8/proprietary/lib/hw/sensors.default.so:system/lib/hw/sensors.shakira.so


## OMX proprietaries
PRODUCT_COPY_FILES += \
    vendor/se/x8/proprietary/lib/libmm-adspsvc.so:system/lib/libmm-adspsvc.so \
    vendor/se/x8/proprietary/lib/libOmxAacDec.so:system/lib/libOmxAacDec.so \
    vendor/se/x8/proprietary/lib/libOmxAmrRtpDec.so:system/lib/libOmxAmrRtpDec.so \
    vendor/se/x8/proprietary/lib/libOmxH264Dec.so:system/lib/libOmxH264Dec.so \
    vendor/se/x8/proprietary/lib/libOmxQcelpDec.so:system/lib/libOmxQcelpDec.so \
    vendor/se/x8/proprietary/lib/libOmxAacEnc.so:system/lib/libOmxAacEnc.so \
    vendor/se/x8/proprietary/lib/libOmxAmrwbDec.so:system/lib/libOmxAmrwbDec.so \
    vendor/se/x8/proprietary/lib/libOmxMp3Dec.so:system/lib/libOmxMp3Dec.so \
    vendor/se/x8/proprietary/lib/libOmxVidEnc.so:system/lib/libOmxVidEnc.so \
    vendor/se/x8/proprietary/lib/libOmxAmrDec.so:system/lib/libOmxAmrDec.so \
    vendor/se/x8/proprietary/lib/libOmxEvrcDec.so:system/lib/libOmxEvrcDec.so \
    vendor/se/x8/proprietary/lib/libOmxMpeg4Dec.so:system/lib/libOmxMpeg4Dec.so \
    vendor/se/x8/proprietary/lib/libOmxWmaDec.so:system/lib/libOmxWmaDec.so \
    vendor/se/x8/proprietary/lib/libOmxAmrEnc.so:system/lib/libOmxAmrEnc.so \
    vendor/se/x8/proprietary/lib/libOmxEvrcEnc.so:system/lib/libOmxEvrcEnc.so \
    vendor/se/x8/proprietary/lib/libOmxQcelp13Enc.so:system/lib/libOmxQcelp13Enc.so \
    vendor/se/x8/proprietary/lib/libOmxWmvDec.so:system/lib/libOmxWmvDec.so

## Hardware properties 
PRODUCT_COPY_FILES += \
    frameworks/base/data/etc/handheld_core_hardware.xml:system/etc/permissions/handheld_core_hardware.xml \
    frameworks/base/data/etc/android.hardware.telephony.gsm.xml:system/etc/permissions/android.hardware.telephony.gsm.xml \
    frameworks/base/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml \
    frameworks/base/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
    frameworks/base/data/etc/android.hardware.sensor.proximity.xml:system/etc/permissions/android.hardware.sensor.proximity.xml \
    frameworks/base/data/etc/android.hardware.sensor.light.xml:system/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/base/data/etc/android.hardware.touchscreen.multitouch.distinct.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.distinct.xml

## Camera proprietaries
PRODUCT_COPY_FILES += \
    vendor/se/x8/proprietary/etc/firmware/yamato_pfp.fw:system/etc/firmware/yamato_pfp.fw \
    vendor/se/x8/proprietary/etc/firmware/yamato_pm4.fw:system/etc/firmware/yamato_pm4.fw \
    vendor/se/x8/proprietary/lib/liboemcamera.so:system/lib/liboemcamera.so \
    vendor/se/x8/proprietary/lib/libmmjpeg.so:system/lib/libmmjpeg.so \
    vendor/se/x8/proprietary/lib/libmmipl.so:system/lib/libmmipl.so

## FM & BT
PRODUCT_COPY_FILES += \
    device/se/x8/modules/sdio.ko:system/lib/modules/sdio.ko \
    device/se/x8/modules/tiwlan_drv.ko:system/lib/modules/tiwlan_drv.ko \
    vendor/se/x8/proprietary/etc/firmware/fm_rx_init_1273.1.bts:system/etc/firmware/fm_rx_init_1273.1.bts \
    vendor/se/x8/proprietary/etc/firmware/fm_rx_init_1273.2.bts:system/etc/firmware/fm_rx_init_1273.2.bts \
    vendor/se/x8/proprietary/etc/firmware/fm_tx_init_1273.1.bts:system/etc/firmware/fm_tx_init_1273.1.bts \
    vendor/se/x8/proprietary/etc/firmware/fm_tx_init_1273.2.bts:system/etc/firmware/fm_tx_init_1273.2.bts \
    vendor/se/x8/proprietary/etc/firmware/fmc_init_1273.1.bts:system/etc/firmware/fmc_init_1273.1.bts \
    vendor/se/x8/proprietary/etc/firmware/fmc_init_1273.2.bts:system/etc/firmware/fmc_init_1273.2.bts \
    vendor/se/x8/proprietary/etc/firmware/TIInit_7.2.31.bts:system/etc/firmware/TIInit_7.2.31.bts \
    vendor/se/x8/proprietary/etc/tiwlan_firmware.bin:system/etc/wifi/wl1271.bin
#    vendor/se/x8/proprietary/bin/tiwlan_cu:system/bin/tiwlan_cu \
#    vendor/se/x8/proprietary/etc/tiwlan.ini:system/etc/wifi/tiwlan.ini \
#    vendor/se/x8/proprietary/bin/tiwlan_loader:system/bin/tiwlan_loader \


## Other libraries and proprietary binaries
PRODUCT_COPY_FILES += \
    vendor/se/x8/proprietary/bin/akmd2:system/bin/akmd2 \
    vendor/se/x8/proprietary/bin/dhcpcd:system/bin/dhcpcd \
    vendor/se/x8/proprietary/bin/nvimport:system/bin/nvimport \
    vendor/se/x8/proprietary/bin/hciattach:system/bin/hciattach

#xrecovery
PRODUCT_COPY_FILES += \
    device/se/x8/prebuilt/chargemon:system/bin/chargemon \
    device/se/x8/prebuilt/ramdisk.tar:system/bin/ramdisk.tar \
    device/se/x8/prebuilt/sh:system/recovery/sh \
    device/se/x8/prebuilt/recovery.tar.bz2:system/recovery/recovery.tar.bz2 
#    device/se/x8/prebuilt/busybox:system/bin/busybox \

#offline charger
PRODUCT_COPY_FILES += \
    vendor/se/x8/proprietary/bin/chargemon:system/bin/charger \
    vendor/se/x8/proprietary/bin/updatemiscta:system/bin/updatemiscta \
    vendor/se/x8/proprietary/bin/semc_chargalg:system/bin/semc_chargalg \
    vendor/se/x8/proprietary/lib/libmiscta.so:system/lib/libmiscta.so \
    vendor/se/x8/proprietary/etc/semc/chargemon/anim1.rle:system/etc/semc/chargemon/anim1.rle \
    vendor/se/x8/proprietary/etc/semc/chargemon/anim2.rle:system/etc/semc/chargemon/anim2.rle \
    vendor/se/x8/proprietary/etc/semc/chargemon/anim3.rle:system/etc/semc/chargemon/anim3.rle \
    vendor/se/x8/proprietary/etc/semc/chargemon/anim4.rle:system/etc/semc/chargemon/anim4.rle \
    vendor/se/x8/proprietary/etc/semc/chargemon/anim5.rle:system/etc/semc/chargemon/anim5.rle \
    vendor/se/x8/proprietary/etc/semc/chargemon/anim6.rle:system/etc/semc/chargemon/anim6.rle \
    vendor/se/x8/proprietary/etc/semc/chargemon/anim7.rle:system/etc/semc/chargemon/anim7.rle \
    vendor/se/x8/proprietary/etc/semc/chargemon/anim8.rle:system/etc/semc/chargemon/anim8.rle 

#audio
PRODUCT_COPY_FILES += \
    vendor/se/x8/proprietary/lib/libaudioeq.so:system/lib/libaudioeq.so \
    vendor/se/x8/proprietary/etc/AudioFilterProduct.csv:system/etc/AudioFilterProduct.csv \
    vendor/se/x8/proprietary/etc/AudioFilterPlatform.csv:system/etc/AudioFilterPlatform.csv 
#    vendor/se/x8/proprietary/lib/libaudio.so:system/lib/libaudio.so 
#    device/se/x8/prebuilt/AudioFilter.csv:system/etc/AudioFilter.csv \
#    device/se/x8/prebuilt/AutoVolumeControl.txt:system/etc/AutoVolumeControl.txt\

#2D & 3D
PRODUCT_COPY_FILES += \
    vendor/se/x8/proprietary/lib/libgsl.so:system/lib/libgsl.so \
    vendor/se/x8/proprietary/lib/egl/egl.cfg:system/lib/egl/egl.cfg \
    vendor/se/x8/proprietary/lib/egl/libGLESv1_CM_adreno200.so:system/lib/egl/libGLESv1_CM_adreno200.so \
    vendor/se/x8/proprietary/lib/egl/libq3dtools_adreno200.so:system/lib/egl/libq3dtools_adreno200.so \
    vendor/se/x8/proprietary/lib/egl/libEGL_adreno200.so:system/lib/egl/libEGL_adreno200.so \
    vendor/se/x8/proprietary/lib/egl/libGLESv2_adreno200.so:system/lib/egl/libGLESv2_adreno200.so

#layout config
PRODUCT_COPY_FILES += \
    device/se/x8/media_profiles.xml:system/etc/media_profiles.xml \
    device/se/x8/dhcpcd.conf:system/etc/dhcpcd/dhcpcd.conf \
    device/se/x8/vold.fstab:system/etc/vold.fstab \
    device/se/x8/prebuilt/hw_config.sh:system/etc/hw_config.sh \
    vendor/se/x8/proprietary/etc/sensors.conf:system/etc/sensors.conf \
    vendor/se/x8/proprietary/usr/keylayout/shakira_keypad.kl:system/usr/keylayout/shakira_keypad.kl \
    vendor/se/x8/proprietary/usr/keylayout/systemconnector.kl:system/usr/keylayout/systemconnector.kl \
    vendor/se/x8/proprietary/usr/keychars/shakira_keypad.kcm.bin:system/usr/keychars/shakira_keypad.kcm.bin \
    vendor/se/x8/proprietary/usr/keychars/systemconnector.kcm.bin:system/usr/keychars/systemconnector.kcm.bin \
    vendor/se/x8/proprietary/etc/loc_parameter.ini:system/etc/loc_parameter.ini 
    
#Kernel modules
PRODUCT_COPY_FILES += \
    device/se/x8/prebuilt/hosts:system/etc/hosts \
    device/se/x8/prebuilt/dm-mod.ko:system/lib/modules/dm-mod.ko \
    device/se/x8/prebuilt/dm-crypt.ko:system/lib/modules/dm-crypt.ko \
    device/se/x8/prebuilt/twofish.ko:system/lib/modules/twofish.ko \
    device/se/x8/prebuilt/twofish_common.ko:system/lib/modules/twofish_common.ko 

#crappy headset
PRODUCT_COPY_FILES += \
    vendor/se/x8/proprietary/lib/hw/hal_seport.default.so:system/lib/hw/hal_seport.shakira.so \
    device/se/x8/prebuilt/SystemConnector.apk:system/app/SystemConnector.apk \
    vendor/se/x8/proprietary/lib/libsystemconnector/libuinputdevicejni.so:system/lib/libsystemconnector/libuinputdevicejni.so \
    vendor/se/x8/proprietary/lib/libsystemconnector_hal_jni.so:system/lib/libsystemconnector_hal_jni.so

#Boot Animation
PRODUCT_COPY_FILES += \
    device/se/x8/prebuilt/bootanimation.zip:system/media/bootanimation.zip 


$(call inherit-product, build/target/product/full_base.mk)

PRODUCT_BUILD_PROP_OVERRIDES += BUILD_UTC_DATE=0
PRODUCT_NAME := x8
PRODUCT_DEVICE := x8
PRODUCT_MODEL := SonyEricsson X8

#CYANOGEN_WITH_GOOGLE := true