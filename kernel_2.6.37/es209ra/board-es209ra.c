#include <linux/kernel.h>
#include <linux/irq.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/android_pmem.h>
#include <linux/bootmem.h>
#include <linux/i2c.h>
#include <linux/spi/spi.h>
#include <linux/delay.h>
#include <linux/mfd/tps65023.h>
#include <linux/power_supply.h>
#include <linux/clk.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/io.h>
#include <asm/setup.h>
#include <asm/mach/mmc.h>
#include <mach/vreg.h>
#include <mach/mpp.h>
#include <mach/gpio.h>
#include <mach/board.h>
#include <mach/sirc.h>
#include <mach/dma.h>
#include <mach/rpc_hsusb.h>
#include <mach/rpc_pmapp.h>
#include <mach/msm_hsusb.h>
#include <mach/msm_hsusb_hw.h>
#include <mach/msm_serial_hs.h>
#include <mach/pmic.h>
#include <mach/camera.h>
#include <mach/memory.h>
#include <mach/msm_spi.h>
#include <mach/msm_tsif.h>
#include <linux/max17040.h>
#include <linux/akm8973.h>
#include "devices.h"
#include "timer.h"
#include "socinfo.h"
#include "msm-keypad-devices.h"
#include "pm.h"
#include "smd_private.h"
#include "proc_comm.h"
#include <linux/msm_kgsl.h>
#include <linux/usb/android.h>
#include "board-es209ra.h"
#include "board-es209ra-keypad.h"
#include "es209ra_headset.h"
#include <linux/spi/es209ra_touch.h>
#include <asm/setup.h>
#include  <linux/semc/msm_pmic_vibrator.h>
#include "q6audio.h"

#define TOUCHPAD_SUSPEND 	34
#define TOUCHPAD_IRQ 		38


#define SMEM_SPINLOCK_I2C	"S:6"

#define MSM_AUDIO_SIZE		    0x00080000
#define MSM_PMEM_SMIPOOL_BASE	0x03200000
#define MSM_PMEM_SMIPOOL_SIZE	0x00E00000
#define PMEM_KERNEL_EBI1_SIZE	0x28000



static int msm7227_platform_set_vib_voltage(u16 volt_mv)
{
	int rc = pmic_vib_mot_set_volt(volt_mv);

	if (rc)
		printk(KERN_ERR "%s: Failed to set motor voltage\n", __func__);
	return rc;
}

static int msm7227_platform_init_vib_hw(void)
{
	int rc = pmic_vib_mot_set_mode(PM_VIB_MOT_MODE__MANUAL);

	if (rc) {
		printk(KERN_ERR "%s: Failed to set pin mode\n", __func__);
		return rc;
	}
	return pmic_vib_mot_set_volt(0);
}

static struct msm_pmic_vibrator_platform_data vibrator_platform_data = {
	.min_voltage = 1200,
	.max_voltage = 2500,
	.off_voltage = 0,
	.default_voltage = 2500,
	.mimimal_on_time = 10,
	.platform_set_vib_voltage = msm7227_platform_set_vib_voltage,
	.platform_init_vib_hw = msm7227_platform_init_vib_hw,
};
static struct platform_device vibrator_device = {
	.name = "msm_pmic_vibrator",
	.id = -1,
	.dev = {
		.platform_data = &vibrator_platform_data,
	},
};

static struct resource ram_console_resources[] = {
	{
		.start	= MSM_RAM_CONSOLE_BASE,
		.end	= MSM_RAM_CONSOLE_BASE + MSM_RAM_CONSOLE_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	},
};

static struct platform_device ram_console_device = {
	.name		= "ram_console",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(ram_console_resources),
	.resource	= ram_console_resources,
};



/* dynamic composition */
static struct usb_composition usb_func_composition[] = {
	{
		/* MSC( + CDROM) */
		.product_id	= 0x312E,
		.functions	= 0xD,
		/* MSC( + CDROM) + ADB */
		.adb_product_id	= 0x212E,
		.adb_functions	= 0x1D,
		/* DIAG + ADB + MODEM + NMEA + MSC( + CDROM) */
		.eng_product_id	= 0x2146,
		.eng_functions	= 0xD7614,
	},
	{
		/* MSC */
		.product_id	= 0xE12E,
		.functions	= 0x02,
		/* MSC + ADB */
		.adb_product_id	= 0x612E,
		.adb_functions	= 0x12,
		/* MSC + ADB + MODEM + NMEA + DIAG */
		.eng_product_id	= 0x6146,
		.eng_functions	= 0x47612,
	},
	{
		/* ADB+MSC+ECM */
		.product_id	= 0x3146,
		.functions	= 0x821,
		.adb_product_id	= 0x3146,
		.adb_functions	= 0x821,
	},
};
static struct usb_mass_storage_lun_config msc_lun_config = {
	.is_cdrom	= false,
	.shift_size	= 9,
	.can_stall	= true,
	.vendor		= "SEMC",
	.product	= "Mass Storage",
	.release	= 0x0001,
};
static struct usb_mass_storage_lun_config cdrom_lun_config = {
	.is_cdrom	= true,
	.shift_size	= 11,
	.can_stall	= false,
	.vendor		= "SEMC",
	.product	= "CD-ROM",
	.release	= 0x0001,
};
static struct usb_mass_storage_lun_config msc_cdrom_lun_config[] = {
	{
		.is_cdrom	= false,
		.shift_size	= 9,
		.can_stall	= true,
		.vendor		= "SEMC",
		.product	= "Mass Storage",
		.release	= 0x0001,
	},
	{
		.is_cdrom	= true,
		.shift_size	= 11,
		.can_stall	= false,
		.vendor		= "SEMC",
		.product	= "CD-ROM",
		.release	= 0x0001,
	},
};
static struct android_usb_platform_data android_usb_pdata = {
	.vendor_id		= 0x0FCE,
	.version		= 0x0100,
	.serial_number		= "1234567890ABCDEF",
	.compositions		= usb_func_composition,
	.num_compositions	= ARRAY_SIZE(usb_func_composition),
	.product_name		= "SEMC HSUSB Device",
	.manufacturer_name	= "SEMC",
	.nluns			= 1,
	.cdrom_lun_conf		= &cdrom_lun_config,
	.msc_lun_conf		= &msc_lun_config,
	.msc_cdrom_lun_conf	= msc_cdrom_lun_config,
};
static struct platform_device android_usb_device = {
	.name	= "android_usb",
	.id		= -1,
	.dev		= {
		.platform_data = &android_usb_pdata,
	},
};

 
static struct platform_device hs_device = {
	.name   = "msm-handset",
	.id     = -1,
	.dev    = {
		.platform_data = "8k_handset",
	},
};


