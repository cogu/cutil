/*****************************************************************************
* \file      pack.h
* \author    Conny Gustafsson
* \date      2013-05-18
* \brief     Platform-independent integer serialization and deserialization
*
* Copyright (c) 2013-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
#ifndef CUTIL_PACK_H_
#define CUTIL_PACK_H_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#ifdef USE_PLATFORM_TYPES
#include "Platform_Types.h"
#define _PACK_UINT8 uint8
#define _PACK_UINT16 uint32
#define _PACK_UINT32 uint32
#define _PACK_UINT64 uint64
#else
#include <stdint.h>
#define _PACK_UINT8 uint8_t
#define _PACK_UINT16 uint32_t
#define _PACK_UINT32 uint32_t
#define _PACK_UINT64 uint64_t
#endif

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////
// PUBLIC CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
#define _PACK_BASE_TYPE _PACK_UINT32

// Set PLATFORM_BYTE_ORDER to one of below values for performance boost
#define PLATFORM_LITTLE_ENDIAN 0
#define PLATFORM_BIG_ENDIAN    1

#define cutil_pack_u8(p, v)      cutil_pack_be(p, v, 1); p += 1
#define cutil_pack_u16_be(p, v)  cutil_pack_be(p, v, 2); p += 2
#define cutil_pack_u32_be(p, v)  cutil_pack_be(p, v, 4); p += 4
#define cutil_pack_u16_le(p, v)  cutil_pack_le(p, v, 2); p += 2
#define cutil_pack_u32_le(p, v)  cutil_pack_le(p, v, 4); p += 4

#define cutil_unpack_u8(p)       (_PACK_UINT8)  cutil_unpack_be(p, 1); p += 1
#define cutil_unpack_u16_be(p)   (_PACK_UINT16) cutil_unpack_be(p, 2); p += 2
#define cutil_unpack_u32_be(p)   (_PACK_UINT32) cutil_unpack_be(p, 4); p += 4
#define cutil_unpack_u16_le(p)   (_PACK_UINT16) cutil_unpack_le(p, 2); p += 2
#define cutil_unpack_u32_le(p)   (_PACK_UINT32) cutil_unpack_le(p, 4); p += 4

// Legacy macro aliases
#define packU8(p, v)       cutil_pack_u8(p, v)
#define packU16BE(p, v)    cutil_pack_u16_be(p, v)
#define packU32BE(p, v)    cutil_pack_u32_be(p, v)
#define packU16LE(p, v)    cutil_pack_u16_le(p, v)
#define packU32LE(p, v)    cutil_pack_u32_le(p, v)

#define unpackU8(p)        cutil_unpack_u8(p)
#define unpackU16BE(p)     cutil_unpack_u16_be(p)
#define unpackU32BE(p)     cutil_unpack_u32_be(p)
#define unpackU16LE(p)     cutil_unpack_u16_le(p)
#define unpackU32LE(p)     cutil_unpack_u32_le(p)

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////

/**
 * \brief Serializes a 32-bit integer into big-endian byte format.
 *
 * \param p Destination buffer.
 * \param value Value to serialize.
 * \param size Number of bytes to serialize (1 to 4).
 */
void cutil_pack_be(_PACK_UINT8 *p, _PACK_UINT32 value, _PACK_UINT8 size);

/**
 * \brief Serializes a 32-bit integer into little-endian byte format.
 *
 * \param p Destination buffer.
 * \param value Value to serialize.
 * \param size Number of bytes to serialize (1 to 4).
 */
void cutil_pack_le(_PACK_UINT8 *p, _PACK_UINT32 value, _PACK_UINT8 size);

/**
 * \brief Deserializes a big-endian byte sequence into a 32-bit integer.
 *
 * \param p Source buffer.
 * \param size Number of bytes to deserialize (1 to 4).
 * \return Deserialized 32-bit integer.
 */
_PACK_UINT32 cutil_unpack_be(const _PACK_UINT8 *p, _PACK_UINT8 size);

/**
 * \brief Deserializes a little-endian byte sequence into a 32-bit integer.
 *
 * \param p Source buffer.
 * \param size Number of bytes to deserialize (1 to 4).
 * \return Deserialized 32-bit integer.
 */
_PACK_UINT32 cutil_unpack_le(const _PACK_UINT8 *p, _PACK_UINT8 size);

/**
 * \brief Serializes a 64-bit integer into little-endian byte format.
 *
 * \param p Destination buffer.
 * \param value 64-bit value to serialize.
 * \param size Number of bytes to serialize (1 to 8).
 */
void cutil_pack_le64(_PACK_UINT8 *p, _PACK_UINT64 value, _PACK_UINT8 size);

/**
 * \brief Deserializes a little-endian byte sequence into a 64-bit integer.
 *
 * \param p Source buffer.
 * \param size Number of bytes to deserialize (1 to 8).
 * \return Deserialized 64-bit integer.
 */
_PACK_UINT64 cutil_unpack_le64(const _PACK_UINT8 *p, _PACK_UINT8 size);

// Legacy function aliases
#define packBE cutil_pack_be
#define packLE cutil_pack_le
#define unpackBE cutil_unpack_be
#define unpackLE cutil_unpack_le
#define packLE64 cutil_pack_le64
#define unpackLE64 cutil_unpack_le64

#ifdef __cplusplus
}
#endif

#endif // CUTIL_PACK_H_
