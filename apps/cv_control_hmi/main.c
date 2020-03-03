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



#define ENABLE_DEBUG (1)

#include <debug.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <isrpipe.h>
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
            printf("Could not send. Maximum CAN-bytes: %d\n", CAN_MAX_DLEN);
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
/*
static const shell_command_t shell_commands[] = {
    { "send", "send some data", _send },
    { "receive", "receive some data", _receive },
    { NULL, NULL, NULL }
};
*/

static void _can_event_callback(candev_t *dev, candev_event_t event, void *arg)
{
    (void)arg;
    struct can_frame *frame;
    char lcd_buf[MAX_LCD_WIDTH] = { '\0' };
    char tmp = 0;

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

            puts("A");
            //process received data
            tmp = frame->data[1];
            if(frame->data[0] == 0x01) {    //Temperature update
                printf("%d degC\n", tmp);
                sprintf(lcd_buf, "%d degC", tmp);
                printf_P(PSTR("Setting: %s\n"), lcd_buf);
                glcd_draw_text(1, 20, &proportional_font, lcd_buf);
                puts("l");
            }
            puts_P(PSTR("leaving"));

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

int main(void)
{
    uint8_t rx_ringbuf[RX_RINGBUFFER_SIZE] = { 0 };
    (void) _can_event_callback;

    glcd_init();
    glcd_clear_display();

    isrpipe_init(&rxbuf, (uint8_t *)rx_ringbuf, sizeof(rx_ringbuf));
    candev_mcp2515_init(&mcp2515_dev, &mcp2515_conf);
    candev = (candev_t *)&mcp2515_dev;

    assert(candev);

    candev->event_callback = _can_event_callback;
    candev->isr_arg = NULL;

    candev->driver->init(candev);

    glcd_draw_text_P(0, 0, &proportional_font, PSTR("Gewenste temperatuur: "));
    glcd_draw_text_P(1, 20, &proportional_font, PSTR("0 degC"));
    glcd_draw_text_P(2, 0, &proportional_font, PSTR("Huidige temperatuur: "));
    glcd_draw_text_P(3, 20, &proportional_font, PSTR("0 degC"));

    char test = 10;
    char buf[] = { '\0'};
    printf("%d degC\n", test);
    sprintf(buf, "%d degC", test);
    printf("Setting: %s\n", buf);
    glcd_draw_text(1, 20, &proportional_font, buf);

    //char line_buf[SHELL_DEFAULT_BUFSIZE];
    //shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

(void) _send;
(void) _receive;

    while(1) {
        xtimer_sleep(1);
    }
    return 0;
}
