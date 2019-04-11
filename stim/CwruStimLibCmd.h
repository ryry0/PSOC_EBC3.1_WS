/**
* @file CwruStimCmd.h
* @brief Head file for the CWRU Stimulation Library (CwruStimLib) Command Function
* @author Lu Li - lxl361@case.edu
*
* @version 1.0a
* @date 08/11/2017
*/

#ifndef CWRUSTIMLIBCMD_H
#define CWRUSTIMLIBCMD_H

#include <project.h>
#include <stdio.h>
#include <stdint.h>

#include "CwruStimLib.h"

// UECU command sets
//UECU Halt
int stim_cmd_halt_rset(cwru_stim_struct_t *cwru_stim, uint8_t halt_flag);

// UECU Delete Schedule
int stim_cmd_del_sched(cwru_stim_struct_t *cwru_stim, uint8_t sched_id);

// UECU Create Schedule
int stim_cmd_crt_sched(cwru_stim_struct_t *cwru_stim, 
						uint8_t sync_signal, uint16_t duration);

// UECU Channel Setup 
int stim_cmd_chan_set( cwru_stim_struct_t *cwru_stim, 
						uint8_t port_chn_id, 
                        uint8_t amp_limit,
                        uint8_t pw_limit,
                        uint16_t ip_delay, 
                        uint8_t asp_ratio, 
                        uint8_t anode_cathode);

// UECU Create Event
int stim_cmd_crt_evnt( cwru_stim_struct_t *cwru_stim, 
						uint8_t sched_id, 
                        uint16_t delay, 
                        uint8_t priority, 
                        uint8_t event_type, 
                        uint8_t port_chn_id,
                        uint8_t pulse_width,
                        uint8_t amplitude,
                        uint8_t zone);

// UECU Change Event Parameter Command
int stim_cmd_set_evnt( cwru_stim_struct_t *cwru_stim, 
						uint8_t event_id,
                        uint8_t pulse_width,
                        uint8_t amplitude,
                        uint8_t zone);

// UECU Change Event Schedule Message
int stim_cmd_chg_evnt_sched(cwru_stim_struct_t *cwru_stim, 
			                  uint8_t event_id,
			                  uint8_t sched_id,
			                  uint16_t delay,
			                  uint8_t priority);

// UECU Change Schedule Message
int stim_cmd_set_sched( cwru_stim_struct_t *cwru_stim, 
						uint8_t sched_id,
                         uint8_t sync_signal,
                         uint16_t duration);

// UECU Sync Message
int stim_cmd_sync_msg(cwru_stim_struct_t *cwru_stim, uint8_t sync_signal);

#endif /* CWRUSTIMLIBCMD_H */