/* Copyright 2017 The ChromiumOS Authors
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * Tests SHA256 implementation.
 */

#include "console.h"
#include "common.h"
#include "sha256.h"
#include "test_util.h"
#include "util.h"

/* Short Msg from NIST FIPS 180-4 (Len = 8) */
static const uint8_t sha256_8_input[] = {
	0xd3
};
static const uint8_t sha256_8_output[SHA256_DIGEST_SIZE] = {
	0x28, 0x96, 0x9c, 0xdf, 0xa7, 0x4a, 0x12, 0xc8, 0x2f, 0x3b, 0xad, 0x96,
	0x0b, 0x0b, 0x00, 0x0a, 0xca, 0x2a, 0xc3, 0x29, 0xde, 0xea, 0x5c, 0x23,
	0x28, 0xeb, 0xc6, 0xf2, 0xba, 0x98, 0x02, 0xc1
};

/* Short Msg from NIST FIPS 180-4 (Len = 72) */
static const uint8_t sha256_72_input[] = {
	0x33, 0x34, 0xc5, 0x80, 0x75, 0xd3, 0xf4, 0x13, 0x9e
};
static const uint8_t sha256_72_output[SHA256_DIGEST_SIZE] = {
	0x07, 0x8d, 0xa3, 0xd7, 0x7e, 0xd4, 0x3b, 0xd3, 0x03, 0x7a, 0x43, 0x3f,
	0xd0, 0x34, 0x18, 0x55, 0x02, 0x37, 0x93, 0xf9, 0xaf, 0xd0, 0x8b, 0x4b,
	0x08, 0xea, 0x1e, 0x55, 0x97, 0xce, 0xef, 0x20
};

