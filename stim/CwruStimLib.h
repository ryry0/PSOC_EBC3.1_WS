/**
* @file CwruStimLib.h
* @brief Header file of the CWRU Stimulation Library (CwruStimLib)
* @author Lu Li - lxl361@case.edu
*
* @version 1.0a
* @date 08/11/2017
*/

#ifndef CWRUSTIMLIB_H
#define CWRUSTIMLIB_H
    
#include <project.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "CwruStimLibConst.h"
#include "CwruStimLibStruct.h"
#include "CwruStimLibUtils.h"
#include "CwruStimLibCmd.h"
#include "CwruStimLibExe.h"
#include "CwruStimLibInit.h"
#include "PresetPatterns/GaitVCK5.h"

#define ARCH_PSOC
// #define ARCH_ARDUINO
// #define ARCH_TEENSY

#ifdef ARCH_PSOC
	#include"CwruStimLibHAL/HAL_PSOC_UART.h"
#elif ARCH_ARDUINO
    // Arduino HAL file
#elif ARCH_TEENSY
    // Arduino HAL file
#else  
    #error "[CwruStimLib][ERROR] Incorrect Target Architecture!" 
#endif 

/* function definitions */
// Stimulation Board Initialize Function
int8_t cwru_stim_init(cwru_stim_struct_t *cwru_stim,
                      const uint8_t brd_type,
                      const uint8_t port_id,
                      const uint8_t setting);

// Stimulation Board Start Function
int8_t cwru_stim_start(cwru_stim_struct_t *cwru_stim, 
                      const uint8_t sync_signal);

// Stimulation Board Update Function
int8_t cwru_stim_update(cwru_stim_struct_t *cwru_stim,
                        const uint8_t cmd,
                        const uint8_t mode,
                        const uint8_t setting, 
                        const float param);  

#endif /* CWRUSTIMLIB_H */  