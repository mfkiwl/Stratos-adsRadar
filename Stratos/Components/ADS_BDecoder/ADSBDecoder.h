/*
 * ADSBDecoder.h
 *
 *  Created on: 21.10.2017
 *      Author: Karol
 */

#ifndef ADS_BDECODER_ADSBDECODER_H_
#define ADS_BDECODER_ADSBDECODER_H_

#include "RTLSDR.h"
#include "ADSBMessage.h"
#include "cmsis_os.h"
#include <array>

#define magnitudeLUTSize 129*129*2

#define MODES_PREAMBLE_US 8       /* microseconds */
#define MODES_UNIT_FEET 0
#define MODES_UNIT_METERS 1

typedef std::array<uint16_t,USB_IN_STREAM_SIZE/2> MagnitudeVectorType;


class ADS_BDecoder
{
public:
	ADS_BDecoder(QueueHandle_t messageQueue);

	void ProcessRawSamples(uint8_t* rawSamples);

private:

	void ComputeMagnitudeVector(uint8_t* rawSamples,
			                    MagnitudeVectorType& magnitude);
	void InitMagnitudeLUT();

	bool CheckIfPreambleCorrect(MagnitudeVectorType& magnitude, const size_t& idx);
	void DetectMessage(MagnitudeVectorType& magnitude);
	int  DetectOutOfPhase(uint16_t *m);
	void ApplyPhaseCorrection(uint16_t *m);
	int  MessageLenByType(int type);
	void DecodeMessage(ADS_BMessage* mm);

	int DecodeAC12Field(unsigned char *msg, int *unit);
	int DecodeAC13Field(unsigned char *msg, int *unit);
	uint32_t CalcMsgChecksum(unsigned char *msg, int bits);
	int FixSingleBitErrors(unsigned char *msg, int bits);
	std::array<uint16_t, magnitudeLUTSize >magnitudeLUT;
	uint16_t maglut[129*129*2];

	QueueHandle_t messageQueue;
};

#endif /* ADS_BDECODER_ADSBDECODER_H_ */
