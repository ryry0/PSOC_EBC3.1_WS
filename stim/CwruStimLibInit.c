/**
* @file CwruStimLibInit.c
* @brief The CWRU Stimulation Library (CwruStimLib) UECU Initialize func.
* @author Lu Li - lxl361@case.edu
*
* @version 1.0a
* @date 08/11/2017
*/

#include "CwruStimLib.h"

// Initialize pattern function
int8_t stim_init_pattern(cwru_stim_struct_t *cwru_stim, uint8_t msg_sync,
                        uint8_t default_ipi)
{
    // Pattern init
    for (uint8_t i=0; i<STIM_CHANNEL_MAX; i++) {
        // Sync messages
        cwru_stim->SYNC_MSG_MULTI[i] = msg_sync;

        // Inter phase interval
        // CHECKLIST: Need to change this later according to gait pattern file!
        cwru_stim->_current_ipi[i] = default_ipi;

        // Pulse width
        cwru_stim->_current_pulse_width[i] = 0;

        // Amplitude
        cwru_stim->_current_amplitude[i] = 0;
    }

    // Fixed Scheduler init
    cwru_stim->_schedule_id = 0;
    for (uint8_t i=0; i<FIXED_SCHED; i++) {
        cwru_stim->_group_event_count[i] = 0;
    }

    return 1;
}

// Board setup for Surf board
int8_t stim_init_brd_surf(cwru_stim_struct_t *cwru_stim)
{
    // Set message destination address
    cwru_stim->MSG_DES_ADDR = MSG_DES_ADDR_SURF;

    // Set number of channels been used
    cwru_stim->STIM_CHANNEL_USED = STIM_CHANNEL_USED_SURF;

    // Reset Halt Message to reset Stim board
    stim_cmd_halt_rset(cwru_stim, UECU_RESET);

    // Delete Schedule
    stim_cmd_del_sched(cwru_stim, 1); // Delete schedule 1

    return 1;
}

// Board setup for Perc board
int8_t stim_init_brd_perc(cwru_stim_struct_t *cwru_stim)
{
    // Set message destination address
    cwru_stim->MSG_DES_ADDR = MSG_DES_ADDR_PERC;

    // Set number of channels been used
    cwru_stim->STIM_CHANNEL_USED = STIM_CHANNEL_USED_PERC;

    // Reset Halt Message to reset Stim board
    stim_cmd_halt_rset(cwru_stim, UECU_RESET);

    return 1;
}


// Channel setup for Surf board
int8_t stim_init_chan_surf(cwru_stim_struct_t *cwru_stim)
{

    // Channels setup
    // stim_cmd_chan_set(port_chn_id, amp_limit, pw_limit, ip_delay, asp_ratio, anode_cathode);

    // Bipolar 01
    stim_cmd_chan_set( cwru_stim,
            0,    // port_chn_id =  0
            100,  // amp_limit = 100mA
            0xFA, // pw_limit = 250usec
            100,  // ip_delay = 100usec
            0x11,   // asp_ratio = 1:1
            0x01);  //anode = 0, cathode = 1, for bipolar mode);

    // Bipolar 23
    stim_cmd_chan_set( cwru_stim,
            1,    // port_chn_id =  0
            100,  // amp_limit = 100mA
            0xFA, // pw_limit = 250usec
            100,  // ip_delay = 100usec
            0x11,   // asp_ratio = 1:1
            0x23);  //anode = 2, cathode = 3, for bipolar mode);

    // Bipolar 45
    stim_cmd_chan_set( cwru_stim,
            2,    // port_chn_id =  0
            100,  // amp_limit = 100mA
            0xFA, // pw_limit = 250usec
            100,  // ip_delay = 100usec
            0x11,   // asp_ratio = 1:1
            0x45);  //anode = 0, cathode = 1, for bipolar mode);

    // Bipolar 67
    stim_cmd_chan_set( cwru_stim,
            3,    // port_chn_id =  0
            100,  // amp_limit = 100mA
            0xFA, // pw_limit = 250usec
            100,  // ip_delay = 100usec
            0x11,   // asp_ratio = 1:1
            0x67);  //anode = 0, cathode = 1, for bipolar mode);

    return 1;
}

// Channel setup for Perc board
int8_t stim_init_chan_perc(cwru_stim_struct_t *cwru_stim)
{
    return 1;
}

