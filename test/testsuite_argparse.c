/*****************************************************************************
* \file      testsuite_argparse.c
* \author    Conny Gustafsson
* \date      2020-04-01
* \brief     Unit tests for argparse
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
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "CuTest.h"
#include "argparse.h"
#include "adt_ary.h"
#include "adt_str.h"
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static void test_no_args_shall_succeed(CuTest* tc);
static void test_double_minus_error(CuTest* tc);
static void test_triple_minus_error(CuTest* tc);
static void test_single_minus_positional(CuTest* tc);
static void test_shortname_with_minus_value(CuTest* tc);
static void test_shortname_no_value(CuTest* tc);
static void test_multiple_flags(CuTest* tc);
static void test_shortname_before_value(CuTest* tc);
static void test_shortname_equals_value(CuTest* tc);
static void test_longname_no_value(CuTest* tc);
static void test_longname_before_value(CuTest* tc);
static void test_longname_equals_value(CuTest* tc);
static void test_positional_arguments_only(CuTest* tc);
static void test_cmake_configure_example(CuTest* tc);
static void test_cmake_build_example(CuTest* tc);



static void argparse_spy_init(void);
static void argparse_spy_destroy(void);
static argparse_result_t argparse_positional_handler(const char *short_name, const char *long_name, const char *value);
static argparse_result_t argparse_shortname_handler(const char *short_name, const char *long_name, const char *value);
static argparse_result_t argparse_shortname_value_handler(const char *short_name, const char *long_name, const char *value);
static argparse_result_t argparse_longname_handler(const char *short_name, const char *long_name, const char *value);
static argparse_result_t argparse_longname_value_handler(const char *short_name, const char *long_name, const char *value);

//////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
//////////////////////////////////////////////////////////////////////////////
static adt_ary_t m_arg_names; //strong references to adt_str
static adt_ary_t m_arg_values; //strong references to adt_str (can contain NULL references)

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
CuSuite* testsuite_argparse(void)
{
   CuSuite* suite = CuSuiteNew();

   SUITE_ADD_TEST(suite, test_no_args_shall_succeed);
   SUITE_ADD_TEST(suite, test_double_minus_error);
   SUITE_ADD_TEST(suite, test_triple_minus_error);
   SUITE_ADD_TEST(suite, test_single_minus_positional);
   SUITE_ADD_TEST(suite, test_shortname_with_minus_value);
   SUITE_ADD_TEST(suite, test_shortname_no_value);
   SUITE_ADD_TEST(suite, test_multiple_flags);
   SUITE_ADD_TEST(suite, test_shortname_before_value);
   SUITE_ADD_TEST(suite, test_shortname_equals_value);
   SUITE_ADD_TEST(suite, test_longname_no_value);
   SUITE_ADD_TEST(suite, test_longname_before_value);
   SUITE_ADD_TEST(suite, test_longname_equals_value);
   SUITE_ADD_TEST(suite, test_positional_arguments_only);
   SUITE_ADD_TEST(suite, test_cmake_configure_example);
   SUITE_ADD_TEST(suite, test_cmake_build_example);

   return suite;
}
//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

static void test_no_args_shall_succeed(CuTest* tc)
{
   CuAssertIntEquals(tc, ARGPARSE_SUCCESS, argparse_exec(1, NULL, NULL));
}

static void test_double_minus_error(CuTest* tc)
{
   const int argc=2;
   const char *argv[2] = {"DUMMY", "--"};
   CuAssertIntEquals(tc, ARGPARSE_PARSE_ERROR, argparse_exec(argc, argv, argparse_shortname_handler));
}

static void test_triple_minus_error(CuTest* tc)
{
   const int argc=2;
   const char *argv[2] = {"DUMMY", "---help"};
   CuAssertIntEquals(tc, ARGPARSE_PARSE_ERROR, argparse_exec(argc, argv, argparse_positional_handler));
}

/**
 * In some programs a single minus ('-') is sometimes used as a positional
 * argument to denote stdin (or stdout) instead of a given file name.
 */
