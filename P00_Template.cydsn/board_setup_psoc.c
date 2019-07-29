#include <board_setup.h>
#include <project.h>
#include <ged_fsm.h>
#include <stim_interface.h>
#include <board_can.h>
#include <stim_configurable.h>

/*---------------------------------------------------------------------------*/
/* Setup Functions */

void setupExo(
    struct coop_task_s *task_arr,
    coop_args_t *coop_args,
    volatile exo_t *exo) {

  coop_args->exo = exo;

  setupExoVars(exo);
  setupThreads((struct coop_task_list_s *)&exo->coop_task_list, task_arr);
  setupBoard();
  setupStim(exo);
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

  task_arr[EXO_USB_INPUT_TASK]= (struct coop_task_s) {
    .task_func = USBInputThread,
    .period = 0, //always scheduled
    .countdown = 0
  };
  */

  task_arr[EXO_STIM_TASK]= (struct coop_task_s) {
    .task_func = stimThread,
    .period = 20, //20 ms scheduled
    .countdown = 0
  };

  task_arr[EXO_CAN_INPUT_TASK]= (struct coop_task_s) {
    .task_func = CANInputThread,
    .period = 0, //always scheduled
    .countdown = 0
  };


  /*
  task_arr[EXO_QUERY_TASK]= (struct coop_task_s) {
    .task_func = exoQueryThread,
    .period = 10, //5ms
    .countdown = 0
  };
  */

  //set up the task list
  task_list->num_tasks = EXO_TASK_MAX;
  task_list->task_arr = task_arr;
}

void setupExoVars(volatile exo_t *exo) {
  setupRightKneeJoint((joint_t *)&exo->r_knee_joint);
  setupLeftKneeJoint((joint_t *)&exo->l_knee_joint);


  setupTrajectories((exo_t *)exo);

  /* setup sensor readings */
  bd_setFSWPins(
      (bd_inputs_t *) &exo->sensor_inputs,
      DIO_13_FSW_BUT13_0,
      DIO_11_FSW_BUT11_0,
      DIO_12_FSW_BUT12_0,
      DIO_10_FSW_BUT10_0);

  pkt_init((pkt_generic_t *) &exo->input_packet);
  exo->send_log = false;

  ged_init((ged_state_machine_t *) &exo->state_machine, (exo_t *)exo);
  ml_init((module_logger_t* ) &exo->module_logger);
}

void setupRightKneeJoint(joint_t *r_knee_joint) {
  /* Initialize the right knee joint */
  joint_initValues(r_knee_joint);

  /* Initialize the Motors */
  joint_initMotor(
      r_knee_joint,
      DIO_8_MOT2_DIN2_0,
      DIO_9_MOT2_DIO3_0,
      false,
      PWM_1_WriteCompare2,
      PWM_1_Start);

  /* Initialize the PID */
  joint_initSpeedPID(r_knee_joint, 0.1, 0.6, 0, MTR_MIN_CURRENT,
      MTR_MAX_CURRENT);

  joint_initPositionPID(r_knee_joint, 5, 15, 0, MTR_MIN_SPEED,
      MTR_MAX_SPEED); //40 25 0.1 @ 1kHz

  joint_initIO(r_knee_joint, bd_analogToRKneeAngle, BD_AN_15_MOT2_ANOUT2,
      BD_AN_14_MOT2_ANOUT1, BD_AN_7_R_KNEE_ENC, DIO_2_VALVE1_0);

  joint_initFC(r_knee_joint, EXO_FC_ALPHA);
}

void setupLeftKneeJoint(joint_t *l_knee_joint) {
  /* Initialize the left knee joint */
  joint_initValues(l_knee_joint);

  /* Initialize the Motors */
  joint_initMotor(
      l_knee_joint,
      DIO_5_MOT1_DIN2_0,
      DIO_6_MOT1_DIO3_0,
      true,
      PWM_1_WriteCompare1,
      PWM_1_Start);

  /* Initialize the PID */
  joint_initSpeedPID(l_knee_joint, 0.1, 0.6, 0, MTR_MIN_CURRENT,
      MTR_MAX_CURRENT);

  joint_initPositionPID(l_knee_joint, 5, 15, 0, MTR_MIN_SPEED,
      MTR_MAX_SPEED); //40 25 0.1 @ 1kHz

  joint_initIO(l_knee_joint, bd_analogToLKneeAngle, BD_AN_13_MOT1_ANOUT2,
      BD_AN_12_MOT1_ANOUT1, BD_AN_6_L_KNEE_ENC, DIO_3_VALVE2_0);

  joint_initFC(l_knee_joint, EXO_FC_ALPHA);
}

