// SPDX-FileCopyrightText: Copyright Fabien Georget
// SPDX-License-Identifier: Apache-2.0 
//

#pragma once
/*
 * This module provide time utilities
 *
 */


#ifdef __cplusplus
extern "C" {
#endif

struct tm;

// init time - call once after reset
int init_time();

// return the wall time
int get_time(struct tm *tm);

// return the wall time as a string
int get_time_string(struct tm *tm, char *buf, int offset);

#ifdef __cplusplus
}
#endif
