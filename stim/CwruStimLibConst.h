/**
* @file CwruStimLibConst.h
* @brief Header file of constants for the CWRU Stimulation Library (CwruStimLib)
* @author Lu Li - lxl361@case.edu
*
* @version 1.0a
* @date 08/11/2017
*/

#ifndef CWRUSTIMLIBCONST_H
#define CWRUSTIMLIBCONST_H
    
#include <project.h>
#include <stdio.h>
#include <stdint.h>
#include "CwruStimLib.h"
    
// Stimulation Board Types
typedef enum stim_brd_type_e {
  STIM_BRD_SURF = 1<<1,
  STIM_BRD_PERC = 1<<2,
  STIM_BRD_ICM_IRS8 = 1<<3,
  STIM_BRD_ICM_IST16 = 1<<4
} stim_brd_type_t;

// Stimulation Board Setting
typedef enum stim_setting_type_e {
  STIM_SETTING_DEFAULT = 1<<0,
  STIM_SETTING_SINGLE_SCHEDULER = 1<<1,
  STIM_SETTING_MULTI_SCHEDULER = 1<<2,
} stim_setting_type_t;

// UART Channel Setting
typedef enum stim_uart_port_e {
  STIM_UART_PORT_0,
  STIM_UART_PORT_1,
  STIM_UART_PORT_2,
  STIM_UART_PORT_USB_DEBUG
} stim_uart_port_t;

// UART Channel Setting
typedef enum stim_update_cmd_e {
  STIM_UPDATE_GAIT_CYCLE,
  STIM_UPDATE_GAIN,
  STIM_UPDATE_MUSCLE_GRP
} stim_update_cmd_t;

// Stim settings
#define STIM_MODE_DEFAULT 0 // Not used
#define STIM_MODE_SURF 1  // For surface stimulation board
#define STIM_MODE_PERC 2  // For percutaneous stimulation board
#define STIM_MODE_PERC_8CH_MULTI_SCHEDULE 3  // multi scheduler for percutaneous stimulation board

// Upper limit of channels
#define STIM_CHANNEL_MAX 12
#define STIM_CHANNEL_USED_SURF 4
#define STIM_CHANNEL_USED_PERC 8

// Error Flag (bitwise encoded)
#define STIM_NO_ERROR 0<<0
#define STIM_ERROR_SERIAL_ERROR 1<<1
#define STIM_ERROR_UPDATE_TYPE_ERROR 1<<2
#define STIM_ERROR_UPDATE_PATTERN_ERROR 1<<3
#define STIM_ERROR_UPDATE_RAMPING_ERROR 1<<4

// UECU Message Header
#define MSG_DES_ADDR_SURF 0x04
#define MSG_DES_ADDR_PERC 0x01
#define MSG_SRC_ADDR 0x80

// UECU Message Type
#define TRIGGER_SETUP_MSG 0x03
#define HALT_MSG 0x04
#define ERROR_REPORT_MSG 0x05
#define EVENT_ERROR_MSG 0x06
#define CREATE_SCHEDULE_MSG 0x10
#define CREATE_SCHEDULE_REPLY_MSG 0x11
#define DELETE_SCHEDULE_MSG 0x12
#define CHANGE_SCHEDULE_MSG 0x13
#define CHANGE_SCHEDULE_STATE_MSG 0x14
#define CREATE_EVENT_MSG 0x15
#define CREATE_EVENT_REPLY_MSG 0x16
#define DELETE_EVENT_MSG 0x17
#define CHANGE_EVENT_SCHED_MSG 0x18
#define CHANGE_EVENT_PARAMS_MSG 0x19
#define SYNC_MSG 0x1B
#define EVENT_COMMAND_MSG 0x1C
#define CHANNEL_SETUP_MSG 0x47
#define EVENT_COMMAND_REPLY 0x49

// UECU Message Length
#define TRIGGER_SETUP_MSG_LEN 4
#define HALT_MSG_LEN 1
#define ERROR_REPORT_MSG_LEN 2
#define EVENT_ERROR_MSG_LEN 4
#define CREATE_SCHEDULE_MSG_LEN 3
#define CREATE_SCHEDULE_REPLY_MSG_LEN 1
#define DELETE_SCHEDULE_MSG_LEN 1
#define CHANGE_SCHEDULE_MSG_LEN 4
#define CHANGE_SCHEDULE_STATE_MSG_LEN 2
#define CREATE_EVENT_MSG_LEN 9
#define CREATE_EVENT_REPLY_MSG_LEN 4
#define DELETE_EVENT_MSG_LEN 1
#define CHANGE_EVENT_SCHED_MSG_LEN 5
#define CHANGE_EVENT_PARAMS_MSG_LEN 4
#define SYNC_MSG_LEN 1
#define EVENT_COMMAND_MSG_LEN 0
#define CHANNEL_SETUP_MSG_LEN 7
#define EVENT_COMMAND_REPLY_LEN 0

#define UECU_MSG_EXTRAL_LEN 5 // 2 Header + 1 msg_type + 1 msg_length +1 Check_sum = 5

// UECU Halt
#define UECU_HALT 0x00
#define UECU_RESET 0x01

// UECU Sync Message
#define UECU_SYNC_MSG 0xAA

// Delay for setup
#define UECU_DELAY_SETUP 25

// Gait Params
#define NUM_GAIT_VCK5_BOARD 2
#define GAIT_VCK5_BOARD1 0
#define GAIT_VCK5_BOARD2 1

#define VCK5_BRD1 0<<7
#define VCK5_BRD2 1<<7

#define IPI 1
#define AMP 2
#define PW 3

#define NUM_PATTERN 8
#define PATTERN_NO_STIM 0
#define PATTERN_STAND 1
#define PATTERN_SIT 2
#define PATTERN_LSETP 3
#define PATTERN_RSETP 4

#define PATTERN_MISC1 11
#define PATTERN_MISC2 12
#define PATTERN_MISC3 13

#define NUM_PATTERN_PARAM 4
#define PATTERN_PARAM_PP 0
#define PATTERN_PARAM_PW 1
#define PATTERN_PARAM_AMP 2
#define PATTERN_PARAM_IPI 3

// gait look up table resolution.
#define GAIT_LUT_RES 8

// Create 3 fixed schedule for 30,50,60 ms IPI.
#define FIXED_SCHED 3
#define FIXED_SCHED_ID1_IPI 30
#define FIXED_SCHED_ID2_IPI 50
#define FIXED_SCHED_ID3_IPI 60

// PW limiter
#define PW_VAL_MAX 255 
#define AMP_VAL_MAX 100 

#endif /* CWRUSTIMLIBCONST_H */  
