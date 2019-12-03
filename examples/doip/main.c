/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>
#include <xtimer.h>
#include <doip.h>

int main(void)
{
    char ip_addr[] = "192.168.1.122";
    doip_sa source = 0xe801;
    doip_ta target = 0x1fff;
    uint8_t uds_data[] = {UDS_SERVICES_TP, UDS_SUPRESS_REPLY};

    puts("Starting doip example");
    xtimer_usleep(2000000);     //necesarry for some reason when sending UDP

    while(1) {
        doip_send_udp(source, target, DOIP_DIAGNOSTIC_MESSAGE, uds_data , 2, ip_addr);
        doip_send_udp(0, 0, DOIP_VEHICLE_IDENTIFICATION_REQUEST, NULL , 0, ip_addr);
        xtimer_usleep(1000000);
    }
    return 0;
}