#define MSM_USB_BASE              ((unsigned)addr)
static unsigned ulpi_read(void __iomem *addr, unsigned reg)
{
	unsigned timeout = 100000;

	/* initiate read operation */
	writel(ULPI_RUN | ULPI_READ | ULPI_ADDR(reg),
	       USB_ULPI_VIEWPORT);

	/* wait for completion */
	while ((readl(USB_ULPI_VIEWPORT) & ULPI_RUN) && (--timeout))
		cpu_relax();

	if (timeout == 0) {
		printk(KERN_ERR "ulpi_read: timeout %08x\n",
			readl(USB_ULPI_VIEWPORT));
		return 0xffffffff;
	}
	return ULPI_DATA_READ(readl(USB_ULPI_VIEWPORT));
}

static int ulpi_write(void __iomem *addr, unsigned val, unsigned reg)
{
	unsigned timeout = 10000;

	/* initiate write operation */
	writel(ULPI_RUN | ULPI_WRITE |
	       ULPI_ADDR(reg) | ULPI_DATA(val),
	       USB_ULPI_VIEWPORT);

	/* wait for completion */
	while ((readl(USB_ULPI_VIEWPORT) & ULPI_RUN) && (--timeout))
		cpu_relax();

	if (timeout == 0) {
		printk(KERN_ERR "ulpi_write: timeout\n");
		return -1;
	}

	return 0;
}

struct clk *hs_clk, *phy_clk;
#define CLKRGM_APPS_RESET_USBH      37
#define CLKRGM_APPS_RESET_USB_PHY   34
static void msm_hsusb_apps_reset_link(int reset)
{
	if (reset)
		clk_reset(hs_clk, CLK_RESET_ASSERT);
	else
		clk_reset(hs_clk, CLK_RESET_DEASSERT);
}

static void msm_hsusb_apps_reset_phy(void)
{
	clk_reset(phy_clk, CLK_RESET_ASSERT);
	msleep(1);
	clk_reset(phy_clk, CLK_RESET_DEASSERT);
}

#define ULPI_VERIFY_MAX_LOOP_COUNT  3
static int msm_hsusb_phy_verify_access(void __iomem *addr)
{
	int temp;

	for (temp = 0; temp < ULPI_VERIFY_MAX_LOOP_COUNT; temp++) {
		if (ulpi_read(addr, ULPI_DEBUG) != (unsigned)-1)
			break;
		msm_hsusb_apps_reset_phy();
	}

	if (temp == ULPI_VERIFY_MAX_LOOP_COUNT) {
		pr_err("%s: ulpi read failed for %d times\n",
				__func__, ULPI_VERIFY_MAX_LOOP_COUNT);
		return -1;
	}

	return 0;
}

static unsigned msm_hsusb_ulpi_read_with_reset(void __iomem *addr, unsigned reg)
{
	int temp;
	unsigned res;

	for (temp = 0; temp < ULPI_VERIFY_MAX_LOOP_COUNT; temp++) {
		res = ulpi_read(addr, reg);
		if (res != -1)
			return res;
		msm_hsusb_apps_reset_phy();
	}

	pr_err("%s: ulpi read failed for %d times\n",
			__func__, ULPI_VERIFY_MAX_LOOP_COUNT);

	return -1;
}

static int msm_hsusb_ulpi_write_with_reset(void __iomem *addr,
		unsigned val, unsigned reg)
{
	int temp;
	int res;

	for (temp = 0; temp < ULPI_VERIFY_MAX_LOOP_COUNT; temp++) {
		res = ulpi_write(addr, val, reg);
		if (!res)
			return 0;
		msm_hsusb_apps_reset_phy();
	}

	pr_err("%s: ulpi write failed for %d times\n",
			__func__, ULPI_VERIFY_MAX_LOOP_COUNT);
	return -1;
}

static int msm_hsusb_phy_caliberate(void __iomem *addr)
{
	int ret;
	unsigned res;

	ret = msm_hsusb_phy_verify_access(addr);
	if (ret)
		return -ETIMEDOUT;

	res = msm_hsusb_ulpi_read_with_reset(addr, ULPI_FUNC_CTRL_CLR);
	if (res == -1)
		return -ETIMEDOUT;

	res = msm_hsusb_ulpi_write_with_reset(addr,
			res | ULPI_SUSPENDM,
			ULPI_FUNC_CTRL_CLR);
	if (res)
		return -ETIMEDOUT;

	msm_hsusb_apps_reset_phy();

	return msm_hsusb_phy_verify_access(addr);
}

