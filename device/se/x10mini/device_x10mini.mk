## This device is the X10mini
DEVICE=x10mini

$(call inherit-product-if-exists, vendor/se/$DEVICE/x10mini-vendor.mk)

DEVICE_PACKAGE_OVERLAYS += device/se/x10mini/overlay

PRODUCT_PACKAGES += \
    librs_jni \
    sensors.x10mini \
    lights.x10mini 

# Passion uses high-density artwork where available
PRODUCT_LOCALES += mdpi

# proprietary side of the device
$(call inherit-product-if-exists, vendor/se/x10mini/device_x10mini-vendor.mk)

PRODUCT_COPY_FILES += \
    vendor/se/x10mini/proprietary/lib/libgps.so:obj/lib/libgps.so \
    vendor/se/x10mini/proprietary/lib/libcamera.so:obj/lib/libcamera.so

PRODUCT_COPY_FILES += \
    device/se/x10mini/prebuilt/atfwd-daemon:system/bin/atfwd-daemon \
    device/se/x10mini/prebuilt/hciattach:system/bin/hciattach \
    device/se/x10mini/prebuilt/netcfg:system/bin/netcfg \
    device/se/x10mini/prebuilt/qmuxd:system/bin/qmuxd \
    device/se/x10mini/prebuilt/rild:system/bin/rild \
    device/se/x10mini/prebuilt/tiwlan_loader:system/bin/tiwlan_loader \
    device/se/x10mini/prebuilt/verify_odex:system/bin/verify_odex \
    device/se/x10mini/prebuilt/wpa_supplicant:system/bin/wpa_supplicant \
    device/se/x10mini/prebuilt/ramdisk.tar:system/bin/ramdisk.tar \
    device/se/x10mini/prebuilt/xrecovery.tar:system/bin/xrecovery.tar \
    device/se/x10mini/prebuilt/chargemon:system/bin/chargemon \
    device/se/x10mini/prebuilt/busybox:system/bin/busybox \
    device/se/x10mini/prebuilt/charger:system/bin/charger \
    vendor/se/x10mini/proprietary/bin/akmd2:system/bin/akmd2 \
    vendor/se/x10mini/proprietary/bin/mm-venc-omx-test:system/bin/mm-venc-omx-test \
    vendor/se/x10mini/proprietary/etc/01_qcomm_omx.cfg:system/etc/01_qcomm_omx.cfg \
    vendor/se/x10mini/proprietary/etc/firmware/TIInit_7.2.31.bts:system/etc/firmware/TIInit_7.2.31.bts \
    vendor/se/x10mini/proprietary/etc/firmware/fm_rx_init_1273.1.bts:system/etc/firmware/fm_rx_init_1273.1.bts \
    vendor/se/x10mini/proprietary/etc/firmware/fm_rx_init_1273.2.bts:system/etc/firmware/fm_rx_init_1273.2.bts \
    vendor/se/x10mini/proprietary/etc/firmware/fm_tx_init_1273.1.bts:system/etc/firmware/fm_tx_init_1273.1.bts \
    vendor/se/x10mini/proprietary/etc/firmware/fm_tx_init_1273.2.bts:system/etc/firmware/fm_tx_init_1273.2.bts \
    vendor/se/x10mini/proprietary/etc/firmware/fmc_init_1273.1.bts:system/etc/firmware/fmc_init_1273.1.bts \
    vendor/se/x10mini/proprietary/etc/firmware/fmc_init_1273.2.bts:system/etc/firmware/fmc_init_1273.2.bts \
    vendor/se/x10mini/proprietary/etc/firmware/yamato_pfp.fw:system/etc/firmware/yamato_pfp.fw \
    vendor/se/x10mini/proprietary/etc/firmware/yamato_pm4.fw:system/etc/firmware/yamato_pm4.fw \
    vendor/se/x10mini/proprietary/lib/egl/libEGL_adreno200.so:system/lib/egl/libEGL_adreno200.so \
    vendor/se/x10mini/proprietary/lib/egl/libGLESv1_CM_adreno200.so:system/lib/egl/libGLESv1_CM_adreno200.so \
    vendor/se/x10mini/proprietary/lib/egl/libGLESv2_adreno200.so:system/lib/egl/libGLESv2_adreno200.so \
    vendor/se/x10mini/proprietary/lib/egl/libq3dtools_adreno200.so:system/lib/egl/libq3dtools_adreno200.so \
    vendor/se/x10mini/proprietary/lib/libcamera.so:system/lib/libcamera.so \
    vendor/se/x10mini/proprietary/lib/libgps.so:system/lib/libgps.so \
    vendor/se/x10mini/proprietary/lib/libgsl.so:system/lib/libgsl.so \
    vendor/se/x10mini/proprietary/lib/libril-qc-1.so:system/lib/libril-qc-1.so \
    vendor/se/x10mini/proprietary/lib/liboemcamera.so:system/lib/liboemcamera.so \
    vendor/se/x10mini/proprietary/lib/libfm_stack.so:system/lib/libfm_stack.so \
    vendor/se/x10mini/proprietary/lib/hw/gralloc.msm7k.so:system/lib/hw/gralloc.msm7k.so \
    vendor/se/x10mini/proprietary/lib/libaudioeq.so:system/lib/libaudioeq.so \
    vendor/se/x10mini/proprietary/lib/libmm-adspsvc.so:system/lib/libmm-adspsvc.so \
    vendor/se/x10mini/proprietary/lib/libOmxCore.so:system/lib/libOmxCore.so \
    vendor/se/x10mini/proprietary/lib/libOmxH264Dec.so:system/lib/libOmxH264Dec.so \
    vendor/se/x10mini/proprietary/lib/libOmxMpeg4Dec.so:system/lib/libOmxMpeg4Dec.so \
    vendor/se/x10mini/proprietary/lib/libOmxVidEnc.so:system/lib/libOmxVidEnc.so \
    vendor/se/x10mini/proprietary/etc/semc/chargemon/anim1.rle:system/etc/semc/chargemon/anim1.rle \
    vendor/se/x10mini/proprietary/etc/semc/chargemon/anim2.rle:system/etc/semc/chargemon/anim2.rle \
    vendor/se/x10mini/proprietary/etc/semc/chargemon/anim3.rle:system/etc/semc/chargemon/anim3.rle \
    vendor/se/x10mini/proprietary/etc/semc/chargemon/anim4.rle:system/etc/semc/chargemon/anim4.rle \
    vendor/se/x10mini/proprietary/etc/semc/chargemon/anim5.rle:system/etc/semc/chargemon/anim5.rle \
    vendor/se/x10mini/proprietary/etc/semc/chargemon/anim6.rle:system/etc/semc/chargemon/anim6.rle \
    vendor/se/x10mini/proprietary/etc/semc/chargemon/anim7.rle:system/etc/semc/chargemon/anim7.rle \
    vendor/se/x10mini/proprietary/etc/semc/chargemon/anim8.rle:system/etc/semc/chargemon/anim8.rle 
