#include <doip.h>
#include <stdio.h>
#include <string.h>
#include "net/sock/udp.h"

//data buffer for doip message
uint8_t dbuf[DOIP_MAX_MSG_LEN];

static void doip_print_msg(uint8_t* data, uint32_t dlen)
{
    uint32_t i = 0;
    for(i = 0; i < dlen; i++)
    {
        printf("0x%x ", data[i]);
    }
    printf("\n");
}

//int doip_data_request(doip_sa sa, doip_ta ta, doip_tat tat , uint8_t* data, uint32_t dlen)
int doip_send_udp(doip_sa sa, doip_ta ta, uint16_t payload_type, uint8_t* data, uint32_t dlen)
{
    if(data == NULL && dlen != 0)
        return -1;


    //TODO: make a message_create function?

    dbuf[0] = DOIP_VERSION;                 //Byte 0: Protocol version
    dbuf[1] = ~DOIP_VERSION;                //Byte 1: Inverse Protocol version
    dbuf[2] = (payload_type >> 8);          //Byte 2 - 3: Payload type
    dbuf[3] = (payload_type & 0xFF);

    dbuf[4] = ((dlen >> 24) & 0xFF); //Byte 4 - 7: Payload length
    dbuf[5] = ((dlen >> 16) & 0xFF);
    dbuf[6] = ((dlen >> 8) & 0xFF);
    dbuf[7] = (dlen & 0xFF);

    dbuf[8] = (sa >> 8);                 //Byte 8 - 9: Source Adress
    dbuf[9] = (sa & 0xFF);

    dbuf[10] = (ta >> 8);               //Byte 10 - 11: Target Adress
    dbuf[11] = (ta & 0xFF);

    if(dlen > 0)
        memcpy(&dbuf[12], data , dlen);         //Byte 12 - 13+dlen: data


    printf("doip Message to be sent: \n");
    doip_print_msg(dbuf, dlen+12);

    //TODO: seperate function for the actual sending?

    sock_udp_ep_t remote = SOCK_IPV4_EP_ANY;
    int ret = 0;

    remote.port = 13400;
    ipv4_addr_from_str((ipv4_addr_t *)&remote.addr.ipv4, "169.254.73.148");

    if((ret = sock_udp_send(NULL, dbuf, dlen + DOIP_HDR_LEN, &remote)) < 0) {
        puts("Error sending datagram");
        printf("Err: %d\n", ret );      //22 = EINVAL --> invalid argument
        return -1;
    }


    return 0;
}
