/*
 * rtlsdr.h
 *
 *  Created on: 08.08.2017
 *      Author: Karol
 */

#ifndef RTLSDR_H_
#define RTLSDR_H_

#define DEF_RTL_XTAL_FREQ	28800000
#define FIR_LEN 16

#include <stdint.h>

static const uint16_t fir_default[FIR_LEN] = {
    -54, -36, -41, -40, -32, -14, 14, 53,	/* 8 bit signed */
    101, 156, 215, 273, 327, 372, 404, 421	/* 12 bit signed */
};

enum rtlsdr_tuner {
	RTLSDR_TUNER_UNKNOWN = 0,
	RTLSDR_TUNER_E4000,
	RTLSDR_TUNER_FC0012,
	RTLSDR_TUNER_FC0013,
	RTLSDR_TUNER_FC2580,
	RTLSDR_TUNER_R820T,
	RTLSDR_TUNER_R828D
};

typedef struct{
    /* tuner interface */
    int (*init)(void*);
    int (*exit)(void*);
    int (*set_freq)(void*, uint32_t freq /* Hz */);
    int (*set_bw)(void*, int bw /* Hz */);
    int (*set_gain)(void*, int gain /* tenth dB */);
    int (*set_if_gain)(void*, int stage, int gain /* tenth dB */);
    int (*set_gain_mode)(void*, int manual);
} rtlsdr_tuner_iface_t;


typedef struct
{
	/* rtl demod context */
	uint32_t rate; /* Hz */
	uint16_t fir[FIR_LEN];

	/* tuner context */
	uint32_t rtl_xtal; /* Hz */
	enum rtlsdr_tuner tuner_type;
	rtlsdr_tuner_iface_t* tuner;
	uint32_t tun_xtal; /* Hz */
    struct r82xx_config r82xx_c;
    struct r82xx_priv r82xx_p;
    uint32_t freq; /* Hz */

	/* status */
	uint8_t driver_active;
	uint8_t dev_lost;
	uint8_t xfer_errors;
}rtlsdr_dev_t;

int rtlsdr_open(rtlsdr_dev_t* dev, uint32_t index);


#endif /* RTLSDR_H_ */