# Kernel modules
PRODUCT_COPY_FILES += \
    vendor/se/x10mini/proprietary/lib/modules/tiwlan_drv.ko:system/lib/modules/tiwlan_drv.ko \
    vendor/se/x10mini/proprietary/lib/modules/sdio.ko:system/lib/modules/sdio.ko \
    device/se/x10mini/prebuilt/jbd.ko:system/lib/modules/jbd.ko \
    device/se/x10mini/prebuilt/ext3.ko:system/lib/modules/ext3.ko \
    device/se/x10mini/prebuilt/jbd2.ko:system/lib/modules/jbd2.ko \
    device/se/x10mini/prebuilt/ext4.ko:system/lib/modules/ext4.ko \
    device/se/x10mini/prebuilt/dm-mod.ko:system/lib/modules/dm-mod.ko \
    device/se/x10mini/prebuilt/dm-crypt.ko:system/lib/modules/dm-crypt.ko \
    device/se/x10mini/prebuilt/twofish.ko:system/lib/modules/twofish.ko \
    device/se/x10mini/prebuilt/twofish_common.ko:system/lib/modules/twofish_common.ko 

PRODUCT_COPY_FILES += \
    vendor/se/x10mini/proprietary/bin/drmdbbackup:/system/bin/drmdbbackup \
    vendor/se/x10mini/proprietary/bin/nvimport:/system/bin/nvimport \
    vendor/se/x10mini/proprietary/bin/semc_chargalg:/system/bin/semc_chargalg \
    vendor/se/x10mini/proprietary/bin/suntrolkac:/system/bin/suntrolkac \
    vendor/se/x10mini/proprietary/bin/DxDrmServerIpc:/system/bin/DxDrmServerIpc \
    vendor/se/x10mini/proprietary/drm/DxDrm/DxDrmConfig.txt:/system/drm/DxDrm/DxDrmConfig.txt \
    vendor/se/x10mini/proprietary/drm/DxDrm/DxDrmConfig_Server.txt:/system/drm/DxDrm/DxDrmConfig_Server.txt \
    vendor/se/x10mini/proprietary/drm/DxDrm/init_drm.rc:/system/drm/DxDrm/init_drm.rc \
    vendor/se/x10mini/proprietary/etc/AudioFilterPlatform.csv:/system/etc/AudioFilterPlatform.csv \
    vendor/se/x10mini/proprietary/etc/AudioFilterProduct.csv:/system/etc/AudioFilterProduct.csv \
    vendor/se/x10mini/proprietary/etc/hw_config.sh:/system/etc/hw_config.sh \
    vendor/se/x10mini/proprietary/lib/libhardware_legacy.so:/system/lib/libhardware_legacy.so

