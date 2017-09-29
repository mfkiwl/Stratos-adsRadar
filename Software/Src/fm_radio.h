/*
 * fm_radio.h
 *
 *  Created on: 01.09.2017
 *      Author: Karol
 */

#ifndef FM_RADIO_H
#define FM_RADIO_H

#include "stdint.h"
#include "main.h"
#include "convenience/convenience.h"
#include "rtl-sdr.h"

// use statically defined rtlsdr instance
extern rtlsdr_dev_t static_dev;
rtlsdr_dev_t *dev;

struct FMRadio {
    int32_t tuner_ppm_error;
    uint32_t tuner_frequency;
    uint32_t adc_samp_rate;
    volatile int32_t raw_buf_remain_bytes;
    volatile uint8_t is_raw_buf_complete;
    volatile uint8_t is_usb_app_started;
};


#define MODES_PREAMBLE_US 8       /* microseconds */
#define MODES_LONG_MSG_BITS 112
#define MODES_SHORT_MSG_BITS 56
#define MODES_FULL_LEN (MODES_PREAMBLE_US+MODES_LONG_MSG_BITS)
#define MODES_LONG_MSG_BYTES (112/8)
#define MODES_SHORT_MSG_BYTES (56/8)

struct modesMessage {
    /* Generic fields */
    unsigned char msg[MODES_LONG_MSG_BYTES]; /* Binary message. */
    int msgbits;                /* Number of bits in message */
    int msgtype;                /* Downlink format # */
    int crcok;                  /* True if CRC was valid */
    uint32_t crc;               /* Message CRC */
    int errorbit;               /* Bit corrected. -1 if no bit corrected. */
    int aa1, aa2, aa3;          /* ICAO Address bytes 1 2 and 3 */
    int phase_corrected;        /* True if phase correction was applied. */

    /* DF 11 */
    int ca;                     /* Responder capabilities. */

    /* DF 17 */
    int metype;                 /* Extended squitter message type. */
    int mesub;                  /* Extended squitter message subtype. */
    int heading_is_valid;
    int heading;
    int aircraft_type;
    int fflag;                  /* 1 = Odd, 0 = Even CPR message. */
    int tflag;                  /* UTC synchronized? */
    int raw_latitude;           /* Non decoded latitude */
    int raw_longitude;          /* Non decoded longitude */
    char flight[9];             /* 8 chars flight number. */
    int ew_dir;                 /* 0 = East, 1 = West. */
    int ew_velocity;            /* E/W velocity. */
    int ns_dir;                 /* 0 = North, 1 = South. */
    int ns_velocity;            /* N/S velocity. */
    int vert_rate_source;       /* Vertical rate source. */
    int vert_rate_sign;         /* Vertical rate sign. */
    int vert_rate;              /* Vertical rate. */
    int velocity;               /* Computed from EW and NS velocity. */

    /* DF4, DF5, DF20, DF21 */
    int fs;                     /* Flight status for DF4,5,20,21 */
    int dr;                     /* Request extraction of downlink request. */
    int um;                     /* Request extraction of downlink request. */
    int identity;               /* 13 bits identity (Squawk). */

    /* Fields used by multiple message types. */
    int altitude, unit;
};

int32_t fmradio_atan2(int32_t y, int32_t x);
void fmradio_cmultiply(int32_t ar, int32_t aj, int32_t br, int32_t bj, int32_t* cr, int32_t* cj);
int32_t fmradio_polar_disc(int32_t ar, int32_t aj, int32_t br, int32_t bj);
void fmradio_process(void);
int fmradio_init(void);

#endif