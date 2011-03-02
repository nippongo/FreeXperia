#!/system/bin/busybox sh

# Vibrator configuration
dev=/sys/devices/platform/msm_pmic_vibrator
echo 2400 > $dev/voltage_mv

# lm3530 LMU configuration
dev=/sys/devices/platform/i2c-adapter/i2c-0/0-0036
echo linear > $dev/br::mapping  # linear exp
echo 32768 > $dev/br::rate::up   # 8, 1024, 2048, 4096, 8192, 16384, 32768, 65538
echo 32768 > $dev/br::rate::down # 8, 1024, 2048, 4096, 8192, 16384, 32768, 65538
echo 226 > $dev/br::limit       # 0 - 255
echo 22.5 > $dev/br::fsc        # 5.0, 8.5, 12.0, 15.5, 19.0, 22.5, 26.0, 29.5
echo 4,16,64,250 > $dev/curve::borders
echo 47,75,114,164,226 > $dev/curve::targets
echo high-z > $dev/als::r1      # high-z, 9360, 5560 .. 677.6 (see chip mnual)
echo high-z > $dev/als::r2      # high-z, 9360, 5560 .. 677.6 (see chip mnual)
echo 1024 > $dev/als::avg-t     # 32, 63, 128, 256, 512, 1024, 2048, 4096
echo i2c_pwm_als > $dev/mode    # i2c, pwm, i2c_pwm, als, pwm_als, i2c_pwm_als, i2_als

# Proximity sensor configuration
dev=/sys/devices/platform/proximity-sensor/semc/proximity-sensor
echo  15 > $dev/led_on_ms         # sensor LED on time in ms
echo  35 > $dev/led_off_ms       # sensor LED off time in ms

echo "ondemand" > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
echo 90 > /sys/devices/system/cpu/cpu0/cpufreq/ondemand/up_threshold
echo 30 > /sys/devices/system/cpu/cpu0/cpufreq/ondemand/down_differential
echo 500000 > /sys/devices/system/cpu/cpu0/cpufreq/ondemand/sampling_rate
echo 245760 > /sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq

# mising modules
insmod /system/lib/modules/dm-mod.ko
insmod /system/lib/modules/dm-crypt.ko
insmod /system/lib/modules/twofish_common.ko
insmod /system/lib/modules/twofish.ko
insmod /system/lib/modules/tun.ko


# Needed by radio
mkdir /data/radio
chmod 0777 /data/radio

chmod 777 /dev/msm*
chmod 777 /dev/pmem_adsp
chmod 777 /dev/msm_camera/*
chmod 777 /dev/graphics/*
chmod 777 /dev/oncrpc/*

#sleep 60
#if [ ! -e /data/boot_complete ]; then
#    echo >/data/boot_complete
#    killall servicemanager
#fi

