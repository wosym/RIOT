/*
 * Copyright (C) 2020 Nalys
 *  
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_bh1900nux
 * @{
 *
 * @file
 * @brief       BH1900NUX temperature sensor driver implementation
 *
 * @author      Wouter Symons <wsymons@nalys-group.com> 
 //TODO: driver is heavily based on other driver --> credit?
 *
 * @}
 */

#include <stdio.h>

#include "bh1900nux.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

int bh1900nux_init(bh1900nux_t *dev, const bh1900nux_params_t *params)
{
    //TODO: check contents? null-pointer? 

    /* initialize the device descriptor */
    dev->i2c = params->i2c;
    dev->addr = params->addr;

    return BH1900NUX_OK;
}

int bh1900nux_read(const bh1900nux_t *dev, double *temp)
{
    int ret = 0;
    int tmp = 0;
    uint8_t bytes[2] = {0};

    /* Read raw sensor value */
    DEBUG("[bh1900nux] read temperature\n");
    ret = i2c_acquire(dev->i2c);
    if(ret < 0) {
        return BH1900NUX_ERR_I2C;
    }
    ret = i2c_read_regs(dev->i2c, dev->addr, BH1900NUX_REG_ADDR, bytes, 2, 0);    //TODO: magic numbers
    if(ret < 0) {
        return ret;
    }
    i2c_release(dev->i2c);  //TODO: return values!

    /* Calculate temperature */
    tmp = ((uint16_t)bytes[0] << 8) | bytes[1];
    *temp = 0.0625 * ((double) tmp / 16);   //TODO: magic number?

    return BH1900NUX_OK;
}

//TODO: read kelvin?
