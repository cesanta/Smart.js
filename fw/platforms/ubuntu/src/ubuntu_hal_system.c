#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "mgos.h"
#include "mgos_hal.h"
#include "mgos_system.h"

void mgos_lock(void) {
  return;
}

void mgos_unlock(void) {
  return;
}

struct mgos_rlock_type *mgos_rlock_create(void) {
  return NULL;
}

void mgos_rlock(struct mgos_rlock_type *l) {
  return;
  (void) l;
}

void mgos_runlock(struct mgos_rlock_type *l) {
  return;
  (void) l;
}

void mgos_rlock_destroy(struct mgos_rlock_type *l) {
  return;
  (void) l;
}

size_t mgos_get_heap_size(void) {
  return 0;
}

size_t mgos_get_free_heap_size(void) {
  return 0;
}

size_t mgos_get_min_free_heap_size(void) {
  return 0;
}

void mgos_dev_system_restart(void) {
  return;
}

size_t mgos_get_fs_memory_usage(void) {
  return 0;
}

/* in vfs-common/src/mgos_vfs.c 
size_t mgos_get_fs_size(void) {
  return 0;
}
*/

/* in vfs-common/src/mgos_vfs.c 
size_t mgos_get_free_fs_size(void) {
  return 0;
}
*/

/* in vfs-common/src/mgos_vfs.c 
void mgos_fs_gc(void) {
  return;
}
*/

void mgos_wdt_feed(void) {
  return;
}

void mgos_wdt_set_timeout(int secs) {
  return;
  (void) secs;
}

void mgos_wdt_enable(void) {
  return;
}

void mgos_wdt_disable(void) {
  return;
}

/* in mongoose-os/fw/src/mgos_system.c
void mgos_system_restart(void) {
  return;
}
*/

void mgos_msleep(uint32_t msecs) {
  usleep(msecs*1000);
  return;
}

void mgos_usleep(uint32_t usecs) {
  usleep(usecs);
  return;
}

void mgos_ints_disable(void) {
  return;
}

void mgos_ints_enable(void) {
  return;
}

bool mgos_invoke_cb(mgos_cb_t cb, void *arg, bool from_isr) {
  return true;
  (void) cb;
  (void) arg;
  (void) from_isr;
}

uint32_t mgos_get_cpu_freq(void) {
  return 0;
}