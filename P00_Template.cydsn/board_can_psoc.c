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
#define CAN_BUFFER_SIZE 1000
#define MAX_CAN_READ 1000
volatile uint8_t can_buffer[CAN_BUFFER_SIZE] = {0};

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
  size_t rec_length_return = nm_constrain(rec_length, 0, MAX_CAN_READ);

#ifdef USE_CAN
  memcpy(data, can_buffer, rec_length);
  //reset length to zero
  rec_length -= rec_length_return;

#endif
  return rec_length_return;
}

/**
 * \brief Callback to interop with PSOC interrupt
 */
void CAN_ReceiveMsg_Callback(uint8 rxMailbox) {
#ifdef USE_CAN
  size_t rec_length_prev = rec_length;
  size_t bytes_read = 0;

  bytes_read = CAN_GET_DLC(rxMailbox);

  for (int i = 0; i < bytes_read; ++i) {
    if ((i + rec_length) >= CAN_BUFFER_SIZE)
        rec_length = 0;

    can_buffer[i + rec_length] = CAN_RX_DATA_BYTE(rxMailbox, i);
  }

  rec_length += bytes_read;
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
  uint32_t amr_value, acr_value = 0;

  /* pass messages with ID from 0x10 to 0x1f */
  amr_value = (0x0000000f << 21);
  acr_value = (0x00000010 << 21);


  /*
  for (size_t i = 0; i < 4; ++i) {
    CAN_RXRegisterInit((reg32 *) &CAN_RX[i].rxamr, amr_value);
    CAN_RXRegisterInit((reg32 *) &CAN_RX[i].rxacr, acr_value);
  }
  */

  CAN_Init();
  CAN_Start();

#endif
}