// Schedule and event setup for Surf board
int8_t stim_init_evnt_surf(cwru_stim_struct_t *cwru_stim)
{
    // Create Schedule
    // stim_cmd_crt_sched(sync_signalcwru_stim,, duration);
    stim_cmd_crt_sched(cwru_stim, UECU_SYNC_MSG, 29); // Sync signal = 0xAA, duration 29msec.

    // Create Events
    // stim_cmd_crt_evnt(sched_id, delay, priority, event_type, port_chn_id);

    // Create Event 1 for port_chn_id 0 in sched_id 1
    stim_cmd_crt_evnt( cwru_stim,
            1,  // sched_id = 1
            4,  // delay = 0msec
            0,  // priority = 0
            3,  // event_type = 3, for for Stimulus Event
            0,  // port_chn_id = 0;
            0,  // pulse_width set to 0,
            0,  // amplitude set to 0,
            0); // zone not implemented;

    // Create Event 2 for port_chn_id 0 in sched_id 1
    stim_cmd_crt_evnt( cwru_stim,
            1,  // sched_id = 1
            6,  // delay = 0msec
            0,  // priority = 0
            3,  // event_type = 3, for for Stimulus Event
            1,  // port_chn_id = 1;
            0,  // pulse_width set to 0,
            0,  // amplitude set to 0,
            0); // zone not implemented;

    // Create Event 3 for port_chn_id 0 in sched_id 1
    stim_cmd_crt_evnt( cwru_stim,
            1,  // sched_id = 1
            8,  // delay = 0msec
            0,  // priority = 0
            3,  // event_type = 3, for for Stimulus Event
            2,  // port_chn_id = 2;
            0,  // pulse_width set to 0,
            0,  // amplitude set to 0,
            0); // zone not implemented;

    // Create Event 4 for port_chn_id 0 in sched_id 1
    stim_cmd_crt_evnt( cwru_stim,
            1,  // sched_id = 1
            10,  // delay = 0msec
            0,  // priority = 0
            3,  // event_type = 3, for for Stimulus Event
            3,  // port_chn_id = 3;
            0,  // pulse_width set to 0,
            0,  // amplitude set to 0,
            0); // zone not implemented;

    return 1;
}

