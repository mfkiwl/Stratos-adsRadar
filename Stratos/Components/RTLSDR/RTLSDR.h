/*
 * RTLSDR.h
 *
 *  Created on: 18.10.2017
 *      Author: Karol
 */

#ifndef RTLSDR_RTLSDR_H_
#define RTLSDR_RTLSDR_H_


#include "rtl-sdr.h"
#include "USBDriver.h"
#include "StaticCircularBuffer.h"


enum usb_reg {
    USB_SYSCTL		= 0x2000,
    USB_CTRL		= 0x2010,
    USB_STAT		= 0x2014,
    USB_EPA_CFG		= 0x2144,
    USB_EPA_CTL		= 0x2148,
    USB_EPA_MAXPKT		= 0x2158,
    USB_EPA_MAXPKT_2	= 0x215a,
    USB_EPA_FIFO_CFG	= 0x2160,
};

enum sys_reg {
    DEMOD_CTL		= 0x3000,
    GPO			= 0x3001,
    GPI			= 0x3002,
    GPOE			= 0x3003,
    GPD			= 0x3004,
    SYSINTE			= 0x3005,
    SYSINTS			= 0x3006,
    GP_CFG0			= 0x3007,
    GP_CFG1			= 0x3008,
    SYSINTE_1		= 0x3009,
    SYSINTS_1		= 0x300a,
    DEMOD_CTL_1		= 0x300b,
    IR_SUSPEND		= 0x300c,
};

enum blocks {
    DEMODB			= 0,
    USBB			= 1,
    SYSB			= 2,
    TUNB			= 3,
    ROMB			= 4,
    IRB			= 5,
    IICB			= 6,
};

#define NUM_REGS		30

static const uint8_t r82xx_init_array[NUM_REGS] = {
	0x83, 0x32, 0x75,
	0xc0, 0x40, 0xd6, 0x6c,
	0xf5, 0x63, 0x75, 0x68,
	0x6c, 0x83, 0x80, 0x00,
	0x0f, 0x00, 0xc0, 0x30,
	0x48, 0xcc, 0x60, 0x00,
	0x54, 0xae, 0x4a, 0xc0
};

struct rtlsdr_tuner_iface_t {
    int (*init)(void*);
    int (*exit)(void*);
    int (*set_freq)(void*, uint32_t freq);
    int (*set_bw)(void*, int bw );
    int (*set_gain)(void*, int gain );
    int (*set_if_gain)(void*, int stage, int gain );
    int (*set_gain_mode)(void*, int manual);
};

#define DEF_RTL_XTAL_FREQ	28800000

#define TWO_POW(n)		((double)(1ULL<<(n)))


extern rtlsdr_dev_t static_dev;

#define USB_IN_STREAM_SIZE 2048
#define USB_BUFFER_SIZE 3
class RTLSDR
{
public:
	RTLSDR(USBDriver* usbDriverHandle);
	uint8_t OpenDevice(  int32_t  tunerPpmError,
    		             uint32_t  tunerFrequency,
    		             uint32_t  adcSampRate);

	void GetNewRawSamples();
	void NotifyNewRawSampleRecived();
	void NotifyRawSampleProcessed();



	volatile bool IsDeviceReady() const;
	uint8_t* GetRawSamplesFromBuffer();
	//std::array<uint8_t,USB_IN_STREAM_SIZE>& GetRawSamplesFromBuffer();
private:
	void InitBaseband();
	void WriteReg(uint8_t block, uint16_t addr, uint16_t val, uint8_t len);
	void DemodWriteReg(uint8_t page, uint16_t addr, uint16_t val, uint8_t len);
	uint16_t DemodReadReg(uint8_t page, uint16_t addr, uint8_t len);
	void SetFir();
	void SetI2CRepeater(bool on);
	uint8_t I2CReadReg(uint8_t i2c_addr, uint8_t reg);
	int I2CWriteReg(uint8_t i2c_addr, uint8_t* buffer, int len) ;
	int ReadArray(uint8_t block, uint16_t addr, uint8_t* array, uint8_t len);
	int WriteArray(uint8_t block, uint16_t addr, uint8_t* array, uint8_t len);

	int R82xxWrite(uint8_t reg, const uint8_t *val,unsigned int len);
	void ShadowStore(uint8_t reg, const uint8_t *val, int len);
	int SetIfFreq(uint32_t freq);
	int GetXtalFreq(uint32_t* rtl_freq, uint32_t* tuner_freq);
	enum rtlsdr_tuner tuner_type;
	int xtal_cap_sel;
	uint8_t regs[NUM_REGS];


	USBDriver* usbDriverHandle;
	rtlsdr_dev_t *dev;

	uint8_t buff[2048];

	volatile bool deviceReady;
};
#endif /* RTLSDR_RTLSDR_H_ */
