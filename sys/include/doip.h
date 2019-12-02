#ifndef _DOIP_H_
#define _DOIP_H_

#include <stdint.h>

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
 * - inidcation: service provider informs a service user about an internal event of the network layer or the service request of a peer protocol layer entity service user
 * - confirm: service provider informs the service user about the result of a preceding service request of the service user.
 */

/* ##### DoIP constants ####*/

#define DOIP_MAX_MSG_LEN 512   //NOTE: Spec defines max as 2^32 (~4GB), but this feels like a bad idea to implement on an embedded device...
#define DOIP_VERSION    0x02
#define DOIP_HDR_LEN    12

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
 * @brief doip logical ta address type
 *
 * @detailed Extension to the doip_ta parameter.
 *      It is used to encode the communication model used by the communicating
 *      peer entities of the doip layer.  Two communication models are specified:
 *      1 to 1 communication, called physical addressing (unicast), and 1 to n
 *      communication, called functional addressing (multicast/broadcast).
 */
typedef enum doip_tat{physical, functional, doip_tat_MAX} doip_tat;

/**
 * @brief Outcome of service execution
 *
 * @detailed Outcome of service execution. If two or more errors are discoverd at the same time, the parameter first found in this list will be given
 */
typedef enum doip_result{
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
 * @brief request transmission of <data> with <dlen> bytes
 */
int doip_data_request(doip_sa sa, doip_ta ta, doip_tat tat , uint8_t* data, uint32_t dlen);

/*
 * @brief confirm completion of doip_data.request
 */
int doip_data_confirm(doip_sa sa, doip_ta ta, doip_tat tat, doip_result res);

/*
 * @brief reply with requested data
 */
int doip_data_indication(doip_sa sa, doip_ta ta, doip_tat tat, uint8_t* data, uint32_t dlen, doip_result res);

/*
 * @brief raw UDP send function for DoIP messages
 * TODO: make static once we're out of development
 */
int doip_send_udp(doip_sa sa, doip_ta ta, uint16_t payload_type, uint8_t* data, uint32_t dlen);

/* ################################ */


//TODO: define headers for empty optional parameters

#endif
