#include <exo.h>
#include <math.h>
#include <numerical.h>
#include <stdint.h>
#include <stdlib.h>
#include <ged_fsm.h>
#include <board_setup.h>
#include <stim_interface.h>
#include <board_can.h>

/** Global variables */
volatile exo_t hybrid_exoskeleton = {
  .r_knee_joint = {{0}},
  .l_knee_joint = {{0}},
  .sensor_inputs = {{0}},
  .coop_task_list = {0},
  .state_machine = {{0}},
  .timestamp = 0,
  .time_marker = 0,
  .max_knee_angle = EXO_KNEE_ANGLE_MAX,
  .max_hip_angle = EXO_HIP_ANGLE_MAX,
  .min_hip_angle = EXO_HIP_ANGLE_MIN,

  .r_low_fsr_thresh = EXO_LOW_FSR_THRESH,
  .r_high_fsr_thresh = EXO_HIGH_FSR_THRESH,
  .r_weight_acc_fsr_thresh = EXO_WEIGHT_ACC_FSR_THRESH,
  .l_low_fsr_thresh = EXO_LOW_FSR_THRESH,
  .l_high_fsr_thresh = EXO_HIGH_FSR_THRESH,
  .l_weight_acc_fsr_thresh = EXO_WEIGHT_ACC_FSR_THRESH,


  .l_knee_extension_thresh = EXO_KNEE_EXTENSION_THRESH,
  .r_knee_extension_thresh = EXO_KNEE_EXTENSION_THRESH,

  .l_hip_flexion_thresh = EXO_HIP_FLEXION_THRESH,
  .r_hip_flexion_thresh = EXO_HIP_FLEXION_THRESH,

  .position_control = EXO_MODE_FLOAT,
  .auto_walk = 1,
  .motor_force_off = false,
  .l_hip_zero_offset = BD_L_HIP_ZERO_OFF,
  .r_hip_zero_offset = BD_R_HIP_ZERO_OFF
};

/*---------------------------------------------------------------------------*/
/** Function prototypes */
/** Threads */
/** Auxiliary functions */
static void handleSingleByteMsg(uint8_t byte, volatile exo_t *exo);

static void handlePacketMsg(pkt_generic_t *packet, volatile exo_t *exo);

static void setParameters(pkt_generic_t *packet, volatile exo_t *exo);

static void getParameters(volatile exo_t *exo);

static void sendAck();

/*---------------------------------------------------------------------------*/
/** Threads */
/* Interrupt Handler */
CY_ISR ( timerInterruptHandler ) {
  DIO_6_MOT1_DIO3_Write(1);

  exo_t *const exo = (exo_t *) &hybrid_exoskeleton;
  struct bd_inputs_s *const sensor_inputs = &exo->sensor_inputs;
  joint_t *const r_knee_joint = &exo->r_knee_joint;
  joint_t *const l_knee_joint = &exo->l_knee_joint;
  ged_state_machine_t *state_machine = &exo->state_machine;

  /** read sensor signals */
  bd_readSensors(sensor_inputs);
  joint_measure(r_knee_joint, sensor_inputs);
  joint_measure(l_knee_joint, sensor_inputs);

  /** set the time marker */
  if (bd_getFSW(sensor_inputs, BD_WHITE_FSW))
    exo->time_marker = !exo->time_marker;

  /** Update the state machine and run the control */
  ged_runState(state_machine, exo);
  ged_checkTransitions(state_machine, exo);

  /** Run the scheduler to schedule the next thread */
  coop_scheduleTasks(&exo->coop_task_list);

  Counter_1_ReadStatusRegister();
  exo->timestamp++;
  DIO_6_MOT1_DIO3_Write(0);
}

/*---------------------------------------------------------------------------*/
/* Thread implementations */

