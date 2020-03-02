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
#include "shell.h"
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
        puts("Failed to send CAN-message!");
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
            puts("Usage: receive <number>");
            return -1;
        }
    }

    for (int i = 0; i < n; i++) {

        puts("Reading from Rxbuf...");
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

static const shell_command_t shell_commands[] = {
    { "send", "send some data", _send },
    { "receive", "receive some data", _receive },
    { NULL, NULL, NULL }
};

static void _can_event_callback(candev_t *dev, candev_event_t event, void *arg)
{
    (void)arg;
    struct can_frame *frame;
    char lcd_buf[CAN_MAX_DLEN + 1] = { '\0' };
    static int rulecounter = 0;

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

            //Store in buffer until user requests the data
            isrpipe_write_one(&rxbuf,
                              (uint8_t)((frame->can_id & 0x1FFFFFFF) >> 6));        //exclude flags
            isrpipe_write_one(&rxbuf,
                              (uint8_t)((frame->can_id & 0xFF0000) >> 4));
            isrpipe_write_one(&rxbuf, (uint8_t)((frame->can_id & 0xFF00) >> 2));
            isrpipe_write_one(&rxbuf, (uint8_t)((frame->can_id & 0xFF)));

            isrpipe_write_one(&rxbuf, frame->can_dlc);
            for (uint8_t i = 0; i < frame->can_dlc; i++) {
                isrpipe_write_one(&rxbuf, frame->data[i]);
                lcd_buf[i] = frame->data[i];
            }


            if(rulecounter == 0) {
                glcd_clear_display();
            }
            glcd_draw_text(rulecounter, 20, &proportional_font, lcd_buf);
            rulecounter++;
            if (rulecounter > 5 ) {
                rulecounter = 0;
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

int main(void)
{
    uint8_t rx_ringbuf[RX_RINGBUFFER_SIZE] = { 0 };
    (void) _can_event_callback;

    glcd_init();
    glcd_clear_display();

    isrpipe_init(&rxbuf, (uint8_t *)rx_ringbuf, sizeof(rx_ringbuf));
    //puts("Initializing MCP2515");
    puts("IM");
    candev_mcp2515_init(&mcp2515_dev, &mcp2515_conf);
    candev = (candev_t *)&mcp2515_dev;

    assert(candev);

    candev->event_callback = _can_event_callback;
    candev->isr_arg = NULL;

    candev->driver->init(candev);
    puts("dr");

    glcd_draw_text(1, 20, &proportional_font, "abcd");
    char tmp[] = "aaa";
    glcd_draw_text(3, 10, &proportional_font, tmp);


    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
