/* ========================================
 *
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
 */
#include <project.h>
//#include <device.h>
#include <stdio.h>  /* sprintf */
#include <stdlib.h> /* atoi */
#include <numerical.h>



/* Global variable used to store ISR flag */
volatile uint8 isrFlag = 0u;
volatile static size_t rec_length = 0;
volatile uint8_t can_buffer[120] = {0};

#define USE_CAN

/**
 * \brief Sends an arbitrary sized packet over CAN on mailbox 0.
 */
void bd_writeCAN(uint8_t *data, size_t len) {
#ifdef USE_CAN
  const size_t MAX_BUFFER_LEN = 0x08;
  size_t buffer_size = MAX_BUFFER_LEN;
  uint32_t clear_default_bits = 0x0F << CAN_TWO_BYTE_OFFSET;

  CAN_TX_MSG packet;
  CAN_DATA_BYTES_MSG packet_data;


  for (size_t index = 0; index < len; index+=buffer_size) {
    //wait for mailbox 0 to be ready
    while ((CAN_TX[0u].txcmd.byte[0u] & CAN_TX_REQUEST_PENDING) != 0u);

    buffer_size = nm_constrain(len - index, 0, MAX_BUFFER_LEN);

    switch (buffer_size) {
      case 8: CAN_TX_DATA_BYTE8(0) = data[7+index];
      case 7: CAN_TX_DATA_BYTE7(0) = data[6+index];
      case 6: CAN_TX_DATA_BYTE6(0) = data[5+index];
      case 5: CAN_TX_DATA_BYTE5(0) = data[4+index];
      case 4: CAN_TX_DATA_BYTE4(0) = data[3+index];
      case 3: CAN_TX_DATA_BYTE3(0) = data[2+index];
      case 2: CAN_TX_DATA_BYTE2(0) = data[1+index];
      case 1: CAN_TX_DATA_BYTE1(0) = data[0+index];
    }

    CY_SET_REG32(CAN_TX_CMD_PTR(0u),
        CY_GET_REG32(CAN_TX_CMD_PTR(0u)) & ((uint32) ~clear_default_bits));
    CY_SET_REG32(CAN_TX_CMD_PTR(0u),
        CY_GET_REG32(CAN_TX_CMD_PTR(0u)) | ((uint32) buffer_size) << CAN_TWO_BYTE_OFFSET);

    CAN_SendMsg0();
  }
#endif
}

/**
 * \brief reads arbitrary sized packet over CAN
 */
size_t bd_readCAN(uint8_t *data) {
  size_t rec_length_return = rec_length;

#ifdef USE_CAN
  memcpy(data, can_buffer, rec_length);
  //reset length to zero
  rec_length = 0;

#endif
  return rec_length_return;
}

/**
 * \brief Callback to interop with PSOC interrupt
 */
void CAN_ReceiveMsg_Callback(uint8 rxMailbox) {
#ifdef USE_CAN
  size_t rec_length_prev = rec_length;

  rec_length += CAN_GET_DLC(rxMailbox);

  for (int i = 0; i < rec_length; ++i) {
    can_buffer[i + rec_length_prev] = CAN_RX_DATA_BYTE(rxMailbox, i);
  }
#endif
}

/**
 * \brief Checks if there is available data on the can bus
 */
int bd_checkCAN() {
  if (rec_length > 0)
    return 1;
  else
    return 0;
}

/**
 * \brief initializes the CAN interface
 */
void bd_initCAN() {
#ifdef USE_CAN
  CAN_Start();
#endif
}
