/**
* @file CwruStimCmd.c
* @brief The CWRU Stimulation Library (CwruStimLib) Command Function
* @author Lu Li - lxl361@case.edu
*
* @version 1.0a
* @date 08/11/2017
*/

#include <project.h>
#include <stdio.h>
#include <stdint.h>
#include "CwruStimLib.h"
#include "CwruStimLibCmd.h"

// uint8_t stim_cmd_halt_rset[12] = {0x04, 0x80, 0x47, 0x07, 0x00, 0x64, 0xFA, 0x00, 0x64, 0x11, 0x01, 0x57};
// UECU command sets
//UECU Halt
int stim_cmd_halt_rset(cwru_stim_struct_t *cwru_stim, uint8_t halt_flag) {
  // calculate message size
  int size = HALT_MSG_LEN + UECU_MSG_EXTRAL_LEN;
  // build message content
  uint8_t msg[HALT_MSG_LEN + UECU_MSG_EXTRAL_LEN] = 
  { cwru_stim->MSG_DES_ADDR,
    MSG_SRC_ADDR,
    HALT_MSG,
    HALT_MSG_LEN,
    halt_flag,
    0x00
  };

  // Insert checksum byte
  msg[size-1] = stim_checksum(msg,size);

  // Send message
  return stim_uart_print_array (cwru_stim, msg, sizeof(msg)/sizeof(uint8_t));

}

// UECU Delete Schedule
int stim_cmd_del_sched(cwru_stim_struct_t *cwru_stim, uint8_t sched_id) {
  // calculate message size
  int size = DELETE_SCHEDULE_MSG_LEN + UECU_MSG_EXTRAL_LEN;
  // build message content
  uint8_t msg[DELETE_SCHEDULE_MSG_LEN + UECU_MSG_EXTRAL_LEN] = 
  { cwru_stim->MSG_DES_ADDR,
    MSG_SRC_ADDR,
    DELETE_SCHEDULE_MSG,
    DELETE_SCHEDULE_MSG_LEN,
    sched_id,
    0x00
  };

  // Insert checksum byte
  msg[size-1] = stim_checksum(msg,size);

  // Send message
  return stim_uart_print_array (cwru_stim, msg, sizeof(msg)/sizeof(uint8_t));
}

// UECU Create Schedule
int stim_cmd_crt_sched(cwru_stim_struct_t *cwru_stim, 
                      uint8_t sync_signal, uint16_t duration) {
  // calculate message size
  int size = CREATE_SCHEDULE_MSG_LEN + UECU_MSG_EXTRAL_LEN;
  // build message content
  uint8_t msg[CREATE_SCHEDULE_MSG_LEN + UECU_MSG_EXTRAL_LEN] = 
  { cwru_stim->MSG_DES_ADDR,
    MSG_SRC_ADDR,
    CREATE_SCHEDULE_MSG,
    CREATE_SCHEDULE_MSG_LEN,
    sync_signal,
    (uint8_t)((duration >> 8) & 0x00FF),
    (uint8_t)(duration & 0x00FF),
    0x00
  };

  // Insert checksum byte
  msg[size-1] = stim_checksum(msg,size);

  // Send message
  return stim_uart_print_array (cwru_stim, msg, sizeof(msg)/sizeof(uint8_t));
}

// UECU Channel Setup 
int stim_cmd_chan_set( cwru_stim_struct_t *cwru_stim, 
                        uint8_t port_chn_id, 
                        uint8_t amp_limit,
                        uint8_t pw_limit,
                        uint16_t ip_delay, 
                        uint8_t asp_ratio, 
                        uint8_t anode_cathode) {
  // calculate message size
  int size = CHANNEL_SETUP_MSG_LEN + UECU_MSG_EXTRAL_LEN;
  // build message content
  uint8_t msg[CHANNEL_SETUP_MSG_LEN + UECU_MSG_EXTRAL_LEN] = 
  { cwru_stim->MSG_DES_ADDR,
    MSG_SRC_ADDR,
    CHANNEL_SETUP_MSG,
    CHANNEL_SETUP_MSG_LEN,
    port_chn_id,
    amp_limit,
    pw_limit,
    (uint8_t)((ip_delay >> 8) & 0x00FF),
    (uint8_t)(ip_delay & 0x00FF),
    asp_ratio,
    anode_cathode,
    0x00
  };

  // Insert checksum byte
  msg[size-1] = stim_checksum(msg,size);

  // Send message
  return stim_uart_print_array (cwru_stim, msg, sizeof(msg)/sizeof(uint8_t));
}