static void test_single_minus_positional(CuTest* tc)
{
   adt_str_t *str;
   const int argc=2;
   const char *argv[2] = {"DUMMY", "-"};
   argparse_spy_init();
   CuAssertIntEquals(tc, ARGPARSE_SUCCESS, argparse_exec(argc, argv, argparse_positional_handler));
   CuAssertIntEquals(tc, 1, adt_ary_length(&m_arg_names));
   CuAssertIntEquals(tc, 1, adt_ary_length(&m_arg_values));
   str = adt_ary_value(&m_arg_values, 0);
   CuAssertStrEquals(tc, "-", adt_str_cstr(str));
   argparse_spy_destroy();
}

static void test_shortname_with_minus_value(CuTest* tc)
{
   adt_str_t *name;
   adt_str_t *value;
   const int argc=3;
   const char *argv[3] = {"DUMMY", "-o", "-"};
   argparse_spy_init();
   CuAssertIntEquals(tc, ARGPARSE_SUCCESS, argparse_exec(argc, argv, argparse_shortname_value_handler));
   CuAssertIntEquals(tc, 1, adt_ary_length(&m_arg_names));
   CuAssertIntEquals(tc, 1, adt_ary_length(&m_arg_values));
   name = adt_ary_value(&m_arg_names, 0);
   value = adt_ary_value(&m_arg_values, 0);
   CuAssertStrEquals(tc, "o", adt_str_cstr(name));
   CuAssertStrEquals(tc, "-", adt_str_cstr(value));
   argparse_spy_destroy();
}

static void test_shortname_no_value(CuTest* tc)
{
   adt_str_t *str;
   const int argc=2;
   const char *argv[2] = {"DUMMY", "-h"};
   argparse_spy_init();
   CuAssertIntEquals(tc, ARGPARSE_SUCCESS, argparse_exec(argc, argv, argparse_shortname_handler));
   CuAssertIntEquals(tc, 1, adt_ary_length(&m_arg_names));
   CuAssertIntEquals(tc, 1, adt_ary_length(&m_arg_values));
   str = adt_ary_value(&m_arg_names, 0);
   CuAssertStrEquals(tc, "h", adt_str_cstr(str));
   argparse_spy_destroy();
}

static void test_multiple_flags(CuTest* tc)
{
   adt_str_t *str;
   const int argc=2;
   const char *argv[2] = {"DUMMY", "-xvf"};
   CuAssertIntEquals(tc, ARGPARSE_SUCCESS, argparse_exec(argc, argv, argparse_shortname_handler));
   CuAssertIntEquals(tc, 1, adt_ary_length(&m_arg_names));
   CuAssertIntEquals(tc, 1, adt_ary_length(&m_arg_values));
   str = adt_ary_value(&m_arg_names, 0);
   CuAssertStrEquals(tc, "xvf", adt_str_cstr(str));
   argparse_spy_destroy();
}

static void test_shortname_before_value(CuTest* tc)
{
   adt_str_t *name;
   adt_str_t *value;
   const int argc=5;
   const char *argv[5] = {"DUMMY", "-p", "5000", "-a", "127.0.0.1"};
   argparse_spy_init();
   CuAssertIntEquals(tc, ARGPARSE_SUCCESS, argparse_exec(argc, argv, argparse_shortname_value_handler));
   CuAssertIntEquals(tc, 2, adt_ary_length(&m_arg_names));
   CuAssertIntEquals(tc, 2, adt_ary_length(&m_arg_values));
   name = adt_ary_value(&m_arg_names, 0);
   value = adt_ary_value(&m_arg_values, 0);
   CuAssertStrEquals(tc, "p", adt_str_cstr(name));
   CuAssertStrEquals(tc, "5000", adt_str_cstr(value));
   name = adt_ary_value(&m_arg_names, 1);
   value = adt_ary_value(&m_arg_values, 1);
   CuAssertStrEquals(tc, "a", adt_str_cstr(name));
   CuAssertStrEquals(tc, "127.0.0.1", adt_str_cstr(value));
   argparse_spy_destroy();
}

