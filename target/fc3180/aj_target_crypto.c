/**
 * @file
 */
/******************************************************************************
 * Copyright (c) 2013, AllSeen Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for any
 *    purpose with or without fee is hereby granted, provided that the above
 *    copyright notice and this permission notice appear in all copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/

#include "aj_target.h"
#include "aj_crypto.h"

static uint8_t seed[16];
static uint8_t key[16];
/*
 * The host has various ADC's. We are going to accumulate entropy by repeatedly
 * reading the ADC and accumulating the least significant bit or each reading.
 */
int GatherBits(uint8_t *buffer, uint32_t len) {
  int i;
  uint32_t val;

  /*
   * Start accumulating entropy one bit at a time
   */
  struct timeb now;
  ftime(&now);
  //	srand(now.time * 1000 + now.millitm);
  for (i = 0; i < (8 * len); ++i) {
    val = rand();
    buffer[i / 8] ^= ((val & 1) << (i & 7));
  }
  return val;
}

void AJ_RandBytes(uint8_t *buffer, uint32_t len) {
  ///*
  // * On the first call we need to accumulate entropy
  // * for the seed and the key.
  // */
  if (seed[0] == 0) {
    GatherBits(seed, sizeof(seed));
    GatherBits(key, sizeof(key));
  }
  AJ_AES_Enable(key);
  /*
   * This follows the NIST guidelines for using AES as a PRF
   */
  //	srand(seed[rand() % 15]);
  while (len) {
    *buffer = rand() % 256;
    len -= 1;
    buffer += 1;
  }
  AJ_AES_Disable();
}