#define USB_LINK_RESET_TIMEOUT      (msecs_to_jiffies(10))
static int msm_hsusb_native_phy_reset(void __iomem *addr)
{
	u32 temp;
	unsigned long timeout;

	msm_hsusb_apps_reset_link(1);
	msm_hsusb_apps_reset_phy();
	msm_hsusb_apps_reset_link(0);

	/* select ULPI phy */
	temp = (readl(USB_PORTSC) & ~PORTSC_PTS);
	writel(temp | PORTSC_PTS_ULPI, USB_PORTSC);

	if (msm_hsusb_phy_caliberate(addr))
		return -1;

	/* soft reset phy */
	writel(USBCMD_RESET, USB_USBCMD);
	timeout = jiffies + USB_LINK_RESET_TIMEOUT;
	while (readl(USB_USBCMD) & USBCMD_RESET) {
		if (time_after(jiffies, timeout)) {
			pr_err("usb link reset timeout\n");
			break;
		}
		msleep(1);
	}

	return 0;
}

static struct msm_hsusb_platform_data msm_hsusb_pdata = {
};

static struct android_pmem_platform_data android_pmem_kernel_ebi1_pdata = {
	.name = PMEM_KERNEL_EBI1_DATA_NAME,
	/* if no allocator_type, defaults to PMEM_ALLOCATORTYPE_BITMAP,
	 * the only valid choice at this time. The board structure is
	 * set to all zeros by the C runtime initialization and that is now
	 * the enum value of PMEM_ALLOCATORTYPE_BITMAP, now forced to 0 in
	 * include/linux/android_pmem.h.
	 */
	.cached = 0,
};

static struct android_pmem_platform_data android_pmem_pdata = {
	.name = "pmem",
	.allocator_type = PMEM_ALLOCATORTYPE_BITMAP,
	.cached = 1,
};

static struct android_pmem_platform_data android_pmem_adsp_pdata = {
	.name = "pmem_adsp",
	.allocator_type = PMEM_ALLOCATORTYPE_BITMAP,
	.cached = 0,
};

static struct android_pmem_platform_data android_pmem_smipool_pdata = {
	.name = "pmem_smipool",
	.start = MSM_PMEM_SMIPOOL_BASE,
	.size = MSM_PMEM_SMIPOOL_SIZE,
	.allocator_type = PMEM_ALLOCATORTYPE_BITMAP,
	.cached = 0,
};

static struct platform_device android_pmem_device = {
	.name = "android_pmem",
	.id = 0,
	.dev = { .platform_data = &android_pmem_pdata },
};

static struct platform_device android_pmem_adsp_device = {
	.name = "android_pmem",
	.id = 1,
	.dev = { .platform_data = &android_pmem_adsp_pdata },
};

static struct platform_device android_pmem_smipool_device = {
	.name = "android_pmem",
	.id = 2,
	.dev = { .platform_data = &android_pmem_smipool_pdata },
};

static struct platform_device android_pmem_kernel_ebi1_device = {
	.name = "android_pmem",
	.id = 3,
	.dev = { .platform_data = &android_pmem_kernel_ebi1_pdata },
};


static struct resource msm_fb_resources[] = {
	{
		.flags  = IORESOURCE_DMA,
	}
};

static int msm_fb_detect_panel(const char *name)
{
	int ret = -EPERM;
		ret = 0;
	return ret;
}

static struct msm_fb_platform_data msm_fb_pdata = {
	.detect_client = msm_fb_detect_panel,
};

static struct platform_device msm_fb_device = {
	.name   = "msm_fb",
	.id     = 0,
	.num_resources  = ARRAY_SIZE(msm_fb_resources),
	.resource       = msm_fb_resources,
	.dev    = {
		.platform_data = &msm_fb_pdata,
	}
};

static struct resource qsd_spi_resources[] = {
	{
		.name   = "spi_irq_in",
		.start	= INT_SPI_INPUT,
		.end	= INT_SPI_INPUT,
		.flags	= IORESOURCE_IRQ,
	},
	{
		.name   = "spi_irq_out",
		.start	= INT_SPI_OUTPUT,
		.end	= INT_SPI_OUTPUT,
		.flags	= IORESOURCE_IRQ,
	},
	{
		.name   = "spi_irq_err",
		.start	= INT_SPI_ERROR,
		.end	= INT_SPI_ERROR,
		.flags	= IORESOURCE_IRQ,
	},
	{
		.name   = "spi_base",
		.start	= 0xA1200000,
		.end	= 0xA1200000 + SZ_4K - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.name   = "spidm_channels",
		.flags  = IORESOURCE_DMA,
	},
	{
		.name   = "spidm_crci",
		.flags  = IORESOURCE_DMA,
	},
};

static struct platform_device qsd_device_spi = {
	.name	        = "spi_qsd",
	.id	        = 0,
	.num_resources	= ARRAY_SIZE(qsd_spi_resources),
	.resource	= qsd_spi_resources,
};

static struct es209ra_touch_platform_data es209ra_touch_data = {
	.gpio_irq_pin		= 37,
	.gpio_reset_pin		= 30,
	.x_min			= 0,
	.x_max			= 480,
	.y_min			= 0,
	.y_max			= 854,
};

static struct spi_board_info msm_spi_board_info[] __initdata = {
	{
		.modalias	= "es209ra_touch",
		.mode		= SPI_MODE_0,
		.irq		= INT_ES209RA_GPIO_TOUCHPAD,
		.platform_data  = &es209ra_touch_data,
		.bus_num	= 0,
		.chip_select	= 0,
		.max_speed_hz	= 1000000,
	}
};

