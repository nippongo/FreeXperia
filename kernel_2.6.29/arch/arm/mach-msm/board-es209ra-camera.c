#include <mach/camera.h>
#include <linux/gpio.h>
#include "board-es209ra.h"

#ifdef CONFIG_MSM_CAMERA
static uint32_t camera_off_gpio_table[] = {
	/* parallel CAMERA interfaces */
	GPIO_CFG(0,  0, GPIO_OUTPUT, GPIO_PULL_DOWN, GPIO_2MA),/* DAT0 */
	GPIO_CFG(1,  0, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* DAT1 */
	GPIO_CFG(4,  0, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* DAT4 */
	GPIO_CFG(5,  0, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* DAT5 */
	GPIO_CFG(6,  0, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* DAT6 */
	GPIO_CFG(7,  0, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* DAT7 */
	GPIO_CFG(8,  0, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* DAT8 */
	GPIO_CFG(9,  0, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* DAT9 */
	GPIO_CFG(10, 0, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* DAT10 */
	GPIO_CFG(11, 0, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* DAT11 */
	GPIO_CFG(12, 0, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* PCLK */
	GPIO_CFG(13, 0, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* HSYNC_IN */
	GPIO_CFG(14, 0, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* VSYNC_IN */
	GPIO_CFG(15, 0, GPIO_OUTPUT, GPIO_PULL_DOWN, GPIO_2MA), /* MCLK */
	GPIO_CFG(27, 0, GPIO_OUTPUT, GPIO_PULL_DOWN, GPIO_2MA), /* VCAML2_EN */
	GPIO_CFG(43, 0, GPIO_OUTPUT, GPIO_PULL_DOWN, GPIO_2MA), /* VCAML_EN */
	GPIO_CFG(142,0, GPIO_OUTPUT, GPIO_PULL_DOWN, GPIO_2MA), /* VCAMSD_EN */
};

static uint32_t camera_on_gpio_table[] = {
	/* parallel CAMERA interfaces */
	GPIO_CFG(0,  0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), /* DAT0 */
	GPIO_CFG(1,  0, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* DAT1 */
	GPIO_CFG(4,  1, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* DAT4 */
	GPIO_CFG(5,  1, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* DAT5 */
	GPIO_CFG(6,  1, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* DAT6 */
	GPIO_CFG(7,  1, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* DAT7 */
	GPIO_CFG(8,  1, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* DAT8 */
	GPIO_CFG(9,  1, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* DAT9 */
	GPIO_CFG(10, 1, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* DAT10 */
	GPIO_CFG(11, 1, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* DAT11 */
	GPIO_CFG(12, 1, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* PCLK */
	GPIO_CFG(13, 1, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* HSYNC_IN */
	GPIO_CFG(14, 1, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), /* VSYNC_IN */
	GPIO_CFG(15, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), /* MCLK */
	GPIO_CFG(27, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), /* VCAML2_EN */
	GPIO_CFG(43, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), /* VCAML_EN */
	GPIO_CFG(142, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), /* VCAMSD_EN */
};

static void config_gpio_table(uint32_t *table, int len)
{
	int n, rc;
	for (n = 0; n < len; n++) {
		rc = gpio_tlmm_config(table[n], GPIO_ENABLE);
		if (rc) {
			printk(KERN_ERR "%s: gpio_tlmm_config(%#x)=%d\n",
				__func__, table[n], rc);
			break;
		}
	}
}

static void config_camera_on_gpios(void)
{
	config_gpio_table(camera_on_gpio_table,
		ARRAY_SIZE(camera_on_gpio_table));
}

static void config_camera_off_gpios(void)
{
	config_gpio_table(camera_off_gpio_table,
		ARRAY_SIZE(camera_off_gpio_table));
}

static struct resource msm_camera_resources[] = {
	{
		.start	= MSM_VFE_PHYS, //MSM_PMEM_CAMERA_BASE,
		.end	= MSM_VFE_PHYS + MSM_VFE_SIZE -1 ,//MSM_PMEM_CAMERA_BASE + MSM_PMEM_CAMERA_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= INT_VFE,
		.end	= INT_VFE,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct msm_camera_device_platform_data msm_camera_device_data = {
	.camera_gpio_on  = config_camera_on_gpios,
	.camera_gpio_off = config_camera_off_gpios,
	.ioext.mdcphy = MSM_MDC_PHYS,
	.ioext.mdcsz  = MSM_MDC_SIZE,
	.ioext.appphy = MSM_CLK_CTL_PHYS,
	.ioext.appsz  = MSM_CLK_CTL_SIZE,
};

static struct msm_camera_sensor_flash_src msm_flash_src = {
	.flash_sr_type = MSM_CAMERA_FLASH_SRC_PMIC,
	._fsrc.pmic_src.low_current  = 30,
	._fsrc.pmic_src.high_current = 100,
};


static struct msm_camera_sensor_flash_data flash_semc_imx046_camera = {
	.flash_type = MSM_CAMERA_FLASH_NONE,
	.flash_src  = &msm_flash_src
};

static struct msm_camera_sensor_info msm_camera_sensor_semc_imx046_camera_data = {
	.sensor_name    = "semc_imx046_camera",
	.sensor_reset   = 0,
	.sensor_pwd     = 0,
	.vcm_pwd        = 0,
	.pdata          = &msm_camera_device_data,
	.resource       = msm_camera_resources,
	.num_resources  = ARRAY_SIZE(msm_camera_resources),
	.flash_data     = &flash_semc_imx046_camera,
	.mclk           = 15,
	.sensor_int     = INT_ES209RA_GPIO_CAM_ISP,
	.sensor_vsync   = INT_ES209RA_GPIO_CAM_VSYNC,
	.vcam_l1        = {.type = MSM_CAMERA_SENSOR_PWR_GPIO, .resource.number = 27,       },
	.vcam_l2        = {.type = MSM_CAMERA_SENSOR_PWR_GPIO, .resource.number = 43,       },
	.vcam_sd        = {.type = MSM_CAMERA_SENSOR_PWR_GPIO, .resource.number = 142,      },
	.vcam_io        = {.type = MSM_CAMERA_SENSOR_PWR_VREG, .resource.name   = "rfrx2",  },
	.vcam_af        = {.type = MSM_CAMERA_SENSOR_PWR_VREG, .resource.name   = "rftx",   },
	.vcam_sa        = {.type = MSM_CAMERA_SENSOR_PWR_VREG, .resource.name   = "gp1",    },
};

static struct platform_device msm_camera_sensor_semc_imx046_camera = {
	.name      = "msm_camera_semc_imx046_camera",
	.dev       = {
		.platform_data = &msm_camera_sensor_semc_imx046_camera_data,
	},
};



static int __init es209ra_init_camera(void)
{
    int ret=0;
    platform_device_register(&msm_camera_sensor_semc_imx046_camera);
    config_camera_off_gpios(); /* might not be necessary */
    return ret;
}

device_initcall(es209ra_init_camera);
#endif

