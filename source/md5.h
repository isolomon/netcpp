#ifndef __MD5_H_
#define __MD5_H_

typedef unsigned int uint32;

struct MD5Context
{
	uint32 buf[4];
	uint32 bits[2];
	union
    {
		unsigned char in[64];
		uint32 in32[16];
	};
};

// Start MD5 accumulation.  Set bit count to 0 and buffer to mysterious
// initialization constants.
void MD5Init(struct MD5Context *ctx);

// Update context to reflect the concatenation of another buffer full of bytes.
void MD5Update(struct MD5Context *ctx, unsigned char const *buf, unsigned len);

// Final wrap up - pad to 64-byte boundary with the bit pattern
// 1 0* (64-bit count of bits processed, MSB-first)
void MD5Final(unsigned char digest[16], struct MD5Context *ctx);

// Calculate and store in 'output' the MD5 digest of 'len' bytes at
// 'input'. 'output' must have enough space to hold 16 bytes.
void MD5 (unsigned char *input, int len, unsigned char output[16]);

#endif//__MD5_H_