/* Long Msg from NIST FIPS 180-4 (Len = 2888) */
static const uint8_t sha256_2888_input[] = {
	0x82, 0x82, 0x96, 0x90, 0xaa, 0x37, 0x33, 0xc6, 0x2b, 0x90, 0xd3, 0x29,
	0x78, 0x86, 0x95, 0x2f, 0xc1, 0xdc, 0x47, 0x3d, 0x67, 0xbb, 0x7d, 0x6b,
	0xb2, 0x99, 0xe0, 0x88, 0xc6, 0x5f, 0xc9, 0x5e, 0xd3, 0xca, 0x0f, 0x36,
	0x8d, 0x11, 0x1d, 0x9f, 0xdc, 0xc9, 0x47, 0x6c, 0xd4, 0x06, 0x5e, 0xfc,
	0xe7, 0xc4, 0x81, 0xbe, 0x59, 0x85, 0x37, 0xf3, 0xf5, 0x3b, 0xbb, 0xb6,
	0xff, 0x67, 0x97, 0x3a, 0x69, 0x83, 0x74, 0x54, 0x49, 0x9e, 0x31, 0x39,
	0x8b, 0x46, 0x32, 0x88, 0xe3, 0xaa, 0xfb, 0x8b, 0x06, 0x00, 0xfd, 0xba,
	0x1a, 0x25, 0xaf, 0x80, 0x6b, 0x83, 0xe1, 0x42, 0x5f, 0x38, 0x4e, 0x9e,
	0xac, 0x75, 0x70, 0xf0, 0xc8, 0x23, 0x98, 0x1b, 0xa2, 0xcd, 0x3d, 0x86,
	0x8f, 0xba, 0x94, 0x64, 0x87, 0x59, 0x62, 0x39, 0x91, 0xe3, 0x0f, 0x99,
	0x7c, 0x3b, 0xfb, 0x33, 0xd0, 0x19, 0x15, 0x0f, 0x04, 0x67, 0xa9, 0x14,
	0xf1, 0xeb, 0x79, 0xcd, 0x87, 0x27, 0x10, 0x6d, 0xbf, 0x7d, 0x53, 0x10,
	0xd0, 0x97, 0x59, 0x43, 0xa6, 0x06, 0x7c, 0xc7, 0x90, 0x29, 0xb0, 0x92,
	0x39, 0x51, 0x14, 0x17, 0xd9, 0x22, 0xc7, 0xc7, 0xac, 0x3d, 0xfd, 0xd8,
	0xa4, 0x1c, 0x52, 0x45, 0x5b, 0x3c, 0x5e, 0x16, 0x4b, 0x82, 0x89, 0xe1,
	0x41, 0xd8, 0x20, 0x91, 0x0f, 0x17, 0xa9, 0x66, 0x81, 0x29, 0x74, 0x3d,
	0x93, 0x6f, 0x73, 0x12, 0xe1, 0x60, 0x4b, 0xc3, 0x5f, 0x73, 0xab, 0x16,
	0x4a, 0x3f, 0xdd, 0xfe, 0x5f, 0xe1, 0x9b, 0x1a, 0x4a, 0x9f, 0x23, 0x7f,
	0x61, 0xcb, 0x8e, 0xb7, 0x92, 0xe9, 0x5d, 0x09, 0x9a, 0x14, 0x55, 0xfb,
	0x78, 0x9d, 0x8d, 0x16, 0x22, 0xf6, 0xc5, 0xe9, 0x76, 0xce, 0xf9, 0x51,
	0x73, 0x7e, 0x36, 0xf7, 0xa9, 0xa4, 0xad, 0x19, 0xee, 0x0d, 0x06, 0x8e,
	0x53, 0xd9, 0xf6, 0x04, 0x57, 0xd9, 0x14, 0x8d, 0x5a, 0x3c, 0xe8, 0x5a,
	0x54, 0x6b, 0x45, 0xc5, 0xc6, 0x31, 0xd9, 0x95, 0xf1, 0x1f, 0x03, 0x7e,
	0x47, 0x2f, 0xe4, 0xe8, 0x1f, 0xa7, 0xb9, 0xf2, 0xac, 0x40, 0x68, 0xb5,
	0x30, 0x88, 0x58, 0xcd, 0x6d, 0x85, 0x86, 0x16, 0x5c, 0x9b, 0xd6, 0xb3,
	0x22, 0xaf, 0xa7, 0x55, 0x40, 0x8d, 0xa9, 0xb9, 0x0a, 0x87, 0xf3, 0x73,
	0x5a, 0x5f, 0x50, 0xeb, 0x85, 0x68, 0xda, 0xa5, 0x8e, 0xe7, 0xcb, 0xc5,
	0x9a, 0xbf, 0x8f, 0xd2, 0xa4, 0x4e, 0x1e, 0xba, 0x72, 0x92, 0x88, 0x16,
	0xc8, 0x90, 0xd1, 0xb0, 0xdb, 0xf6, 0x00, 0x42, 0x08, 0xff, 0x73, 0x81,
	0xc6, 0x97, 0x75, 0x5a, 0xda, 0xc0, 0x13, 0x7c, 0xca, 0x34, 0x2b, 0x16,
	0x93
};
static const uint8_t sha256_2888_output[SHA256_DIGEST_SIZE] = {
	0x5f, 0x4e, 0x16, 0xa7, 0x2d, 0x6c, 0x98, 0x57, 0xda, 0x0b, 0xa0, 0x09,
	0xcc, 0xac, 0xd4, 0xf2, 0x6d, 0x7f, 0x6b, 0xf6, 0xc1, 0xb7, 0x8a, 0x2e,
	0xd3, 0x5e, 0x68, 0xfc, 0xb1, 0x5b, 0x8e, 0x40
};

