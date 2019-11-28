#ifndef _DOIP_H_
#define _DOIP_H_

#include <stdint.h>

/*
 * All DoIP layer services have the same general format. Service primitives are written in the form:
 * service_name.type(parameter A, parameter B [, parameter C, ...])
 *
 * where “service_name” is the name of the service, e.g. _DoIP_Data; “type” indicates the type of the service
 * primitive; and “parameter A, parameter B, [parameter C, ...]” are the DoIP_SDU as a list of values passed by
 * the service primitive. The brackets indicate that this part of the parameter list may be empty.
 *
 * Available service primitives:
 * - request: request a service from the service provider
 * - inidcation: service provider informs a service user about an internal event of the network layer or the service request of a peer protocol layer entity service user
 * - confirm: service provider informs the service user about the result of a preceding service request of the service user.
 */

/* ##### DoIP constants ####*/

#define DoIP_MAX_MSG_LEN 2048   //NOTE: Spec defines max as 2^32 (~4GB), but this feels like a bad idea to implement on an embedded device...
#define DoIP_VERSION    0x02

/* ########################*/


/* ##### DoIP Types #### */

/**
 * @brief DoIP logical source address
 */
typedef uint16_t DoIP_SA;

/**
 * @brief DoIP logical target address
 */
typedef uint16_t DoIP_TA;

/**
 * @brief DoIP logical target address type
 *
 * @detailed Extension to the DoIP_TA parameter.
 *      It is used to encode the communication model used by the communicating
 *      peer entities of the DoIP layer.  Two communication models are specified:
 *      1 to 1 communication, called physical addressing (unicast), and 1 to n
 *      communication, called functional addressing (multicast/broadcast).
 */
typedef enum DoIP_TAType{physical, functional, DoIP_TAType_MAX} DoIP_TAType;

/**
 * @brief Outcome of service execution
 *
 * @detailed Outcome of service execution. If two or more errors are discoverd at the same time, the parameter first found in this list will be given
 */
typedef enum DoIP_Result{
    DoIP_OK,
    DoIP_HDR_ERROR,
    DoIP_TIMEOUT_A,
    DoIP_UNKNOWN_SA,
    DoIP_INVALID_SA,
    DoIP_UNKNOWN_TA,
    DoIP_MESSAGE_TOO_LARGE,
    DoIP_OUT_OF_MEMORY,
    DoIP_TARGET_UNREACHABLE,
    DoIP_NO_LINK,
    DoIP_NO_SOCKET,
    DoIP_ERROR
} DoIP_Result;

/* ####################*/


/* ####### DoIP Functions ########## */

/*
 * @brief request transmission of <data> with <dlen> bytes
 */
int DoIP_Data_request(DoIP_SA source, DoIP_TA target, DoIP_TAType target_type , uint8_t* data, uint32_t dlen);

/*
 * @brief confirm completion of DoIP_Data.request
 */
int DoIP_Data_confirm(DoIP_SA source, DoIP_TA target, DoIP_TAType target_type, DoIP_Result result);

/*
 * @brief
 */
int DoIP_Data_indication(DoIP_SA source, DoIP_TA target, DoIP_TAType target_type, uint8_t* data, uint32_t dlen, DoIP_Result result);

/* ################################ */


//TODO: define headers for empty optional parameters

#endif
