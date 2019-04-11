/**
* @file HAL_PSOC_UART.c
* @brief Source file of the CWRU Stimulation Library (CwruStimLib) HAL
* @author Lu Li - lxl361@case.edu
*
* @version 1.0a
* @date 08/11/2017
*/

#include <project.h>
#include <stdio.h>
#include <stdint.h>
#include "HAL_PSOC_UART.h"



// HAL uart start wrapper function
int8_t hal_uart_start(uint8_t port_id) 
{
  if (port_id == STIM_UART_PORT_0){
    UART_STIM_0_Start();
  } else if (port_id == STIM_UART_PORT_1){
    UART_STIM_1_Start();
  } else if (port_id == STIM_UART_PORT_2){
    UART_STIM_2_Start();
  } else if (port_id == STIM_UART_PORT_USB_DEBUG){
    // Only init the USBUART when it is not ready
    if(!USBUART_1_CDCIsReady()){
      // Choose carefully according to the hardware voltage!!!
      USBUART_1_Start(0,USBUART_1_3V_OPERATION);
      //USBUART_Start(0,USBUART_3V_OPERATION);
      while(!USBUART_1_bGetConfiguration());
      USBUART_1_CDC_Init();
    }
  } else {
    // error occur
    return -1;
  }

  return 1;
}

// HAL uart print array wrapper function
int8_t hal_uart_print_array(uint8_t port_id, 
                            const uint8_t string[], uint16_t byteCount)
{

  if (port_id == STIM_UART_PORT_0){
    UART_STIM_0_PutArray(string, byteCount);
  } else if (port_id == STIM_UART_PORT_1){
    UART_STIM_1_PutArray(string, byteCount);
  } else if (port_id == STIM_UART_PORT_2){
    UART_STIM_2_PutArray(string, byteCount);
  } else if (port_id == STIM_UART_PORT_USB_DEBUG){
    USBUART_1_PutData(string,byteCount);
  } else {
    // error occur
    return -1;
  }

  return 1;
}


