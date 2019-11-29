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
    uint8_t testdata[] = {0xaa, 0xab, 0xac};
    xtimer_usleep(2000000);
    puts("Test");
    test_funcc();
    DoIP_Data_request(0, 0, physical, NULL, 0);
    DoIP_Data_request(0xabcd, 0xdcba, physical, testdata , 3);
    puts("test2");
    return 0;
}