// Schedule and event setup for Perc board
int8_t stim_init_evnt_perc(cwru_stim_struct_t *cwru_stim)
{
    if (cwru_stim->setting == ( STIM_SETTING_SINGLE_SCHEDULER |
                                STIM_SETTING_DEFAULT)) {

        // Create Schedule ------------------------------
        // crt_sched  : 01 80 10 03 AA 00 1D A3

        // this->cmd_crt_sched(sync_signal, duration);
        stim_cmd_crt_sched(cwru_stim, UECU_SYNC_MSG, 29); // Sync signal = 0xAA, duration 29msec (0d29 = 0x1D).

        // Create Event 1-12  ----------------------------------
        // crt_evnt1  : 01 80 15 09 01 00 00 00 03 00 50 10 00 FB
        // crt_evnt2  : 01 80 15 09 01 00 05 00 03 01 80 15 00 C0
        // ...
        // crt_evnt12 : 01 80 15 09 01 00 0F 00 03 0B F0 26 00 2B

        // stim_cmd_crt_evnt(sched_id, delay, priority, event_type, port_chn_id);
        // Create Event 1 for port_chn_id 0 in sched_id 1
        stim_cmd_crt_evnt(cwru_stim,
                1,  // sched_id = 1
                0,  // delay = 0msec
                0,  // priority = 0
                3,  // event_type = 3, for for Stimulus Event
                0,  // port_chn_id = 0;
                0x50, // pulse_width set to 0,
                0x10, // amplitude set to 0,
                0); // zone not implemented;

        // Create Event 2 for port_chn_id 0 in sched_id 1
        stim_cmd_crt_evnt(cwru_stim,
                1,  // sched_id = 1
                0x05, // delay = 5msec
                0,  // priority = 0
                3,  // event_type = 3, for for Stimulus Event
                1,  // port_chn_id = 1;
                0x80, // pulse_width set to 0,
                0x15, // amplitude set to 0,
                0); // zone not implemented;

        // Create Event 3 for port_chn_id 0 in sched_id 1
        stim_cmd_crt_evnt(cwru_stim,
                1,  // sched_id = 1
                0x0A, // delay = 10msec
                0,  // priority = 0
                3,  // event_type = 3, for for Stimulus Event
                2,  // port_chn_id = 2;
                0xB0, // pulse_width set to 0,
                0x20, // amplitude set to 0,
                0); // zone not implemented;

        // Create Event 4 for port_chn_id 0 in sched_id 1
        stim_cmd_crt_evnt(cwru_stim,
                1,  // sched_id = 1
                0x0F, // delay = 15msec
                0,  // priority = 0
                3,  // event_type = 3, for for Stimulus Event
                3,  // port_chn_id = 3;
                0xF0, // pulse_width set to 0,
                0x26, // amplitude set to 0,
                0); // zone not implemented;

        // Create Event 5 for port_chn_id 0 in sched_id 1
        stim_cmd_crt_evnt(cwru_stim,
                1,  // sched_id = 1
                0,  // delay = 0msec
                0,  // priority = 0
                3,  // event_type = 3, for for Stimulus Event
                4,  // port_chn_id = 4;
                0x50, // pulse_width set to 0,
                0x10, // amplitude set to 0,
                0); // zone not implemented;

        // Create Event 6 for port_chn_id 0 in sched_id 1
        stim_cmd_crt_evnt(cwru_stim,
                1,  // sched_id = 1
                0x05, // delay = 5msec
                0,  // priority = 0
                3,  // event_type = 3, for for Stimulus Event
                5,  // port_chn_id = 5;
                0x80, // pulse_width set to 0,
                0x15, // amplitude set to 0,
                0); // zone not implemented;

        // Create Event 7 for port_chn_id 0 in sched_id 1
        stim_cmd_crt_evnt(cwru_stim,
                1,  // sched_id = 1
                0x0A, // delay = 10msec
                0,  // priority = 0
                3,  // event_type = 3, for for Stimulus Event
                6,  // port_chn_id = 6;
                0xB0, // pulse_width set to 0,
                0x20, // amplitude set to 0,
                0); // zone not implemented;

        // Create Event 8 for port_chn_id 0 in sched_id 1
        stim_cmd_crt_evnt(cwru_stim,
                1,  // sched_id = 1
                0x0F, // delay = 15msec
                0,  // priority = 0
                3,  // event_type = 3, for for Stimulus Event
                7,  // port_chn_id = 7;
                0xF0, // pulse_width set to 0,
                0x26, // amplitude set to 0,
                0); // zone not implemented;

        // Create Event 9 for port_chn_id 0 in sched_id 1
        stim_cmd_crt_evnt(cwru_stim,
                1,  // sched_id = 1
                0,  // delay = 0msec
                0,  // priority = 0
                3,  // event_type = 3, for for Stimulus Event
                8,  // port_chn_id = 8;
                0x50, // pulse_width set to 0,
                0x10, // amplitude set to 0,
                0); // zone not implemented;

        // Create Event 10 for port_chn_id 0 in sched_id 1
        stim_cmd_crt_evnt(cwru_stim,
                1,  // sched_id = 1
                0x05, // delay = 5msec
                0,  // priority = 0
                3,  // event_type = 3, for for Stimulus Event
                9,  // port_chn_id = 9;
                0x80, // pulse_width set to 0,
                0x15, // amplitude set to 0,
                0); // zone not implemented;

        // Create Event 11 for port_chn_id 0 in sched_id 1
        stim_cmd_crt_evnt(cwru_stim,
                1,  // sched_id = 1
                0x0A, // delay = 10msec
                0,  // priority = 0
                3,  // event_type = 3, for for Stimulus Event
                10, // port_chn_id = 10;
                0xB0, // pulse_width set to 0,
                0x20, // amplitude set to 0,
                0); // zone not implemented;

        // Create Event 12 for port_chn_id 0 in sched_id 1
        stim_cmd_crt_evnt(cwru_stim,
                1,  // sched_id = 1
                0x0F, // delay = 15msec
                0,  // priority = 0
                3,  // event_type = 3, for for Stimulus Event
                11, // port_chn_id = 11;
                0xF0, // pulse_width set to 0,
                0x26, // amplitude set to 0,
                0); // zone not implemented;

        // Create Sync Message ----------------------------------
        // sync_msg1  : 01 80 1B 01 AA B7

        return 1;
    } else if (cwru_stim->setting == STIM_SETTING_MULTI_SCHEDULER) {

        return 1;
    } else {
        return -1;
    }

}

