/*
 * Copyright (c) 2014-2018 Cesanta Software Limited
 * All rights reserved
 *
 * Licensed under the Apache License, Version 2.0 (the ""License"");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an ""AS IS"" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CS_FW_PLATFORMS_ESP8266_SRC_ESP_FLASH_WRITER_H_
#define CS_FW_PLATFORMS_ESP8266_SRC_ESP_FLASH_WRITER_H_

#include <stdbool.h>
#include <stdint.h>

#include "common/mg_str.h"

#ifdef __cplusplus
extern "C" {
#endif

struct esp_flash_write_ctx {
  uint32_t addr;
  uint32_t max_size;
  uint32_t num_erased;
  uint32_t num_written;
};

bool esp_init_flash_write_ctx(struct esp_flash_write_ctx *wctx, uint32_t addr,
                              uint32_t max_size);
int esp_flash_write(struct esp_flash_write_ctx *wctx, const struct mg_str data);

#ifdef __cplusplus
}
#endif

#endif /* CS_FW_PLATFORMS_ESP8266_SRC_ESP_FLASH_WRITER_H_ */
