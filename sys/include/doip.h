/**
 * DoIP: Diagnostics over IP
 *
 * @brief   Basic implementation of DoIP protocol
 * @author  Wouter Symons <wsymons@nalys-group.com>
 * @}
 */
#ifndef _DOIP_H_
#define _DOIP_H_

#include <stdint.h>
#include "net/sock/udp.h"
#include "net/sock/tcp.h"

/*
 * All DoIP layer services have the same general format. Service primitives are written in the form:
 * service_name.type(parameter A, parameter B [, parameter C, ...])
 *
 * where “service_name” is the name of the service, e.g. _doip_data; “type” indicates the type of the service
 * primitive; and “parameter A, parameter B, [parameter C, ...]” are the doip_SDU as a list of values passed by
 * the service primitive. The brackets indicate that this part of the parameter list may be empty.
 *
 * Available service primitives:
 * - request: request a service from the service provider
 * - indication: service provider informs a service user about an internal event of the network layer or the service request of a peer protocol layer entity service user
 * - confirm: service provider informs the service user about the result of a preceding service request of the service user.
 */

/* ##### DoIP constants ####*/

#define DOIP_MAX_MSG_LEN 512   //NOTE: Spec defines max as 2^32 (~4GB), but this feels like a bad idea to implement on an embedded device...
#define DOIP_VERSION    0x02
#define DOIP_HDR_LEN    12

#define UDS_SERVICES_DSC     0x10
#define UDS_SERVICES_ER      0x11
#define UDS_SERVICES_CDTCI   0x14
#define UDS_SERVICES_RDTCI   0x19
#define UDS_SERVICES_RDBI    0x22
#define UDS_SERVICES_RMBA    0x23
#define UDS_SERVICES_RSDBI   0x24
#define UDS_SERVICES_SA      0x27
#define UDS_SERVICES_CC      0x28
#define UDS_SERVICES_RDBPI   0x2A
#define UDS_SERVICES_DDDI    0x2C
#define UDS_SERVICES_WDBI    0x2E
#define UDS_SERVICES_IOCBI   0x2F
#define UDS_SERVICES_RC      0x31
#define UDS_SERVICES_RD      0x34
#define UDS_SERVICES_RU      0x35
#define UDS_SERVICES_TD      0x36
#define UDS_SERVICES_RTE     0x37
#define UDS_SERVICES_RFT     0x38
#define UDS_SERVICES_WMBA    0x3D
#define UDS_SERVICES_TP      0x3E
#define UDS_SERVICES_ERR     0x3F
#define UDS_SERVICES_CDTCS   0x85

#define UDS_RESPONSE_CODES_GR       0x10
#define UDS_RESPONSE_CODES_SNS      0x11
#define UDS_RESPONSE_CODES_SFNS     0x12
#define UDS_RESPONSE_CODES_IMLOIF   0x13
#define UDS_RESPONSE_CODES_RTL      0x14
#define UDS_RESPONSE_CODES_BRR      0x21
#define UDS_RESPONSE_CODES_CNC      0x22
#define UDS_RESPONSE_CODES_RSE      0x24
#define UDS_RESPONSE_CODES_NRFSC    0x25
#define UDS_RESPONSE_CODES_FPEORA   0x26
#define UDS_RESPONSE_CODES_ROOR     0x31
#define UDS_RESPONSE_CODES_SAD      0x33
#define UDS_RESPONSE_CODES_IK       0x35
#define UDS_RESPONSE_CODES_ENOA     0x36
#define UDS_RESPONSE_CODES_RTDNE    0x37
#define UDS_RESPONSE_CODES_UDNA     0x70
#define UDS_RESPONSE_CODES_TDS      0x71
#define UDS_RESPONSE_CODES_GPF      0x72
#define UDS_RESPONSE_CODES_WBSC     0x73
#define UDS_RESPONSE_CODES_RCRRP    0x78
#define UDS_RESPONSE_CODES_SFNSIAS  0x7E
#define UDS_RESPONSE_CODES_SNSIAS   0x7F

#define UDS_SUPRESS_REPLY           0x80

