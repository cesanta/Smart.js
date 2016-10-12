/*
 * Copyright (c) 2014-2016 Cesanta Software Limited
 * All rights reserved
 */

#ifndef CS_FW_PLATFORMS_ESP8266_USER_ESP_GPIO_H_
#define CS_FW_PLATFORMS_ESP8266_USER_ESP_GPIO_H_

#include <stdint.h>

#include "fw/src/mg_gpio.h"

#define ENTER_CRITICAL(type) ETS_INTR_DISABLE(type)
#define EXIT_CRITICAL(type) ETS_INTR_ENABLE(type)

int mg_gpio_set_mode(int pin, enum gpio_mode mode, enum gpio_pull_type pull);
int mg_gpio_write(int pin, enum gpio_level level);

#endif /* CS_FW_PLATFORMS_ESP8266_USER_ESP_GPIO_H_ */
