
#ifndef BOARD_CAN_H_
#define BOARD_CAN_H_
#include <stdint.h>
#include <string.h>


#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Sends an arbitrary sized packet over CAN
 */
void bd_writeCAN(uint8_t *data, size_t len);

/**
 * \brief reads arbitrary sized packet over CAN
 */
size_t bd_readCAN(uint8_t *data);

/**
 * \brief Checks if there is available data on the can bus
 */
int bd_checkCAN();

/**
 * \brief initializes the CAN interface
 */
void bd_initCAN();

#ifdef __cplusplus
}
#endif

#endif
