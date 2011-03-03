#!/bin/sh

DEVICE=x10mini
# If you don't have the device connected but you have it's /system available pickit up from there
LOCAL_PROPR_DIR=/data/android/xrecovery/backup/2011-02-06.18.11.31
mkdir -p ../../../vendor/se/$DEVICE/proprietary

DIRS="
bin
drm
etc/firmware
etc/semc/chargemon
lib/egl
lib/hw
lib/libsystemconnector
lib/modules
"

for DIR in $DIRS; do
	mkdir -p ../../../vendor/se/$DEVICE/proprietary/$DIR
done


FILES="
bin/akmd2
bin/mm-venc-omx-test
bin/drmdbbackup
bin/nvimport
bin/semc_chargalg
bin/suntrolkac
bin/DxDrmServerIpc

etc/01_qcomm_omx.cfg
etc/firmware/TIInit_7.2.31.bts
etc/firmware/fm_rx_init_1273.1.bts
etc/firmware/fm_rx_init_1273.2.bts
etc/firmware/fm_tx_init_1273.1.bts
etc/firmware/fm_tx_init_1273.2.bts
etc/firmware/fmc_init_1273.1.bts
etc/firmware/fmc_init_1273.2.bts
etc/firmware/yamato_pfp.fw
etc/firmware/yamato_pm4.fw
etc/AudioFilterPlatform.csv
etc/AudioFilterProduct.csv
etc/hw_config.sh

lib/egl/libEGL_adreno200.so
lib/egl/libGLESv1_CM_adreno200.so
lib/egl/libGLESv2_adreno200.so
lib/egl/libq3dtools_adreno200.so

lib/hw/gralloc.msm7k.so
lib/hw/lights.default.so
lib/hw/gralloc.default.so
lib/hw/copybit.msm7k.so
lib/hw/sensors.default.so
lib/hw/hal_seport.default.so

lib/libsystemconnector/libuinputdevicejni.so

lib/libcamera.so
lib/libgps.so
lib/libgsl.so
lib/libril-qc-1.so
lib/liboemcamera.so
lib/libfm_stack.so
lib/libaudioeq.so
lib/libmm-adspsvc.so
lib/liboemcamera.so
lib/libOmxCore.so
lib/libOmxH264Dec.so
lib/libOmxMpeg4Dec.so
lib/libOmxVidEnc.so
lib/libOmxWmaDec.so
lib/libhardware_legacy.so
lib/libvdmengine.so
lib/libloc.so
lib/libmmgsdilib.so
lib/libqmi.so
lib/libril.so
lib/libloc-rpc.so
lib/libwms.so
lib/libfuse.so
lib/libpbmlib.so
lib/libqueue.so
lib/liboncrpc.so
lib/libmiscta.so
lib/libfacedetect.so
lib/libsystemconnector_hal_jni.so
lib/libgsdi_exp.so
lib/libauth.so
lib/libskiagl.so
lib/libmmjpeg.so
lib/libwpa_client.so
lib/libaudio.so
lib/libreference-ril.so
lib/libmmipl.so
lib/libdiag.so
lib/libcm.so
lib/libnv.so
lib/libvdmfumo.so
lib/libZiEngine.so
lib/libgstk_exp.so
lib/libfacedetectjnitest.so
lib/libcommondefs.so
lib/libwmsts.so
lib/libdll.so
lib/libdsm.so
lib/libdss.so

lib/modules/tiwlan_drv.ko
lib/modules/sdio.ko

etc/semc/chargemon/anim1.rle
etc/semc/chargemon/anim2.rle
etc/semc/chargemon/anim3.rle
etc/semc/chargemon/anim4.rle
etc/semc/chargemon/anim5.rle
etc/semc/chargemon/anim6.rle
etc/semc/chargemon/anim7.rle
etc/semc/chargemon/anim8.rle
"

if [ $# = 0 ]
then
	echo "Pulling from device..."
	for FILE in $FILES; do
		adb pull system/$FILE ../../../vendor/se/$DEVICE/proprietary/$FILE
	done
else
	echo "Copying from $LOCAL_PROPR_DIR"
		for FILE in $FILES; do
		cp -pr $LOCAL_PROPR_DIR/system/$FILE ../../../vendor/se/$DEVICE/proprietary/$FILE
	done
fi