/* HMAC short key (40 bytes) from NIST FIPS 198-1 (Count = 34) */
static const uint8_t hmac_short_msg[] = {
	0x49, 0x53, 0x40, 0x8b, 0xe3, 0xdd, 0xde, 0x42, 0x52, 0x1e, 0xb6, 0x25,
	0xa3, 0x7a, 0xf0, 0xd2, 0xcf, 0x9e, 0xd1, 0x84, 0xf5, 0xb6, 0x27, 0xe5,
	0xe7, 0xe0, 0xe8, 0x24, 0xe8, 0xe1, 0x16, 0x48, 0xb4, 0x18, 0xe5, 0xc4,
	0xc1, 0xb0, 0x20, 0x4b, 0xc5, 0x19, 0xc9, 0xe5, 0x78, 0xb8, 0x00, 0x43,
	0x9b, 0xdd, 0x25, 0x4f, 0x39, 0xf6, 0x41, 0x08, 0x2d, 0x03, 0xa2, 0x8d,
	0xe4, 0x4a, 0xc6, 0x77, 0x64, 0x4c, 0x7b, 0x6c, 0x8d, 0xf7, 0x43, 0xf2,
	0x9f, 0x1d, 0xfd, 0x80, 0xfd, 0x25, 0xc2, 0xdb, 0x31, 0x01, 0x0e, 0xa0,
	0x2f, 0x60, 0x20, 0x1c, 0xde, 0x24, 0xa3, 0x64, 0xd4, 0x16, 0x8d, 0xa2,
	0x61, 0xd8, 0x48, 0xae, 0xd0, 0x1c, 0x10, 0xde, 0xe9, 0x14, 0x9c, 0x1e,
	0xbb, 0x29, 0x00, 0x43, 0x98, 0xf0, 0xd2, 0x9c, 0x60, 0x5a, 0x8b, 0xca,
	0x03, 0x2b, 0x31, 0xd2, 0x41, 0xad, 0x33, 0x71
};
static const uint8_t hmac_short_key[] = {
	0x9d, 0xa0, 0xc1, 0x14, 0x68, 0x2f, 0x82, 0xc1, 0xd1, 0xe9, 0xb5, 0x44,
	0x30, 0x58, 0x0b, 0x9c, 0x56, 0x94, 0x89, 0xca, 0x16, 0xb9, 0x2e, 0xe1,
	0x04, 0x98, 0xd5, 0x5d, 0x7c, 0xad, 0x5d, 0xb5, 0xe6, 0x52, 0x06, 0x34,
	0x39, 0x31, 0x1e, 0x04
};
static const uint8_t hmac_short_output[] = {
	0xcd, 0xea, 0xcf, 0xce, 0xbf, 0x46, 0xcc, 0x9d, 0x7e, 0x4d, 0x41, 0x75,
	0xe5, 0xd8, 0xd2, 0x67, 0xc2, 0x3a, 0x64, 0xcd, 0xe8, 0x3e, 0x86, 0x7e,
	0x50, 0x01, 0xec, 0xf2, 0x6f, 0xbd, 0x30, 0xd2
};

/* HMAC medium key (64 bytes) from NIST FIPS 198-1 (Count = 120) */
static const uint8_t hmac_medium_msg[] = {
	0xed, 0x4f, 0x26, 0x9a, 0x88, 0x51, 0xeb, 0x31, 0x54, 0x77, 0x15, 0x16,
	0xb2, 0x72, 0x28, 0x15, 0x52, 0x00, 0x77, 0x80, 0x49, 0xb2, 0xdc, 0x19,
	0x63, 0xf3, 0xac, 0x32, 0xba, 0x46, 0xea, 0x13, 0x87, 0xcf, 0xbb, 0x9c,
	0x39, 0x15, 0x1a, 0x2c, 0xc4, 0x06, 0xcd, 0xc1, 0x3c, 0x3c, 0x98, 0x60,
	0xa2, 0x7e, 0xb0, 0xb7, 0xfe, 0x8a, 0x72, 0x01, 0xad, 0x11, 0x55, 0x2a,
	0xfd, 0x04, 0x1e, 0x33, 0xf7, 0x0e, 0x53, 0xd9, 0x7c, 0x62, 0xf1, 0x71,
	0x94, 0xb6, 0x61, 0x17, 0x02, 0x8f, 0xa9, 0x07, 0x1c, 0xc0, 0xe0, 0x4b,
	0xd9, 0x2d, 0xe4, 0x97, 0x2c, 0xd5, 0x4f, 0x71, 0x90, 0x10, 0xa6, 0x94,
	0xe4, 0x14, 0xd4, 0x97, 0x7a, 0xbe, 0xd7, 0xca, 0x6b, 0x90, 0xba, 0x61,
	0x2d, 0xf6, 0xc3, 0xd4, 0x67, 0xcd, 0xed, 0x85, 0x03, 0x25, 0x98, 0xa4,
	0x85, 0x46, 0x80, 0x4f, 0x9c, 0xf2, 0xec, 0xfe
};
static const uint8_t hmac_medium_key[] = {
	0x99, 0x28, 0x68, 0x50, 0x4d, 0x25, 0x64, 0xc4, 0xfb, 0x47, 0xbc, 0xbd,
	0x4a, 0xe4, 0x82, 0xd8, 0xfb, 0x0e, 0x8e, 0x56, 0xd7, 0xb8, 0x18, 0x64,
	0xe6, 0x19, 0x86, 0xa0, 0xe2, 0x56, 0x82, 0xda, 0xeb, 0x5b, 0x50, 0x17,
	0x7c, 0x09, 0x5e, 0xdc, 0x9e, 0x97, 0x1d, 0xa9, 0x5c, 0x32, 0x10, 0xc3,
	0x76, 0xe7, 0x23, 0x36, 0x5a, 0xc3, 0x3d, 0x1b, 0x4f, 0x39, 0x18, 0x17,
	0xf4, 0xc3, 0x51, 0x24
};
static const uint8_t hmac_medium_output[] = {
	0x2f, 0x83, 0x21, 0xf4, 0x16, 0xb9, 0xbb, 0x24, 0x9f, 0x11, 0x3b, 0x13,
	0xfc, 0x12, 0xd7, 0x0e, 0x16, 0x68, 0xdc, 0x33, 0x28, 0x39, 0xc1, 0x0d,
	0xaa, 0x57, 0x17, 0x89, 0x6c, 0xb7, 0x0d, 0xdf
};

