#!/bin/sh
cd ramdisk
find . | cpio -o -H newc | gzip> ../ramdisk.gz
cd ..
./mkbootimg --cmdline 'mtdparts=msm_nand:0x00440000@0x3fbc0000(appslog),0x06f40000@0x38c80000(cache),0x160a0000@0x05ae0000(system),0x1d100000@0x1bb80000(userdata) g_android.product_id=0xE12E console=ttyMSM0 reset_devices' --kernel Image --ramdisk ramdisk.gz --base 0x20000000 -o boot.img