static void test_shortname_equals_value(CuTest* tc)
{
   adt_str_t *name;
   adt_str_t *value;
   const int argc=3;
   const char *argv[3] = {"DUMMY", "-p=5000", "-a=127.0.0.1"};
   argparse_spy_init();
   CuAssertIntEquals(tc, ARGPARSE_SUCCESS, argparse_exec(argc, argv, argparse_shortname_value_handler));
   CuAssertIntEquals(tc, 2, adt_ary_length(&m_arg_names));
   CuAssertIntEquals(tc, 2, adt_ary_length(&m_arg_values));
   name = adt_ary_value(&m_arg_names, 0);
   value = adt_ary_value(&m_arg_values, 0);
   CuAssertStrEquals(tc, "p", adt_str_cstr(name));
   CuAssertStrEquals(tc, "5000", adt_str_cstr(value));
   name = adt_ary_value(&m_arg_names, 1);
   value = adt_ary_value(&m_arg_values, 1);
   CuAssertStrEquals(tc, "a", adt_str_cstr(name));
   CuAssertStrEquals(tc, "127.0.0.1", adt_str_cstr(value));
   argparse_spy_destroy();
}

static void test_longname_no_value(CuTest* tc)
{
   adt_str_t *str;
   const int argc=2;
   const char *argv[2] = {"DUMMY", "--version"};
   argparse_spy_init();
   CuAssertIntEquals(tc, ARGPARSE_SUCCESS, argparse_exec(argc, argv, argparse_longname_handler));
   CuAssertIntEquals(tc, 1, adt_ary_length(&m_arg_names));
   CuAssertIntEquals(tc, 1, adt_ary_length(&m_arg_values));
   str = adt_ary_value(&m_arg_names, 0);
   CuAssertStrEquals(tc, "version", adt_str_cstr(str));
   argparse_spy_destroy();
}

static void test_longname_before_value(CuTest* tc)
{
   adt_str_t *name;
   adt_str_t *value;
   const int argc=5;
   const char *argv[5] = {"DUMMY", "--port", "5000", "--address", "127.0.0.1"};
   argparse_spy_init();
   CuAssertIntEquals(tc, ARGPARSE_SUCCESS, argparse_exec(argc, argv, argparse_longname_value_handler));
   CuAssertIntEquals(tc, 2, adt_ary_length(&m_arg_names));
   CuAssertIntEquals(tc, 2, adt_ary_length(&m_arg_values));
   name = adt_ary_value(&m_arg_names, 0);
   value = adt_ary_value(&m_arg_values, 0);
   CuAssertStrEquals(tc, "port", adt_str_cstr(name));
   CuAssertStrEquals(tc, "5000", adt_str_cstr(value));
   name = adt_ary_value(&m_arg_names, 1);
   value = adt_ary_value(&m_arg_values, 1);
   CuAssertStrEquals(tc, "address", adt_str_cstr(name));
   CuAssertStrEquals(tc, "127.0.0.1", adt_str_cstr(value));
   argparse_spy_destroy();
}

static void test_longname_equals_value(CuTest* tc)
{
   adt_str_t *name;
   adt_str_t *value;
   const int argc=3;
   const char *argv[3] = {"DUMMY", "--port=5000", "--address=127.0.0.1"};
   argparse_spy_init();
   CuAssertIntEquals(tc, ARGPARSE_SUCCESS, argparse_exec(argc, argv, argparse_longname_value_handler));
   CuAssertIntEquals(tc, 2, adt_ary_length(&m_arg_names));
   CuAssertIntEquals(tc, 2, adt_ary_length(&m_arg_values));
   name = adt_ary_value(&m_arg_names, 0);
   value = adt_ary_value(&m_arg_values, 0);
   CuAssertStrEquals(tc, "port", adt_str_cstr(name));
   CuAssertStrEquals(tc, "5000", adt_str_cstr(value));
   name = adt_ary_value(&m_arg_names, 1);
   value = adt_ary_value(&m_arg_values, 1);
   CuAssertStrEquals(tc, "address", adt_str_cstr(name));
   CuAssertStrEquals(tc, "127.0.0.1", adt_str_cstr(value));
   argparse_spy_destroy();
}

