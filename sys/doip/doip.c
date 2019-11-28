#include <doip.h>
#include <stdio.h>
#include <string.h>
#include "net/sock/udp.h"


static void DoIP_print_msg(uint8_t* data, uint32_t dlen)
{
    uint32_t i = 0;
    for(i = 0; i < dlen; i++)
    {
        printf("0x%x ", data[i]);
    }
    printf("\n");
}


int DoIP_Data_request(DoIP_SA source, DoIP_TA target, DoIP_TAType target_type , uint8_t* data, uint32_t dlen)
{
    (void) target_type;             //TODO: what to do with this?

    uint16_t payload_type = 0x8001; //TODO: variable?
    uint32_t payload_len = dlen + 4;

    //Data buffer for DoIP message
    uint8_t dbuf[DoIP_MAX_MSG_LEN] = {0};

    printf("entering DDR block\n");

    if(target_type >= DoIP_TAType_MAX)
        return -1;
    if(data == NULL && dlen != 0)
        return -1;


    //TODO: make a message_create function?

    dbuf[0] = DoIP_VERSION;                 //Byte 0: Protocol version
    dbuf[1] = ~DoIP_VERSION;                //Byte 1: Inverse Protocol version
    memcpy(&dbuf[2], &payload_type, 2);     //Byte 2 - 3: Payload type
    memcpy(&dbuf[4], &payload_len, 4);     //Byte 4 - 8: Payload length
    memcpy(&dbuf[9], &source, 2);           //Byte 9 - 10: Source Adress
    memcpy(&dbuf[11], &target, 2);          //Byte 11 - 12: Target Adress
    memcpy(&dbuf[13], data , dlen);         //Byte 13 - 13+dlen: data


    printf("DoIP Message to be sent: \n");
    DoIP_print_msg(dbuf, dlen+12);

    //TODO: seperate function for the actual sending?

    sock_udp_ep_t remote = SOCK_IPV4_EP_ANY;
    int ret = 0;

    remote.port = 13400;
    ipv4_addr_from_str((ipv4_addr_t *)&remote.addr.ipv4, "169.254.73.148");

    if((ret = sock_udp_send(NULL, dbuf, dlen+12, &remote)) < 0) {
        puts("Error sending datagram");
        printf("Err: %d\n", ret );      //22 = EINVAL --> invalid argument
        return -1;
    }




    return 0;
}
