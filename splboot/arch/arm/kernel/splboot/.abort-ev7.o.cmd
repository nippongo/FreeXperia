cmd_/home/user/xperia/kernel/arch/arm/kernel/splboot/abort-ev7.o := arm-linux-gcc -Wp,-MD,/home/user/xperia/kernel/arch/arm/kernel/splboot/.abort-ev7.o.d  -nostdinc -isystem /home/user/bin/arm-2009q1/bin/../lib/gcc/arm-none-linux-gnueabi/4.3.3/include -Iinclude  -I/home/user/xperia/X10_Donut_100325_01/esheep_linux/LINUX/android/kernel/arch/arm/include -include include/linux/autoconf.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-msm/include -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -D__LINUX_ARM_ARCH__=7 -march=armv7-a  -msoft-float -gdwarf-2    -DMODULE -c -o /home/user/xperia/kernel/arch/arm/kernel/splboot/abort-ev7.o /home/user/xperia/kernel/arch/arm/kernel/splboot/abort-ev7.S

deps_/home/user/xperia/kernel/arch/arm/kernel/splboot/abort-ev7.o := \
  /home/user/xperia/kernel/arch/arm/kernel/splboot/abort-ev7.S \
    $(wildcard include/config/verify/permission/fault.h) \
  include/linux/linkage.h \
  include/linux/compiler.h \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  /home/user/xperia/X10_Donut_100325_01/esheep_linux/LINUX/android/kernel/arch/arm/include/asm/linkage.h \
  /home/user/xperia/X10_Donut_100325_01/esheep_linux/LINUX/android/kernel/arch/arm/include/asm/assembler.h \
    $(wildcard include/config/cpu/feroceon.h) \
  /home/user/xperia/X10_Donut_100325_01/esheep_linux/LINUX/android/kernel/arch/arm/include/asm/ptrace.h \
    $(wildcard include/config/arm/thumb.h) \
    $(wildcard include/config/smp.h) \
  /home/user/xperia/X10_Donut_100325_01/esheep_linux/LINUX/android/kernel/arch/arm/include/asm/hwcap.h \

/home/user/xperia/kernel/arch/arm/kernel/splboot/abort-ev7.o: $(deps_/home/user/xperia/kernel/arch/arm/kernel/splboot/abort-ev7.o)

$(deps_/home/user/xperia/kernel/arch/arm/kernel/splboot/abort-ev7.o):
