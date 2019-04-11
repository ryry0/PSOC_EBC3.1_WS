/**
* @file CwruStimLibUtils.c
* @brief the CWRU Stimulation Library (CwruStimLib) Utility Functions.
* @author Lu Li - lxl361@case.edu
*
* @version 1.0a
* @date 08/11/2017
*/

#include "CwruStimLib.h"

// Interface to HAL for UART print array function 
int8_t stim_uart_print_array(cwru_stim_struct_t *cwru_stim, 
                        const uint8_t string[], uint16_t byteCount) 
{
	return hal_uart_print_array(cwru_stim->port_id, string, byteCount);
}

// Calulate check sum byte
uint8_t stim_checksum(uint8_t vals[], int length){
  uint16_t csum = 0;
  for(int i=0; i<length-1; i++) {
    csum += (uint16_t)vals[i];
  }
  csum = ((0x00FF & csum) + (csum >> 8))^0xFF;
  return csum;
} 

// // return value management based on Sucess/Error Status
// int8_t return_management(int8_t sucessStatus, int8_t errorStatus)
// {
//     // when sucess return 1, fail return -1, nothing return 0.
//     if (errorStatus > 0) { 
//         return -1;
//     } else if (sucessStatus >0) {
//         return 1;
//     } else {
//         return 0;
//     }

// }