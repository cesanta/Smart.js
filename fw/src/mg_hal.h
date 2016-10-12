/*
 * Copyright (c) 2014-2016 Cesanta Software Limited
 * All rights reserved
 */

#ifndef CS_FW_SRC_MG_HAL_H_
#define CS_FW_SRC_MG_HAL_H_

/*
 * Interfaces that need to be implemented for each devices.
 */

#include <inttypes.h>
#include <stdlib.h>

/* Get system memory size. */
size_t mg_get_heap_size(void);

/* Get system free memory. */
size_t mg_get_free_heap_size(void);

/* Get minimal watermark of the system free memory. */
size_t mg_get_min_free_heap_size(void);

/* Get filesystem memory usage */
size_t mg_get_fs_memory_usage(void);

/* Feed watchdog */
void mg_wdt_feed(void);

/* Set watchdog timeout*/
void mg_wdt_set_timeout(int secs);

/* Enable watchdog */
void mg_wdt_enable(void);

/* Disable watchdog */
void mg_wdt_disable(void);

/* Restart system */
void mg_system_restart(int exit_code);

/* Delay usecs */
void mg_usleep(int usecs);

/* Get storage free space, bytes */
int64_t mg_get_storage_free_space(void);

#endif /* CS_FW_SRC_MG_HAL_H_ */
/* Jumps between file systems */
uint8_t sj_fs_jump(int fs);