PRODUCT_COPY_FILES += \
    vendor/se/x10mini/proprietary/lib/hw/lights.default.so:/system/lib/hw/lights.default.so \
    vendor/se/x10mini/proprietary/lib/hw/gralloc.default.so:/system/lib/hw/gralloc.default.so \
    vendor/se/x10mini/proprietary/lib/hw/copybit.msm7k.so:/system/lib/hw/copybit.msm7k.so \
    vendor/se/x10mini/proprietary/lib/hw/sensors.default.so:/system/lib/hw/sensors.default.so \
    vendor/se/x10mini/proprietary/lib/hw/hal_seport.default.so:/system/lib/hw/hal_seport.default.so \
    vendor/se/x10mini/proprietary/lib/libvdmengine.so:/system/lib/libvdmengine.so \
    vendor/se/x10mini/proprietary/lib/libloc.so:/system/lib/libloc.so \
    vendor/se/x10mini/proprietary/lib/libmmgsdilib.so:/system/lib/libmmgsdilib.so \
    vendor/se/x10mini/proprietary/lib/libqmi.so:/system/lib/libqmi.so \
    vendor/se/x10mini/proprietary/lib/libril.so:/system/lib/libril.so \
    vendor/se/x10mini/proprietary/lib/libloc-rpc.so:/system/lib/libloc-rpc.so \
    vendor/se/x10mini/proprietary/lib/libsystemconnector/libuinputdevicejni.so:/system/lib/libsystemconnector/libuinputdevicejni.so \
    vendor/se/x10mini/proprietary/lib/libwms.so:/system/lib/libwms.so \
    vendor/se/x10mini/proprietary/lib/libfuse.so:/system/lib/libfuse.so \
    vendor/se/x10mini/proprietary/lib/libpbmlib.so:/system/lib/libpbmlib.so \
    vendor/se/x10mini/proprietary/lib/libqueue.so:/system/lib/libqueue.so \
    vendor/se/x10mini/proprietary/lib/liboncrpc.so:/system/lib/liboncrpc.so \
    vendor/se/x10mini/proprietary/lib/libmiscta.so:/system/lib/libmiscta.so \
    vendor/se/x10mini/proprietary/lib/libfacedetect.so:/system/lib/libfacedetect.so \
    vendor/se/x10mini/proprietary/lib/libsystemconnector_hal_jni.so:/system/lib/libsystemconnector_hal_jni.so \
    vendor/se/x10mini/proprietary/lib/libgsdi_exp.so:/system/lib/libgsdi_exp.so \
    vendor/se/x10mini/proprietary/lib/libauth.so:/system/lib/libauth.so \
    vendor/se/x10mini/proprietary/lib/libskiagl.so:/system/lib/libskiagl.so \
    vendor/se/x10mini/proprietary/lib/libmmjpeg.so:/system/lib/libmmjpeg.so \
    vendor/se/x10mini/proprietary/lib/libwpa_client.so:/system/lib/libwpa_client.so \
    vendor/se/x10mini/proprietary/lib/libaudio.so:/system/lib/libaudio.so \
    vendor/se/x10mini/proprietary/lib/libreference-ril.so:/system/lib/libreference-ril.so \
    vendor/se/x10mini/proprietary/lib/libmmipl.so:/system/lib/libmmipl.so \
    vendor/se/x10mini/proprietary/lib/libdiag.so:/system/lib/libdiag.so \
    vendor/se/x10mini/proprietary/lib/libcm.so:/system/lib/libcm.so \
    vendor/se/x10mini/proprietary/lib/libnv.so:/system/lib/libnv.so \
    vendor/se/x10mini/proprietary/lib/libvdmfumo.so:/system/lib/libvdmfumo.so \
    vendor/se/x10mini/proprietary/lib/libZiEngine.so:/system/lib/libZiEngine.so \
    vendor/se/x10mini/proprietary/lib/libgstk_exp.so:/system/lib/libgstk_exp.so \
    vendor/se/x10mini/proprietary/lib/libfacedetectjnitest.so:/system/lib/libfacedetectjnitest.so \
    vendor/se/x10mini/proprietary/lib/libcommondefs.so:/system/lib/libcommondefs.so \
    vendor/se/x10mini/proprietary/lib/libwmsts.so:/system/lib/libwmsts.so \
    vendor/se/x10mini/proprietary/lib/libdll.so:/system/lib/libdll.so \
    vendor/se/x10mini/proprietary/lib/libdsm.so:/system/lib/libdsm.so \
    vendor/se/x10mini/proprietary/lib/libdss.so:/system/lib/libdss.so

