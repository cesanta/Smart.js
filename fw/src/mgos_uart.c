/*
 * Copyright (c) 2014-2016 Cesanta Software Limited
 * All rights reserved
 */

#include "fw/src/mgos_uart.h"

#include <stdlib.h>

#include "common/cs_dbg.h"

#include "fw/src/mgos_hal.h"
#include "fw/src/mgos_mongoose.h"
#include "fw/src/mgos_uart_hal.h"
#include "fw/src/mgos_utils.h"
#include "fw/src/mgos_gpio.h"
#include "fw/src/mgos_hal.h"

#ifndef IRAM
#define IRAM
#endif

#ifndef MGOS_MAX_NUM_UARTS
#error Please define MGOS_MAX_NUM_UARTS
#endif

static struct mgos_uart_state *s_uart_state[MGOS_MAX_NUM_UARTS];

IRAM void mgos_uart_schedule_dispatcher(int uart_no, bool from_isr) {
  struct mgos_uart_state *us = s_uart_state[uart_no];
  if (us == NULL) return;
  mongoose_schedule_poll(from_isr);
}

void mgos_uart_dispatcher(void *arg) {
  uint32_t final_char_delay_us;
  int uart_no = (intptr_t) arg;
  struct mgos_uart_state *us = s_uart_state[uart_no];
  if (us == NULL) return;
  mgos_lock();
  if (us->rx_enabled) mgos_uart_hal_dispatch_rx_top(us);
  mgos_uart_hal_dispatch_tx_top(us);
  if (us->dispatcher_cb != NULL) {
    us->dispatcher_cb(uart_no, us->dispatcher_data);
  }
  mgos_uart_hal_dispatch_bottom(us);
  if (us->rx_buf.len == 0) mbuf_trim(&us->rx_buf);
  if (us->tx_buf.len == 0) mbuf_trim(&us->tx_buf);
  
  if (us->cfg.rs485_ena) {
    if ((mgos_uart_is_tx_fifo_empty(us)) && (us->cfg.rs485_active)) {  
      /* If the uart_tx_fifo is empty, all the data has been sent so
       * it is time to de-assert the TxEn line and start listening again
       * but the final character may not have been transmitted so
       * wait 1.5 character periods to ensure that the UART has sent it.
       * 
       * Assume worst case of 8bit, 1bit parity, 2 stop bits = 12 bits
       * therefore wait 18 bit periods */			 
      us->cfg.rs485_active = false;
      final_char_delay_us = 18000000;
      final_char_delay_us /= us->cfg.baud_rate;
      mgos_usleep(final_char_delay_us);
      mgos_gpio_write(us->cfg.dev.tx_en_gpio, 0);
    } else {
      /* Be sure to come back here regularly until all data has been sent
       * and the TxEn line has been de-asserted to start listening again */
      mgos_uart_schedule_dispatcher(uart_no, true /* from_isr */);
    }
  }
  
  mgos_unlock();
}

size_t mgos_uart_write(int uart_no, const void *buf, size_t len) {
  size_t written = 0;
  struct mgos_uart_state *us = s_uart_state[uart_no];
  if (us == NULL) return 0;
  mgos_lock();
  while (written < len) {
    size_t nw = MIN(len - written, mgos_uart_write_avail(uart_no));
    mbuf_append(&us->tx_buf, ((const char *) buf) + written, nw);
    written += nw;
    if (written < len) mgos_uart_flush(uart_no);
  }
  mgos_unlock();
  mgos_uart_schedule_dispatcher(uart_no, false /* from_isr */);
  return written;
}

int mgos_uart_printf(int uart_no, const char *fmt, ...) {
  int len;
  va_list ap;
  char buf[100], *data = buf;
  struct mgos_uart_state *us = s_uart_state[uart_no];
  if (us == NULL) return 0;
  va_start(ap, fmt);
  len = mg_avprintf(&data, sizeof(buf), fmt, ap);
  va_end(ap);
  if (len > 0) {
    len = mgos_uart_write(uart_no, data, len);
  }
  if (data != buf) free(data);
  return len;
}

size_t mgos_uart_read(int uart_no, void *buf, size_t len) {
  struct mgos_uart_state *us = s_uart_state[uart_no];
  if (us == NULL || !us->rx_enabled) return 0;
  mgos_lock();
  size_t tr = MIN(len, us->rx_buf.len);
  memcpy(buf, us->rx_buf.buf, tr);
  mbuf_remove(&us->rx_buf, tr);
  mgos_unlock();
  return tr;
}

size_t mgos_uart_read_mbuf(int uart_no, struct mbuf *mb, size_t len) {
  struct mgos_uart_state *us = s_uart_state[uart_no];
  if (us == NULL || !us->rx_enabled) return 0;
  mgos_lock();
  size_t nr = MIN(len, mgos_uart_read_avail(uart_no));
  if (nr > 0) {
    size_t free_bytes = mb->size - mb->len;
    if (free_bytes < nr) {
      mbuf_resize(mb, mb->len + nr);
    }
    nr = mgos_uart_read(uart_no, mb->buf + mb->len, nr);
    mb->len += nr;
  }
  mgos_unlock();
  return nr;
}

