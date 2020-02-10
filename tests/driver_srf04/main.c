/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test for srf04 ultra sonic range finder driver
 *
 * @author      Semjon Kerner <semjon.kerner@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xtimer.h>

#define SRF04_PARAM_TRIGGER     GPIO_PIN(PORT_C,1)
#define SRF04_PARAM_ECHO        GPIO_PIN(PORT_C,0)
//#define SRF04_PARAM_TRIGGER     GPIO_PIN(PORT_D,5)
//#define SRF04_PARAM_ECHO        GPIO_PIN(PORT_A,1)
//#define SRF04_PARAM_TRIGGER     GPIO_PIN(PORT_B,5)
//#define SRF04_PARAM_ECHO        GPIO_PIN(PORT_B,6)

#include "srf04_params.h"
#include "srf04.h"

int main(void)
{
    int ret = 0;
    puts("SRF04 range finder example");

    srf04_t dev;
    if (srf04_init(&dev, &srf04_params[0]) != SRF04_OK) {
        puts("Error: initializing");
        return 1;
    }

    while (1) {
        puts("BBBB");
        int distance = srf04_get_distance(&dev);
        puts("AAAA");
        //xtimer_sleep(2);
        if ((ret = distance) < SRF04_OK) {
            printf("Error: no valid data available: %d", ret);
        } else {
            printf("D: %d mm\n", distance);
        }
        //xtimer_sleep(1);
        puts("----");
    }

    return 0;
}
