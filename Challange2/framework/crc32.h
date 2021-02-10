/**
 * Adapted from: http://create.stephan-brumme.com/crc32/
 * Zlib-style license
 *
 ************************************************************
 *                                                          *
 **                 DO NOT EDIT THIS FILE                  **
 ***         YOU DO NOT HAVE TO LOOK IN THIS FILE         ***
 **** IF YOU HAVE QUESTIONS PLEASE DO ASK A TA FOR HELP  ****
 *****                                                  *****
 ************************************************************
 */

#ifndef _CRC32_H_
#define _CRC32_H_

#include <stdlib.h>
#include <cstdint>

const uint32_t Polynomial = 0x04C11DB7;

/// Slicing-By-16
const size_t MaxSlice = 16;
/// forward declaration, table is at the end of this file
extern const uint32_t Crc32Lookup[MaxSlice][256]; // extern is needed to keep compiler happy

/// compute CRC32 (standard algorithm)
uint32_t crc32_1byte(const void*, size_t, uint32_t);
#endif
