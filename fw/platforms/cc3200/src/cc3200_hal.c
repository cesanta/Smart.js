/*
 * Copyright (c) 2014-2016 Cesanta Software Limited
 * All rights reserved
 */

#ifndef __TI_COMPILER_VERSION__
#include <malloc.h>
#endif
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "hw_types.h"
#include "prcm.h"
#include "rom.h"
#include "rom_map.h"

#include <common/platform.h>

#include "simplelink.h"
#include "device.h"
#include "oslib/osi.h"

#include "fw/src/mg_hal.h"
#include "fw/src/mg_v7_ext.h"

#include "fw/platforms/cc3200/src/cc3200_fs.h"
#include "fw/platforms/cc3200/src/cc3200_main_task.h"

#include "common/umm_malloc/umm_malloc.h"

#if MG_ENABLE_JS
#include "v7/v7.h"

static void mg_invoke_cb_cb(void *arg);

struct v7_invoke_event_data {
  struct v7 *v7;
  v7_val_t func;
  v7_val_t this_obj;
  v7_val_t args;
};

void mg_invoke_cb(struct v7 *v7, v7_val_t func, v7_val_t this_obj,
                  v7_val_t args) {
  struct v7_invoke_event_data *ied = calloc(1, sizeof(*ied));
  ied->v7 = v7;
  ied->func = func;
  ied->this_obj = this_obj;
  ied->args = args;
  v7_own(v7, &ied->func);
  v7_own(v7, &ied->this_obj);
  v7_own(v7, &ied->args);
  invoke_cb(mg_invoke_cb_cb, ied);
}

static void mg_invoke_cb_cb(void *arg) {
  struct v7_invoke_event_data *ied = (struct v7_invoke_event_data *) arg;
  _mg_invoke_cb(ied->v7, ied->func, ied->this_obj, ied->args);
  v7_disown(ied->v7, &ied->args);
  v7_disown(ied->v7, &ied->this_obj);
  v7_disown(ied->v7, &ied->func);
  free(ied);
}
#endif /* MG_ENABLE_JS */

#ifdef __TI_COMPILER_VERSION__
size_t mg_get_heap_size(void) {
  return UMM_MALLOC_CFG__HEAP_SIZE;
}

size_t mg_get_free_heap_size(void) {
  return umm_free_heap_size();
}

#else

/* Defined in linker script. */
extern unsigned long _heap;
extern unsigned long _eheap;

size_t mg_get_heap_size(void) {
  return ((char *) &_eheap - (char *) &_heap);
}

size_t mg_get_free_heap_size(void) {
  size_t avail = mg_get_heap_size();
  struct mallinfo mi = mallinfo();
  avail -= mi.arena;    /* Claimed by allocator. */
  avail += mi.fordblks; /* Free in the area claimed by allocator. */
  return avail;
}
#endif

size_t mg_get_min_free_heap_size(void) {
  /* Not supported */
  return 0;
}

size_t mg_get_fs_memory_usage(void) {
  return 0; /* Not even sure if it's possible to tell. */
}

void mg_wdt_feed(void) {
  /* TODO */
}

void mg_wdt_set_timeout(int secs) {
  /* TODO */
}

void mg_wdt_enable(void) {
  /* TODO */
}

void mg_wdt_disable(void) {
  /* TODO */
}

void mg_system_restart(int exit_code) {
  (void) exit_code;
  if (exit_code != 100) {
    cc3200_fs_umount();
    sl_Stop(50 /* ms */);
  }
  /* Turns out to be not that easy. In particular, using *Reset functions is
   * not a good idea.
   * https://e2e.ti.com/support/wireless_connectivity/f/968/p/424736/1516404
   * Instead, the recommended way is to enter hibernation with immediate wakeup.
   */
  MAP_PRCMHibernateIntervalSet(328 /* 32KHz ticks, 100 ms */);
  MAP_PRCMHibernateWakeupSourceEnable(PRCM_HIB_SLOW_CLK_CTR);
  MAP_PRCMHibernateEnter();
}

void mg_usleep(int usecs) {
  osi_Sleep(usecs / 1000 /* ms */);
}

void mongoose_poll_cb(void *arg);

bool s_mg_poll_scheduled;

void mongoose_schedule_poll(void) {
  /* Prevent piling up of poll callbacks. */
  if (s_mg_poll_scheduled) return;
  s_mg_poll_scheduled = invoke_cb(mongoose_poll_cb, NULL);
}

void mongoose_poll_cb(void *arg) {
  s_mg_poll_scheduled = false;
  (void) arg;
}
