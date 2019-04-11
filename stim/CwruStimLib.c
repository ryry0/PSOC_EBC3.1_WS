/**
* @file CwruStimLib.c
* @brief Source file of the CWRU Stimulation Library (CwruStimLib)
* @author Lu Li - lxl361@case.edu
*
* @version 1.0a
* @date 08/11/2017
*/

#include <project.h>
#include <stdio.h>
#include <stdint.h>
#include "CwruStimLib.h"

// Stimulation Board Initialize Function
int8_t cwru_stim_init(cwru_stim_struct_t *cwru_stim, const uint8_t brd_type,
                    const uint8_t port_id, const uint8_t setting)
{
    int8_t sucessStatus = 1;

    cwru_stim->brd_type = brd_type;
    cwru_stim->port_id = port_id;
    cwru_stim->setting = setting;

    sucessStatus *= hal_uart_start(port_id);

    // Base on board type, call different UECU exe functions.
    switch (brd_type) {
        case STIM_MODE_SURF:
            sucessStatus *= stim_init_pattern(cwru_stim,
                                              UECU_SYNC_MSG,
                                              FIXED_SCHED_ID1_IPI);
            sucessStatus *= stim_init_brd_surf(cwru_stim);
            sucessStatus *= stim_init_chan_surf(cwru_stim);
            sucessStatus *= stim_init_evnt_surf(cwru_stim);
            break;

        case STIM_MODE_PERC:
            sucessStatus *= stim_init_pattern(cwru_stim,
                                              UECU_SYNC_MSG,
                                              FIXED_SCHED_ID1_IPI);
            sucessStatus *= stim_init_brd_perc(cwru_stim);
            sucessStatus *= stim_init_chan_perc(cwru_stim);
            sucessStatus *= stim_init_evnt_perc(cwru_stim);
            break;

        case STIM_BRD_ICM_IRS8:

            break;

        case STIM_BRD_ICM_IST16:

            break;

        default:
            return -1;
            break;
    }


    return sucessStatus;
}

int8_t cwru_stim_start(cwru_stim_struct_t *cwru_stim, const uint8_t sync_signal)
{
    int8_t sucessStatus = 1;

    if (cwru_stim->setting == ( STIM_SETTING_SINGLE_SCHEDULER |
                                STIM_SETTING_DEFAULT)) {
        // Send Sync to start
        //stim_cmd_sync_msg(0xAA); // Sent Sync_message 0xAA to start schedule.
        // Sent Sync_message to start schedule.
        sucessStatus *= stim_cmd_sync_msg(cwru_stim, sync_signal);

        return sucessStatus;
    } else if (cwru_stim->setting == STIM_SETTING_MULTI_SCHEDULER) {
        // Loop through all schedules
        for (int i=0; i<cwru_stim->STIM_CHANNEL_USED; i++) {

        //Send Sync to start
        sucessStatus *= stim_cmd_sync_msg(cwru_stim, cwru_stim->SYNC_MSG_MULTI[i]); // Sent Sync_message to start schedule.

        // Delay duration need to be save as IPI.
        CyDelay(cwru_stim->_current_ipi[i]); //TODO HAL
        }
        return 1;
    } else {
        return -1;
    }
}

// Stimulation Board Update Function
int8_t cwru_stim_update(cwru_stim_struct_t *cwru_stim,
                        const uint8_t cmd, const uint8_t mode,
                        const uint8_t setting, const float param)
{
    int8_t sucessStatus = 1;

    switch (cmd) {
        case STIM_UPDATE_GAIT_CYCLE:
            // stim_update_cycle(cwru_stim, type, pattern, cycle_percentage);
            // stim_update_cycle(VCK5_BRD1|IPI, PATTERN_STAND, cycle_percentage_value);
            sucessStatus *= stim_update_cycle(cwru_stim, mode, setting, (uint16_t)param);
            break;
        case STIM_UPDATE_GAIN:
            if (mode == PW) {
                //stim_set_chan_pw_gain(cwru_stim, channel, gain);
                sucessStatus *= stim_set_chan_pw_gain(cwru_stim,setting,param);
            } else if (mode == AMP) {
                //stim_set_chan_amp_gain(cwru_stim, channel, gain);
                sucessStatus *= stim_set_chan_amp_gain(cwru_stim,setting,param);
            } else {
                return -1;
            }
            break;
        case STIM_UPDATE_MUSCLE_GRP:

            break;
        default:
            return -1;
            break;
    }
    return sucessStatus;
}