static void test_positional_arguments_only(CuTest* tc)
{
   adt_str_t *value;
   const int argc=4;
   const char *argv[5] = {"DUMMY", "a", "b", "c"};
   argparse_spy_init();
   CuAssertIntEquals(tc, ARGPARSE_SUCCESS, argparse_exec(argc, argv, argparse_positional_handler));
   CuAssertIntEquals(tc, 3, adt_ary_length(&m_arg_names));
   CuAssertIntEquals(tc, 3, adt_ary_length(&m_arg_values));
   value = adt_ary_value(&m_arg_values, 0);
   CuAssertStrEquals(tc, "a", adt_str_cstr(value));
   value = adt_ary_value(&m_arg_values, 1);
   CuAssertStrEquals(tc, "b", adt_str_cstr(value));
   value = adt_ary_value(&m_arg_values, 2);
   CuAssertStrEquals(tc, "c", adt_str_cstr(value));
   argparse_spy_destroy();
}

static void test_cmake_configure_example(CuTest* tc)
{
   adt_str_t *name;
   adt_str_t *value;
   const int argc=6;
   const char *argv[6] = {"cmake", "-S", ".", "-B", "build", "-DUNIT_TEST=ON"};
   argparse_spy_init();
   CuAssertIntEquals(tc, ARGPARSE_SUCCESS, argparse_exec(argc, argv, argparse_shortname_value_handler));
   CuAssertIntEquals(tc, 3, adt_ary_length(&m_arg_names));
   CuAssertIntEquals(tc, 3, adt_ary_length(&m_arg_values));
   name = adt_ary_value(&m_arg_names, 0);
   value = adt_ary_value(&m_arg_values, 0);
   CuAssertStrEquals(tc, "S", adt_str_cstr(name));
   CuAssertStrEquals(tc, ".", adt_str_cstr(value));
   name = adt_ary_value(&m_arg_names, 1);
   value = adt_ary_value(&m_arg_values, 1);
   CuAssertStrEquals(tc, "B", adt_str_cstr(name));
   CuAssertStrEquals(tc, "build", adt_str_cstr(value));
   name = adt_ary_value(&m_arg_names, 2);
   value = adt_ary_value(&m_arg_values, 2);
   CuAssertStrEquals(tc, "DUNIT_TEST", adt_str_cstr(name));
   CuAssertStrEquals(tc, "ON", adt_str_cstr(value));
   argparse_spy_destroy();
}

static void test_cmake_build_example(CuTest* tc)
{
   adt_str_t *name;
   adt_str_t *value;
   const int argc=7;
   const char *argv[7] = {"cmake", "--build", "build", "--config", "Debug", "--target", "cutil_unit"};
   argparse_spy_init();
   CuAssertIntEquals(tc, ARGPARSE_SUCCESS, argparse_exec(argc, argv, argparse_longname_value_handler));
   CuAssertIntEquals(tc, 3, adt_ary_length(&m_arg_names));
   CuAssertIntEquals(tc, 3, adt_ary_length(&m_arg_values));
   name = adt_ary_value(&m_arg_names, 0);
   value = adt_ary_value(&m_arg_values, 0);
   CuAssertStrEquals(tc, "build", adt_str_cstr(name));
   CuAssertStrEquals(tc, "build", adt_str_cstr(value));
   name = adt_ary_value(&m_arg_names, 1);
   value = adt_ary_value(&m_arg_values, 1);
   CuAssertStrEquals(tc, "config", adt_str_cstr(name));
   CuAssertStrEquals(tc, "Debug", adt_str_cstr(value));
   name = adt_ary_value(&m_arg_names, 2);
   value = adt_ary_value(&m_arg_values, 2);
   CuAssertStrEquals(tc, "target", adt_str_cstr(name));
   CuAssertStrEquals(tc, "cutil_unit", adt_str_cstr(value));
   argparse_spy_destroy();
}

