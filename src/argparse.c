/*****************************************************************************
* \file      argparse.c
* \author    Conny Gustafsson
* \date      2020-04-10
* \brief     Platform-independent argument parser
*
* Copyright (c) 2020 Conny Gustafsson
* Permission is hereby granted, free of charge, to any person obtaining a copy of
* this software and associated documentation files (the "Software"), to deal in
* the Software without restriction, including without limitation the rights to
* use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
* the Software, and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions:

* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.

* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
* FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
* COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
* IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
******************************************************************************/
//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "argparse.h"
#include "adt_str.h"

//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
#define ARGPARSE_DESTROY_VARS()  adt_str_destroy(&str);\
                                 adt_str_destroy(&short_name);\
                                 adt_str_destroy(&long_name)


//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

argparse_result_t argparse_exec(int argc, const char **argv, argparse_callback_fn *callback)
{
   if ( argc < 2)
   {
      return ARGPARSE_SUCCESS; //Nothing to parse
   }
   if ( (argv != 0) && (callback != 0) )
   {
      int arg_id;
      adt_str_t short_name;
      adt_str_t long_name;
      adt_str_t str;
      bool has_short_name = false;
      bool has_long_name = false;
      adt_str_create(&str);
      adt_str_create(&short_name);
      adt_str_create(&long_name);

      for (arg_id = 1; arg_id < argc; arg_id++)
      {
         char c;
         int arg_len;
         unsigned int minus_count = 0u;
         unsigned int equal_count = 0u;
         const char *p = argv[arg_id];
         adt_str_clear(&str);
         for(c= *p; c == '-'; c = *(++p))
         {
            minus_count++;
         }
         for(c = *p; (c != '\0') && (c != '='); c = *(++p))
         {
            adt_str_push(&str, c);
         }
         if (c == '=')
         {
            equal_count++;
            p++;
         }
         arg_len = adt_str_length(&str);
         if ( (arg_len > 0) && (minus_count <= 2u))
         {
            argparse_result_t result = ARGPARSE_PARSE_ERROR;
            switch(minus_count)
            {
            case 0u:
               if (has_short_name)
               {
                  has_short_name = false;
                  result = callback(adt_str_cstr(&short_name), (const char*) 0, adt_str_cstr(&str));
               }
               else if (has_long_name)
               {
                  has_long_name = false;
                  result = callback((const char*) 0, adt_str_cstr(&long_name), adt_str_cstr(&str));
               }
               else
               {
                  //positional argument
                  result = callback((const char*) 0, (const char*) 0, adt_str_cstr(&str));
               }
               break;
            case 1u:
               if (equal_count == 0)
               {
                  result = callback(adt_str_cstr(&str), (const char*) 0, (const char*) 0);
                  if (result == ARGPARSE_NEED_VALUE)
                  {
                     adt_error_t rc = adt_str_set(&short_name, &str);
                     has_short_name = true;
                     if (rc != ADT_NO_ERROR)
                     {
                        ARGPARSE_DESTROY_VARS();
                        return ARGPARSE_MEM_ERROR;
                     }
                  }
               }
               else if (equal_count == 1)
               {
                  adt_error_t rc = adt_str_set(&short_name, &str);
                  if (rc != ADT_NO_ERROR)
                  {
                     ARGPARSE_DESTROY_VARS();
                     return ARGPARSE_MEM_ERROR;
                  }
                  adt_str_clear(&str);
                  for(c = *p; (c != '\0') ; c = *(++p))
                  {
                     adt_str_push(&str, c);
                  }
                  result = callback(adt_str_cstr(&short_name), (const char*) 0, adt_str_cstr(&str));
               }
               else
               {
                  //parse error already set as default
               }
               break;
            case 2u:
               if (equal_count==0)
               {
                  result = callback((const char*) 0, adt_str_cstr(&str), (const char*) 0);
                  if (result == ARGPARSE_NEED_VALUE)
                  {
                     adt_error_t rc = adt_str_set(&long_name, &str);
                     has_long_name = true;
                     if (rc != ADT_NO_ERROR)
                     {
                        ARGPARSE_DESTROY_VARS();
                        return ARGPARSE_MEM_ERROR;
                     }
                  }
               }
               else if (equal_count == 1)
               {
                  adt_error_t rc = adt_str_set(&long_name, &str);
                  if (rc != ADT_NO_ERROR)
                  {
                     ARGPARSE_DESTROY_VARS();
                     return ARGPARSE_MEM_ERROR;
                  }
                  adt_str_clear(&str);
                  for(c = *p; (c != '\0') ; c = *(++p))
                  {
                     adt_str_push(&str, c);
                  }
                  result = callback((const char*) 0, adt_str_cstr(&long_name), adt_str_cstr(&str));
               }
               else
               {
                  //parse error already set as default
               }
               break;
            }
            if (result < 0)
            {
               ARGPARSE_DESTROY_VARS();
               return result;
            }
         }
         else if ((arg_len == 0) && (minus_count == 1))
         {
            /* A standalone minus sign is sometimes used as a positional argument to mean
               stdin/stdout instead of a file name. */

            argparse_result_t result;
            if (has_short_name)
            {
               has_short_name = false;
               result = callback(adt_str_cstr(&short_name), (const char*) 0, "-");
            }
            else if (has_long_name)
            {
               has_long_name = false;
               result = callback((const char*) 0, adt_str_cstr(&long_name), "-");
            }
            else
            {
               result = callback((const char*) 0, (const char*) 0, "-");
            }

            if (result < 0)
            {
               ARGPARSE_DESTROY_VARS();
               return result;
            }
         }
         else
         {
            //User gave empty arg or just typed minus sign(s) with nothing following it or typed three or more minus signs.
            ARGPARSE_DESTROY_VARS();
            return ARGPARSE_PARSE_ERROR;
         }
      }
      ARGPARSE_DESTROY_VARS();
      return ARGPARSE_SUCCESS;
   }
   return ARGPARSE_INVALID_ARGUMENT_ERROR;
}
//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////


