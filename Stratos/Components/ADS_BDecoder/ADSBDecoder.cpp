/*
 * ADSBDecoder.cpp
 *
 *  Created on: 21.10.2017
 *      Author: Karol
 */

#include "ADSBDecoder.h"
#include <cmath>
#include <cstring>
#include "FlightControlControler.h"

extern FlightControlControler controler;

static uint32_t modes_checksum_table[112] = {
0x3935ea, 0x1c9af5, 0xf1b77e, 0x78dbbf, 0xc397db, 0x9e31e9, 0xb0e2f0, 0x587178,
0x2c38bc, 0x161c5e, 0x0b0e2f, 0xfa7d13, 0x82c48d, 0xbe9842, 0x5f4c21, 0xd05c14,
0x682e0a, 0x341705, 0xe5f186, 0x72f8c3, 0xc68665, 0x9cb936, 0x4e5c9b, 0xd8d449,
0x939020, 0x49c810, 0x24e408, 0x127204, 0x093902, 0x049c81, 0xfdb444, 0x7eda22,
0x3f6d11, 0xe04c8c, 0x702646, 0x381323, 0xe3f395, 0x8e03ce, 0x4701e7, 0xdc7af7,
0x91c77f, 0xb719bb, 0xa476d9, 0xadc168, 0x56e0b4, 0x2b705a, 0x15b82d, 0xf52612,
0x7a9309, 0xc2b380, 0x6159c0, 0x30ace0, 0x185670, 0x0c2b38, 0x06159c, 0x030ace,
0x018567, 0xff38b7, 0x80665f, 0xbfc92b, 0xa01e91, 0xaff54c, 0x57faa6, 0x2bfd53,
0xea04ad, 0x8af852, 0x457c29, 0xdd4410, 0x6ea208, 0x375104, 0x1ba882, 0x0dd441,
0xf91024, 0x7c8812, 0x3e4409, 0xe0d800, 0x706c00, 0x383600, 0x1c1b00, 0x0e0d80,
0x0706c0, 0x038360, 0x01c1b0, 0x00e0d8, 0x00706c, 0x003836, 0x001c1b, 0xfff409,
0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000
};

ADS_BDecoder::ADS_BDecoder(QueueHandle_t messageQueue) : messageQueue(messageQueue)
{
    for (uint8_t i = 0; i <= 128; i++) {
        for (uint8_t q = 0; q <= 128; q++) {
            maglut[i*129+q] = round(sqrt(i*i+q*q)*360);
        }
    }

}

void ADS_BDecoder::ProcessRawSamples(uint8_t* rawSamples)
{
	MagnitudeVectorType magnitude;
	ComputeMagnitudeVector(rawSamples,magnitude);
	DetectMessage(magnitude);
}

void ADS_BDecoder::ComputeMagnitudeVector(uint8_t* rawSamples,
		                                  MagnitudeVectorType& magnitude)
{
    uint32_t j;

    for (j = 0; j < 2048; j += 2) {
        int i = rawSamples[j]-127;
        int q = rawSamples[j+1]-127;

        if (i < 0) i = -i;
        if (q < 0) q = -q;
        magnitude[j/2] = maglut[i*129+q];
    }

}

void ADS_BDecoder::InitMagnitudeLUT()
{
    for (uint8_t i = 0; i <= 128; i++) {
        for (uint8_t q = 0; q <= 128; q++) {
        	magnitudeLUT[i*129+q] = round(sqrt(i*i+q*q)*360);
        }
    }
}

bool ADS_BDecoder::CheckIfPreambleCorrect(MagnitudeVectorType& magnitude, const size_t& idx)
{

    /* The Mode S preamble is made of impulses of 0.5 microseconds at
     * the following time offsets:
     *
     * 0   - 0.5 usec: first impulse.
     * 1.0 - 1.5 usec: second impulse.
     * 3.5 - 4   usec: third impulse.
     * 4.5 - 5   usec: last impulse.
     *
     * Since we are sampling at 2 Mhz every sample in our magnitude vector
     * is 0.5 usec, so the preamble will look like this, assuming there is
     * an impulse at offset 0 in the array:
     *
     * 0   -----------------
     * 1   -
     * 2   ------------------
     * 3   --
     * 4   -
     * 5   --
     * 6   -
     * 7   ------------------
     * 8   --
     * 9   -------------------
     */
	if(!(magnitude[idx] > magnitude[idx+1] &&
	   magnitude[idx+1] < magnitude[idx+2] &&
	   magnitude[idx+2] > magnitude[idx+3] &&
	   magnitude[idx+3] < magnitude[idx] &&
	   magnitude[idx+4] < magnitude[idx] &&
	   magnitude[idx+5] < magnitude[idx] &&
	   magnitude[idx+6] < magnitude[idx] &&
	   magnitude[idx+7] > magnitude[idx+8] &&
	   magnitude[idx+8] < magnitude[idx+9] &&
	   magnitude[idx+9] > magnitude[idx+6]))
		{
			return true;
		}
	return false;
}

