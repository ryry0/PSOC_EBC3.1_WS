#include <stim_module.h>
#include <math.h>
#include <numerical.h>
#include <stdint.h>
#include <stdlib.h>
#include <ged_fsm.h>
#include <board_setup.h>
#include <stim_interface.h>
#include <board_can.h>
#include <stim_configurable.h>

/** Global variables */
volatile stim_module_t hybrid_stimulator = {
  .sensor_inputs = {{0}},
  .coop_task_list = {0},
  .state_machine = {{0}},
  .timestamp = 0,
  .stim_scaling_factor = STIM_MODULE_SCALING_FACTOR,
  .biotilc_l_hip_flexion_scaling_factor = STIM_MODULE_PW_SCALING_FACTOR,
  .biotilc_l_knee_flexion_scaling_factor = STIM_MODULE_PW_SCALING_FACTOR,
  .biotilc_l_knee_extension_scaling_factor =
    STIM_MODULE_PW_SCALING_FACTOR,

  .biotilc_r_hip_flexion_scaling_factor = STIM_MODULE_PW_SCALING_FACTOR,
  .biotilc_r_knee_flexion_scaling_factor = STIM_MODULE_PW_SCALING_FACTOR,
  .biotilc_r_knee_extension_scaling_factor =
    STIM_MODULE_PW_SCALING_FACTOR,
  .mode = STIM_MODULE_MODE_FLOAT,
};

/*---------------------------------------------------------------------------*/
/** Function prototypes */
/** Threads */
/** Auxiliary functions */
static void handlePacketMsg(pkt_generic_t *packet, volatile stim_module_t *stimulator);

static void setParameters(pkt_generic_t *packet, volatile stim_module_t *stimulator);

static void getParameters(volatile stim_module_t *stimulator);

static void stim_setMode(pkt_generic_t *packet, volatile stim_module_t *stimulator);

static void sendAck();

static void setStimParameters(pkt_generic_t *packet, volatile stim_module_t
    *stimulator);

static void setLeftBiotilcParams(pkt_generic_t *packet, volatile stim_module_t *stimulator);
static void setRightBiotilcParams(pkt_generic_t *packet, volatile stim_module_t *stimulator);

/*---------------------------------------------------------------------------*/
/** Threads */
/* Interrupt Handler */
CY_ISR ( timerInterruptHandler ) {
  DIO_6_MOT1_DIO3_Write(1);

  stim_module_t *const stimulator = (stim_module_t *) &hybrid_stimulator;
  struct bd_inputs_s *const sensor_inputs = &stimulator->sensor_inputs;
  ged_state_machine_t *state_machine = &stimulator->state_machine;

  /** read sensor signals */
  bd_readSensors(sensor_inputs);

  /** Update the state machine and run the control */
  ged_runState(state_machine, stimulator);
  ged_checkTransitions(state_machine, stimulator);

  /** Run the scheduler to schedule the next thread */
  coop_scheduleTasks(&stimulator->coop_task_list);

  Counter_1_ReadStatusRegister();
  stimulator->timestamp++;
  DIO_6_MOT1_DIO3_Write(0);
}

/*---------------------------------------------------------------------------*/
/* Thread implementations */

void USBOutputThread(coop_args_t *args) {

  volatile stim_module_t *const stimulator = args->stimulator;

  bd_inputs_t *const sensor_inputs = (bd_inputs_t *) &stimulator->sensor_inputs;

  pkt_generic_t output_packet;

  pkt_init(&output_packet);
  pkt_setHeader(&output_packet, PKT_LOG_CAN_DATA, sizeof(pkt_log_can_data_t));

  if (stimulator->send_log) {
    pkt_log_can_data_t *log_payload = pkt_interp(pkt_log_can_data_t, output_packet);

    for (size_t i = MODULE_RIGHT_HIP_ID; i < MODULE_ID_MAX; ++i) {
      memcpy(&log_payload->module_data[i], (pkt_module_data_t *) &stimulator->module_data[i], sizeof(pkt_module_data_t));
    }

    log_payload->timestamp = stimulator->timestamp;

    log_payload->an_11_bat_volt = bd_getAin(sensor_inputs, BD_AN_11_BAT_VOLT);

    log_payload->state = ged_getStateID((ged_state_machine_t *) &stimulator->state_machine);

    bd_putLargeDataReady(pkt_encodeBuffer(&output_packet),
        pkt_getTotalLength(&output_packet));
  }
}


void USBInputThread(coop_args_t *args) {
  volatile stim_module_t *stimulator = args->stimulator;

  uint8_t uart_count;
  uint8_t uart_buffer[128];
  pkt_generic_t *input_packet = (pkt_generic_t *) &stimulator->input_packet;

  uart_count = USBUART_1_GetCount();

  /* Check for input data from PC */
  if(uart_count != 0) {
    USBUART_1_GetAll(uart_buffer);
    bd_usbTimeout();

    for (size_t i = 0; i < uart_count; ++i) {
      if(pkt_readByte(input_packet, uart_buffer[i])) {
        pkt_decodePacket(input_packet);
        handlePacketMsg(input_packet, stimulator);
        pkt_clear(input_packet);
      }
    }
  } //end if(uart_count

} //end USBInputThread