#define DOIP_GENERIC_DOIP_HEADER_NEGATIVE_ACK           0x0000
#define DOIP_VEHICLE_IDENTIFICATION_REQUEST             0x0001
#define DOIP_VEHICLE_IDENTIFICATION_REQUEST_WITH_EID    0x0002
#define DOIP_VEHICLE_IDENTIFICATION_REQUEST_WITH_VIN    0x0003
#define DOIP_VEHICLE_IDENTIFCATION_RESPONSE             0x0004
#define DOIP_ROUTING_ACTIVAION_REQUEST                  0x0005
#define DOIP_ALIVE_CHECHK_REQUEST                       0x0007
#define DOIP_ALIVE_CHECK_RESPONSE                       0x0008
#define DOIP_DOIP_ENTITY_STATUS_REQUEST                 0x4001
#define DOIP_DOIP_ENTITY_STATUS_RESPONSE                0x4002
#define DOIP_DIAGNOSTIC_POWER_MODE_INFO_REQUEST         0x4003
#define DOIP_DIAGNOSTIC_POWER_MODE_INFO_RESPONSE        0x4004
#define DOIP_DIAGNOSTIC_MESSAGE                         0x8001
#define DOIP_DIAGNOSTIC_MESSAGE_POSITIVE_ACK            0x8002
#define DOIP_DIAGNOSTIC_MESSAGE_NEGATVE_ACK             0x8003

//added these from scapy uds file, not sure if they are standard or not 
#define UDS_REQUEST_READ_DATA_BY_IDENTIFIER_POSITIVE_RESPONSE    0x62

/* ########################*/


/* ##### DoIP Types #### */

/**
 * @brief doip logical sa address
 */
typedef uint16_t doip_sa;

/**
 * @brief doip logical ta address
 */
typedef uint16_t doip_ta;

/**
 * @brief doip socket
 */
typedef struct sock_doip_t {
        sock_udp_t udp_sock;
        sock_tcp_t tcp_sock;
        //TODO: should contain more ()

} sock_doip_t;

/**
 * @brief doip logical ta address type
 *
 * @detailed Extension to the doip_ta parameter.
 *      It is used to encode the communication model used by the communicating
 *      peer entities of the doip layer.  Two communication models are specified:
 *      1 to 1 communication, called physical addressing (unicast), and 1 to n
 *      communication, called functional addressing (multicast/broadcast).
 */
typedef enum doip_tat { physical, functional, doip_tat_MAX } doip_tat;

/**
 * @brief Outcome of service execution
 *
 * @detailed Outcome of service execution. If two or more errors are discoverd at the same time, the parameter first found in this list will be given
 */
typedef enum doip_result {
    DOIP_OK,
    DOIP_HDR_ERROR,
    DOIP_TIMEOUT_A,
    DOIP_UNKNOWN_SA,
    DOIP_INVALID_SA,
    DOIP_UNKNOWN_TA,
    DOIP_MESSAGE_TOO_LARGE,
    DOIP_OUT_OF_MEMORY,
    DOIP_taRGET_UNREACHABLE,
    DOIP_NO_LINK,
    DOIP_NO_SOCKET,
    DOIP_ERROR
} doip_result;



/* ####################*/


/* ####### DoIP Functions ########## */

/*
 * @brief create DoIP socket
 */
int sock_doip_create(sock_doip_t *sock);

/*
 * @brief close DoIP socket
 */
int sock_doip_close(sock_doip_t *sock);

/*
 * @brief request transmission of <data> with <dlen> bytes
 */
int doip_data_request(doip_sa sa, doip_ta ta, doip_tat tat, uint8_t *data,
                      uint32_t dlen);

/*
 * @brief confirm completion of doip_data.request
 */
int doip_data_confirm(doip_sa sa, doip_ta ta, doip_tat tat, doip_result res);

/*
 * @brief reply with requested data
 */
int doip_data_indication(doip_sa sa, doip_ta ta, doip_tat tat, uint8_t *data,
                         uint32_t dlen, doip_result res);

/*
 * @brief raw UDP send function for DoIP messages
 *
 * @param[in]       sa              Source Adress
 * @param[in]       ta              Target Address
 * @param[in]       payload_type    Type of DoIP message in payload
 * @param[in]       data            UDS bytes to be added as payload to DoIP message
 * @param[in]       ip_addr         IP address the message should be sent to
 *
 * TODO: make static once we're out of development
 */
int doip_send_udp(sock_doip_t *sock, doip_sa sa, doip_ta ta, uint16_t payload_type, uint8_t *data,
                  uint32_t dlen, char *ip_addr);

/*
 * @brief open TCP socket for DoIP communication
 */
int doip_tcp_connect(sock_doip_t *sock, char* ip);

int doip_tcp_disconnect(sock_doip_t *sock);

/*
 * @brief Send TCP message
 */
int doip_send_tcp(sock_doip_t *sock, doip_sa sa, doip_ta ta, uint16_t payload_type, uint8_t *data, uint32_t dlen, char* ip);

//TODO: define headers for empty optional parameters

/* ################################ */

#endif