int ADS_BDecoder::DetectOutOfPhase(uint16_t *m) {
    if (m[3] > m[2]/3) return 1;
    if (m[10] > m[9]/3) return 1;
    if (m[6] > m[7]/3) return -1;
    if (m[-1] > m[1]/3) return -1;
    return 0;
}

void ADS_BDecoder::ApplyPhaseCorrection(uint16_t *m) {
    int j;

    m += 16; /* Skip preamble. */
    for (j = 0; j < (MODES_LONG_MSG_BITS-1)*2; j += 2) {
        if (m[j] > m[j+1]) {
            /* One */
            m[j+2] = (m[j+2] * 5) / 4;
        } else {
            /* Zero */
            m[j+2] = (m[j+2] * 4) / 5;
        }
    }
}

int ADS_BDecoder::MessageLenByType(int type) {
    if (type == 16 || type == 17 ||
        type == 19 || type == 20 ||
        type == 21)
        return MODES_LONG_MSG_BITS;
    else
        return MODES_SHORT_MSG_BITS;
}

int ADS_BDecoder::DecodeAC12Field(unsigned char *msg, int *unit) {
    int q_bit = msg[5] & 1;

    if (q_bit) {
        /* N is the 11 bit integer resulting from the removal of bit
         * Q */
        *unit = MODES_UNIT_FEET;
        int n = ((msg[5]>>1)<<4) | ((msg[6]&0xF0) >> 4);
        /* The final altitude is due to the resulting number multiplied
         * by 25, minus 1000. */
        return n*25-1000;
    } else {
        return 0;
    }
}

int ADS_BDecoder::DecodeAC13Field(unsigned char *msg, int *unit) {
    int m_bit = msg[3] & (1<<6);
    int q_bit = msg[3] & (1<<4);

    if (!m_bit) {
        *unit = MODES_UNIT_FEET;
        if (q_bit) {
            /* N is the 11 bit integer resulting from the removal of bit
             * Q and M */
            int n = ((msg[2]&31)<<6) |
                    ((msg[3]&0x80)>>2) |
                    ((msg[3]&0x20)>>1) |
                     (msg[3]&15);
            /* The final altitude is due to the resulting number multiplied
             * by 25, minus 1000. */
            return n*25-1000;
        } else {
            /* TODO: Implement altitude where Q=0 and M=0 */
        }
    } else {
        *unit = MODES_UNIT_METERS;
        /* TODO: Implement altitude when meter unit is selected. */
    }
    return 0;
}

uint32_t ADS_BDecoder::CalcMsgChecksum(unsigned char *msg, int bits) {
    uint32_t crc = 0;
    int offset = (bits == 112) ? 0 : (112-56);
    int j;

    for(j = 0; j < bits; j++) {
        int byte = j/8;
        int bit = j%8;
        int bitmask = 1 << (7-bit);

        /* If bit is set, xor with corresponding table entry. */
        if (msg[byte] & bitmask)
            crc ^= modes_checksum_table[j+offset];
    }
    return crc; /* 24 bit checksum. */
}

