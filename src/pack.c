
/********************************* Includes **********************************/
#include "pack.h"
#ifdef PLATFORM_BYTE_ORDER
#include <string.h>
#endif

/**************************** Constants and Types ****************************/

/********************************* Variables *********************************/

/************************* Local Function Prototypes *************************/

/***************************** Exported Functions ****************************/

void packLE64(_PACK_UINT8* p, _PACK_UINT64 value, _PACK_UINT8 u8Size); //forces data to be 64-bits (8 bytes), even on 32-bit machines
_PACK_UINT64 unpackLE64(const _PACK_UINT8* p, _PACK_UINT8 u8Size); //forces data to be 64-bits (8 bytes), even on 32-bit machines

void packBE(_PACK_UINT8* p, _PACK_UINT32 value, _PACK_UINT8 u8Size)
{
   if ((u8Size > 0) && (u8Size <= sizeof(_PACK_UINT32)))
   {
#if defined(PLATFORM_BYTE_ORDER) && (PLATFORM_BYTE_ORDER==PLATFORM_BIG_ENDIAN)
      memcpy(p, &value, u8Size);
#else
      register _PACK_UINT32 tmp = value;
      p += (u8Size - 1);
      while (u8Size > 0)
      {
         *(p--) = (_PACK_UINT8)tmp;
         tmp = tmp >> 8;
         u8Size--;
      }
#endif
   }
}


void packLE(_PACK_UINT8* p, _PACK_UINT32 value, _PACK_UINT8 u8Size)
{
   if ((u8Size > 0) && (u8Size <= sizeof(_PACK_UINT32)))
   {
#if defined(PLATFORM_BYTE_ORDER) && (PLATFORM_BYTE_ORDER==PLATFORM_LITTLE_ENDIAN)
      memcpy(p, &value, u8Size);
#else

      register _PACK_UINT32 tmp = value;
      while (u8Size > 0)
      {
         *(p++) = (_PACK_UINT8)tmp;
         tmp = tmp >> 8;
         u8Size--;
      }
#endif
   }
}

_PACK_UINT32 unpackBE(const _PACK_UINT8* p, _PACK_UINT8 u8Size)
{
   if ((u8Size > 0) && (u8Size <= sizeof(_PACK_UINT32)))
   {
#if defined(PLATFORM_BYTE_ORDER) && (PLATFORM_BYTE_ORDER==PLATFORM_BIG_ENDIAN)
      _PACK_UINT32 value = 0u;
      memcpy(&value, p, u8Size);
      return value;
#else
      register _PACK_UINT32 tmp = 0;
      while (u8Size > 0)
      {
         tmp = (tmp << 8) | *(p++);
         u8Size--;
      }
      return tmp;
#endif
   }
   return 0;
}


_PACK_UINT32 unpackLE(const _PACK_UINT8* p, _PACK_UINT8 u8Size)
{
   if ((u8Size > 0) && (u8Size <= sizeof(_PACK_UINT32)))
   {
#if defined(PLATFORM_BYTE_ORDER) && (PLATFORM_BYTE_ORDER==PLATFORM_LITTLE_ENDIAN)
      _PACK_UINT32 value = 0u;
      memcpy(&value, p, u8Size);
      return value;
#else
      register _PACK_UINT32 tmp = 0;
      p += (u8Size - 1);
      while (u8Size > 0)
      {
         tmp = (tmp << 8) | *(p--);
         u8Size--;
      }
      return tmp;
#endif
   }
   return 0;
}

void packLE64(_PACK_UINT8* p, _PACK_UINT64 value, _PACK_UINT8 u8Size)
{
   if ((u8Size > 0) && (u8Size <= sizeof(_PACK_UINT64)))
   {
#if defined(PLATFORM_BYTE_ORDER) && (PLATFORM_BYTE_ORDER==PLATFORM_LITTLE_ENDIAN)
      memcpy(p, &value, u8Size);
#else

      register _PACK_UINT64 tmp = value;
      while (u8Size > 0)
      {
         *(p++) = (_PACK_UINT8)tmp;
         tmp = tmp >> 8;
         u8Size--;
      }
#endif
   }
}

_PACK_UINT64 unpackLE64(const _PACK_UINT8* p, _PACK_UINT8 u8Size)
{
   if ((u8Size > 0) && (u8Size <= sizeof(_PACK_UINT64)))
   {
#if defined(PLATFORM_BYTE_ORDER) && (PLATFORM_BYTE_ORDER==PLATFORM_LITTLE_ENDIAN)
      _PACK_UINT64 value = 0u;
      memcpy(&value, p, u8Size);
      return value;
#else
      _PACK_UINT64 tmp = 0;
      p += (u8Size - 1);
      while (u8Size > 0)
      {
         tmp = (tmp << 8) | *(p--);
         u8Size--;
      }
      return tmp;
#endif
   }
   return 0;
}

/****************************** Local Functions ******************************/