// UECU Create Event
int stim_cmd_crt_evnt( cwru_stim_struct_t *cwru_stim, 
                        uint8_t sched_id, 
                        uint16_t delay, 
                        uint8_t priority, 
                        uint8_t event_type, 
                        uint8_t port_chn_id,
                        uint8_t pulse_width,
                        uint8_t amplitude,
                        uint8_t zone) {
  // calculate message size
  int size = CREATE_EVENT_MSG_LEN + UECU_MSG_EXTRAL_LEN;
  // build message content
  uint8_t msg[CREATE_EVENT_MSG_LEN + UECU_MSG_EXTRAL_LEN] = 
  { cwru_stim->MSG_DES_ADDR,
    MSG_SRC_ADDR,
    CREATE_EVENT_MSG,
    CREATE_EVENT_MSG_LEN,
    sched_id,
    (uint8_t)((delay >> 8) & 0x00FF),
    (uint8_t)(delay & 0x00FF),
    priority,
    event_type,
    port_chn_id,
    pulse_width, //Param[1]
    amplitude, //Param[2]
    zone, //Param[3] not implemented
    0x00
  };

  // Insert checksum byte
  msg[size-1] = stim_checksum(msg,size);

  // Send message
  return stim_uart_print_array (cwru_stim, msg, sizeof(msg)/sizeof(uint8_t));
}

// UECU Change Event Parameter Command
int stim_cmd_set_evnt( cwru_stim_struct_t *cwru_stim, 
                        uint8_t event_id,
                        uint8_t pulse_width,
                        uint8_t amplitude,
                        uint8_t zone) {
  // calculate message size
  int size = CHANGE_EVENT_PARAMS_MSG_LEN + UECU_MSG_EXTRAL_LEN;
  // build message content
  uint8_t msg[CHANGE_EVENT_PARAMS_MSG_LEN + UECU_MSG_EXTRAL_LEN] = 
  { cwru_stim->MSG_DES_ADDR,
    MSG_SRC_ADDR,
    CHANGE_EVENT_PARAMS_MSG,
    CHANGE_EVENT_PARAMS_MSG_LEN,
    event_id,
    pulse_width,
    amplitude,
    zone, //Param[3] not implemented
    0x00
  };

  // Insert checksum byte
  msg[size-1] = stim_checksum(msg,size);

  // //-Serial.print("In set event loop, ");
  // //-Serial.print("event_id = ");
  // //-Serial.print(event_id,HEX);
  // //-Serial.print(";\t pulse_width = ");
  // //-Serial.print(pulse_width,HEX);
  // //-Serial.print(";\t amplitude = ");
  // //-Serial.println(amplitude,HEX);


  // Send message
  return stim_uart_print_array (cwru_stim, msg, sizeof(msg)/sizeof(uint8_t));
}

// UECU Change Event Schedule Message
int stim_cmd_chg_evnt_sched( cwru_stim_struct_t *cwru_stim, 
                  uint8_t event_id,
                  uint8_t sched_id,
                  uint16_t delay,
                  uint8_t priority) {
  // calculate message size
  int size = CHANGE_EVENT_SCHED_MSG_LEN + UECU_MSG_EXTRAL_LEN;
  // build message content
  uint8_t msg[CHANGE_EVENT_SCHED_MSG_LEN + UECU_MSG_EXTRAL_LEN] = 
  { cwru_stim->MSG_DES_ADDR,
    MSG_SRC_ADDR,
    CHANGE_EVENT_SCHED_MSG,
    CHANGE_EVENT_SCHED_MSG_LEN,
    event_id,
    sched_id,
    (uint8_t)((delay >> 8) & 0x00FF),
    (uint8_t)(delay & 0x00FF),
    priority,
    0x00
  };

  // Insert checksum byte
  msg[size-1] = stim_checksum(msg,size);

  // Send message
  return stim_uart_print_array (cwru_stim, msg, sizeof(msg)/sizeof(uint8_t));  
}

// UECU Change Schedule Message
int stim_cmd_set_sched( cwru_stim_struct_t *cwru_stim, 
                        uint8_t sched_id,
                        uint8_t sync_signal,
                        uint16_t duration) {
  // calculate message size
  int size = CHANGE_SCHEDULE_MSG_LEN + UECU_MSG_EXTRAL_LEN;
  // build message content
  uint8_t msg[CHANGE_SCHEDULE_MSG_LEN + UECU_MSG_EXTRAL_LEN] = 
  { cwru_stim->MSG_DES_ADDR,
    MSG_SRC_ADDR,
    CHANGE_SCHEDULE_MSG,
    CHANGE_SCHEDULE_MSG_LEN,
    sched_id,
    sync_signal,
    (uint8_t)((duration >> 8) & 0x00FF),
    (uint8_t)(duration & 0x00FF),
    0x00
  };

  // Insert checksum byte
  msg[size-1] = stim_checksum(msg,size);

  // Send message
  return stim_uart_print_array (cwru_stim, msg, sizeof(msg)/sizeof(uint8_t));
}

// UECU Sync Message
int stim_cmd_sync_msg(cwru_stim_struct_t *cwru_stim, uint8_t sync_signal) {
  // calculate message size
  int size = SYNC_MSG_LEN + UECU_MSG_EXTRAL_LEN;
  // build message content
  uint8_t msg[SYNC_MSG_LEN + UECU_MSG_EXTRAL_LEN] = 
  { cwru_stim->MSG_DES_ADDR,
    MSG_SRC_ADDR,
    SYNC_MSG,
    SYNC_MSG_LEN,
    sync_signal,
    0x00
  };

  // Insert checksum byte
  msg[size-1] = stim_checksum(msg,size);

  // Send message
  return stim_uart_print_array (cwru_stim, msg, sizeof(msg)/sizeof(uint8_t));
}