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
    puts("Starting doip example");
    //uint8_t testdata[] = {0xaa, 0xab, 0xac};
    uint8_t uds_data[] = {0x3e, 0x80};
    xtimer_usleep(2000000);     //necesarry for some reason when sending UDP
    puts("Sending ");
    //doip_data_request(0, 0, physical, NULL, 0);
    //doip_data_request(0xabcd, 0xdcba, physical, testdata , 3);
    doip_send_udp(0xe801, 0x1fff, 0x8001, uds_data , 6);
    doip_send_udp(0, 0, 0x0001, NULL , 0);  //Sending vehicle identification request --> SA and TA ignored
    return 0;
}
