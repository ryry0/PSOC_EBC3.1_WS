#include <board_setup.h>
#include <project.h>
#include <ged_fsm.h>
#include <stim_interface.h>
#include <board_can.h>
#include <stim_configurable.h>
#include <HNP1_gait_withmisc.h>
#include <gait_zero_pattern.h>

/*---------------------------------------------------------------------------*/
/* Setup Functions */

void setupStimulator(
    struct coop_task_s *task_arr,
    coop_args_t *coop_args,
    volatile stim_module_t *stimulator) {

  coop_args->stimulator = stimulator;

  setupStimulatorVars(stimulator);
  setupThreads((struct coop_task_list_s *)&stimulator->coop_task_list, task_arr);
  setupBoard();
  setupStim(stimulator);
}

void setupThreads(struct coop_task_list_s *task_list, struct coop_task_s
    *task_arr) {
  //setup the task array
  /*
  task_arr[EXO_USB_OUTPUT_TASK] = (struct coop_task_s) {
    .task_func = USBOutputThread,
    .period = 10, //10 @ 10 ms/100Hz
    .countdown = 0
  };
  */

#ifdef STIM_ENABLE_USB
  task_arr[STIM_USB_INPUT_TASK]= (struct coop_task_s) {
    .task_func = USBInputThread,
    .period = 0, //always scheduled
    .countdown = 0
  };
#endif

  task_arr[STIM_STIM_TASK]= (struct coop_task_s) {
    .task_func = stimThread,
    .period = 20, //20 ms scheduled
    .countdown = 0
  };

  task_arr[STIM_CAN_INPUT_TASK]= (struct coop_task_s) {
    .task_func = CANInputThread,
    .period = 0, //always scheduled
    .countdown = 0
  };

  //set up the task list
  task_list->num_tasks = STIM_TASK_MAX;
  task_list->task_arr = task_arr;
}

void setupStimulatorVars(volatile stim_module_t *stimulator) {

  /* setup sensor readings */
  bd_setFSWPins(
      (bd_inputs_t *) &stimulator->sensor_inputs,
      DIO_13_FSW_BUT13_0,
      DIO_11_FSW_BUT11_0,
      DIO_12_FSW_BUT12_0,
      DIO_10_FSW_BUT10_0);

  pkt_init((pkt_generic_t *) &stimulator->input_packet);
  stimulator->send_log = false;

  ged_init((ged_state_machine_t *) &stimulator->state_machine, (stim_module_t *) stimulator);
}

void setupBoard() {
  /* Enable global interrupts. */
  CyGlobalIntEnable;

  /* Place your initialization/startup code here (e.g. MyInst_Start()) */
  /* Initialize the serial communications interface */

#ifdef EXO_ENABLE_USB
  USBUART_1_Start(0, USBUART_1_3V_OPERATION);//!!NOTE!! Make sure this matches your board voltage!
  while(!USBUART_1_bGetConfiguration());
  USBUART_1_CDC_Init();
#endif

  AMuxSeq_1_Init();
  ADC_SAR_1_Start(); //initialize the adc
  Counter_1_Start();

  bd_initCAN();
  isr_1_StartEx(timerInterruptHandler); //register the interrupt handler
}

void setupStim(volatile stim_module_t *stimulator) {
  stim_pattern_t *const pattern_brd1  = (stim_pattern_t *) &stimulator->pattern_brd1;
  stim_pattern_t *const pattern_brd2  = (stim_pattern_t *) &stimulator->pattern_brd2;
  //stim_pattern_t *const configurable_pattern  = (stim_pattern_t *) &stimulator->configurable_pattern;

  cwru_stim_struct_t *const cwru_stim_brd1  = (cwru_stim_struct_t *) &stimulator->cwru_stim_brd1;
  cwru_stim_struct_t *const cwru_stim_brd2  = (cwru_stim_struct_t *) &stimulator->cwru_stim_brd2;

  //const uint8_t STIM_BOARD_IPI = 50; //everything in BAL06 is 50
  const uint8_t STIM_BOARD_IPI = 30; //everything in HNP1 is 30 kinda

  stimpat_initPattern(pattern_brd1,
      &gait_zero_PP,
      &gait_zero_PW,
      1.0,
      1000);

  stimint_initBoardUART(cwru_stim_brd1, STIM_UART_PORT_0); //SCB J115
  STIM_SETUP_FUNC(cwru_stim_brd1, STIM_BOARD_IPI);
  stimpat_setChannelAmplitudes(pattern_brd1, gait_B1_AMP);

  stimpat_initPattern(pattern_brd2,
      &gait_zero_PP,
      &gait_zero_PW,
      1.0,
      1000);

  stimint_initBoardUART(cwru_stim_brd2, STIM_UART_PORT_1); //SCB J116
  STIM_SETUP_FUNC(cwru_stim_brd2, STIM_BOARD_IPI);
  stimpat_setChannelAmplitudes(pattern_brd2, gait_B2_AMP);

#if defined(STIM_CONFIG_3RD_SURFACE)
  stim_pattern_t *const pattern_brd3  = (stim_pattern_t *) &stimulator->pattern_brd3;
  cwru_stim_struct_t *const cwru_stim_brd3  = (cwru_stim_struct_t *) &stimulator->cwru_stim_brd3;

  stimpat_initPattern(pattern_brd3,
      &gait_stand_B2_PP,
      &gait_stand_B2_PW,
      gait_stand_duration,
      1000);

  stimint_initBoardUART(cwru_stim_brd3, STIM_UART_PORT_2); //PSOC EP2 Connector
  stimint_initSurfBoard(cwru_stim_brd3, STIM_BOARD_IPI);
#endif
}

