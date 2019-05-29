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
#include <trajectory.h>
#include <coop.h>
#include <packet_hybrid.h>
#include <board_io.h>
#include <joint.h>
#include <ged_fsm.h>
#include <stim_interface.h>

#include <module_logger.h>

/** Global Defaults */
#define EXO_TF 0.75
#define EXO_TPS_PERC 0.5
#define EXO_TRAJ_LOOKUP_SIZE 750
#define EXO_DELTA_TIME 0.001
#define EXO_HIP_ANGLE_MIN -15
#define EXO_HIP_ANGLE_MAX 15
#define EXO_KNEE_ANGLE_MAX 60
#define EXO_FC_ALPHA 0.5
#define EXO_LOW_FSR_THRESH 500
#define EXO_HIGH_FSR_THRESH 1500
#define EXO_WEIGHT_ACC_FSR_THRESH 1000
#define EXO_KNEE_EXTENSION_THRESH 15
#define EXO_HIP_FLEXION_THRESH 0

typedef enum exo_swing_mode_e {
  EXO_MODE_FLOAT,
  EXO_MODE_POSITION_CONTROL,
} exo_swing_mode_t;

/** Global Definitions */
typedef struct exo_s {
  stim_pattern_t pattern_brd1;
  stim_pattern_t pattern_brd2;
  cwru_stim_struct_t cwru_stim_brd1;
  cwru_stim_struct_t cwru_stim_brd2;

  float max_knee_angle;
  float max_hip_angle;
  float min_hip_angle;

  float l_hip_zero_offset;
  float r_hip_zero_offset;

  joint_t r_knee_joint;
  joint_t l_knee_joint;

  tr_coeff_t hip_traj_coeff;
  tr_coeff_t knee_ps_coeff;
  tr_coeff_t knee_ls_coeff;
  float knee_traj_lut[EXO_TRAJ_LOOKUP_SIZE];

  float l_knee_extension_thresh;
  float r_knee_extension_thresh;

  float l_hip_flexion_thresh;
  float r_hip_flexion_thresh;

  bd_inputs_t sensor_inputs;
  ged_state_machine_t state_machine;

  uint32_t timestamp;
  bool time_marker;

  uint8_t position_control;
  uint8_t auto_walk;

  uint16_t r_low_fsr_thresh;
  uint16_t r_high_fsr_thresh;
  uint16_t r_weight_acc_fsr_thresh;

  uint16_t l_low_fsr_thresh;
  uint16_t l_high_fsr_thresh;
  uint16_t l_weight_acc_fsr_thresh;

  pkt_generic_t input_packet;
  bool send_log;
  bool motor_force_off;

  pkt_module_data_t module_data[MODULE_ID_MAX];
  module_logger_t module_logger;

  coop_task_list_t coop_task_list;
} exo_t;

typedef enum {
  //EXO_USB_OUTPUT_TASK,
  //EXO_USB_INPUT_TASK,
  EXO_CAN_INPUT_TASK,
  //EXO_QUERY_TASK,
  EXO_STIM_TASK,
  EXO_TASK_MAX
} exo_coop_task_index_t;

/** Global variables */
extern volatile exo_t hybrid_exoskeleton;

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

/** \brief This thread queries the modules for relevant data. */
void exoQueryThread(coop_args_t *args);


#endif
