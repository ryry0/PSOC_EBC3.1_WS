/**
 * \file
 *
 * \author Ryan-David Reyes
 *
 * \brief This file contains all the functions that are run to setup the board
 * and threads that run the exoskeleton.
 */

#ifndef EXO_H_
#define EXO_H_
/** Standard includes */
#include <stdbool.h>

/** Cypress includes */
#include <project.h>

/** Module includes */
#include <coop.h>
#include <packet_hybrid.h>
#include <board_io.h>
#include <ged_fsm.h>
#include <stim_interface.h>

#include <stim_module_constants.h>


typedef enum stim_module_mode_e {
  STIM_MODULE_MODE_PASSIVE,
  STIM_MODULE_MODE_FLOAT,
  STIM_MODULE_MODE_BURST,
  STIM_MODULE_MODE_ASSIST,
  STIM_MODULE_MODE_GRAVITY,
} stim_module_mode_t;

/** Global Definitions */
typedef struct stim_module_s {
  stim_pattern_t pattern_brd1;
  stim_pattern_t pattern_brd2;
  stim_pattern_t pattern_brd3;
  cwru_stim_struct_t cwru_stim_brd1;
  cwru_stim_struct_t cwru_stim_brd2;
  cwru_stim_struct_t cwru_stim_brd3;

  stim_pattern_t configurable_pattern;

  bd_inputs_t sensor_inputs;
  ged_state_machine_t state_machine;

  float stim_scaling_factor;

  //biotilc
  float biotilc_l_hip_flexion_scaling_factor;
  float biotilc_l_knee_flexion_scaling_factor;
  float biotilc_l_knee_extension_scaling_factor;

  float biotilc_r_hip_flexion_scaling_factor;
  float biotilc_r_knee_flexion_scaling_factor;
  float biotilc_r_knee_extension_scaling_factor;

  uint32_t timestamp;

  pkt_generic_t input_packet;
  bool send_log;

  uint8_t mode;
  pkt_module_data_t module_data[MODULE_ID_MAX];

  coop_task_list_t coop_task_list;
} stim_module_t;

typedef enum {
  //STIM_USB_OUTPUT_TASK,
#ifdef STIM_ENABLE_USB
  STIM_USB_INPUT_TASK,
#endif
  STIM_CAN_INPUT_TASK,
  STIM_STIM_TASK,
  STIM_TASK_MAX
} stim_coop_task_index_t;

/** Global variables */
extern volatile stim_module_t hybrid_stimulator;

/** Threads */

/**
 * \brief The Interrupt Handler
 *
 * This runs the time critical PID control algorithms.
 */
CY_ISR_PROTO ( timerInterruptHandler );

/** \brief this thread is scheduled every 10 ms and is responsible for sending
 * the streaming data */
void USBOutputThread(coop_args_t *args);

/** \brief This thread runs every iteration and is responsible for receiving any
 * incoming data and parsing it. It is also responsible for replying to commands */
void USBInputThread(coop_args_t *args);

/** \brief this thread is scheduled every 20 ms and is responsible for sending
 * stimulation commands*/
void stimThread(coop_args_t *args);

/** \brief This thread runs every iteration and is responsible for receiving any
* incoming data and parsing it. It is also responsible for replying to commands */
void CANInputThread(coop_args_t *args);


#endif