#define CT_CSR_PHYS		0xA8700000
#define TCSR_SPI_MUX		(ct_csr_base + 0x54)
static int msm_qsd_spi_dma_config(void)
{
	void __iomem *ct_csr_base = 0;
	u32 spi_mux;
	int ret = 0;

	ct_csr_base = ioremap(CT_CSR_PHYS, PAGE_SIZE);
	if (!ct_csr_base) {
		pr_err("%s: Could not remap %x\n", __func__, CT_CSR_PHYS);
		return -1;
	}

	spi_mux = readl(TCSR_SPI_MUX);
	switch (spi_mux) {
	case (1):
		qsd_spi_resources[4].start  = DMOV_HSUART1_RX_CHAN;
		qsd_spi_resources[4].end    = DMOV_HSUART1_TX_CHAN;
		qsd_spi_resources[5].start  = DMOV_HSUART1_RX_CRCI;
		qsd_spi_resources[5].end    = DMOV_HSUART1_TX_CRCI;
		break;
	case (2):
		qsd_spi_resources[4].start  = DMOV_HSUART2_RX_CHAN;
		qsd_spi_resources[4].end    = DMOV_HSUART2_TX_CHAN;
		qsd_spi_resources[5].start  = DMOV_HSUART2_RX_CRCI;
		qsd_spi_resources[5].end    = DMOV_HSUART2_TX_CRCI;
		break;
	case (3):
		qsd_spi_resources[4].start  = DMOV_CE_OUT_CHAN;
		qsd_spi_resources[4].end    = DMOV_CE_IN_CHAN;
		qsd_spi_resources[5].start  = DMOV_CE_OUT_CRCI;
		qsd_spi_resources[5].end    = DMOV_CE_IN_CRCI;
		break;
	default:
		ret = -1;
	}

	iounmap(ct_csr_base);
	return ret;
}

static struct msm_gpio qsd_spi_gpio_config_data[] = {
	{ GPIO_CFG(17, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_16MA), "spi_clk" },
	{ GPIO_CFG(18, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), "spi_mosi" },
	{ GPIO_CFG(19, 1, GPIO_INPUT, GPIO_PULL_UP, GPIO_2MA), "spi_miso" },
	{ GPIO_CFG(20, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), "spi_cs0" },
};

static int msm_qsd_spi_gpio_config(void)
{
	int rc;

	rc = msm_gpios_request_enable(qsd_spi_gpio_config_data,
		ARRAY_SIZE(qsd_spi_gpio_config_data));
	if (rc)
		return rc;

	return 0;
}

static void msm_qsd_spi_gpio_release(void)
{
	msm_gpios_disable_free(qsd_spi_gpio_config_data,
		ARRAY_SIZE(qsd_spi_gpio_config_data));
}

static struct msm_spi_platform_data qsd_spi_pdata = {
	.max_clock_speed = 19200000,
	.gpio_config  = msm_qsd_spi_gpio_config,
	.gpio_release = msm_qsd_spi_gpio_release,
	.dma_config = msm_qsd_spi_dma_config,
};

static void __init msm_qsd_spi_init(void)
{
	qsd_device_spi.dev.platform_data = &qsd_spi_pdata;
}


static struct resource msm_audio_resources[] = {
	{
		.flags  = IORESOURCE_DMA,
	},
	{
		.name   = "aux_pcm_dout",
		.start  = 68,
		.end    = 68,
		.flags  = IORESOURCE_IO,
	},
	{
		.name   = "aux_pcm_din",
		.start  = 69,
		.end    = 69,
		.flags  = IORESOURCE_IO,
	},
	{
		.name   = "aux_pcm_syncout",
		.start  = 70,
		.end    = 70,
		.flags  = IORESOURCE_IO,
	},
	{
		.name   = "aux_pcm_clkin_a",
		.start  = 71,
		.end    = 71,
		.flags  = IORESOURCE_IO,
	},
	{
		.name	= "audio_base_addr",
		.start	= 0xa0700000,
		.end	= 0xa0700000 + 4,
		.flags	= IORESOURCE_MEM,
	},
};

static unsigned audio_gpio_on[] = {
	GPIO_CFG(68, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA),	/* PCM_DOUT */
	GPIO_CFG(69, 1, GPIO_INPUT,  GPIO_NO_PULL, GPIO_2MA),	/* PCM_DIN */
	GPIO_CFG(70, 2, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA),	/* PCM_SYNC */
	GPIO_CFG(71, 2, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA),	/* PCM_CLK */

};

static void __init audio_gpio_init(void)
{
	int pin, rc;

	for (pin = 0; pin < ARRAY_SIZE(audio_gpio_on); pin++) {
		rc = gpio_tlmm_config(audio_gpio_on[pin],
			GPIO_ENABLE);
		if (rc) {
			printk(KERN_ERR
				"%s: gpio_tlmm_config(%#x)=%d\n",
				__func__, audio_gpio_on[pin], rc);
			return;
		}
	}
	set_audio_gpios(msm_audio_resources[1].start);
}

static struct platform_device msm_audio_device = {
	.name   = "msm_audio",
	.id     = 0,
	.num_resources  = ARRAY_SIZE(msm_audio_resources),
	.resource       = msm_audio_resources,
};

