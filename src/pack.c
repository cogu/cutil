/*****************************************************************************
* \file      pack.c
* \author    Conny Gustafsson
* \date      2013-05-18
* \brief     Platform-independent integer serialization and deserialization
*
* Copyright (c) 2013-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "pack.h"
#ifdef PLATFORM_BYTE_ORDER
#include <string.h>
#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

void cutil_pack_be(_PACK_UINT8 *p, _PACK_UINT32 value, _PACK_UINT8 size)
{
   if ((size > 0) && (size <= sizeof(_PACK_UINT32)))
   {
#if defined(PLATFORM_BYTE_ORDER) && (PLATFORM_BYTE_ORDER == PLATFORM_BIG_ENDIAN)
      memcpy(p, &value, size);
#else
      _PACK_UINT32 tmp = value;
      p += (size - 1);
      while (size > 0)
      {
         *(p--) = (_PACK_UINT8) tmp;
         tmp = tmp >> 8;
         size--;
      }
#endif
   }
}

void cutil_pack_le(_PACK_UINT8 *p, _PACK_UINT32 value, _PACK_UINT8 size)
{
   if ((size > 0) && (size <= sizeof(_PACK_UINT32)))
   {
#if defined(PLATFORM_BYTE_ORDER) && (PLATFORM_BYTE_ORDER == PLATFORM_LITTLE_ENDIAN)
      memcpy(p, &value, size);
#else
      _PACK_UINT32 tmp = value;
      while (size > 0)
      {
         *(p++) = (_PACK_UINT8) tmp;
         tmp = tmp >> 8;
         size--;
      }
#endif
   }
}

_PACK_UINT32 cutil_unpack_be(const _PACK_UINT8 *p, _PACK_UINT8 size)
{
   if ((size > 0) && (size <= sizeof(_PACK_UINT32)))
   {
#if defined(PLATFORM_BYTE_ORDER) && (PLATFORM_BYTE_ORDER == PLATFORM_BIG_ENDIAN)
      _PACK_UINT32 value = 0u;
      memcpy(&value, p, size);
      return value;
#else
      _PACK_UINT32 tmp = 0;
      while (size > 0)
      {
         tmp = (tmp << 8) | *(p++);
         size--;
      }
      return tmp;
#endif
   }
   return 0;
}

_PACK_UINT32 cutil_unpack_le(const _PACK_UINT8 *p, _PACK_UINT8 size)
{
   if ((size > 0) && (size <= sizeof(_PACK_UINT32)))
   {
#if defined(PLATFORM_BYTE_ORDER) && (PLATFORM_BYTE_ORDER == PLATFORM_LITTLE_ENDIAN)
      _PACK_UINT32 value = 0u;
      memcpy(&value, p, size);
      return value;
#else
      _PACK_UINT32 tmp = 0;
      p += (size - 1);
      while (size > 0)
      {
         tmp = (tmp << 8) | *(p--);
         size--;
      }
      return tmp;
#endif
   }
   return 0;
}

void cutil_pack_le64(_PACK_UINT8 *p, _PACK_UINT64 value, _PACK_UINT8 size)
{
   if ((size > 0) && (size <= sizeof(_PACK_UINT64)))
   {
#if defined(PLATFORM_BYTE_ORDER) && (PLATFORM_BYTE_ORDER == PLATFORM_LITTLE_ENDIAN)
      memcpy(p, &value, size);
#else
      _PACK_UINT64 tmp = value;
      while (size > 0)
      {
         *(p++) = (_PACK_UINT8) tmp;
         tmp = tmp >> 8;
         size--;
      }
#endif
   }
}

_PACK_UINT64 cutil_unpack_le64(const _PACK_UINT8 *p, _PACK_UINT8 size)
{
   if ((size > 0) && (size <= sizeof(_PACK_UINT64)))
   {
#if defined(PLATFORM_BYTE_ORDER) && (PLATFORM_BYTE_ORDER == PLATFORM_LITTLE_ENDIAN)
      _PACK_UINT64 value = 0u;
      memcpy(&value, p, size);
      return value;
#else
      _PACK_UINT64 tmp = 0;
      p += (size - 1);
      while (size > 0)
      {
         tmp = (tmp << 8) | *(p--);
         size--;
      }
      return tmp;
#endif
   }
   return 0;
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
