/**
* @file CwruStimExe.h
* @brief Head file for the CWRU Stimulation Library (CwruStimLib) UECU exe func.
* @author Lu Li - lxl361@case.edu
*
* @version 1.0a
* @date 08/11/2017
*/

#ifndef CWRUSTIMLIBEXE_H
#define CWRUSTIMLIBEXE_H

#include <project.h>
#include <stdio.h>
#include <stdint.h>

#include "CwruStimLib.h"

// Calulate pattern ramping function
uint8_t stim_get_PW_ramping(int channel_i,
                        const uint16_t (*LUT_PP_t)[12][8],
                        const uint8_t (*LUT_PW_t)[12][8],
                        uint16_t cycle_pp_t);

// channel pw gains for dynamic gait control
int stim_set_chan_pw_gain(cwru_stim_struct_t *cwru_stim, uint8_t channel, float gain);

// get channel pw gain function
float stim_get_chan_pw_gain(cwru_stim_struct_t *cwru_stim, uint8_t channel);

// execute command, apply the gain to _current_pulse_width[], but not save;
uint8_t stim_exe_chan_pw_gain(cwru_stim_struct_t *cwru_stim, uint8_t channel);

// channel amp gains for dynamic gait control
int stim_set_chan_amp_gain(cwru_stim_struct_t *cwru_stim, uint8_t channel, float gain);

// get channel amp gain function
float stim_get_chan_amp_gain(cwru_stim_struct_t *cwru_stim,  uint8_t channel);

// execute command, apply the gain to _current_amplitude[], but not save;
uint8_t stim_exe_chan_amp_gain(cwru_stim_struct_t *cwru_stim,  uint8_t channel);

// Update Stim pattern via UART
// stim_update(type, pattern, cycle_percentage)
int stim_update_cycle(cwru_stim_struct_t *cwru_stim, 
                      int type, int pattern, uint16_t cycle_percentage);


#endif /* CWRUSTIMLIBEXE_H */