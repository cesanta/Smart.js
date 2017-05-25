/*
 * Copyright (c) 2014-2016 Cesanta Software Limited
 * All rights reserved
 */

#include "esp_attr.h"
#include "esp_system.h"
#include "esp_task_wdt.h"

#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"

#include "driver/adc.h"

#include "fw/src/mgos_debug.h"
#include "fw/src/mgos_hal.h"
#include "fw/src/mgos_wifi.h"
#include "fw/src/mgos_sys_config.h"
#include "fw/platforms/esp32/src/esp32_fs.h"

size_t mgos_get_heap_size(void) {
  return xPortGetHeapSize();
}

size_t mgos_get_free_heap_size(void) {
  return xPortGetFreeHeapSize();
}

size_t mgos_get_min_free_heap_size(void) {
  return xPortGetMinimumEverFreeHeapSize();
}

void mgos_system_restart(int exit_code) {
  (void) exit_code;
  esp32_fs_deinit();
#if MGOS_ENABLE_WIFI
  mgos_wifi_disconnect();
#endif
  LOG(LL_INFO, ("Restarting"));
  mgos_debug_flush();
  esp_restart();
}

void device_get_mac_address(uint8_t mac[6]) {
  esp_efuse_mac_get_default(mac);
}

/* In components/newlib/time.c. Returns a monotonic microsecond counter. */
uint64_t get_time_since_boot();

void mgos_msleep(uint32_t msecs) {
  mgos_usleep(msecs * 1000);
}

void mgos_usleep(uint32_t usecs) {
  uint64_t threshold = get_time_since_boot() + (uint64_t) usecs;
  int ticks = usecs / (1000000 / configTICK_RATE_HZ);
  if (ticks > 0) vTaskDelay(ticks);
  while (get_time_since_boot() < threshold) {
  }
}

void mgos_wdt_feed(void) {
  esp_task_wdt_feed();
}

void mgos_wdt_disable(void) {
  esp_task_wdt_delete();
}

void mgos_wdt_enable(void) {
  /* Feeding the dog re-adds it back to the list if needed. */
  esp_task_wdt_feed();
}

void mgos_wdt_set_timeout(int secs) {
  /* WDT0 is configured in esp_task_wdt_init, we only modify the timeout. */
  TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
  TIMERG0.wdt_config2 = secs * 2000; /* Units: 0.5 ms ticks */
  TIMERG0.wdt_config3 = (secs + 1) * 2000;
  TIMERG0.wdt_config0.en = 1;
  TIMERG0.wdt_feed = 1;
  TIMERG0.wdt_wprotect = 0;
}

SemaphoreHandle_t s_mgos_mux = NULL;

inline void mgos_lock() {
  while (!xSemaphoreTakeRecursive(s_mgos_mux, 10)) {
  }
}

inline void mgos_unlock() {
  while (!xSemaphoreGiveRecursive(s_mgos_mux)) {
  }
}

IRAM_ATTR void mgos_ints_disable(void) {
  __asm volatile("rsil a2, 3" : : : "a2");
}

IRAM_ATTR void mgos_ints_enable(void) {
  __asm volatile("rsil a2, 0" : : : "a2");
}

int mg_ssl_if_mbed_random(void *ctx, unsigned char *buf, size_t len) {
  while (len > 0) {
    uint32_t r = esp_random(); /* Uses hardware RNG. */
    for (int i = 0; i < 4 && len > 0; i++, len--) {
      *buf++ = (uint8_t) r;
      r >>= 8;
    }
  }
  (void) ctx;
  return 0;
}

int mgos_adc_read(int pin) {
  static int pins[] = {36, 37, 38, 39, 32, 33, 34, 35};
  static adc1_channel_t channels[] = {
      ADC1_CHANNEL_0, ADC1_CHANNEL_1, ADC1_CHANNEL_2, ADC1_CHANNEL_3,
      ADC1_CHANNEL_4, ADC1_CHANNEL_5, ADC1_CHANNEL_6, ADC1_CHANNEL_7};
  int i;
  for (i = 0; i < ARRAY_SIZE(pins); i++) {
    if (pins[i] == pin) {
      adc1_config_width(ADC_WIDTH_12Bit);
      adc1_config_channel_atten(channels[i], ADC_ATTEN_11db);
      return 0x0FFF & adc1_get_voltage(channels[i]);
    }
  }

  return -1;
}
