/**
 * @file acss_debug.h
 * @author Nhat Pham  <phamhuudangnhat@gmail.com>.
 * @version 1.0
 * @date 04-Oct-2016
 * @brief This is header file debug functions/macros.
 */

#ifndef ACSS_DEBUG_H_
#define ACSS_DEBUG_H_

#include <stdio.h>

#if ACSS_DEBUG_EN
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#endif
