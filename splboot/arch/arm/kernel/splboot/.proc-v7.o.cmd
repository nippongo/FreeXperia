cmd_/home/user/xperia/kernel/arch/arm/kernel/splboot/proc-v7.o := arm-linux-gcc -Wp,-MD,/home/user/xperia/kernel/arch/arm/kernel/splboot/.proc-v7.o.d  -nostdinc -isystem /home/user/bin/arm-2009q1/bin/../lib/gcc/arm-none-linux-gnueabi/4.3.3/include -Iinclude  -I/home/user/xperia/X10_Donut_100325_01/esheep_linux/LINUX/android/kernel/arch/arm/include -include include/linux/autoconf.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-msm/include -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -D__LINUX_ARM_ARCH__=7 -march=armv7-a  -msoft-float -gdwarf-2    -DMODULE -c -o /home/user/xperia/kernel/arch/arm/kernel/splboot/proc-v7.o /home/user/xperia/kernel/arch/arm/kernel/splboot/proc-v7.S

deps_/home/user/xperia/kernel/arch/arm/kernel/splboot/proc-v7.o := \
  /home/user/xperia/kernel/arch/arm/kernel/splboot/proc-v7.S \
    $(wildcard include/config/smp.h) \
    $(wildcard include/config/mmu.h) \
    $(wildcard include/config/emulate/domain/manager/v7.h) \
    $(wildcard include/config/arch/msm/scorpion.h) \
    $(wildcard include/config/arch/msm7x30.h) \
    $(wildcard include/config/cpu/cache/err/report.h) \
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
  /home/user/xperia/X10_Donut_100325_01/esheep_linux/LINUX/android/kernel/arch/arm/include/asm/hwcap.h \
  /home/user/xperia/X10_Donut_100325_01/esheep_linux/LINUX/android/kernel/arch/arm/include/asm/domain.h \
    $(wildcard include/config/io/36.h) \
  include/asm/asm-offsets.h \
  /home/user/xperia/X10_Donut_100325_01/esheep_linux/LINUX/android/kernel/arch/arm/include/asm/pgtable-hwdef.h \
  /home/user/xperia/X10_Donut_100325_01/esheep_linux/LINUX/android/kernel/arch/arm/include/asm/pgtable.h \
  include/asm-generic/4level-fixup.h \
  /home/user/xperia/X10_Donut_100325_01/esheep_linux/LINUX/android/kernel/arch/arm/include/asm/proc-fns.h \
    $(wildcard include/config/cpu/32.h) \
    $(wildcard include/config/cpu/arm610.h) \
    $(wildcard include/config/cpu/arm7tdmi.h) \
    $(wildcard include/config/cpu/arm710.h) \
    $(wildcard include/config/cpu/arm720t.h) \
    $(wildcard include/config/cpu/arm740t.h) \
    $(wildcard include/config/cpu/arm9tdmi.h) \
    $(wildcard include/config/cpu/arm920t.h) \
    $(wildcard include/config/cpu/arm922t.h) \
    $(wildcard include/config/cpu/arm925t.h) \
    $(wildcard include/config/cpu/arm926t.h) \
    $(wildcard include/config/cpu/arm940t.h) \
    $(wildcard include/config/cpu/arm946e.h) \
    $(wildcard include/config/cpu/sa110.h) \
    $(wildcard include/config/cpu/sa1100.h) \
    $(wildcard include/config/cpu/arm1020.h) \
    $(wildcard include/config/cpu/arm1020e.h) \
    $(wildcard include/config/cpu/arm1022.h) \
    $(wildcard include/config/cpu/arm1026.h) \
    $(wildcard include/config/cpu/xscale.h) \
    $(wildcard include/config/cpu/xsc3.h) \
    $(wildcard include/config/cpu/v6.h) \
    $(wildcard include/config/cpu/v7.h) \
  /home/user/xperia/X10_Donut_100325_01/esheep_linux/LINUX/android/kernel/arch/arm/include/asm/memory.h \
    $(wildcard include/config/page/offset.h) \
    $(wildcard include/config/dram/size.h) \
    $(wildcard include/config/dram/base.h) \
    $(wildcard include/config/zone/dma.h) \
    $(wildcard include/config/discontigmem.h) \
    $(wildcard include/config/sparsemem.h) \
  include/linux/const.h \
  arch/arm/mach-msm/include/mach/memory.h \
    $(wildcard include/config/msm/stacked/memory.h) \
    $(wildcard include/config/arch/msm/arm11.h) \
    $(wildcard include/config/cache/l2x0.h) \
  /home/user/xperia/X10_Donut_100325_01/esheep_linux/LINUX/android/kernel/arch/arm/include/asm/sizes.h \
  include/asm-generic/memory_model.h \
    $(wildcard include/config/flatmem.h) \
    $(wildcard include/config/sparsemem/vmemmap.h) \
  arch/arm/mach-msm/include/mach/vmalloc.h \
    $(wildcard include/config/vmsplit/2g.h) \
  /home/user/xperia/kernel/arch/arm/kernel/splboot/proc-macros.S \
    $(wildcard include/config/cpu/dcache/writethrough.h) \
  /home/user/xperia/X10_Donut_100325_01/esheep_linux/LINUX/android/kernel/arch/arm/include/asm/thread_info.h \
    $(wildcard include/config/arm/thumbee.h) \
  /home/user/xperia/X10_Donut_100325_01/esheep_linux/LINUX/android/kernel/arch/arm/include/asm/fpstate.h \
    $(wildcard include/config/vfpv3.h) \
    $(wildcard include/config/iwmmxt.h) \

/home/user/xperia/kernel/arch/arm/kernel/splboot/proc-v7.o: $(deps_/home/user/xperia/kernel/arch/arm/kernel/splboot/proc-v7.o)

$(deps_/home/user/xperia/kernel/arch/arm/kernel/splboot/proc-v7.o):