void setupTrajectories(volatile exo_t *exo) {
  /* setup the trajectory coefficients */
  tr_computeHipTrajCoeffScaled((tr_coeff_t *) &exo->hip_traj_coeff,
      exo->min_hip_angle, exo->max_hip_angle, EXO_TF, EXO_TPS_PERC);

  tr_computePreSwingKneeTrajCoeff((tr_coeff_t *) &exo->knee_ps_coeff,
      exo->max_knee_angle, exo->min_hip_angle);

  tr_computeLateSwingKneeTrajCoeff((tr_coeff_t *) &exo->knee_ls_coeff,
      exo->max_knee_angle, exo->max_hip_angle);

  tr_genKneeLookupTable(
      (tr_coeff_t *) &exo->knee_ps_coeff,
      (tr_coeff_t *) &exo->knee_ls_coeff,
      (float *) exo->knee_traj_lut,
      EXO_TRAJ_LOOKUP_SIZE,
      BD_HIP_ANGLE_SLOPE,
      exo->min_hip_angle,
      exo->max_hip_angle);
}

void setupBoard() {
  /* Enable global interrupts. */
  CyGlobalIntEnable;

  /* Place your initialization/startup code here (e.g. MyInst_Start()) */
  /* Initialize the serial communications interface */
  /*
  USBUART_1_Start(0, USBUART_1_3V_OPERATION);//!!NOTE!! Make sure this matches your board voltage!
  while(!USBUART_1_bGetConfiguration());
  USBUART_1_CDC_Init();
  */
  AMuxSeq_1_Init();
  ADC_SAR_1_Start(); //initialize the adc
  Counter_1_Start();

  bd_initCAN();
  isr_1_StartEx(timerInterruptHandler); //register the interrupt handler
}

void setupStim(volatile exo_t *exo) {
  stim_pattern_t *const pattern_brd1  = (stim_pattern_t *) &exo->pattern_brd1;
  stim_pattern_t *const pattern_brd2  = (stim_pattern_t *) &exo->pattern_brd2;
  stim_pattern_t *const pattern_brd3  = (stim_pattern_t *) &exo->pattern_brd3;
  stim_pattern_t *const configurable_pattern  = (stim_pattern_t *) &exo->configurable_pattern;

  cwru_stim_struct_t *const cwru_stim_brd1  = (cwru_stim_struct_t *) &exo->cwru_stim_brd1;
  cwru_stim_struct_t *const cwru_stim_brd2  = (cwru_stim_struct_t *) &exo->cwru_stim_brd2;
  cwru_stim_struct_t *const cwru_stim_brd3  = (cwru_stim_struct_t *) &exo->cwru_stim_brd3;

  stimpat_initPattern(pattern_brd1,
      &configurable_pattern1_PP,
      &configurable_pattern1_PW,
      configurable_pattern1_duration,
      1000);
  const uint8_t STIM_BOARD_IPI = 30;

  stimint_initBoardUART(cwru_stim_brd1, STIM_UART_PORT_0); //SCB J115
  STIM_SETUP_FUNC(cwru_stim_brd1, STIM_BOARD_IPI);

  stimpat_initPattern(pattern_brd2,
      &gait_stand_B2_PP,
      &gait_stand_B2_PW,
      gait_stand_duration,
      1000);

  stimint_initBoardUART(cwru_stim_brd2, STIM_UART_PORT_1); //SCB J116
  STIM_SETUP_FUNC(cwru_stim_brd2, STIM_BOARD_IPI);

#if defined(STIM_CONFIG_3RD_SURFACE)
  stimpat_initPattern(pattern_brd3,
      &gait_stand_B2_PP,
      &gait_stand_B2_PW,
      gait_stand_duration,
      1000);

  stimint_initBoardUART(cwru_stim_brd3, STIM_UART_PORT_2); //SCB J116
  stimint_initPercBoard(cwru_stim_brd3, STIM_BOARD_IPI); //TODO: setup as surface board
#endif
}

