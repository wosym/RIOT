/*
 * Copyright (C) 2019 Nalys
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     apps
 * @{
 *
 * @file
 * @brief       cv_hmi
 *
 * @author      Wouter Symons   <wsymons@nalys-group.com>
 *
 * @}
 */



#define ENABLE_DEBUG (0)

#include <debug.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <isrpipe.h>
#include <periph/gpio.h>
#include <periph/adc.h>
#include <math.h>
//#include "shell.h"
#include "can/device.h"

#include "candev_mcp2515.h"


#include <avr/pgmspace.h>
#include "glcd_io_avrgcc_atmega328p.h"
#include "glcd.h"
#include "proportional_font.h"

#define xtimer_msleep(t) (xtimer_usleep((uint32_t)t * 1000))

#ifndef TEST_MCP2515_SPI
#error "TEST_MCP2515_SPI not defined"
#endif
#ifndef TEST_MCP2515_CS
#error "TEST_MCP2515_CS not defined"
#endif
#ifndef TEST_MCP2515_RESET
#error "TEST_MCP2515_RESET not defined"
#endif
#ifndef TEST_MCP2515_SPI_MODE
#error "TEST_MCP2515_MODE not defined"
#endif
#ifndef TEST_MCP2515_SPI_CLK
#error "TEST_MCP2515_SPI_CLK not defined"
#endif
#ifndef TEST_MCP2515_INT
#error "TEST_MCP2515_INT not defined"
#endif
#ifndef TEST_MCP2515_CLK
#error "TEST_MCP2515_CLK not defined"
#endif
#ifndef CANDEV_MCP2515_DEFAULT_BITRATE
#error "CANDEV_MCP2515_DEFAULT_BITRATE not defined"
#endif

#define MAX_LCD_WIDTH 90

candev_mcp2515_conf_t mcp2515_conf = {
    .spi = TEST_MCP2515_SPI,
    .spi_mode = TEST_MCP2515_SPI_MODE,
    .spi_clk = TEST_MCP2515_SPI_CLK,
    .cs_pin = TEST_MCP2515_CS,
    .rst_pin = TEST_MCP2515_RESET,
    .int_pin = TEST_MCP2515_INT,
    .clk = TEST_MCP2515_CLK,
};

candev_mcp2515_t mcp2515_dev = { 0 };

#define RX_RINGBUFFER_SIZE 128      //Needs to be a power of 2!
isrpipe_t rxbuf;

static candev_t *candev = NULL;

float setTemp = 10;
float actTemp = 1;
uint8_t pumpstatus = 0;     //0: idle, 1: heating, 2: overheat, 3: err
uint8_t update_actTemp = 1;
uint8_t update_setTemp = 1;
uint8_t update_pumpStatus = 1;

#define ENC1    GPIO_PIN(PORT_D, 7)
#define ENC2    GPIO_PIN(PORT_D, 6)
#define SETTEMP_MAX     30
#define SETTEMP_MIN     10

#define T_SENSOR    ADC_LINE(5)
#define T_C1        0.001129148         //Steinhart-hart coefficients for thermistor
#define T_C2        0.000234125
#define T_C3        0.0000000876741
#define T_R1        10000               //Resistor for voltage divider

#define REFRESHTIME     10000   //in us
#define SENDTEMPINTERVAL    1000000 //in us

#define MA_WIDTH    20

union FloatByteUnion{
    uint8_t bytes[4];
    float floatNum;
};
/*
 * Send update of set and actual temperatures over CAN to controller
 */
static int send_tempUpdate(void)
{
    int ret = 0;
    struct can_frame frame = {
        .can_id = 0x01,     //Abuse can_ID as data-identifier. 0x01 = temperature update
        .can_dlc = 8,
        .data[0] = (uint8_t)((((union FloatByteUnion)actTemp).bytes[0])),
        .data[1] = (uint8_t)((((union FloatByteUnion)actTemp).bytes[1])),
        .data[2] = (uint8_t)((((union FloatByteUnion)actTemp).bytes[2])),
        .data[3] = (uint8_t)((((union FloatByteUnion)actTemp).bytes[3])),
        .data[4] = (uint8_t)((((union FloatByteUnion)setTemp).bytes[0])),
        .data[5] = (uint8_t)((((union FloatByteUnion)setTemp).bytes[1])),
        .data[6] = (uint8_t)((((union FloatByteUnion)setTemp).bytes[2])),
        .data[7] = (uint8_t)((((union FloatByteUnion)setTemp).bytes[3])),
    };

    ret = candev->driver->send(candev, &frame);
    if (ret >= 0) {
        DEBUG("sent using mailbox: %d\n", ret);
        return -1;
    }
    else {
        puts_P(PSTR("Failed to send CAN-message!"));
    }

    return 0;

}
static int _send(int argc, char **argv)
{
    int ret = 0;

    struct can_frame frame = {
        .can_id = 1,
        .can_dlc = 3,
        .data[0] = 0xAB,
        .data[1] = 0xCD,
        .data[2] = 0xEF,
    };

    if (argc > 1) {
        if (argc > 1 + CAN_MAX_DLEN) {
            printf_P(PSTR("Could not send. Maximum CAN-bytes: %d\n"), CAN_MAX_DLEN);
            return -1;
        }
        for (int i = 1; i < argc; i++) {
            frame.data[i - 1] = atoi(argv[i]);
        }
        frame.can_dlc = argc - 1;
    }

    ret = candev->driver->send(candev, &frame);
    if (ret >= 0) {
        DEBUG("sent using mailbox: %d\n", ret);
    }
    else {
        puts_P(PSTR("Failed to send CAN-message!"));
    }

    return 0;
}