void USBOutputThread(coop_args_t *args) {

  volatile exo_t *const exo = args->exo;

  bd_inputs_t *const sensor_inputs = (bd_inputs_t *) &exo->sensor_inputs;

  joint_t *const r_knee_joint = (joint_t *) &exo->r_knee_joint;
  joint_t *const l_knee_joint = (joint_t *) &exo->l_knee_joint;

  pkt_generic_t output_packet;

  pkt_init(&output_packet);
  pkt_setHeader(&output_packet, PKT_LOG_CAN_DATA, sizeof(pkt_log_can_data_t));

  if (exo->send_log) {
    pkt_log_can_data_t *log_payload = pkt_interp(pkt_log_can_data_t, output_packet);

    for (size_t i = MODULE_RIGHT_HIP_ID; i < MODULE_ID_MAX; ++i) {
      memcpy(&log_payload->module_data[i], (pkt_module_data_t *) &exo->module_data[i], sizeof(pkt_module_data_t));
    }

    log_payload->timestamp = exo->timestamp;

    log_payload->an_11_bat_volt = bd_getAin(sensor_inputs, BD_AN_11_BAT_VOLT);

    log_payload->state = ged_getStateID((ged_state_machine_t *) &exo->state_machine);
    log_payload->time_marker = exo->time_marker;

    bd_putLargeDataReady(pkt_encodeBuffer(&output_packet),
        pkt_getTotalLength(&output_packet));

    //check and clear the module logger
    if (ml_checkAllReceived((module_logger_t *) &exo->module_logger))
      ml_clear((module_logger_t *) &exo->module_logger);
  }
}


void USBInputThread(coop_args_t *args) {
  volatile exo_t *exo = args->exo;

  uint8_t uart_count;
  uint8_t uart_buffer[128];
  pkt_generic_t *input_packet = (pkt_generic_t *) &exo->input_packet;

  uart_count = USBUART_1_GetCount();

  /* Check for input data from PC */
  if(uart_count != 0) {
    USBUART_1_GetAll(uart_buffer);
    bd_usbTimeout();

    for (size_t i = 0; i < uart_count; ++i) {
      if(pkt_readByte(input_packet, uart_buffer[i])) {
        pkt_decodePacket(input_packet);
        handlePacketMsg(input_packet, exo);
        pkt_clear(input_packet);
      }
    }

    //If we send a one byte message
    if (pkt_getIndex(input_packet) == 0)
      handleSingleByteMsg(uart_buffer[0], exo);

  } //end if(uart_count

} //end USBInputThread

/*---------------------------------------------------------------------------*/
/* Function implementations */
void handleSingleByteMsg(
    uint8_t byte,
    volatile exo_t *exo) {
  switch(byte) {
    case 0xFF:
      exo->send_log = false;
      sendAck();
      break;

    case 'e':
      //psoc_putStringReady("Motor enable\n");
      if (ged_getStateID((ged_state_machine_t *) &exo->state_machine) == GED_LOCKED) {
        joint_setMotorOffOverride((joint_t *) &exo->r_knee_joint, false);
        joint_setMotorOffOverride((joint_t *) &exo->l_knee_joint, false);
        exo->motor_force_off = false;
      }
      break;

    case 'd':
      //psoc_putStringReady("Motor disable\n");
      if (ged_getStateID((ged_state_machine_t *) &exo->state_machine) == GED_LOCKED) {
        joint_setMotorOffOverride((joint_t *) &exo->r_knee_joint, true);
        joint_setMotorOffOverride((joint_t *) &exo->l_knee_joint, true);
        joint_disableMotor((joint_t *) &exo->r_knee_joint);
        joint_disableMotor((joint_t *) &exo->l_knee_joint);
        exo->motor_force_off = true;
      }
      break;
  }
}

