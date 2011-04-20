#!/bin/sh
cd ramdisk
find . | cpio -o -H newc | gzip > ../ramdisk.gz
cd ..
./mkbootimg --cmdline 'androidboot.hardware=delta vmalloc=215M g_android.product_id=0x3149 console=ttyMSM0 semcandroidboot.startup=0x00000078' --kernel zImage --ramdisk ramdisk.gz --base 0x20000000 -o boot.img