/*** test handlers ***/

//Handles only positional arguments
static argparse_result_t argparse_positional_handler(const char *short_name, const char *long_name, const char *value)
{
   if ( (short_name == 0) && (long_name == 0) && (value != 0))
   {
      adt_str_t *str = adt_str_new_cstr(value);
      adt_ary_push(&m_arg_names, NULL);
      adt_ary_push(&m_arg_values, (void*) str);
      return ARGPARSE_SUCCESS; //Go to next argument
   }
   return ARGPARSE_PARSE_ERROR;
}

//Handles only flags (single character options of short_name type)
static argparse_result_t argparse_shortname_handler(const char *short_name, const char *long_name, const char *value)
{
   if ( (short_name != 0) && (long_name == 0) && (value == 0))
   {
      adt_str_t *flags = adt_str_new_cstr(short_name);
      adt_ary_push(&m_arg_names, (void*) flags);
      adt_ary_push(&m_arg_values, NULL);
      return ARGPARSE_SUCCESS; //Go to next argument
   }
   return ARGPARSE_PARSE_ERROR;
}

static argparse_result_t argparse_shortname_value_handler(const char *short_name, const char *long_name, const char *value)
{
   (void) long_name;
   if ( (short_name != 0) && (value == 0) )
   {
      return ARGPARSE_NEED_VALUE;
   }
   else if(short_name != 0 && value != 0)
   {
      adt_str_t *name_str, *val_str;
      name_str = adt_str_new_cstr(short_name);
      val_str = adt_str_new_cstr(value);
      adt_ary_push(&m_arg_names, (void*) name_str);
      adt_ary_push(&m_arg_values, (void*) val_str);
      return ARGPARSE_SUCCESS;
   }
   return ARGPARSE_PARSE_ERROR;
}

//Handles longname without any values
static argparse_result_t argparse_longname_handler(const char *short_name, const char *long_name, const char *value)
{
   if ( (short_name == 0) && (long_name != 0) && (value == 0))
   {
      adt_str_t *str = adt_str_new_cstr(long_name);
      adt_ary_push(&m_arg_names, (void*) str);
      adt_ary_push(&m_arg_values, NULL);
      return ARGPARSE_SUCCESS; //Go to next argument
   }
   return ARGPARSE_PARSE_ERROR;
}

static argparse_result_t argparse_longname_value_handler(const char *short_name, const char *long_name, const char *value)
{
   (void) short_name;
   if ( (long_name != 0) && (value == 0) )
   {
      return ARGPARSE_NEED_VALUE;
   }
   else if( (long_name != 0) && (value != 0) )
   {
      adt_str_t *name_str, *val_str;
      name_str = adt_str_new_cstr(long_name);
      val_str = adt_str_new_cstr(value);
      adt_ary_push(&m_arg_names, (void*) name_str);
      adt_ary_push(&m_arg_values, (void*) val_str);
      return ARGPARSE_SUCCESS;
   }
   return ARGPARSE_PARSE_ERROR;
}

/*** argparse spy ***/

static void argparse_spy_init(void)
{
   adt_ary_create(&m_arg_names, adt_str_vdelete);
   adt_ary_create(&m_arg_values, adt_str_vdelete);
}

static void argparse_spy_destroy(void)
{
   adt_ary_destroy(&m_arg_names);
   adt_ary_destroy(&m_arg_values);
}