void handlePacketMsg(
    pkt_generic_t *packet,
    volatile exo_t *exo) {

  pkt_set_pid_t *pid_packet;
  pkt_set_state_t *state_payload;
  struct bd_inputs_s *const sensor_inputs = &exo->sensor_inputs;

  switch (packet->type) {
    case PKT_SYN:
      exo->send_log = false;
      sendAck();
      break;

      /*
    case PKT_SET_PID:
      pid_packet = pkt_interpPtr(pkt_set_pid_t, packet);

      //exo->new_setpoint = pid_packet->setpoint;
      exo->r_knee_joint.position_pid.proportional_gain = pid_packet->Kp;
      exo->r_knee_joint.position_pid.integral_gain = pid_packet->Ki;
      exo->r_knee_joint.position_pid.derivative_gain = pid_packet->Kd;
      exo->l_knee_joint.position_pid.proportional_gain = pid_packet->Kp;
      exo->l_knee_joint.position_pid.integral_gain = pid_packet->Ki;
      exo->l_knee_joint.position_pid.derivative_gain = pid_packet->Kd;
      break;
      */

    case PKT_GET_PARAMS:
      getParameters(exo);
      break;

    case PKT_SET_PARAMS:
      setParameters(packet, exo);
      break;

    case PKT_DATA_START:
      exo->send_log = true;
      break;

    case PKT_ZERO_PID:
      //pid_init(&exo->speed_pid, 0, 0, 0, 5000);
      //exo->setpoint = exo->new_setpoint;
      break;

    case PKT_SET_STATE:
      state_payload = pkt_interpPtr(pkt_set_state_t, packet);
      ged_forceTrans((ged_state_machine_t *) &exo->state_machine, (void *) exo, state_payload->state);
      break;

    case PKT_CALIBRATE_LHIP:
      {
        float l_hip_angle = bd_analogToLHipAngle(bd_getAin(sensor_inputs,
          BD_AN_4_L_HIP_ENC), exo->l_hip_zero_offset);
        exo->l_hip_zero_offset = exo->l_hip_zero_offset - l_hip_angle;
      }
      break;

    case PKT_CALIBRATE_RHIP:
      {
        float r_hip_angle = bd_analogToRHipAngle(bd_getAin(sensor_inputs,
          BD_AN_5_R_HIP_ENC), exo->r_hip_zero_offset);
        exo->r_hip_zero_offset = exo->r_hip_zero_offset + r_hip_angle;
      }
      break;


    /*
    case PKT_QUERY_MODULE:
      {
        pkt_query_module_t *query_payload = pkt_interpPtr(pkt_query_module_t, packet);
        const uint8_t module_id = query_payload->module_id;
        const uint8_t command_state = query_payload->command_state;

        //force a state change if the host has a different state.
        if (command_state != ged_getStateID((ged_state_machine_t *) &module->state_machine))
          ged_forceTrans((ged_state_machine_t *) &module->state_machine,
              (void *) module, (ged_state_id_t) command_state);

        //if this module is the queried one, reply.
        if (module_id == MODULE_ID)
          moduleReply(module);
      }
      break;
      */

    case PKT_MODULE_DATA:
      {
        pkt_module_data_t *module_data_received =
          pkt_interpPtr(pkt_module_data_t, packet);

        //if valid module id
        if (module_data_received->module_id < MODULE_ID_MAX) {
          //copy the data to the module data struct
          memcpy(&exo->module_data[ module_data_received->module_id ],
              module_data_received, sizeof(pkt_module_data_t));

          ml_moduleReceived(&exo->module_logger,
              module_data_received->module_id);
        }
      }
      break;

    default:
      break;
  }
}

static void setParameters(pkt_generic_t *packet,
    volatile exo_t *exo) {

  pkt_set_params_t *parameter_packet = pkt_interpPtr(pkt_set_params_t, packet);

  joint_t *const r_knee_joint = (joint_t *) &exo->r_knee_joint;
  joint_t *const l_knee_joint = (joint_t *) &exo->l_knee_joint;

  exo->min_hip_angle = parameter_packet->float_params[MIN_HIP_ANGLE];
  exo->max_hip_angle = parameter_packet->float_params[MAX_HIP_ANGLE];
  exo->max_knee_angle = parameter_packet->float_params[MAX_KNEE_ANGLE];
  exo->position_control = parameter_packet->float_params[POSITION_CONTROL];

  /* AUTO WALKING */
  exo->auto_walk = parameter_packet->float_params[AUTO_WALK];

  /* MOTOR ON OFF */
  bool motor_force_off_recv = parameter_packet->float_params[MOTOR_FORCE_OFF];

  if (ged_getStateID((ged_state_machine_t *) &exo->state_machine) == GED_LOCKED) {
      if (motor_force_off_recv) {
        joint_setMotorOffOverride((joint_t *) &exo->r_knee_joint, true);
        joint_setMotorOffOverride((joint_t *) &exo->l_knee_joint, true);
        joint_disableMotor((joint_t *) &exo->r_knee_joint);
        joint_disableMotor((joint_t *) &exo->l_knee_joint);
      }
      else if (motor_force_off_recv == false) {
        joint_setMotorOffOverride((joint_t *) &exo->r_knee_joint, false);
        joint_setMotorOffOverride((joint_t *) &exo->l_knee_joint, false);
      }
    exo->motor_force_off = motor_force_off_recv;
  }

  setupTrajectories(exo);
}

