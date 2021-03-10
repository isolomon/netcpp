#ifndef __CRC16_CCITT_H
#define __CRC16_CCITT_H

typedef unsigned char  byte;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

// 16 bit CRC with polynomial x^16+x^12+x^5+1 (CRC-CCITT)
uint16_t crc16_ccitt(uint16_t crc_start, const byte *s, int len);

uint16_t CalculateCrc(const byte *data, uint32_t len);

#endif __CRC16_CCITT_H