static struct resource bluesleep_resources[] = {
	{
		.name	= "gpio_host_wake",
		.start	= 21,
		.end	= 21,
		.flags	= IORESOURCE_IO,
	},
	{
		.name	= "gpio_ext_wake",
		.start	= 29,
		.end	= 29,
		.flags	= IORESOURCE_IO,
	},
	{
		.name	= "host_wake",
		.start	= INT_ES209RA_GPIO_BT_HOSTWAKE,
		.end	= INT_ES209RA_GPIO_BT_HOSTWAKE,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device msm_bluesleep_device = {
	.name = "bluesleep",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(bluesleep_resources),
	.resource	= bluesleep_resources,
};


static struct platform_device msm_bt_power_device = {
	.name = "bt_power",
};

enum {
	BT_SYSRST,
	BT_WAKE,
	BT_HOST_WAKE,
	BT_VDD_IO,
	BT_RFR,
	BT_CTS,
	BT_RX,
	BT_TX,
	BT_VDD_FREG
};

static struct msm_gpio bt_config_power_on[] = {
	{ GPIO_CFG(29, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA),	"BT_WAKE" },
	{ GPIO_CFG(21, 0, GPIO_INPUT,  GPIO_NO_PULL, GPIO_2MA),	"HOST_WAKE" },
	{ GPIO_CFG(77, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), "BT_VDD_IO" },
	{ GPIO_CFG(157, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), "UART1DM_RFR" },
	{ GPIO_CFG(141, 1, GPIO_INPUT,  GPIO_NO_PULL, GPIO_2MA), "UART1DM_CTS" },
	{ GPIO_CFG(139, 1, GPIO_INPUT,  GPIO_NO_PULL, GPIO_2MA), "UART1DM_RX" },
	{ GPIO_CFG(140, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), "UART1DM_TX" }
};
#if 0
static unsigned bt_config_power_off[] = {
	GPIO_CFG(29, 0, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA),	/* WAKE */
	GPIO_CFG(21, 0, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA),	/* HOST_WAKE */
	GPIO_CFG(77, 0, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA),	/* PWR_EN */
	GPIO_CFG(157, 0, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA),	/* RFR */
	GPIO_CFG(141, 0, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA),	/* CTS */
	GPIO_CFG(139, 0, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA),	/* Rx */
	GPIO_CFG(140, 0, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA),	/* Tx */
};
#endif
/* SEMC:LC: Update for Phase3.2 end */ 

#if 1
static int bluetooth_power(int on)
{
	printk(KERN_DEBUG "Bluetooth power switch: %d\n", on);

	gpio_set_value(77, on); /* PWR_EN */

	return 0;
}

static void __init bt_power_init(void)
{
	int pin, rc;

	for (pin = 0; pin < ARRAY_SIZE(bt_config_power_on); pin++) {
		rc = gpio_tlmm_config(bt_config_power_on[pin].gpio_cfg,
				      GPIO_ENABLE);
		if (rc) {
			printk(KERN_ERR
			       "%s: gpio_tlmm_config(%#x)=%d\n",
			       __func__, bt_config_power_on[pin].gpio_cfg, rc);
			return;
		}
	}
	gpio_set_value(77, 0); /* PWR_EN */

	msm_bt_power_device.dev.platform_data = &bluetooth_power;
	printk(KERN_DEBUG "Bluetooth power switch initialized\n");
}
#else
static int bluetooth_power(int on)
{
	struct vreg *vreg_bt;
	struct vreg *vreg_wlan;
	int pin, rc;

	/* do not have vreg bt defined, gp6 is the same */
	/* vreg_get parameter 1 (struct device *) is ignored */
	vreg_bt = vreg_get(NULL, "gp6");

	if (IS_ERR(vreg_bt)) {
		printk(KERN_ERR "%s: vreg get failed (%ld)\n",
		       __func__, PTR_ERR(vreg_bt));
		return PTR_ERR(vreg_bt);
	}

	vreg_wlan = vreg_get(NULL, "wlan");

	if (IS_ERR(vreg_wlan)) {
		printk(KERN_ERR "%s: vreg get failed (%ld)\n",
		       __func__, PTR_ERR(vreg_wlan));
		return PTR_ERR(vreg_wlan);
	}

	if (on) {
		for (pin = 0; pin < ARRAY_SIZE(bt_config_power_on); pin++) {
			rc = gpio_tlmm_config(bt_config_power_on[pin],
					      GPIO_ENABLE);
			if (rc) {
				printk(KERN_ERR
				       "%s: gpio_tlmm_config(%#x)=%d\n",
				       __func__, bt_config_power_on[pin], rc);
				return -EIO;
			}
		}

		/* units of mV, steps of 50 mV */
		rc = vreg_set_level(vreg_bt, PMIC_VREG_GP6_LEVEL);
		if (rc) {
			printk(KERN_ERR "%s: vreg bt set level failed (%d)\n",
			       __func__, rc);
			return -EIO;
		}
		rc = vreg_enable(vreg_bt);
		if (rc) {
			printk(KERN_ERR "%s: vreg bt enable failed (%d)\n",
			       __func__, rc);
			return -EIO;
		}

		/* units of mV, steps of 50 mV */
		rc = vreg_set_level(vreg_wlan, PMIC_VREG_WLAN_LEVEL);
		if (rc) {
			printk(KERN_ERR "%s: vreg wlan set level failed (%d)\n",
			       __func__, rc);
			return -EIO;
		}
		rc = vreg_enable(vreg_wlan);
		if (rc) {
			printk(KERN_ERR "%s: vreg wlan enable failed (%d)\n",
			       __func__, rc);
			return -EIO;
		}

		gpio_set_value(22, on); /* VDD_IO */
		gpio_set_value(18, on); /* SYSRST */

	} else {
		gpio_set_value(18, on); /* SYSRST */
		gpio_set_value(22, on); /* VDD_IO */

		rc = vreg_disable(vreg_wlan);
		if (rc) {
			printk(KERN_ERR "%s: vreg wlan disable failed (%d)\n",
			       __func__, rc);
			return -EIO;
		}
		rc = vreg_disable(vreg_bt);
		if (rc) {
			printk(KERN_ERR "%s: vreg bt disable failed (%d)\n",
			       __func__, rc);
			return -EIO;
		}

		for (pin = 0; pin < ARRAY_SIZE(bt_config_power_off); pin++) {
			rc = gpio_tlmm_config(bt_config_power_off[pin],
					      GPIO_ENABLE);
			if (rc) {
				printk(KERN_ERR
				       "%s: gpio_tlmm_config(%#x)=%d\n",
				       __func__, bt_config_power_off[pin], rc);
				return -EIO;
			}
		}
	}

	printk(KERN_DEBUG "Bluetooth power switch: %d\n", on);

	return 0;
}

static void __init bt_power_init(void)
{
	msm_bt_power_device.dev.platform_data = &bluetooth_power;
}
#endif


static struct resource kgsl_resources[] = {
       {
		.name  = "kgsl_reg_memory",
		.start = 0xA0000000,
		.end = 0xA001ffff,
		.flags = IORESOURCE_MEM,
       },
       {
		.name   = "kgsl_phys_memory",
		.start = MSM_GPU_MEM_BASE,
		.end = MSM_GPU_MEM_BASE + MSM_GPU_MEM_SIZE - 1,
		.flags = IORESOURCE_MEM,
       },
       {
		.name = "kgsl_yamato_irq",
		.start = INT_GRAPHICS,
		.end = INT_GRAPHICS,
		.flags = IORESOURCE_IRQ,
       },
};
static struct kgsl_platform_data kgsl_pdata = {
	.high_axi_3d = 128000, /*Max for 8K*/
	.max_grp2d_freq = 0,
	.min_grp2d_freq = 0,
	.set_grp2d_async = NULL,
	.max_grp3d_freq = 0,
	.min_grp3d_freq = 0,
	.set_grp3d_async = NULL,
};

static struct platform_device msm_device_kgsl = {
       .name = "kgsl",
       .id = -1,
       .num_resources = ARRAY_SIZE(kgsl_resources),
       .resource = kgsl_resources,
	.dev = {
		.platform_data = &kgsl_pdata,
	},
};

struct es209ra_headset_platform_data es209ra_headset_data = {
	.keypad_name = "es209ra_keypad",
	.gpio_detout = 114,
	.gpio_detin = 132,
	.wait_time = 800,
};

static struct platform_device es209ra_audio_jack_device = {
	.name		= "es209ra_audio_jack",
	.dev = {
        .platform_data = &es209ra_headset_data,
    },
};


#define TPS65023_MAX_DCDC1	1600

static int qsd8x50_tps65023_set_dcdc1(int mVolts)
{
	int rc = 0;
	rc = tps65023_set_dcdc1_level(mVolts);
	/* By default the TPS65023 will be initialized to 1.225V.
	 * So we can safely switch to any frequency within this
	 * voltage even if the device is not probed/ready.
	 */
	if (rc == -ENODEV && mVolts <= CONFIG_QSD_PMIC_DEFAULT_DCDC1)
		rc = 0;
	return rc;
}

static struct msm_acpu_clock_platform_data qsd8x50_clock_data = {
	.acpu_switch_time_us = 20,
	.max_speed_delta_khz = 256000,
	.vdd_switch_time_us = 62,
	.max_vdd = TPS65023_MAX_DCDC1,
	.acpu_set_vdd = qsd8x50_tps65023_set_dcdc1,
};


static struct max17040_i2c_platform_data max17040_platform_data = {
	.data = &max17040_dev_data
};


#define AKM8973_GPIO_RESET_PIN 2
#define AKM8973_GPIO_RESET_ON 0
#define AKM8973_GPIO_RESET_OFF 1

static int ak8973_gpio_config(int enable)
{
	if (enable) {
		if (gpio_request(AKM8973_GPIO_RESET_PIN, "akm8973_xres")) {
			printk(KERN_ERR "%s: gpio_req xres"
				" - Fail!", __func__);
			return -EIO;
		}
		if (gpio_tlmm_config(GPIO_CFG(AKM8973_GPIO_RESET_PIN, 0,
			GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA),GPIO_ENABLE)) {
			printk(KERN_ERR "%s: gpio_tlmm_conf xres"
				" - Fail!", __func__);
			goto ak8973_gpio_fail_0;
		}
		/* Reset is active low, so just a precaution setting. */
		gpio_set_value(AKM8973_GPIO_RESET_PIN, 1);

	} else {
		gpio_free(AKM8973_GPIO_RESET_PIN);
	}
	return 0;

ak8973_gpio_fail_0:
	gpio_free(AKM8973_GPIO_RESET_PIN);
	return -EIO;
}

static int ak8973_xres(void)
{
	gpio_set_value(AKM8973_GPIO_RESET_PIN, 0);
	msleep(10);
	gpio_set_value(AKM8973_GPIO_RESET_PIN, 1);
	msleep(20);
	return 0;
}

static struct akm8973_i2c_platform_data akm8973_platform_data = {
	.gpio_config = ak8973_gpio_config,
	.xres = ak8973_xres
};

static struct i2c_board_info msm_i2c_board_info[] __initdata = {
	{
		I2C_BOARD_INFO("tps65023", 0x48),
	},
	{
		I2C_BOARD_INFO("max17040_fuel_gauge", 0x36),
		.platform_data = &max17040_platform_data,
	},
	{
		I2C_BOARD_INFO("lv5219lg", 0x74),
	},
	{
		I2C_BOARD_INFO("akm8973", 0x1C),
		.platform_data = &akm8973_platform_data,
	},
	{
		I2C_BOARD_INFO("bma150", 0x38), 
		.irq		   =  INT_ES209RA_GPIO_ACCEL,
	},
	{
		I2C_BOARD_INFO("semc_imx046_camera", 0x1F),
		.irq		   =  INT_ES209RA_GPIO_CAM_ISP,
	},
};


static struct platform_device msm_wlan_ar6000_pm_device = {
        .name           = "wlan_ar6000_pm_dev",
        .id             = 1,
        .num_resources  = 0,
        .resource       = NULL,
};

static int hsusb_rpc_connect(int connect)
{
	if (connect)
		return msm_hsusb_rpc_connect();
	else
		return msm_hsusb_rpc_close();
}

static struct msm_otg_platform_data msm_otg_pdata = {
	.rpc_connect	= hsusb_rpc_connect,
	.phy_reset	= msm_hsusb_native_phy_reset,
	.pmic_notif_init         = msm_pm_app_rpc_init,
	.pmic_notif_deinit       = msm_pm_app_rpc_deinit,
	.pmic_register_vbus_sn   = msm_pm_app_register_vbus_sn,
	.pmic_unregister_vbus_sn = msm_pm_app_unregister_vbus_sn,
	.pmic_enable_ldo         = msm_pm_app_enable_usb_ldo,
};

static struct msm_hsusb_gadget_platform_data msm_gadget_pdata;


static struct platform_device pmic_time_device = {
	.name = "pmic_time",
};


static struct platform_device *devices[] __initdata = {
	&msm_wlan_ar6000_pm_device,
	&msm_fb_device,
	&msm_device_smd,
	&msm_device_dmov,
	&android_pmem_kernel_ebi1_device,
	&android_pmem_device,
	&android_pmem_adsp_device,
	&android_pmem_smipool_device,
	&msm_device_nand,
	&msm_device_i2c,
	&qsd_device_spi,
	&android_usb_device,
	&msm_device_tssc,
	&msm_audio_device,
	&msm_device_uart1,
	&msm_bluesleep_device,
	&msm_bt_power_device,
	&msm_device_uart_dm2,
	&msm_device_kgsl,
	&hs_device,
	//&msm_camera_sensor_semc_imx046_camera,
	&vibrator_device,
	&ram_console_device,
	&es209ra_audio_jack_device,
	&pmic_time_device,
};

static void __init es209ra_init_irq(void)
{
	msm_init_irq();
	msm_init_sirc();
}

static void kgsl_phys_memory_init(void)
{
	request_mem_region(kgsl_resources[1].start,
		resource_size(&kgsl_resources[1]), "kgsl");
}

static void __init es209ra_init_usb(void)
{
	hs_clk = clk_get(NULL, "usb_hs_clk");
	if (IS_ERR(hs_clk)) {
		printk(KERN_ERR "%s: hs_clk clk get failed\n", __func__);
		return;
	}

	phy_clk = clk_get(NULL, "usb_phy_clk");
	if (IS_ERR(phy_clk)) {
		printk(KERN_ERR "%s: phy_clk clk get failed\n", __func__);
		return;
	}
	platform_device_register(&msm_device_otg);
	platform_device_register(&msm_device_gadget_peripheral);
}



static struct msm_pm_platform_data msm_pm_data[MSM_PM_SLEEP_MODE_NR] = {
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE].supported = 1,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE].suspend_enabled = 1,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE].idle_enabled = 1,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE].latency = 8594,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE].residency = 23740,

	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN].supported = 1,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN].suspend_enabled = 1,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN].idle_enabled = 1,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN].latency = 4594,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN].residency = 23740,

	[MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT].supported = 1,
	[MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT].suspend_enabled
		= 1,
	[MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT].idle_enabled = 0,
	[MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT].latency = 443,
	[MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT].residency = 1098,

	[MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT].supported = 1,
	[MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT].suspend_enabled = 1,
	[MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT].idle_enabled = 1,
	[MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT].latency = 2,
	[MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT].residency = 0,
};