static int test_sha256(const uint8_t *input, int input_len,
		       const uint8_t *output)
{
	struct sha256_ctx ctx;
	uint8_t *tmp;
	int i;

	/* Basic test */
	SHA256_init(&ctx);
	SHA256_update(&ctx, input, input_len);
	tmp = SHA256_final(&ctx);

	if (memcmp(tmp, output, SHA256_DIGEST_SIZE) != 0) {
		ccprintf("SHA256 test failed\n");
		return 0;
	}

	/* Splitting the input string in chunks of 1 byte also works. */
	SHA256_init(&ctx);
	for (i = 0; i < input_len; i++)
		SHA256_update(&ctx, &input[i], 1);
	tmp = SHA256_final(&ctx);

	if (memcmp(tmp, output, SHA256_DIGEST_SIZE) != 0) {
		ccprintf("SHA256 test failed (1-byte chunks)\n");
		return 0;
	}

	return 1;
}

static int test_hmac(const uint8_t *key, int key_len,
		     const uint8_t *input, int input_len,
		     const uint8_t *output)
{
	uint8_t tmp[SHA256_DIGEST_SIZE];

	hmac_SHA256(tmp, key, key_len, input, input_len);

	if (memcmp(tmp, output, SHA256_DIGEST_SIZE) != 0) {
		ccprintf("hmac_SHA256 test failed\n");
		return 0;
	}

	return 1;
}

void run_test(void)
{
	ccprintf("Testing short message (8 bytes)\n");
	if (!test_sha256(sha256_8_input, sizeof(sha256_8_input),
			 sha256_8_output)) {
		test_fail();
		return;
	}

	ccprintf("Testing short message (72 bytes)\n");
	if (!test_sha256(sha256_72_input, sizeof(sha256_72_input),
			 sha256_72_output)) {
		test_fail();
		return;
	}

	ccprintf("Testing long message (2888 bytes)\n");
	if (!test_sha256(sha256_2888_input, sizeof(sha256_2888_input),
			 sha256_2888_output)) {
		test_fail();
		return;
	}

	ccprintf("HMAC: Testing short key\n");
	if (!test_hmac(hmac_short_key, sizeof(hmac_short_key),
		       hmac_short_msg, sizeof(hmac_short_msg),
		       hmac_short_output)) {
		test_fail();
		return;
	}

	ccprintf("HMAC: Testing medium key\n");
	if (!test_hmac(hmac_medium_key, sizeof(hmac_medium_key),
		       hmac_medium_msg, sizeof(hmac_medium_msg),
		       hmac_medium_output)) {
		test_fail();
		return;
	}

	/*
	 * Note: Our HMAC implementation does not support longer than
	 * 64 bytes keys.
	 */

	test_pass();
}
