#include <doip.h>
#include <stdio.h>
#include <string.h>
#include "xtimer.h"
#include "color.h"     //enable colored output to be used in demo

//data buffer for doip message
uint8_t dbuf[DOIP_MAX_MSG_LEN];

const uint32_t recv_timeout = 1000000;  //us

static void doip_print_msg(uint8_t *data, uint32_t dlen)
{
    uint32_t i = 0;

    for (i = 0; i < dlen; i++) {
        printf("0x%x ", data[i]);
    }
    printf("\n");
}

static int doip_print_msg_parsed(uint8_t *data, uint32_t dlen)
{
    uint16_t payload_type = 0;
    uint32_t payload_len = 0;
    uint16_t source = 0;
    uint16_t target = 0;
    uint32_t i = 0;


    if (dlen < 8 || data == NULL) {     //basic DoIP is always 8 bytes. TA and SA can be dropped
        return -1;
    }

    if (data[0] == 0x0) {
        puts("DoIp version: 01");
    }
    if (data[0] == 0x02) {
        puts("DoIP version: 02");
    }

    payload_type = (data[2] << 8 | data[3]);
    //printf("payload_type: %x\n", payload_type);
    switch (payload_type) {
        case DOIP_GENERIC_DOIP_HEADER_NEGATIVE_ACK:
            puts("Payload Type: Generic negative ACK");
            break;
        case DOIP_VEHICLE_IDENTIFICATION_REQUEST:
            puts("Payload Type: Vehicle Identification Request");
            break;
        case DOIP_VEHICLE_IDENTIFICATION_REQUEST_WITH_VIN:
            puts("Payload Type: Vehicle Identification Request with VIN");
            break;
        case DOIP_VEHICLE_IDENTIFICATION_REQUEST_WITH_EID:
            puts("Payload Type: Vehicle Identification Request with EID");
            break;
        case DOIP_VEHICLE_IDENTIFCATION_RESPONSE:
            puts("Payload Type: Vehicle Identification Reply");
            break;
        case DOIP_DIAGNOSTIC_MESSAGE:
            puts("Payload Type: Diagnostic Message Request");
            break;
        case DOIP_DIAGNOSTIC_MESSAGE_POSITIVE_ACK:
            puts("Payload Type: Diagnostic Message Positive Acknowledge");
            break;

        //TODO: implement others!
        default:
            puts("Unknown payload type");
            break;
    }
    payload_len = (data[4] << 24 | data[5] << 16 | data[6] << 8 | data[7]);
    printf("Payload length: %ld\n", payload_len);

    if (dlen < 12) {
        return payload_type;
    }
    source = (data[8] << 8 | data[9]);
    target = (data[10] << 8 | data[11]);
    printf("Source address: %x, target address: %x\n", source, target);

    puts("UDS data:");
    for (i = 12; i < dlen; i++) {
        printf("0x%x ", data[i]);
    }
    printf("\n");


    return payload_type;


}

int sock_doip_create(sock_doip_t *sock) //TODO: make seperate create func for tcp and udp part?
{
    sock_udp_ep_t local = SOCK_IPV4_EP_ANY;

    local.port = 41123;

    if (sock_udp_create(&(sock->udp_sock), &local, NULL, 0)) {
        puts("Error creating UDP sock");
        return 1;
    }

    //NOTE: If it is called here, user changing IP will not have effect on this socket! //TODO!
    //doip_tcp_connect(sock, ip);   //NOTE: we can't place this here, because it's likely we didn't get an IP through DHCP yet. Maybe we can place this here with some kind of check for dhcp? for now, we let user do it.

    return 0;
}

int sock_doip_close(sock_doip_t *sock)
{
    sock_udp_close(&(sock->udp_sock)); //TODO: error check?

    return 0;
}

static int doip_create_message(doip_sa sa, doip_ta ta, uint16_t payload_type, uint8_t *data, uint32_t dlen, uint8_t* dbuf)
{
    uint32_t payload_len = dlen;
    if(payload_type != 0x0001) {
        payload_len += 4;   //TA and SA should also be included in the payload length!
    }

    int msglen = 0;         //Total length of message to be sent over UDP

    if (data == NULL && dlen != 0) {
        return -1;
    }

    dbuf[0] = DOIP_VERSION;                     //Byte 0: Protocol version
    dbuf[1] = ~DOIP_VERSION;                    //Byte 1: Inverse Protocol version
    dbuf[2] = (payload_type >> 8);              //Byte 2 - 3: Payload type
    dbuf[3] = (payload_type & 0xFF);

    dbuf[4] = ((payload_len >> 24) & 0xFF);     //Byte 4 - 7: Payload length
    dbuf[5] = ((payload_len >> 16) & 0xFF);
    dbuf[6] = ((payload_len >> 8) & 0xFF);
    dbuf[7] = (payload_len & 0xFF);

    msglen += 8;

    if (payload_type != 0x0001) {       //No SA and TA sent with VID req
        dbuf[8] = (sa >> 8);            //Byte 8 - 9: Source Adress
        dbuf[9] = (sa & 0xFF);

        dbuf[10] = (ta >> 8);                       //Byte 10 - 11: Target Adress
        dbuf[11] = (ta & 0xFF);

        msglen += 4;
    }

    if (dlen > 0) {
        memcpy(&dbuf[12], data, dlen);                  //Byte 12 - 13+dlen: data
        msglen += dlen;
    }

    return msglen;
}

