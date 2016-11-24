---
title: Call RESTful server
---

This example shows you how to send periodic measurements to an external
RESTful server. This example uses http://httpbin.org, which is a useful public
RESTful service that allows you to test or debug RESTful interfaces.

Every 2 seconds we send a http://httpbin.org/get?n=NUMBER request,
where `NUMBER` is an incremented counter which simulates measured data.
httpbin.org echoes back a JSON object that describes the request.
We simply log this received reply.

Create an empty directory, execute `miot fw init --arch cc3200` in it.
If you have NodeMCU, use `esp8266` instead of `cc3200`.
Copy-paste this into the `src/main.c`:

```c
#include "fw/src/miot_app.h"
#include "fw/src/miot_mongoose.h"
#include "common/cs_dbg.h"
#include "fw/src/miot_timers.h"

#define INTERVAL_MILLISECONDS 2000

static void on_http_reply(struct mg_connection *c, int ev, void *ev_data) {
  if (ev == MG_EV_HTTP_REPLY) {
    struct http_message *hm = (struct http_message *) ev_data;
    LOG(LL_INFO, ("Got reply:\n%.*s", (int) hm->body.len, hm->body.p));
    c->flags |= MG_F_CLOSE_IMMEDIATELY;
  }
}

// Timer callback - called every INTERVAL_MILLISECONDS milliseconds
static void report_data(void *param) {
  static int counter;
  char url[100];
  snprintf(url, sizeof(url), "http://httpbin.org/get?n=%d", counter++);
  mg_connect_http(miot_get_mgr(), on_http_reply, url, NULL, NULL);
  LOG(LL_INFO, ("Sending %s", url));
  (void) param;
}

enum miot_app_init_result miot_app_init(void) {
  miot_set_timer(INTERVAL_MILLISECONDS, true /* repeat */, report_data, NULL);
  return MIOT_APP_INIT_SUCCESS;
}
```

Attach a device to your computer. Compile, flash and configure the firmware:

```
$ miot build
$ miot flash
$ miot config-set wifi.sta.enable=true wifi.sta.ssid=WIFI_NETWORK wifi.sta.pass=WIFI_PASSWORD
```

Attach a terminal to the device and see messages printed:

```
$ picocom /dev/ttyUSB0 -b 115200 --imap lfcrlf --omap crcrlf

report_data          Sending http://httpbin.org/get?n=32
on_http_reply        Got reply:
{
  "args": {
    "n": "32"
  },
  "headers": {
    "Content-Length": "0",
    "Host": "httpbin.org"
  },
  "origin": "213.79.52.5",
  "url": "http://httpbin.org/get?n=32"
}
```