static void msm_i2c_gpio_config(int iface, int config_type)
{
	int gpio_scl;
	int gpio_sda;
	
	switch (iface)
	{
		case 0: /* primary */
			gpio_scl = 95;
			gpio_sda = 96;
			break;
		case 1: /* secondary */
		default:
			printk(KERN_INFO "%s: es209ra has only primary I2C.\n", __func__);
			return;
	}
	
	if (config_type)
	{
		gpio_tlmm_config(GPIO_CFG(gpio_scl, 1, GPIO_INPUT,
					GPIO_NO_PULL, GPIO_16MA), GPIO_ENABLE);
		gpio_tlmm_config(GPIO_CFG(gpio_sda, 1, GPIO_INPUT,
					GPIO_NO_PULL, GPIO_16MA), GPIO_ENABLE);
	}
	else
	{
		gpio_tlmm_config(GPIO_CFG(gpio_scl, 0, GPIO_OUTPUT,
					GPIO_NO_PULL, GPIO_16MA), GPIO_ENABLE);
		gpio_tlmm_config(GPIO_CFG(gpio_sda, 0, GPIO_OUTPUT,
					GPIO_NO_PULL, GPIO_16MA), GPIO_ENABLE);
	}
}

static struct msm_i2c_platform_data msm_i2c_pdata = {
	.clk_freq = 384000,
	.rsl_id = SMEM_SPINLOCK_I2C,
	.pri_clk = 95,
	.pri_dat = 96,
/* SEMC:SYS: Es209ra has only primary I2C. */
	.msm_i2c_config_gpio = msm_i2c_gpio_config,
};