/*---------------------------------------------------------------------------*/
/* Function implementations */
void handlePacketMsg(
    pkt_generic_t *packet,
    volatile stim_module_t *stimulator) {

  pkt_set_state_t *state_payload;

  switch (packet->type) {
    case PKT_SYN:
      stimulator->send_log = false;
      sendAck();
      break;

    case PKT_GET_PARAMS:
      getParameters(stimulator);
      break;

    case PKT_SET_PARAMS:
      setParameters(packet, stimulator);
      break;

    case PKT_DATA_START:
      break;

    case PKT_SET_STATE:
      state_payload = pkt_interpPtr(pkt_set_state_t, packet);
      ged_forceTrans((ged_state_machine_t *) &stimulator->state_machine, (void *) stimulator, state_payload->state);
      break;

    case PKT_CALIBRATE_LHIP:
      break;

    case PKT_CALIBRATE_RHIP:
      break;


    case PKT_QUERY_MODULE:
      {
        pkt_query_module_t *query_payload = pkt_interpPtr(pkt_query_module_t, packet);
        //const uint8_t module_id = query_payload->module_id;
        const uint8_t command_state = query_payload->command_state;

        //force a state change if the host has a different state.
        if (command_state != ged_getStateID((ged_state_machine_t *) &stimulator->state_machine))
          ged_forceTrans((ged_state_machine_t *) &stimulator->state_machine,
              (void *) stimulator, (ged_state_id_t) command_state);

        /*
        //if this module is the queried one, reply.
        if (module_id == MODULE_ID)
          moduleReply(module);
          */
      }
      break;

      /*
    case PKT_MODULE_DATA:
      {
        pkt_module_data_t *module_data_received =
          pkt_interpPtr(pkt_module_data_t, packet);

        //if valid module id
        if (module_data_received->module_id < MODULE_ID_MAX) {
          //copy the data to the module data struct
          memcpy(&stimulator->module_data[ module_data_received->module_id ],
              module_data_received, sizeof(pkt_module_data_t));

          //ml_moduleReceived(&stimulator->module_logger,
              //module_data_received->module_id);
        }
      }
      break;
      */

        /*
    case PKT_MODULE_SET_STIM_PATTERN_CHANNEL:
      {
      pkt_set_stim_pattern_channel *stim_packet_payload =
        pkt_interpPtr(pkt_set_stim_pattern_channel, packet);

      const uint8_t channel_number = stim_packet_payload->channel_number;

      memcpy(&configurable_pattern1_PW[channel_number][0],
          stim_packet_payload->pulse_widths, sizeof(uint8_t)*STIM_NUM_POINTS);

      memcpy(&configurable_pattern1_PP[channel_number][0],
          stim_packet_payload->pulse_percentages, sizeof(uint16_t)*STIM_NUM_POINTS);
      }
      break;
        */

    case PKT_MODULE_SET_SHARED_PARAMS:
      stim_setMode(packet, stimulator);

      break;

    case PKT_STIM_CONFIGURE:
      setStimParameters(packet, stimulator);
      break;

    case PKT_LEFT_BIOTILC:
      setLeftBiotilcParams(packet, stimulator);
      break;

    case PKT_RIGHT_BIOTILC:
      setRightBiotilcParams(packet, stimulator);
      break;

    default:
      break;
  }
}

static void setLeftBiotilcParams(pkt_generic_t *packet, volatile stim_module_t *stimulator) {
  pkt_biotilc_t *parameter_packet =
    pkt_interpPtr(pkt_biotilc_t, packet);

  stimulator->biotilc_l_hip_flexion_scaling_factor =
    parameter_packet->float_params[PKT_BT_HIP_FLEXION_SF];
  stimulator->biotilc_l_knee_flexion_scaling_factor =
    parameter_packet->float_params[PKT_BT_KNEE_FLEXION_SF];
  stimulator->biotilc_l_knee_extension_scaling_factor =
    parameter_packet->float_params[PKT_BT_KNEE_EXTENSION_SF];
}

static void setRightBiotilcParams(pkt_generic_t *packet, volatile stim_module_t *stimulator) {
  pkt_biotilc_t *parameter_packet =
    pkt_interpPtr(pkt_biotilc_t, packet);

  stimulator->biotilc_r_hip_flexion_scaling_factor =
    parameter_packet->float_params[PKT_BT_HIP_FLEXION_SF];
  stimulator->biotilc_r_knee_flexion_scaling_factor =
    parameter_packet->float_params[PKT_BT_KNEE_FLEXION_SF];
  stimulator->biotilc_r_knee_extension_scaling_factor =
    parameter_packet->float_params[PKT_BT_KNEE_EXTENSION_SF];
}

static void stim_setMode(pkt_generic_t *packet, volatile stim_module_t *stimulator) {

  pkt_shared_module_params_t *parameter_packet =
    pkt_interpPtr(pkt_shared_module_params_t, packet);

  stimulator->mode =
    parameter_packet->float_params[PKT_SHARED_MODE];
}