static int _receive(int argc, char **argv)
{
    uint8_t buf[CAN_MAX_DLEN];
    uint32_t can_id = 0;
    uint8_t can_dlc = 0;
    int n = 1;
    char *pEnd;

    if (argc > 1) {
        n = strtol(argv[1], &pEnd, 10);
        if (n < 1) {
            puts_P(PSTR("Usage: receive <number>"));
            return -1;
        }
    }

    for (int i = 0; i < n; i++) {

        puts_P(PSTR("Reading from Rxbuf..."));
        isrpipe_read(&rxbuf, buf, 4);       //id
        can_id = (buf[0] << 6) | (buf[1] << 4) | (buf[2] << 2) | (buf[3]);
        isrpipe_read(&rxbuf, buf, 1);       //dlc
        can_dlc = buf[0];
        isrpipe_read(&rxbuf, buf, can_dlc); //data

        printf("id: %" PRIx32 " dlc: %" PRIx8 " Data: \n", can_id, can_dlc);
        for (int i = 0; i < can_dlc; i++) {
            printf("0x%X ", buf[i]);
        }
        puts("");
    }

    return 0;
}

static void _can_event_callback(candev_t *dev, candev_event_t event, void *arg)
{
    (void)arg;
    struct can_frame *frame;

    switch (event) {
        case CANDEV_EVENT_ISR:
            DEBUG("_can_event: CANDEV_EVENT_ISR\n");
            dev->driver->isr(candev);
            break;
        case CANDEV_EVENT_WAKE_UP:
            DEBUG("_can_event: CANDEV_EVENT_WAKE_UP\n");
            break;
        case CANDEV_EVENT_TX_CONFIRMATION:
            DEBUG("_can_event: CANDEV_EVENT_TX_CONFIRMATION\n");
            break;
        case CANDEV_EVENT_TX_ERROR:
            DEBUG("_can_event: CANDEV_EVENT_TX_ERROR\n");
            break;
        case CANDEV_EVENT_RX_INDICATION:
            DEBUG("_can_event: CANDEV_EVENT_RX_INDICATION\n");

            frame = (struct can_frame *)arg;

            DEBUG("\tid: %" PRIx32 " dlc: %" PRIx8 " Data: \n\t", frame->can_id,
                  frame->can_dlc);
            for (uint8_t i = 0; i < frame->can_dlc; i++) {
                DEBUG("0x%X ", frame->data[i]);
            }
            DEBUG(" ");

            //TODO: also change this to CAN_ID instead of identifier byte?
            if (frame->can_id == 0x02) {    //update pump status
                pumpstatus = frame->data[0];
                update_pumpStatus = 1;
            }

            break;
        case CANDEV_EVENT_RX_ERROR:
            DEBUG("_can_event: CANDEV_EVENT_RX_ERROR\n");
            break;
        case CANDEV_EVENT_BUS_OFF:
            dev->state = CAN_STATE_BUS_OFF;
            break;
        case CANDEV_EVENT_ERROR_PASSIVE:
            dev->state = CAN_STATE_ERROR_PASSIVE;
            break;
        case CANDEV_EVENT_ERROR_WARNING:
            dev->state = CAN_STATE_ERROR_WARNING;
            break;
        default:
            DEBUG("_can_event: unknown event\n");
            break;
    }
}

static float pseudo_MA(float avg, float val, int width)
{
    return avg + ((val - avg) / width);
}

