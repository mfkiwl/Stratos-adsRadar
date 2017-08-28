/*
 * rtlsdr.c
 *
 *  Created on: 08.08.2017
 *      Author: Karol
 */

#include <stddef.h>
#include <string.h>

#include "tuner_r820t.h"
#include "rtlsdr.h"

int rtlsdr_open(rtlsdr_dev_t* dev, uint32_t index) {
	uint8_t r;
    uint8_t reg;
    size_t cnt;

    memset(dev, 0, sizeof(rtlsdr_dev_t));
    memcpy(dev->fir, fir_default, sizeof(fir_default));

    dev->dev_lost = 1;
    dev->driver_active = 1;
    dev->rtl_xtal = DEF_RTL_XTAL_FREQ;

    rtlsdr_init_baseband(dev);
    dev->dev_lost = 0;

    /* Probe tuners */
    rtlsdr_set_i2c_repeater(dev, 1);

    reg = rtlsdr_i2c_read_reg(dev, R820T_I2C_ADDR, R82XX_CHECK_ADDR);

    if (reg == R82XX_CHECK_VAL) {
        dev->tuner_type = RTLSDR_TUNER_R820T;
    }
    else {
    	return 1;
    }

    dev->tun_xtal = dev->rtl_xtal;
    dev->tuner->init = r820t_init;
    dev->tuner->exit = r820t_exit;
    dev->tuner->set_freq =  r820t_set_freq;
    dev->tuner->set_bw = r820t_set_bw;
    dev->tuner->set_gain = r820t_set_gain;
    dev->tuner->set_if_gain = NULL;
    dev->tuner->set_gain_mode = r820t_set_gain_mode;
}

static int rtlsdr_set_if_freq(rtlsdr_dev_t* dev, uint32_t freq) {
    uint32_t rtl_xtal;
    int32_t if_freq;
    uint8_t tmp;
    int r;

    if (!dev)
    { return -1; }

    /* read corrected clock value */
    //if (rtlsdr_get_xtal_freq(dev, &rtl_xtal, NULL))
    //{ return -2; }

    if_freq = ((freq * TWO_POW(22)) / rtl_xtal) * (-1);

    tmp = (if_freq >> 16) & 0x3f;
    r = rtlsdr_demod_write_reg(dev, 1, 0x19, tmp, 1);
    tmp = (if_freq >> 8) & 0xff;
    r |= rtlsdr_demod_write_reg(dev, 1, 0x1a, tmp, 1);
    tmp = if_freq & 0xff;
    r |= rtlsdr_demod_write_reg(dev, 1, 0x1b, tmp, 1);

    return r;
}