int ADS_BDecoder::FixSingleBitErrors(unsigned char *msg, int bits) {
    int j;
    unsigned char aux[MODES_LONG_MSG_BITS/8];

    for (j = 0; j < bits; j++) {
        int byte = j/8;
        int bitmask = 1 << (7-(j%8));
        uint32_t crc1, crc2;

        memcpy(aux,msg,bits/8);
        aux[byte] ^= bitmask; /* Flip j-th bit. */

        crc1 = ((uint32_t)aux[(bits/8)-3] << 16) |
               ((uint32_t)aux[(bits/8)-2] << 8) |
                (uint32_t)aux[(bits/8)-1];
        crc2 = CalcMsgChecksum(aux,bits);

        if (crc1 == crc2) {
            /* The error is fixed. Overwrite the original buffer with
             * the corrected sequence, and returns the error bit
             * position. */
            memcpy(msg,aux,bits/8);
            return j;
        }
    }
    return -1;
}
void ADS_BDecoder::DecodeMessage(ADS_BMessage* mm)
{
    uint32_t crc2;   /* Computed CRC, used to verify the message CRC. */
    char ais_charset[] = "?ABCDEFGHIJKLMNOPQRSTUVWXYZ????? ???????????????0123456789??????";

    unsigned char* msg = mm->msg;
    /* Work on our local copy */
    //msg = mm->msg;

    /* Get the message type ASAP as other operations depend on this */
    mm->msgtype = msg[0]>>3;    /* Downlink Format */
    mm->msgbits = MessageLenByType(mm->msgtype);

    /* CRC is always the last three bytes. */
    mm->crc = ((uint32_t)msg[(mm->msgbits/8)-3] << 16) |
              ((uint32_t)msg[(mm->msgbits/8)-2] << 8) |
               (uint32_t)msg[(mm->msgbits/8)-1];
    crc2 = CalcMsgChecksum(msg,mm->msgbits);

    /* Check CRC and fix single bit errors using the CRC when
     * possible (DF 11 and 17). */
    mm->errorbit = -1;  /* No error */
    mm->crcok = (mm->crc == crc2);

    if (!mm->crcok &&
         (mm->msgtype == 17))
     {
         if ((mm->errorbit = FixSingleBitErrors(msg,mm->msgbits)) != -1) {
             mm->crc = CalcMsgChecksum(msg,mm->msgbits);
             mm->crcok = 1;
         }else
         {
        	 xQueueSend(messageQueue, mm, portMAX_DELAY);
        	 return;
         }
     }

    /* Note that most of the other computation happens *after* we fix
        * the single bit errors, otherwise we would need to recompute the
        * fields again. */
       mm->ca = msg[0] & 7;        /* Responder capabilities. */

       /* ICAO address */
       mm->aa1 = msg[1];
       mm->aa2 = msg[2];
       mm->aa3 = msg[3];

       /* DF 17 type (assuming this is a DF17, otherwise not used) */
       mm->metype = msg[4] >> 3;   /* Extended squitter message type. */
       mm->mesub = msg[4] & 7;     /* Extended squitter message subtype. */

       /* Fields for DF4,5,20,21 */
       mm->fs = msg[0] & 7;        /* Flight status for DF4,5,20,21 */
       mm->dr = msg[1] >> 3 & 31;  /* Request extraction of downlink request. */
       mm->um = ((msg[1] & 7)<<3)| /* Request extraction of downlink request. */
                 msg[2]>>5;

       /* In the squawk (identity) field bits are interleaved like that
        * (message bit 20 to bit 32):
        *
        * C1-A1-C2-A2-C4-A4-ZERO-B1-D1-B2-D2-B4-D4
        *
        * So every group of three bits A, B, C, D represent an integer
        * from 0 to 7.
        *
        * The actual meaning is just 4 octal numbers, but we convert it
        * into a base ten number tha happens to represent the four
        * octal numbers.
        *
        * For more info: http://en.wikipedia.org/wiki/Gillham_code */
       {
           int a,b,c,d;

           a = ((msg[3] & 0x80) >> 5) |
               ((msg[2] & 0x02) >> 0) |
               ((msg[2] & 0x08) >> 3);
           b = ((msg[3] & 0x02) << 1) |
               ((msg[3] & 0x08) >> 2) |
               ((msg[3] & 0x20) >> 5);
           c = ((msg[2] & 0x01) << 2) |
               ((msg[2] & 0x04) >> 1) |
               ((msg[2] & 0x10) >> 4);
           d = ((msg[3] & 0x01) << 2) |
               ((msg[3] & 0x04) >> 1) |
               ((msg[3] & 0x10) >> 4);
           mm->identity = a*1000 + b*100 + c*10 + d;
       }

       /* Decode 13 bit altitude for DF0, DF4, DF16, DF20 */
       if (mm->msgtype == 0 || mm->msgtype == 4 ||
           mm->msgtype == 16 || mm->msgtype == 20) {
           mm->altitude = DecodeAC13Field(msg, &mm->unit);

       }

       /* Decode extended squitter specific stuff. */
       if (mm->msgtype == 17) {
           /* Decode the extended squitter message. */

           if (mm->metype >= 1 && mm->metype <= 4) {
               /* Aircraft Identification and Category */
               mm->aircraft_type = mm->metype-1;
               mm->flight[0] = ais_charset[msg[5]>>2];
               mm->flight[1] = ais_charset[((msg[5]&3)<<4)|(msg[6]>>4)];
               mm->flight[2] = ais_charset[((msg[6]&15)<<2)|(msg[7]>>6)];
               mm->flight[3] = ais_charset[msg[7]&63];
               mm->flight[4] = ais_charset[msg[8]>>2];
               mm->flight[5] = ais_charset[((msg[8]&3)<<4)|(msg[9]>>4)];
               mm->flight[6] = ais_charset[((msg[9]&15)<<2)|(msg[10]>>6)];
               mm->flight[7] = ais_charset[msg[10]&63];
               mm->flight[8] = '\0';
           } else if (mm->metype >= 9 && mm->metype <= 18) {
               /* Airborne position Message */
               mm->fflag = msg[6] & (1<<2);
               mm->tflag = msg[6] & (1<<3);
               mm->altitude = DecodeAC12Field(msg,&mm->unit);
               mm->raw_latitude = ((msg[6] & 3) << 15) |
                                   (msg[7] << 7) |
                                   (msg[8] >> 1);
               mm->raw_longitude = ((msg[8]&1) << 16) |
                                    (msg[9] << 8) |
                                    msg[10];

           } else if (mm->metype == 19 && mm->mesub >= 1 && mm->mesub <= 4) {
               /* Airborne Velocity Message */
               if (mm->mesub == 1 || mm->mesub == 2) {
                   mm->ew_dir = (msg[5]&4) >> 2;
                   mm->ew_velocity = ((msg[5]&3) << 8) | msg[6];
                   mm->ns_dir = (msg[7]&0x80) >> 7;
                   mm->ns_velocity = ((msg[7]&0x7f) << 3) | ((msg[8]&0xe0) >> 5);
                   mm->vert_rate_source = (msg[8]&0x10) >> 4;
                   mm->vert_rate_sign = (msg[8]&0x8) >> 3;
                   mm->vert_rate = ((msg[8]&7) << 6) | ((msg[9]&0xfc) >> 2);
                   /* Compute velocity and angle from the two speed
                    * components. */
                   mm->velocity = sqrt(mm->ns_velocity*mm->ns_velocity+
                                       mm->ew_velocity*mm->ew_velocity);
                   if (mm->velocity) {
                       int ewv = mm->ew_velocity;
                       int nsv = mm->ns_velocity;

                       if (mm->ew_dir) ewv *= -1;
                       if (mm->ns_dir) nsv *= -1;
                       mm->heading = atan2(ewv,nsv);


                   } else {
                       mm->heading = 0.0F;
                   }
               } else if (mm->mesub == 3 || mm->mesub == 4) {
                   mm->heading_is_valid = msg[5] & (1<<2);
                   mm->heading = (360.0/128.0) * float(((msg[5] & 3) << 5) |
                                                 (msg[6] >> 3));
               }
           }
           xQueueSend(messageQueue, mm, portMAX_DELAY);
       }
       mm->phase_corrected = 0; /* Set to 1 by the caller if needed. */

    return;
}
void ADS_BDecoder::DetectMessage(MagnitudeVectorType& magnitude)
{
	unsigned char bits[MODES_LONG_MSG_BITS];
	unsigned char msg[MODES_LONG_MSG_BITS/2];
	uint16_t aux[MODES_LONG_MSG_BITS*2];
	uint16_t* m = magnitude.data();
	uint32_t j;
	int use_correction = 0;

	    /* The Mode S preamble is made of impulses of 0.5 microseconds at
	     * the following time offsets:
	     *
	     * 0   - 0.5 usec: first impulse.
	     * 1.0 - 1.5 usec: second impulse.
	     * 3.5 - 4   usec: third impulse.
	     * 4.5 - 5   usec: last impulse.
	     *
	     * Since we are sampling at 2 Mhz every sample in our magnitude vector
	     * is 0.5 usec, so the preamble will look like this, assuming there is
	     * an impulse at offset 0 in the array:
	     *
	     * 0   -----------------
	     * 1   -
	     * 2   ------------------
	     * 3   --
	     * 4   -
	     * 5   --
	     * 6   -
	     * 7   ------------------
	     * 8   --
	     * 9   -------------------
	     */
	    for (j = 0; j < magnitude.size() - MODES_FULL_LEN*2; j++) {
	        int low, high, delta, i, errors;
	        int good_message = 0;

	        if (use_correction) goto good_preamble; /* We already checked it. */

	        /* First check of relations between the first 10 samples
	         * representing a valid preamble. We don't even investigate further
	         * if this simple test is not passed. */
	        if (!(m[j] > m[j+1] &&
	            m[j+1] < m[j+2] &&
	            m[j+2] > m[j+3] &&
	            m[j+3] < m[j] &&
	            m[j+4] < m[j] &&
	            m[j+5] < m[j] &&
	            m[j+6] < m[j] &&
	            m[j+7] > m[j+8] &&
	            m[j+8] < m[j+9] &&
	            m[j+9] > m[j+6]))
	        {
	            continue;
	        }

	        /* The samples between the two spikes must be < than the average
	         * of the high spikes level. We don't test bits too near to
	         * the high levels as signals can be out of phase so part of the
	         * energy can be in the near samples. */
	        high = (m[j]+m[j+2]+m[j+7]+m[j+9])/6;
	        if (m[j+4] >= high ||
	            m[j+5] >= high)
	        {
	            continue;
	        }

	        /* Similarly samples in the range 11-14 must be low, as it is the
	         * space between the preamble and real data. Again we don't test
	         * bits too near to high levels, see above. */
	        if (m[j+11] >= high ||
	            m[j+12] >= high ||
	            m[j+13] >= high ||
	            m[j+14] >= high)
	        {
	            continue;
	        }

	good_preamble:
	        /* If the previous attempt with this message failed, retry using
	         * magnitude correction. */
	        if (use_correction) {
	            memcpy(aux,m+j+MODES_PREAMBLE_US*2,sizeof(aux));
	            if (j && DetectOutOfPhase(m+j)) {
	                ApplyPhaseCorrection(m+j);
	            }
	            /* TODO ... apply other kind of corrections. */
	        }

	        /* Decode all the next 112 bits, regardless of the actual message
	         * size. We'll check the actual message type later. */
	        errors = 0;
	        for (i = 0; i < MODES_LONG_MSG_BITS*2; i += 2) {
	            low = m[j+i+MODES_PREAMBLE_US*2];
	            high = m[j+i+MODES_PREAMBLE_US*2+1];
	            delta = low-high;
	            if (delta < 0) delta = -delta;

	            if (i > 0 && delta < 256) {
	                bits[i/2] = bits[i/2-1];
	            } else if (low == high) {
	                /* Checking if two adiacent samples have the same magnitude
	                 * is an effective way to detect if it's just random noise
	                 * that was detected as a valid preamble. */
	                bits[i/2] = 2; /* error */
	                if (i < MODES_SHORT_MSG_BITS*2) errors++;
	            } else if (low > high) {
	                bits[i/2] = 1;
	            } else {
	                /* (low < high) for exclusion  */
	                bits[i/2] = 0;
	            }
	        }

	        /* Restore the original message if we used magnitude correction. */
	        if (use_correction)
	            memcpy(m+j+MODES_PREAMBLE_US*2,aux,sizeof(aux));

	        /* Pack bits into bytes */
	        for (i = 0; i < MODES_LONG_MSG_BITS; i += 8) {
	            msg[i/8] =
	                bits[i]<<7 |
	                bits[i+1]<<6 |
	                bits[i+2]<<5 |
	                bits[i+3]<<4 |
	                bits[i+4]<<3 |
	                bits[i+5]<<2 |
	                bits[i+6]<<1 |
	                bits[i+7];
	        }

	        int msgtype = msg[0]>>3;
	        int msglen = MessageLenByType(msgtype)/8;

	        /* Last check, high and low bits are different enough in magnitude
	         * to mark this as real message and not just noise? */
	        delta = 0;
	        for (i = 0; i < msglen*8*2; i += 2) {
	            delta += abs(m[j+i+MODES_PREAMBLE_US*2]-
	                         m[j+i+MODES_PREAMBLE_US*2+1]);
	        }
	        delta /= msglen*4;

	        /* Filter for an average delta of three is small enough to let almost
	         * every kind of message to pass, but high enough to filter some
	         * random noise. */
	        if (delta < 10*255) {
	            use_correction = 0;
	            continue;
	        }
        /* If we reached this point, and error is zero, we are very likely
         * with a Mode S message in our hands, but it may still be broken
         * and CRC may not be correct. This is handled by the next layer. */
        if (errors == 0) {
        	ADS_BMessage mm;
        	memcpy(mm.msg,msg,MODES_LONG_MSG_BYTES);
            /* Decode the received message and update statistics */
            DecodeMessage(&mm);


            /* Skip this message if we are sure it's fine. */
            if (mm.crcok) {

                j += (MODES_PREAMBLE_US+(msglen*8))*2;
                good_message = 1;
                if (use_correction)
                    mm.phase_corrected = 1;
            }

            /* Pass data to the next layer */

        }
        /* Retry with phase correction if possible. */
        if (!good_message && !use_correction) {
            j--;
            use_correction = true;
        } else {
            use_correction = false;
        }
    }
}
