#ifndef PACK_H
#define PACK_H
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

#define _PACK_BASE_TYPE _PACK_UINT32

#define packU8(p,v) packBE(p,v,1);p+=1
#define packU16BE(p,v) packBE(p,v,2);p+=2
#define packU32BE(p,v) packBE(p,v,4);p+=4
#define packU16LE(p,v) packLE(p,v,2);p+=2
#define packU32LE(p,v) packLE(p,v,4);p+=4

#define unpackU8(p) (_PACK_UINT8) unpackBE(p,1);p+=1
#define unpackU16BE(p) (_PACK_UINT16) unpackBE(p,2);p+=2
#define unpackU32BE(p) (_PACK_UINT32) unpackBE(p,4);p+=4
#define unpackU16LE(p) (_PACK_UINT16) unpackLE(p,2);p+=2
#define unpackU32LE(p) (_PACK_UINT32) unpackLE(p,4);p+=4

/***************** Public Function Declarations *******************/
void packBE(_PACK_UINT8* p, _PACK_UINT32 value, _PACK_UINT8 u8Size);
void packLE(_PACK_UINT8* p, _PACK_UINT32 value, _PACK_UINT8 u8Size);
_PACK_UINT32 unpackBE(const _PACK_UINT8* p, _PACK_UINT8 u8Size);
_PACK_UINT32 unpackLE(const _PACK_UINT8* p, _PACK_UINT8 u8Size);
void packLE64(_PACK_UINT8* p, _PACK_UINT64 value, _PACK_UINT8 u8Size); //forces data to be 64-bits (8 bytes), even on 32-bit machines
_PACK_UINT64 unpackLE64(const _PACK_UINT8* p, _PACK_UINT8 u8Size); //forces data to be 64-bits (8 bytes), even on 32-bit machines

//set PLATFORM_BYTE_ORDER to one of below values for performance boost
#define PLATFORM_LITTLE_ENDIAN 0
#define PLATFORM_BIG_ENDIAN    1

#endif //PACK_H

