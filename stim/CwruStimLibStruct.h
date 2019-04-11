/**
* @file CwruStimLibStruct.h
* @brief Header file of the CWRU Stimulation Library (CwruStimLib) data structure
* @author Lu Li - lxl361@case.edu
*
* @version 1.0a
* @date 08/11/2017
*/

#ifndef CWRUSTIMLIBSTRUCT_H
#define CWRUSTIMLIBSTRUCT_H
    
#include <project.h>
#include <stdio.h>
#include <stdint.h>

#include "CwruStimLib.h"

// typedef void (*stim_init_func_t)(int8);
// typedef void (*stim_config_func_t)(int8);
// typedef void (*stim_update_func_t)(int8);


typedef struct cwru_stim_struct_s {

    // stim_init_func_t stim_init_func;
    // stim_config_func_t stim_config_func;
    // stim_update_func_t stim_update_func;

    uint8_t brd_type;
    uint8_t port_id;
    uint8_t setting; 

    uint8_t MSG_DES_ADDR;  
    uint8_t STIM_CHANNEL_USED;  

    uint16_t stim_param_pp;
    uint16_t stim_param_ipi;
    uint16_t stim_param_pw;
    uint16_t stim_param_amp;

    // Stim board setup
    int _stim_error;

    // Multi scheduler sync
    uint8_t SYNC_MSG_MULTI[STIM_CHANNEL_MAX];

    // Pulse width
    uint8_t _current_pulse_width[STIM_CHANNEL_MAX];

    // Amplitude
    uint8_t _current_amplitude[STIM_CHANNEL_MAX];

    // Inter phase interval
    uint8_t _current_ipi[STIM_CHANNEL_MAX];

    // for fixed scheduler
    uint16_t _group_event_count[FIXED_SCHED];//count num of events in one schedule
    uint8_t _schedule_id; //schedule_id 

    // channel pw gains for dynamic gait control
    float _current_pw_gains[STIM_CHANNEL_MAX];

    // channel amp gains for dynamic gait control
    float _current_amp_gains[STIM_CHANNEL_MAX];

} cwru_stim_struct_t;

#endif /* CWRUSTIMLIBSTRUCT_H */  