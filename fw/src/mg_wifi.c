/*
 * Copyright (c) 2014-2016 Cesanta Software Limited
 * All rights reserved
 */

#include "fw/src/mg_wifi.h"

#include <stdbool.h>
#include <stdlib.h>

#include "common/cs_dbg.h"
#include "common/queue.h"
#include "fw/src/mg_sys_config.h"

struct wifi_cb {
  SLIST_ENTRY(wifi_cb) entries;
  mg_wifi_changed_t cb;
  void *arg;
};

SLIST_HEAD(wifi_cbs, wifi_cb) s_wifi_cbs;

void mg_wifi_on_change_cb(enum mg_wifi_status event) {
  switch (event) {
    case MG_WIFI_DISCONNECTED:
      LOG(LL_INFO, ("Wifi: disconnected"));
      break;
    case MG_WIFI_CONNECTED:
      LOG(LL_INFO, ("Wifi: connected"));
      break;
    case MG_WIFI_IP_ACQUIRED: {
      char *ip = mg_wifi_get_sta_ip();
      LOG(LL_INFO, ("WiFi: ready, IP %s", ip));
      free(ip);
      break;
    }
  }

  struct wifi_cb *e, *te;
  SLIST_FOREACH_SAFE(e, &s_wifi_cbs, entries, te) {
    e->cb(event, e->arg);
  }
}

void mg_wifi_add_on_change_cb(mg_wifi_changed_t cb, void *arg) {
  struct wifi_cb *e = calloc(1, sizeof(*e));
  if (e == NULL) return;
  e->cb = cb;
  e->arg = arg;
  SLIST_INSERT_HEAD(&s_wifi_cbs, e, entries);
}

void mg_wifi_remove_on_change_cb(mg_wifi_changed_t cb, void *arg) {
  struct wifi_cb *e;
  SLIST_FOREACH(e, &s_wifi_cbs, entries) {
    if (e->cb == cb && e->arg == arg) {
      SLIST_REMOVE(&s_wifi_cbs, e, wifi_cb, entries);
      return;
    }
  }
}

static bool validate_wifi_cfg(const struct sys_config *cfg, char **msg) {
  if (cfg->wifi.sta.enable) {
    if (cfg->wifi.sta.ssid == NULL || strlen(cfg->wifi.sta.ssid) > 31) {
      if (!asprintf(msg, "%s %s must be between %d and %d chars", "STA", "SSID",
                    1, 31)) {
      }
      return false;
    }
    if (cfg->wifi.sta.pass != NULL &&
        (strlen(cfg->wifi.sta.pass) < 8 || strlen(cfg->wifi.sta.pass) > 63)) {
      if (!asprintf(msg, "%s %s must be between %d and %d chars", "STA",
                    "password", 8, 63)) {
      }
      return false;
    }
    if (cfg->wifi.sta.ip != NULL) {
      if (cfg->wifi.sta.netmask == NULL) {
        if (!asprintf(msg,
                      "Station static IP is set but no netmask provided")) {
        }
        return false;
      }
      /* TODO(rojer): More validation here: IP & gw within the same net. */
    }
  }
  if (cfg->wifi.ap.enable) {
    if (cfg->wifi.ap.ssid == NULL || strlen(cfg->wifi.ap.ssid) > 31) {
      if (!asprintf(msg, "%s %s must be between %d and %d chars", "AP", "SSID",
                    1, 31)) {
      }
      return false;
    }
    if (cfg->wifi.ap.pass != NULL &&
        (strlen(cfg->wifi.ap.pass) < 8 || strlen(cfg->wifi.ap.pass) > 63)) {
      if (!asprintf(msg, "%s %s must be between %d and %d chars", "AP",
                    "password", 8, 63)) {
      }
      return false;
    }
    if (cfg->wifi.ap.ip == NULL || cfg->wifi.ap.netmask == NULL ||
        cfg->wifi.ap.dhcp_start == NULL || cfg->wifi.ap.dhcp_end == NULL) {
      *msg = strdup("AP IP, netmask, DHCP start and end addresses must be set");
      return false;
    }
    /* TODO(rojer): More validation here. DHCP range, netmask, GW (if set). */
  }
  return true;
}

void mg_wifi_init(void) {
  mg_register_config_validator(validate_wifi_cfg);
  mg_wifi_hal_init();
}