int doip_send_udp(sock_doip_t *sock, doip_sa sa, doip_ta ta, uint16_t payload_type, uint8_t *data, uint32_t dlen, char *ip_addr)
{
    int ret = 0;
    uint8_t buf[50] = { '\0' };
    int msglen = 0;
    sock_udp_ep_t remote = SOCK_IPV4_EP_ANY;

    if(sock == 0) {
        puts("DoIP sock empty.\n");
        return -1;
    }

    msglen = doip_create_message(sa, ta, payload_type, data, dlen, dbuf);
    if(msglen < 0)
        return -1;

    puts(YEL);
    printf("doip Message to be sent: \n");
    doip_print_msg(dbuf, msglen);
    puts(reset);


    remote.port = 13400;
    ipv4_addr_from_str((ipv4_addr_t *)&remote.addr.ipv4, ip_addr);

    if ((ret = sock_udp_send(&(sock->udp_sock), dbuf, msglen, &remote)) < 0) {
        puts("Error sending datagram");
        printf("Err: %d\n", ret );              //22 = EINVAL --> invalid argument
        return -1;
    }

    if ((ret = sock_udp_recv(&(sock->udp_sock), buf, sizeof(buf), recv_timeout,
                           NULL)) < 0) {
        if (ret == -ETIMEDOUT) {            //TODO: I think there's still something off with the timeout. It keeps blocking...
            puts("Timed out");
        }
        else {
            puts("Error receiving message");
        }
    }
    else {
        puts(GRN);
        printf("Received answer: \"");
        for (int i = 0; i < ret; i++) {
            printf("0x%x ", buf[i]);
        }
        printf("\"\n");
        doip_print_msg_parsed(buf, ret);
        puts(reset);
    }

    puts("======================");



    return 0;
}

int doip_tcp_connect(sock_doip_t *sock, char* ip)
{
    int ret = 0;
    sock_tcp_ep_t remote = SOCK_IPV4_EP_ANY;

    //remote.port = 12345;
    remote.port = 13400;
    ipv4_addr_from_str((ipv4_addr_t *)&remote.addr, ip);
    ret = sock_tcp_connect(&(sock->tcp_sock), &remote, 0, 0);
    if(ret < 0) {
        puts("Error connecting sock");
        return -1;
    }


    return 0;
}
int doip_tcp_disconnect(sock_doip_t *sock)
{
    sock_tcp_disconnect(&(sock->tcp_sock));
    return 0;
}

int doip_send_tcp(sock_doip_t *sock, doip_sa sa, doip_ta ta, uint16_t payload_type, uint8_t *data, uint32_t dlen, char* ip)
{
    int ret = 0;
    int msglen = 0;
    int msg_type = 0;

    ret = doip_tcp_connect(sock, ip);
    if(ret != 0) {
        doip_tcp_disconnect(sock);
        return ret;
    }


    msglen = doip_create_message(sa, ta, payload_type, data, dlen, dbuf);
    if(msglen < 0)
        return -1;


    ret = sock_tcp_write(&(sock->tcp_sock), dbuf, msglen);
    if(ret < 0) {
        printf("Error on write: %d\n", ret);
    } else {
        ret = sock_tcp_read(&(sock->tcp_sock), &dbuf, sizeof(dbuf), recv_timeout);
        if(ret < 0) {
            puts("Disconnected");
        } else {
            puts(GRN);
            printf("Received answer: ");
            for(int i = 1; i < ret; i++)
            {
                printf("0x%.2x ", dbuf[i]);
            }
            puts("\"");
            msg_type = doip_print_msg_parsed(dbuf, ret);
            puts(reset);
        }
        //DOIP_ACK usually means we expect an extra reply with the data we initially requested
        if(msg_type == DOIP_DIAGNOSTIC_MESSAGE_POSITIVE_ACK)
        {
            puts("Waiting for data...");
            ret = sock_tcp_read(&(sock->tcp_sock), &dbuf, sizeof(dbuf), recv_timeout);
            if(ret < 0) {
                puts("Disconnected");
            } else {
                puts(GRN);
                printf("Received data: \"");
                for(int i = 1; i < ret; i++)
                {
                    printf("0x%.2x ", dbuf[i]);
                }
                puts("\"");
                msg_type = doip_print_msg_parsed(dbuf, ret);
                puts(reset);
            }

        }
        puts("=================================");
    }

    doip_tcp_disconnect(sock);


    return 0;
}