static void __init msm_device_i2c_init(void)
{
	if (gpio_request(95, "i2c_pri_clk"))
		pr_err("failed to request gpio i2c_pri_clk\n");
	if (gpio_request(96, "i2c_pri_dat"))
		pr_err("failed to request gpio i2c_pri_dat\n");

/* SEMC:SYS: Es209ra has only primary I2C. */

	msm_i2c_pdata.rmutex = (uint32_t)smem_alloc(SMEM_I2C_MUTEX, 8);
	msm_i2c_pdata.pm_lat =
		msm_pm_data[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN]
		.latency;
	msm_device_i2c.dev.platform_data = &msm_i2c_pdata;
}

static unsigned pmem_kernel_ebi1_size = PMEM_KERNEL_EBI1_SIZE;
static void __init pmem_kernel_ebi1_size_setup(char **p)
{
	pmem_kernel_ebi1_size = memparse(*p, p);
}
__early_param("pmem_kernel_ebi1_size=", pmem_kernel_ebi1_size_setup);


static unsigned pmem_mdp_size = MSM_PMEM_MDP_SIZE;
static void __init pmem_mdp_size_setup(char **p)
{
	pmem_mdp_size = memparse(*p, p);
}
__early_param("pmem_mdp_size=", pmem_mdp_size_setup);