PRODUCT_COPY_FILES += \
    frameworks/base/data/etc/handheld_core_hardware.xml:system/etc/permissions/handheld_core_hardware.xml \
    frameworks/base/data/etc/android.hardware.camera.autofocus.xml:system/etc/permissions/android.hardware.camera.autofocus.xml \
    frameworks/base/data/etc/android.hardware.telephony.gsm.xml:system/etc/permissions/android.hardware.telephony.gsm.xml \
    frameworks/base/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml \
    frameworks/base/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
    frameworks/base/data/etc/android.hardware.touchscreen.multitouch.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.xml

# x10mini specific gps.conf
PRODUCT_COPY_FILES += \
    device/se/x10mini/gps.conf:system/etc/gps.conf

PRODUCT_PROPERTY_OVERRIDES += \
    ro.media.dec.jpeg.memcap=10000000

PRODUCT_PROPERTY_OVERRIDES += \
    rild.libpath=/system/lib/libril-qc-1.so \
    rild.libargs=-d /dev/smd0 \
    ro.ril.hsxpa=2 \
    ro.ril.gprsclass=10 \
    wifi.interface=wlan0

# Time between scans in seconds. Keep it high to minimize battery drain.
# This only affects the case in which there are remembered access points,
# but none are in range.
#
PRODUCT_PROPERTY_OVERRIDES += \
    wifi.supplicant_scan_interval=45

PRODUCT_PROPERTY_OVERRIDES += \
    ro.ril.def.agps.mode=2 \
    ro.ril.def.agps.feature=1

# density in DPI of the LCD of this board. This is used to scale the UI
# appropriately. If this property is not defined, the default value is 160 dpi. 
PRODUCT_PROPERTY_OVERRIDES += \
    ro.sf.lcd_density=120

# Default network type
# 0 => WCDMA Preferred.
PRODUCT_PROPERTY_OVERRIDES += \
    ro.telephony.default_network=0

# media configuration xml file
PRODUCT_COPY_FILES += \
    device/se/x10mini/media_profiles.xml:/system/etc/media_profiles.xml

# stuff common to all HTC phones
#$(call inherit-product, device/se/common/common.mk)

$(call inherit-product, build/target/product/full.mk)

