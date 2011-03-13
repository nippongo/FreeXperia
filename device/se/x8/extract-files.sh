#!/bin/sh

mkdir -p ../../../vendor/se/x8/proprietary

DIRS="
bin
etc/firmware
lib/egl
lib/hw
"

for DIR in $DIRS; do
	mkdir -p ../../../vendor/se/x8/proprietary/$DIR
done

FILES="
bin/akmd2
bin/dhcpcd
bin/hciattach
bin/nvimport
bin/port-bridge
bin/qmuxd
bin/rild
bin/semc_chargalg
bin/tiwlan_cu
bin/tiwlan_loader
bin/updatemiscta
bin/wpa_supplicant

etc/firmware/fm_rx_init_1273.1.bts
etc/firmware/fm_rx_init_1273.2.bts
etc/firmware/fm_tx_init_1273.1.bts
etc/firmware/fm_tx_init_1273.2.bts
etc/firmware/fmc_init_1273.1.bts
etc/firmware/fmc_init_1273.2.bts
etc/firmware/TIInit_7.2.31.bts
etc/firmware/yamato_pfp.fw
etc/firmware/yamato_pm4.fw
etc/firmware/cyttspfw.hex

etc/permissions/com.ti.fm.fmreceiverif.xml

etc/semc/chargemon/anim1.rle
etc/semc/chargemon/anim2.rle
etc/semc/chargemon/anim3.rle
etc/semc/chargemon/anim4.rle
etc/semc/chargemon/anim5.rle
etc/semc/chargemon/anim6.rle
etc/semc/chargemon/anim7.rle
etc/semc/chargemon/anim8.rle
etc/wifi/wpa_supplicant.conf
etc/tiwlan_firmware.bin
etc/sensors.conf
etc/vold.conf
etc/AudioFilterPlatform.csv
etc/AudioFilterProduct.csv
etc/loc_parameter.ini
etc/tiwlan.ini

lib/egl/egl.cfg
lib/egl/libEGL_adreno200.so
lib/egl/libGLESv1_CM_adreno200.so
lib/egl/libGLESv2_adreno200.so
lib/egl/libq3dtools_adreno200.so

lib/hw/gralloc.msm7k.so
lib/hw/hal_seport.default.so
lib/hw/lights.default.so
lib/hw/sensors.default.so

lib/modules/sdio.ko
lib/modules/tiwlan_drv.ko

lib/libad2p.so
lib/libaudio.so
lib/libaudioeq.so
lib/libauth.so
lib/libcm.so
lib/libdiag.so
lib/libdll.so
lib/libdsm.so
lib/libdss.so
lib/libfm_stack.so
lib/libfmrx.so
lib/libgsdi_exp.so
lib/libgsl.so
lib/libgstk_exp.so
lib/libmiscta.so
lib/libmm-adspsvc.so
lib/libmmgsdilib.so
lib/libmmipl.so
lib/libmmjpeg.so
lib/libmvs.so
lib/libnv.so
lib/liboem_rapi.so
lib/liboemcamera.so
lib/libOmxAacDec.so
lib/libOmxAacEnc.so
lib/libOmxAmrDec.so
lib/libOmxAmrEnc.so
lib/libOmxAmrRtpDec.so
lib/libOmxAmrwbDec.so
lib/libOmxEvrcDec.so
lib/libOmxEvrcEnc.so
lib/libOmxH264Dec.so
lib/libOmxMp3Dec.so
lib/libOmxMpeg4Dec.so
lib/libOmxQcelp13Enc.so
lib/libOmxQcelpDec.so
lib/libOmxVidEnc.so
lib/libOmxWmaDec.so
lib/libOmxWmvDec.so
lib/liboncrpc.so
lib/libpbmlib.so
lib/libqmi.so
lib/libqueue.so
lib/libril.so
lib/libril-qc-1.so
lib/libsystemconnector/libuinputdevicejni.so
lib/libsystemconnector_hal_jni.so
lib/libwms.so
lib/libwmsts.so
"

for FILE in $FILES; do
	adb pull system/$FILE ../../../vendor/se/x8/proprietary/$FILE
done

