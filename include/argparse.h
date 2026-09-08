/*****************************************************************************
* \file      argparse.h
* \author    Conny Gustafsson
* \date      2020-04-10
* \brief     Platform-independent argument parser
*
* Copyright (c) 2020-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
#ifndef CUTIL_ARGPARSE_H_
#define CUTIL_ARGPARSE_H_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////
// PUBLIC CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
typedef enum argparse_action_tag
{
   ARGPARSE_INVALID_ARGUMENT_ERROR  = -5,
   ARGPARSE_PARSE_ERROR             = -4,
   ARGPARSE_MEM_ERROR               = -3,
   ARGPARSE_VALUE_ERROR             = -2,
   ARGPARSE_NAME_ERROR              = -1,
   ARGPARSE_SUCCESS                 =  0,
   ARGPARSE_NEED_VALUE              =  1
} argparse_result_t;

typedef argparse_result_t cutil_argparse_result_t;

/**
 * # Short name definition:
 *
 * A short name is any argument that starts with a single minus character: -[^-]+
 *
 * ## Examples of short names:
 * ```
 *   -f
 *   -h
 *   -xvf
 *   -version
 * ```
 * ## Short name exceptions
 *
 *  A single minus character as a stand-alone argument is treated as a positional (or value) argument.
 *
 * # Long name definition
 *
 * A long name is any argument that starts with two minus characters: --[^-]+
 *
 * ## Examples of long names:
 * ```
 *   --version
 *   --target
 *   --help
 *   --build
 * ```
 *
 * ## Errors:
 *
 * Two minus characters by themselves is treated as an error. Three or more minus characters is also
 * treated as an error.
 *
 * callback requirements:
 * 1. Positional arguments will have short_name and long_name both set to NULL while value will be non-null.
 * 2. If a name is encountered for the first time, value will be NULL unless the argument contains an equal sign (=).
 * 2a. When an argument contains a name, an equal sign and text after the equal sign the callback function
 *     will be called with both name and value set (the first time the name is encountered).
 * 3. short_name and long_name are always mutually exclusive (they cannot be non-null simultaneously).
 * 4. If a name is encountered and a value is expected (to come next) the callback function shall return ARGPARSE_NEED_VALUE.
 * 5. If a name is encountered and no value is expected the callback function shall return ARGPARSE_SUCCESS.
 * 5.1. After a callback returns ARGPARSE_NEED_VALUE the callback will be called again with both name and value.
 * 5.2. This means that the callback does not need to "store" or "remember" the name when returning ARGPARSE_NEED_VALUE.
 * 6. Any error code returned by the callback will be returned as a result back to the caller of the argparse_exec function.
 */
typedef argparse_result_t (argparse_callback_fn)(const char *short_name, const char *long_name, const char *value);
typedef argparse_callback_fn cutil_argparse_callback_fn;

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////

/**
 * \brief Executes argument parsing across an argv array invoking callback for each item.
 *
 * \param argc Number of elements in argv.
 * \param argv Array of argument strings.
 * \param callback Callback invoked on each parsed argument.
 * \return ARGPARSE_SUCCESS on success, or negative error code on failure.
 */
argparse_result_t cutil_argparse_exec(int argc, const char **argv, argparse_callback_fn *callback);

#define argparse_exec cutil_argparse_exec

#ifdef __cplusplus
}
#endif

#endif // CUTIL_ARGPARSE_H_
