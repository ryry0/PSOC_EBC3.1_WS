/**
* @file PSOC_UART_HAL.h
* @brief Head file of the CWRU Stimulation Library (CwruStimLib) HAL
* @author Lu Li - lxl361@case.edu
*
* @version 1.0a
* @date 08/11/2017
*/

#ifndef HAL_PSOC_UART_H
#define HAL_PSOC_UART_H

#include <project.h>
#include <stdio.h>
#include <stdint.h>
#include "../CwruStimLib.h"
#include "../CwruStimLibConst.h"

int8_t hal_uart_print_array(uint8_t port_id, const uint8_t string[], uint16_t byteCount);
int8_t hal_uart_start(uint8_t port_id);

#endif /* HAL_PSOC_UART_H */  