static void getParameters(volatile exo_t *exo) {
  pkt_generic_t output_packet;
  joint_t *const r_knee_joint = (joint_t *) &exo->r_knee_joint;
  joint_t *const l_knee_joint = (joint_t *) &exo->l_knee_joint;

  pkt_init(&output_packet);
  pkt_setHeader(&output_packet, PKT_GET_PARAMS, sizeof(pkt_get_params_t));

  pkt_get_params_t *parameter_packet = pkt_interp(pkt_get_params_t, output_packet);

  parameter_packet->float_params[MIN_HIP_ANGLE] = exo->min_hip_angle;
  parameter_packet->float_params[MAX_HIP_ANGLE] = exo->max_hip_angle;
  parameter_packet->float_params[MAX_KNEE_ANGLE] = exo->max_knee_angle;
  parameter_packet->float_params[POSITION_CONTROL] = exo->position_control;
  parameter_packet->float_params[AUTO_WALK] = exo->auto_walk;

  parameter_packet->float_params[MOTOR_FORCE_OFF] = exo->motor_force_off;

  bd_putLargeDataReady(pkt_encodeBuffer(&output_packet),
      pkt_getTotalLength(&output_packet));
}

void sendAck() {
  pkt_generic_t output_packet;

  pkt_setHeaderTypeOnly(&output_packet, PKT_ACK);

  bd_putDataReady(pkt_encodeBuffer(&output_packet),
      pkt_getTotalLength(&output_packet));
}

void stimThread(coop_args_t *args) {
  volatile exo_t *exo = args->exo;
  stim_pattern_t *const pattern_brd1  = &exo->pattern_brd1;
  stim_pattern_t *const pattern_brd2  = &exo->pattern_brd2;
  cwru_stim_struct_t *const cwru_stim_brd1  = &exo->cwru_stim_brd1;
  cwru_stim_struct_t *const cwru_stim_brd2  = &exo->cwru_stim_brd2;

  stimpat_applyPatternPercLoop(cwru_stim_brd1, pattern_brd1);
  stimpat_incrementCounter(pattern_brd1, 20);

  stimpat_applyPatternPercLoop(cwru_stim_brd2, pattern_brd2);
  stimpat_incrementCounter(pattern_brd2, 20);
}

void CANInputThread(coop_args_t *args) {
  volatile exo_t *exo = args->exo;

  size_t can_count;
  uint8_t can_buffer[128];
  pkt_generic_t *input_packet = (pkt_generic_t *) &exo->input_packet;

  /* Check for input data from PC */
  while(bd_checkCAN()) {

    can_count = bd_readCAN(can_buffer);

    for (size_t i = 0; i < can_count; ++i) {
      if(pkt_readByte(input_packet, can_buffer[i])) {
        pkt_decodePacket(input_packet);
        handlePacketMsg(input_packet, exo);
        pkt_clear(input_packet);
      }
    }

    //If we send a one byte message
    //if (pkt_getIndex(input_packet) == 0)
      //handleSingleByteMsg(can_buffer[0], exo);

  } //end if(can_count
}

void exoQueryThread(coop_args_t *args) {
  volatile exo_t *exo = args->exo;

  if (exo->send_log) {
    ml_queryModules(&exo->module_logger);
  }
}
