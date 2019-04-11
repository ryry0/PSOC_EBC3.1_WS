/**
* @file CwruStimLibInit.h
* @brief Head file for the CWRU Stimulation Library (CwruStimLib) UECU Initialize func.
* @author Lu Li - lxl361@case.edu
*
* @version 1.0a
* @date 08/11/2017
*/

#ifndef CWRUSTIMLIBINIT_H
#define CWRUSTIMLIBINIT_H

#include <project.h>
#include <stdio.h>
#include <stdint.h>

#include "CwruStimLib.h"

// Initialize pattern function
int8_t stim_init_pattern(cwru_stim_struct_t *cwru_stim, uint8_t msg_sync, 
                        uint8_t default_ipi);

// Board setup for Surf board
int8_t stim_init_brd_surf(cwru_stim_struct_t *cwru_stim);
// Board setup for Perc board
int8_t stim_init_brd_perc(cwru_stim_struct_t *cwru_stim);

// Channel setup for Surf board
int8_t stim_init_chan_surf(cwru_stim_struct_t *cwru_stim);
// Channel setup for Perc board
int8_t stim_init_chan_perc(cwru_stim_struct_t *cwru_stim);

// Event setup for Surf board
int8_t stim_init_evnt_surf(cwru_stim_struct_t *cwru_stim);
// Schedule and event setup for Perc board
int8_t stim_init_evnt_perc(cwru_stim_struct_t *cwru_stim);


#endif /* CWRUSTIMLIBINIT_H */