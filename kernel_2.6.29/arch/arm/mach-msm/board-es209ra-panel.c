#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <mach/pmic.h>
#include <linux/platform_device.h>
#include <linux/workqueue.h>
#include <asm/io.h>
#include <asm/mach-types.h>
#include <mach/msm_fb.h>
#include <mach/msm_iomap.h>
#include <mach/vreg.h>
#include "proc_comm.h"
#include "board-es209ra.h"
#include "devices.h"
#include <../../../drivers/video/msm/mddi_tmd_nt35580.h>
#include "../../../drivers/video/msm/msm_fb_panel.h"
#include "../../../drivers/video/msm/mddihost.h"


static int mddi_power_save_on;
static void msm_fb_mddi_power_save(int on)
{
	int flag_on = !!on;
	int ret;


	if (mddi_power_save_on == flag_on)
		return;

	mddi_power_save_on = flag_on;

	ret = pmic_lp_mode_control(flag_on ? OFF_CMD : ON_CMD,
		PM_VREG_LP_MSME2_ID);
	if (ret)
		printk(KERN_ERR "%s: pmic_lp_mode failed!\n", __func__);
}

static int msm_fb_mddi_sel_clk(u32 *clk_rate)
{
	*clk_rate *= 2;
	return 0;
}

static struct mddi_platform_data mddi_pdata = {
	.mddi_power_save = msm_fb_mddi_power_save,
	.mddi_sel_clk = msm_fb_mddi_sel_clk,
};

static struct msm_panel_common_pdata mdp_pdata = {
	.gpio = 98,
};

static void __init msm_fb_add_devices(void)
{
	msm_fb_register_device("mdp", &mdp_pdata);
	msm_fb_register_device("pmdh", &mddi_pdata);
	msm_fb_register_device("emdh", &mddi_pdata);
	msm_fb_register_device("tvenc", 0);
	msm_fb_register_device("lcdc", 0);
}

#define NT35580_GPIO_XRST 100
static struct vreg *vreg_mmc;
static struct vreg *vreg_gp2;

static void tmd_wvga_lcd_power_on(void)
{
	int rc = 0;

	local_irq_disable();

	rc = vreg_enable(vreg_gp2);
	if (rc) {
		local_irq_enable();
		printk(KERN_ERR"%s:vreg_enable(gp2)err. rc=%d\n", __func__, rc);
		return;
	}
	rc = vreg_enable(vreg_mmc);
	if (rc) {
		local_irq_enable();
		printk(KERN_ERR"%s:vreg_enable(mmc)err. rc=%d\n", __func__, rc);
		return;
	}
	local_irq_enable();

	msleep(50);
	gpio_set_value(NT35580_GPIO_XRST, 1);
	msleep(10);
	gpio_set_value(NT35580_GPIO_XRST, 0);
	msleep(1);
	gpio_set_value(NT35580_GPIO_XRST, 1);
	msleep(210);
}

static void tmd_wvga_lcd_power_off(void)
{
	gpio_set_value(NT35580_GPIO_XRST, 0);
	msleep(10);
	local_irq_disable();
	vreg_disable(vreg_mmc);
	vreg_disable(vreg_gp2);
	local_irq_enable();
}

static struct panel_data_ext tmd_wvga_panel_ext = {
	.power_on = tmd_wvga_lcd_power_on,
	.power_off = tmd_wvga_lcd_power_off,
};

static struct msm_fb_panel_data tmd_wvga_panel_data;

static struct platform_device mddi_tmd_wvga_display_device = {
	.name = "mddi_tmd_wvga",
	.id = -1,
};

static void __init msm_mddi_tmd_fwvga_display_device_init(void)
{
	struct msm_fb_panel_data *panel_data = &tmd_wvga_panel_data;

	printk(KERN_DEBUG "%s \n", __func__);

	panel_data->panel_info.xres = 480;
	panel_data->panel_info.yres = 854;
	panel_data->panel_info.type = MDDI_PANEL;
	panel_data->panel_info.pdest = DISPLAY_1;
	panel_data->panel_info.wait_cycle = 0;
	panel_data->panel_info.bpp = 16;
	panel_data->panel_info.clk_rate = 192000000;
	panel_data->panel_info.clk_min =  190000000;
	panel_data->panel_info.clk_max = 200000000;
	panel_data->panel_info.fb_num = 2;

	panel_data->panel_info.mddi.vdopkt = MDDI_DEFAULT_PRIM_PIX_ATTR;

	panel_data->panel_info.lcd.vsync_enable = TRUE;
	panel_data->panel_info.lcd.v_back_porch = 12;
	panel_data->panel_info.lcd.v_front_porch = 2;
	panel_data->panel_info.lcd.v_pulse_width = 0;
	panel_data->panel_info.lcd.hw_vsync_mode = 0;
	panel_data->panel_info.lcd.vsync_notifier_period = 0;

	//panel_data->panel_info.lcd.refx100 = 100000000 / 16766;
	panel_data->panel_info.lcd.refx100 = 200000000 / 16766;
   
	panel_data->panel_ext = &tmd_wvga_panel_ext;

	mddi_tmd_wvga_display_device.dev.platform_data =&tmd_wvga_panel_data;

	vreg_gp2 = vreg_get(NULL, "gp2");
	if (IS_ERR(vreg_gp2)) {
		printk(KERN_ERR "%s: vreg_get(gp2) err.\n", __func__);
		return;
	}
	vreg_mmc = vreg_get(NULL, "mmc");
	if (IS_ERR(vreg_mmc)) {
		printk(KERN_ERR "%s: vreg_get(mmc) err.\n", __func__);
		return;
	}

	platform_device_register(&mddi_tmd_wvga_display_device);
}


static int __init es209ra_init_panel(void)
{
    int ret=0;
    msm_fb_add_devices();
    msm_mddi_tmd_fwvga_display_device_init();
    return ret;
}

device_initcall(es209ra_init_panel);