void mgos_uart_flush(int uart_no) {
  struct mgos_uart_state *us = s_uart_state[uart_no];
  if (us == NULL) return;
  while (us->tx_buf.len > 0) {
    mgos_lock();
    mgos_uart_hal_dispatch_tx_top(us);
    mgos_unlock();
  }
  mgos_uart_hal_flush_fifo(us);
}

bool mgos_uart_configure(int uart_no, const struct mgos_uart_config *cfg) {
  if (uart_no < 0 || uart_no >= MGOS_MAX_NUM_UARTS) return false;
  mgos_lock();
  bool res = false;
  struct mgos_uart_state *us = s_uart_state[uart_no];
  if (us == NULL) {
    us = (struct mgos_uart_state *) calloc(1, sizeof(*us));
    us->uart_no = uart_no;
    mbuf_init(&us->rx_buf, 0);
    mbuf_init(&us->tx_buf, 0);
    if (mgos_uart_hal_init(us)) {
      mgos_add_poll_cb(mgos_uart_dispatcher, (void *) (intptr_t) uart_no);
      s_uart_state[uart_no] = us;
      res = true;
    } else {
      mbuf_free(&us->rx_buf);
      mbuf_free(&us->tx_buf);
      free(us);
      us = NULL;
    }
  }
  if (us != NULL) {
    res = mgos_uart_hal_configure(us, cfg);
    if (res) {
      memcpy(&us->cfg, cfg, sizeof(us->cfg));
    }
  }
  mgos_unlock();
  if (res) {
    mgos_uart_schedule_dispatcher(uart_no, false /* from_isr */);
  }
  return res;
}

void mgos_uart_config_set_defaults(int uart_no, struct mgos_uart_config *cfg) {
  if (uart_no < 0 || uart_no >= MGOS_MAX_NUM_UARTS) return;
  memset(cfg, 0, sizeof(*cfg));
  cfg->baud_rate = 115200;
  cfg->rx_buf_size = cfg->tx_buf_size = 256;
  cfg->rx_linger_micros = 15;
  mgos_uart_hal_config_set_defaults(uart_no, cfg);
}

struct mgos_uart_config *mgos_uart_config_get_default(int uart_no) {
  struct mgos_uart_config *ret = malloc(sizeof(*ret));
  mgos_uart_config_set_defaults(uart_no, ret);
  return ret;
}

void mgos_uart_config_set_baud_rate(struct mgos_uart_config *cfg,
                                    int baud_rate) {
  cfg->baud_rate = baud_rate;
}

void mgos_uart_config_set_rx_params(struct mgos_uart_config *cfg,
                                    int rx_buf_size, bool rx_fc_ena,
                                    int rx_linger_micros) {
  cfg->rx_buf_size = rx_buf_size;
  cfg->rx_fc_ena = rx_fc_ena;
  cfg->rx_linger_micros = rx_linger_micros;
}

void mgos_uart_config_set_tx_params(struct mgos_uart_config *cfg,
                                    int tx_buf_size, bool tx_fc_ena, bool rs485_ena) {
  cfg->tx_buf_size = tx_buf_size;
  cfg->tx_fc_ena = tx_fc_ena;
  cfg->rs485_ena = rs485_ena;
  if (rs485_ena) cfg->rs485_active = false;
}

void mgos_uart_set_dispatcher(int uart_no, mgos_uart_dispatcher_t cb,
                              void *arg) {
  struct mgos_uart_state *us = s_uart_state[uart_no];
  if (us == NULL) return;
  us->dispatcher_cb = cb;
  us->dispatcher_data = arg;
}

bool mgos_uart_is_rx_enabled(int uart_no) {
  struct mgos_uart_state *us = s_uart_state[uart_no];
  if (us == NULL) return false;
  return us->rx_enabled;
}

void mgos_uart_set_rx_enabled(int uart_no, bool enabled) {
  struct mgos_uart_state *us = s_uart_state[uart_no];
  if (us == NULL) return;
  us->rx_enabled = enabled;
  mgos_uart_hal_set_rx_enabled(us, enabled);
}

size_t mgos_uart_rxb_free(const struct mgos_uart_state *us) {
  if (us == NULL || ((int) us->rx_buf.len) > us->cfg.rx_buf_size) return 0;
  return us->cfg.rx_buf_size - us->rx_buf.len;
}

size_t mgos_uart_read_avail(int uart_no) {
  const struct mgos_uart_state *us = s_uart_state[uart_no];
  if (us == NULL) return 0;
  return us->rx_buf.len;
}

size_t mgos_uart_write_avail(int uart_no) {
  const struct mgos_uart_state *us = s_uart_state[uart_no];
  if (us == NULL || ((int) us->tx_buf.len) > us->cfg.tx_buf_size) return 0;
  return us->cfg.tx_buf_size - us->tx_buf.len;
}

const struct mgos_uart_stats *mgos_uart_get_stats(int uart_no) {
  const struct mgos_uart_state *us = s_uart_state[uart_no];
  if (us == NULL) return NULL;
  return &us->stats;
}

IRAM NOINSTR struct mgos_uart_state *mgos_uart_hal_get_state(int uart_no) {
  return s_uart_state[uart_no];
}