static void setParameters(pkt_generic_t *packet,
    volatile stim_module_t *stimulator) {

}

static void getParameters(volatile stim_module_t *stimulator) {
}

static void setStimParameters(pkt_generic_t *packet, volatile stim_module_t
    *stimulator) {

  pkt_stim_config_t *parameter_packet =
    pkt_interpPtr(pkt_stim_config_t, packet);

  stimulator->stim_scaling_factor =
    parameter_packet->float_params[PKT_STIM_SCALING_FACTOR];
}

void sendAck() {
  pkt_generic_t output_packet;

  pkt_setHeaderTypeOnly(&output_packet, PKT_ACK);

  bd_putDataReady(pkt_encodeBuffer(&output_packet),
      pkt_getTotalLength(&output_packet));
}

void stimThread(coop_args_t *args) {
  volatile stim_module_t *stimulator = args->stimulator;
  stim_pattern_t *const pattern_brd1  =
    (stim_pattern_t *) &stimulator->pattern_brd1;

  stim_pattern_t *const pattern_brd2  =
    (stim_pattern_t *) &stimulator->pattern_brd2;

  cwru_stim_struct_t *const cwru_stim_brd1  =
    (cwru_stim_struct_t *) &stimulator->cwru_stim_brd1;

  cwru_stim_struct_t *const cwru_stim_brd2  =
    (cwru_stim_struct_t *) &stimulator->cwru_stim_brd2;

  const uint8_t current_state = ged_getStateID((ged_state_machine_t *) &stimulator->state_machine);

  float left_swing_scaling[STIM_CHANNEL_USED_PERC] = {
    stimulator->biotilc_l_knee_extension_scaling_factor,
    stimulator->biotilc_l_knee_flexion_scaling_factor,
    stimulator->biotilc_l_knee_extension_scaling_factor,
    1,
    1,
    stimulator->biotilc_l_hip_flexion_scaling_factor,
    stimulator->biotilc_l_hip_flexion_scaling_factor,
    1
  };

  float right_swing_scaling[STIM_CHANNEL_USED_PERC] = {
    stimulator->biotilc_r_knee_extension_scaling_factor,
    stimulator->biotilc_r_hip_flexion_scaling_factor,
    1,
    1,
    stimulator->biotilc_r_knee_extension_scaling_factor,
    stimulator->biotilc_r_hip_flexion_scaling_factor,
    stimulator->biotilc_r_knee_flexion_scaling_factor,
    1
  };

  if (current_state == GED_LEFT_SWING) { //scale left side
    stimpat_applyPatternLoopScaling(cwru_stim_brd1, pattern_brd1, STIM_AMPLITUDE,
        left_swing_scaling);
    stimpat_incrementCounter(pattern_brd1, 20);

    stimpat_applyPatternLoop(cwru_stim_brd2, pattern_brd2, STIM_AMPLITUDE);
    stimpat_incrementCounter(pattern_brd2, 20);
  }
  else if (current_state == GED_RIGHT_SWING) { //scale right side
    stimpat_applyPatternLoop(cwru_stim_brd1, pattern_brd1, STIM_AMPLITUDE);
    stimpat_incrementCounter(pattern_brd1, 20);

    stimpat_applyPatternLoopScaling(cwru_stim_brd2, pattern_brd2, STIM_AMPLITUDE,
        right_swing_scaling);
    stimpat_incrementCounter(pattern_brd2, 20);
  }
  else {
    stimpat_applyPatternLoop(cwru_stim_brd1, pattern_brd1, STIM_AMPLITUDE);
    stimpat_incrementCounter(pattern_brd1, 20);

    stimpat_applyPatternLoop(cwru_stim_brd2, pattern_brd2, STIM_AMPLITUDE);
    stimpat_incrementCounter(pattern_brd2, 20);
  }

#if defined(STIM_CONFIG_3RD_SURFACE)
  stim_pattern_t *const pattern_brd3  = &stimulator->pattern_brd3;
  cwru_stim_struct_t *const cwru_stim_brd3  = &stimulator->cwru_stim_brd3;

  stimpat_applyPatternLoop(cwru_stim_brd3, pattern_brd3, STIM_AMPLITUDE);
  stimpat_incrementCounter(pattern_brd2, 20);
#endif
}

void CANInputThread(coop_args_t *args) {
  volatile stim_module_t *stimulator = args->stimulator;

  size_t can_count;
  uint8_t can_buffer[1000];
  pkt_generic_t *input_packet = (pkt_generic_t *) &stimulator->input_packet;

  /* Check for input data from PC */
  while(bd_checkCAN()) {

    can_count = bd_readCAN(can_buffer);

    for (size_t i = 0; i < can_count; ++i) {
      if(pkt_readByte(input_packet, can_buffer[i])) {
        pkt_decodePacket(input_packet);
        handlePacketMsg(input_packet, stimulator);
        pkt_clear(input_packet);
      }
    }
  } //end if(can_count
}