static unsigned pmem_adsp_size = MSM_PMEM_ADSP_SIZE;
static void __init pmem_adsp_size_setup(char **p)
{
	pmem_adsp_size = memparse(*p, p);
}
__early_param("pmem_adsp_size=", pmem_adsp_size_setup);

static unsigned audio_size = MSM_AUDIO_SIZE;
static void __init audio_size_setup(char **p)
{
	audio_size = memparse(*p, p);
}
__early_param("audio_size=", audio_size_setup);


static void __init es209ra_init(void)
{
	smsm_wait_for_modem();
	msm_acpu_clock_init(&qsd8x50_clock_data);
	msm_hsusb_pdata.swfi_latency = msm_pm_data
	[MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT].latency;
	msm_device_hsusb_peripheral.dev.platform_data = &msm_hsusb_pdata;
	msm_device_otg.dev.platform_data = &msm_otg_pdata;
	msm_device_gadget_peripheral.dev.platform_data = &msm_gadget_pdata;
	platform_add_devices(devices, ARRAY_SIZE(devices));
	es209ra_init_usb();
	bt_power_init();
	audio_gpio_init();
	msm_device_i2c_init();
	msm_qsd_spi_init();
	i2c_register_board_info(0, msm_i2c_board_info,ARRAY_SIZE(msm_i2c_board_info));
	spi_register_board_info(msm_spi_board_info,ARRAY_SIZE(msm_spi_board_info));
	msm_pm_set_platform_data(msm_pm_data);
	kgsl_phys_memory_init();
	platform_device_register(&es209ra_keypad_device);
}

static void __init es209ra_allocate_memory_regions(void)
{
	void *addr;
	unsigned long size;

	size = pmem_kernel_ebi1_size;
	if (size) {
		addr = alloc_bootmem_aligned(size, 0x100000);
		android_pmem_kernel_ebi1_pdata.start = __pa(addr);
		android_pmem_kernel_ebi1_pdata.size = size;
		pr_info("allocating %lu bytes at %p (%lx physical) for kernel"
			" ebi1 pmem arena\n", size, addr, __pa(addr));
	}


	size = pmem_mdp_size;
	if (size) {
		addr = alloc_bootmem(size);
		android_pmem_pdata.start = __pa(addr);
		android_pmem_pdata.size = size;
		pr_info("allocating %lu bytes at %p (%lx physical) for mdp "
			"pmem arena\n", size, addr, __pa(addr));
	}

	size = pmem_adsp_size;
	if (size) {
		addr = alloc_bootmem(size);
		android_pmem_adsp_pdata.start = __pa(addr);
		android_pmem_adsp_pdata.size = size;
		pr_info("allocating %lu bytes at %p (%lx physical) for adsp "
			"pmem arena\n", size, addr, __pa(addr));
	}

	size = MSM_FB_SIZE;
	addr = (void *)MSM_FB_BASE;
	msm_fb_resources[0].start = (unsigned long)addr;
	msm_fb_resources[0].end = msm_fb_resources[0].start + size - 1;
	pr_info("using %lu bytes of SMI at %lx physical for fb\n",
	       size, (unsigned long)addr);

	size = audio_size ? : MSM_AUDIO_SIZE;
	addr = alloc_bootmem(size);
	msm_audio_resources[0].start = __pa(addr);
	msm_audio_resources[0].end = msm_audio_resources[0].start + size - 1;
	pr_info("allocating %lu bytes at %p (%lx physical) for audio\n",
		size, addr, __pa(addr));
}

static void __init es209ra_fixup(struct machine_desc *desc, struct tag *tags,
                               char **cmdline, struct meminfo *mi)
{
	mi->nr_banks=2;
	mi->bank[0].start = PHYS_OFFSET;
	mi->bank[0].size = MSM_EBI1_BANK0_SIZE;
	mi->bank[1].start = MSM_EBI1_BANK1_BASE;
	mi->bank[1].size = MSM_EBI1_BANK1_SIZE;
}

static void __init es209ra_map_io(void)
{
	msm_shared_ram_phys = 0x00100000;
	msm_map_qsd8x50_io();
	es209ra_allocate_memory_regions();
	msm_clock_init(msm_clocks_8x50, msm_num_clocks_8x50);
}

MACHINE_START(ES209RA, "ES209RA")
	.boot_params  = PHYS_OFFSET + 0x100,
	.fixup        = es209ra_fixup,
	.map_io       = es209ra_map_io,
	.init_irq     = es209ra_init_irq,
	.init_machine	= es209ra_init,
	.timer        = &msm_timer,
MACHINE_END

