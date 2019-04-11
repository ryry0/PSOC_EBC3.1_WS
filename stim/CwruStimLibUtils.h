/**
* @file CwruStimLibUtils.h
* @brief Header file of the CWRU Stimulation Library (CwruStimLib) Utility Functions.
* @author Lu Li - lxl361@case.edu
*
* @version 1.0a
* @date 08/11/2017
*/

#ifndef CWRUSTIMLIBUTILS_H
#define CWRUSTIMLIBUTILS_H

#include <project.h>
#include <stdio.h>
#include <stdint.h>

#include "CwruStimLib.h"

// Interface to HAL for UART print array function 
int8_t stim_uart_print_array(cwru_stim_struct_t *cwru_stim, 
                        const uint8_t string[], uint16_t byteCount);

// Calulate check sum byte
uint8_t stim_checksum(uint8_t vals[], int length);

// // return value management based on Sucess/Error Status
// int8_t return_management(int8_t sucessStatus, int8_t errorStatus);

#endif /* CWRUSTIMLIBUTILS_H */