void updateLCD(void)
{
    char lcd_buf[MAX_LCD_WIDTH] = { '\0' };
    if(update_setTemp) {
        sprintf(lcd_buf, "%.1f C", setTemp);
        glcd_draw_text(1, 20, &proportional_font, lcd_buf);
        xtimer_usleep(1000);
        update_setTemp = 0;
    }
    if (update_actTemp) {
        sprintf(lcd_buf, "%.1f C", actTemp);
        glcd_draw_text(3, 20, &proportional_font, lcd_buf);
        xtimer_usleep(1000);
        update_actTemp = 0;
    }
    if(update_pumpStatus) {
        switch (pumpstatus) {
            case 0:
                glcd_draw_text(5, 30, &proportional_font, "Idle              ");
                break;
            case 1:
                glcd_draw_text(5, 30, &proportional_font, "Heating       ");
                break;
            case 2:
                glcd_draw_text(5, 30, &proportional_font, "Overheat       ");
                break;
            case 3:
                glcd_draw_text(5, 30, &proportional_font, "Error       ");
                break;
            default:
                glcd_draw_text(5, 30, &proportional_font, "Error       ");
                puts_P(PSTR("Error: unknown pumpstate"));
                break;
        }
        xtimer_usleep(1000);
        update_pumpStatus = 0;
    }
}

static void read_encoder(void *arg)
{
    (void) arg;
    int stat1 = 0;
    int stat2 = 0;
    static int stat1_prev = 1;


    stat1 = gpio_read(ENC1);
    stat2 = gpio_read(ENC2);

    if(!stat1_prev  && stat1 ) {
        if(!stat2 ) {
            setTemp -= 0.5;
        } else {
            setTemp += 0.5;
        }
        if(setTemp > SETTEMP_MAX) {
            setTemp = SETTEMP_MAX;
        } else if(setTemp < SETTEMP_MIN) {
            setTemp = SETTEMP_MIN;
        }
        update_setTemp = 1;
    }
    stat1_prev = stat1;
}

static int read_temperature(void)
{
    int val = 0;
    float R2 = 0;
    float logR2 = 0;
    float temperature = 0;

    val = adc_sample(T_SENSOR, ADC_RES_10BIT);
    if(val == -1){
        puts("Failed sample");
        return -1;
    }
    R2 = T_R1 * (1023.0 / (float) val - 1.0);
    logR2 = log(R2);
    temperature = (1.0 / (T_C1 + T_C2*logR2 + T_C3*logR2*logR2*logR2));
    temperature = temperature - 273.15;

    actTemp = pseudo_MA(actTemp, temperature, MA_WIDTH); 

    update_actTemp = 1;


    return 0;

}

static int init_temperature_sensor(void) 
{
    int ret = adc_init(T_SENSOR);
    if(ret) {
        puts_P(PSTR("Error initializing temperature sensor"));
    }

    return ret;
}

int main(void)
{
    uint8_t rx_ringbuf[RX_RINGBUFFER_SIZE] = { 0 };
    int res = 0;
    (void) _can_event_callback;
    int prescaler = 0;

    //initialize temperature sensor
    if(init_temperature_sensor()) {
        puts("Error: exiting!");
        while(1);
    }

    //initialize glcd
    glcd_init();
    glcd_clear_display();

    //initialize CAN
    isrpipe_init(&rxbuf, (uint8_t *)rx_ringbuf, sizeof(rx_ringbuf));
    candev_mcp2515_init(&mcp2515_dev, &mcp2515_conf);
    candev = (candev_t *)&mcp2515_dev;

    assert(candev);

    candev->event_callback = _can_event_callback;
    candev->isr_arg = NULL;

    candev->driver->init(candev);

    //initialize encoder
    res = gpio_init_int(ENC1, GPIO_IN, GPIO_BOTH, read_encoder, NULL);
    if(res) {
        puts_P(PSTR("Error setting irq"));
    }
    res = gpio_init_int(ENC2, GPIO_IN, GPIO_BOTH, read_encoder, NULL);
    if(res) {
        puts_P(PSTR("Error setting irq"));
    }


    //Display initial screen
    glcd_draw_text_P(0, 0, &proportional_font, PSTR("Gewenste temperatuur: "));
    glcd_draw_text_P(2, 0, &proportional_font, PSTR("Huidige temperatuur: "));
    glcd_draw_text_P(5, 0, &proportional_font, PSTR("Status: "));

    (void) _send;
    (void) _receive;

    while(1) {
        xtimer_usleep(REFRESHTIME);
        read_temperature();
        if(update_actTemp || update_setTemp || update_pumpStatus) {
            updateLCD();
        }

        if(prescaler >= SENDTEMPINTERVAL / REFRESHTIME)
        {
            send_tempUpdate();
            prescaler = 0;
        }
        prescaler++;

    }
    return 0;
}
