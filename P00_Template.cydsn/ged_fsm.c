#include <ged_fsm.h>
#include <board_io.h>
#include <joint.h>
#include <exo.h>
#include <board_can.h>
#include <gait_zero_pattern.h>

#define KNEE_ANGLE_EXTENSION_LIMIT 15
/*----------------------------------------------------------------------------*/
/*                    GENERAL API IMPLEMENTATION                              */
/*----------------------------------------------------------------------------*/
void ged_runState(const ged_state_machine_t *state_machine, void *v_exo) {
  (*state_machine->state.run_func)(v_exo);
}

void ged_checkTransitions(ged_state_machine_t *state_machine, void *v_exo) {
  (*state_machine->state.checktrans_func)(state_machine, v_exo);
  ged_defaultTrans(state_machine, v_exo);
}

void ged_transition(ged_state_machine_t *state_machine, ged_state_t state, void
    *v_exo) {
  state_machine->state = state;
  (*state_machine->state.onentry_func)(v_exo);
}

ged_state_id_t ged_getStateID(ged_state_machine_t *state_machine) {
  return state_machine->state.id;
}

ged_state_machine_t *ged_create() {
  return malloc(sizeof(ged_state_machine_t));
}

static void ged_sendStateMsg(uint8_t state) {
  pkt_generic_t state_message;
  pkt_init(&state_message);
  pkt_setHeader(&state_message, PKT_SET_STATE, sizeof(pkt_set_state_t));
  pkt_set_state_t *set_state_payload = pkt_interp(pkt_set_state_t,
      state_message);

  set_state_payload->state = state;

  /*
  bd_writeCAN(pkt_encodeBuffer(&state_message),
      pkt_getTotalLength(&state_message));
      */
}

/*----------------------------------------------------------------------------*/
/*                    STATE FUNCTION AND TYPE PROTOTYPES                      */
/*----------------------------------------------------------------------------*/

static void ged_generic_LockJointsOnEntryCANSend(void *v_exo);
static void ged_generic_LockJointsOnEntry(void *v_exo);
static void ged_generic_unlockJointsOnEntry(void *v_exo);
static void ged_generic_nopRun(void *v_exo);
static void ged_generic_nopCheckTrans(ged_state_machine_t *state_machine, void *v_exo);

/* see documentation in hybrid-module on how to add new states */
/* HALF LEFT STEP*/
/*----------------------------------------------------------------------------*/

static void ged_leftSwingOnEntry(void *v_exo);
static void ged_halfLeftStepRun(void *v_exo);
static void ged_halfLeftStepCheckTrans(ged_state_machine_t *state_machine, void *v_exo);
static ged_state_t ged_half_left_late_swing;

static ged_state_t ged_half_left_step = {
  .id = GED_HALF_LEFT_STEP,
  .onentry_func = ged_generic_LockJointsOnEntry,
  .checktrans_func = ged_halfLeftStepCheckTrans,
  .run_func = ged_halfLeftStepRun
};

static void ged_halfLeftStepRun(void *v_exo) {
  exo_t *exo = (exo_t *) v_exo;

}

void ged_halfLeftStepCheckTrans(ged_state_machine_t *state_machine, void *v_exo) {
}

/* HALF LEFT LATE SWING */
/*----------------------------------------------------------------------------*/

static void ged_halfLeftLateSwingRun(void *v_exo);
static void ged_halfLeftLateSwingCheckTrans(ged_state_machine_t *state_machine, void *v_exo);

static ged_state_t ged_half_left_late_swing = {
  .id = GED_HALF_LEFT_LATE_SWING,
  .onentry_func = ged_generic_LockJointsOnEntry,
  .checktrans_func = ged_halfLeftLateSwingCheckTrans,
  .run_func = ged_halfLeftLateSwingRun
};

static void ged_halfLeftLateSwingRun(void *v_exo) {
  exo_t *exo = (exo_t *) v_exo;

}

void ged_halfLeftLateSwingCheckTrans(ged_state_machine_t *state_machine, void *v_exo) {
}

/* SIT TO STAND */
/*----------------------------------------------------------------------------*/

static void ged_sitToStandRun(void *v_exo);
static void ged_sitToStandOnEntry(void *v_exo);

static ged_state_t ged_sit_to_stand = {
  .id = GED_SIT_TO_STAND,
  .onentry_func = ged_sitToStandOnEntry,
  .checktrans_func = ged_generic_nopCheckTrans,
  .run_func = ged_sitToStandRun
};

static void ged_sitToStandRun(void *v_exo) {
  exo_t *exo = (exo_t *) v_exo;

}

static void ged_sitToStandOnEntry(void *v_exo) {
  exo_t *exo = (exo_t *) v_exo;
  stim_pattern_t *const pattern_brd1  = &exo->pattern_brd1;
  stim_pattern_t *const pattern_brd2  = &exo->pattern_brd2;

  stimpat_initPattern(pattern_brd1,
  &gait_stand_B1_PP,
  &gait_stand_B2_PW,
  gait_stand_duration,
  1000);
  stimpat_activatePattern(pattern_brd1);

  stimpat_initPattern(pattern_brd2,
  &gait_stand_B2_PP,
  &gait_stand_B2_PW,
  gait_stand_duration,
  1000);
  stimpat_activatePattern(pattern_brd2);
}

/* STAND TO SIT */
/*----------------------------------------------------------------------------*/

static void ged_standToSitRun(void *v_exo);
static void ged_standToSitOnEntry(void *v_exo);

static ged_state_t ged_stand_to_sit = {
  .id = GED_STAND_TO_SIT,
  .onentry_func = ged_standToSitOnEntry,
  .checktrans_func = ged_generic_nopCheckTrans,
  .run_func = ged_standToSitRun
};

static void ged_standToSitRun(void *v_exo) {
  exo_t *exo = (exo_t *) v_exo;

}

static void ged_standToSitOnEntry(void *v_exo) {
  exo_t *exo = (exo_t *) v_exo;
  stim_pattern_t *const pattern_brd1  = &exo->pattern_brd1;
  stim_pattern_t *const pattern_brd2  = &exo->pattern_brd2;

  stimpat_initPattern(pattern_brd1,
  &gait_sit_B1_PP,
  &gait_sit_B1_PW,
  gait_sit_duration,
  1000);
  stimpat_activatePattern(pattern_brd1);

  stimpat_initPattern(pattern_brd2,
  &gait_sit_B2_PP,
  &gait_sit_B2_PW,
  gait_sit_duration,
  1000);
  stimpat_activatePattern(pattern_brd2);
}

/* UNLOCKED */
/*----------------------------------------------------------------------------*/
static void ged_unlockedRun(void *v_exo);

static ged_state_t ged_unlocked = {
  .id = GED_UNLOCKED,
  .onentry_func = ged_generic_unlockJointsOnEntry,
  .checktrans_func = ged_generic_nopCheckTrans,
  .run_func = ged_generic_nopRun,
};

/* LOCKED */
/*----------------------------------------------------------------------------*/
static void ged_lockedCheckTrans(ged_state_machine_t *state_machine, void
    *exo);

static ged_state_t ged_locked = {
  .id = GED_LOCKED,
  .onentry_func = ged_generic_LockJointsOnEntryCANSend,
  .checktrans_func = ged_lockedCheckTrans,
  .run_func = ged_generic_nopRun,
};



static ged_state_t ged_locked_no_send = {
  .id = GED_LOCKED,
  .onentry_func = ged_generic_LockJointsOnEntry,
  .checktrans_func = ged_lockedCheckTrans,
  .run_func = ged_generic_nopRun,
};


/* PASSIVE */
/*----------------------------------------------------------------------------*/
static ged_state_t ged_passive = {
  .id = GED_PASSIVE,
  .onentry_func = ged_generic_LockJointsOnEntry,
  .checktrans_func = ged_lockedCheckTrans,
  .run_func = ged_generic_nopRun,
};

/* LEFT SWING */
/*----------------------------------------------------------------------------*/
static void ged_leftSwingOnEntry(void *v_exo);
static void ged_leftSwingRun(void *v_exo);
static void ged_leftSwingCheckTrans(ged_state_machine_t *state_machine, void
    *exo);

static ged_state_t ged_leftSwing = {
  .id = GED_LEFT_SWING,
  .onentry_func = ged_leftSwingOnEntry,
  .checktrans_func = ged_leftSwingCheckTrans,
  .run_func = ged_generic_nopRun,
};

/* RIGHT SWING */
/*----------------------------------------------------------------------------*/
static void ged_rightSwingOnEntry(void *v_exo);
static void ged_rightSwingRun(void *v_exo);
static void ged_rightSwingCheckTrans(ged_state_machine_t *state_machine, void
    *exo);

static ged_state_t ged_rightSwing = {
  .id = GED_RIGHT_SWING,
  .onentry_func = ged_rightSwingOnEntry,
  .checktrans_func = ged_rightSwingCheckTrans,
  .run_func = ged_generic_nopRun,
};


/* LEFT DBL STANCE */
/*----------------------------------------------------------------------------*/
void ged_leftDblStanceCheckTrans(ged_state_machine_t *state_machine, void *v_exo);

static ged_state_t ged_leftDblStance = {
  .id = GED_LEFT_DBL_STANCE,
  .onentry_func = ged_generic_LockJointsOnEntry,
  .checktrans_func = ged_leftDblStanceCheckTrans,
  .run_func = ged_generic_nopRun,
};

/* RIGHT DBL STANCE */
/*----------------------------------------------------------------------------*/
void ged_rightDblStanceCheckTrans(ged_state_machine_t *state_machine, void *v_exo);

static ged_state_t ged_rightDblStance = {
  .id = GED_RIGHT_DBL_STANCE,
  .onentry_func = ged_generic_LockJointsOnEntry,
  .checktrans_func = ged_rightDblStanceCheckTrans,
  .run_func = ged_generic_nopRun,
};

/*----------------------------------------------------------------------------*/
/*                             STATE IMPLEMENTATION                           */
/*----------------------------------------------------------------------------*/

void ged_init(ged_state_machine_t *state_machine, void *v_exo) {
  ged_transition(state_machine, ged_locked_no_send, v_exo);
}

void ged_defaultTrans(ged_state_machine_t *state_machine, void *v_exo) {
  exo_t *exo = (exo_t *) v_exo;
  if (bd_getFSW(&exo->sensor_inputs, BD_BLUE_FSW)) {
    ged_transition(state_machine, ged_unlocked, exo);
    ged_sendStateMsg(GED_UNLOCKED);
  }
  else if (bd_getFSW(&exo->sensor_inputs, BD_RED_FSW)) {
    ged_transition(state_machine, ged_locked, exo);
    ged_sendStateMsg(GED_LOCKED);
  }
}

void ged_forceTrans(ged_state_machine_t *state_machine, void *v_exo,
    ged_state_id_t state_id) {

  exo_t *exo = (exo_t *) v_exo;

  switch(state_id) {
    case GED_UNLOCKED:
      ged_transition(state_machine, ged_unlocked, exo);
      break;

    case GED_LOCKED:
      ged_transition(state_machine, ged_locked, exo);
      break;

    case GED_LEFT_SWING:
      ged_transition(state_machine, ged_leftSwing, exo);
      break;

    case GED_RIGHT_SWING:
      ged_transition(state_machine, ged_rightSwing, exo);
      break;

    case GED_LEFT_DBL_STANCE:
      ged_transition(state_machine, ged_leftDblStance, exo);
      break;

    case GED_RIGHT_DBL_STANCE:
      ged_transition(state_machine, ged_rightDblStance, exo);
      break;

    case GED_PASSIVE:
      ged_transition(state_machine, ged_passive, exo);
      break;

    case GED_SIT_TO_STAND:
      ged_transition(state_machine, ged_sit_to_stand, exo);
      break;

    case GED_STAND_TO_SIT:
      ged_transition(state_machine, ged_stand_to_sit, exo);
      break;

    case GED_HALF_LEFT_STEP:
      ged_transition(state_machine, ged_half_left_step, exo);
      break;

    default:
      break;
  }
}
/*----------------------------------------------------------------------------*/
/*                     GENERIC STATE IMPLEMENTATION                           */
/*----------------------------------------------------------------------------*/

void ged_generic_LockJointsOnEntry(void *v_exo) {
  exo_t *exo = (exo_t *) v_exo;
  stim_pattern_t *const pattern_brd1  = &exo->pattern_brd1;
  stim_pattern_t *const pattern_brd2  = &exo->pattern_brd2;

  stimpat_initPattern(pattern_brd1,
  &gait_zero_PP,
  &gait_zero_PW,
  1.0,
  1000);
  stimpat_activatePattern(pattern_brd1);

  stimpat_initPattern(pattern_brd2,
  &gait_zero_PP,
  &gait_zero_PW,
  1.0,
  1000);
  stimpat_activatePattern(pattern_brd2);


}

void ged_generic_LockJointsOnEntryCANSend(void *v_exo) {
  ged_generic_LockJointsOnEntry(v_exo);
  ged_sendStateMsg(GED_LOCKED);
}

void ged_generic_unlockJointsOnEntry(void *v_exo) {
  exo_t *exo = (exo_t *) v_exo;

  ged_sendStateMsg(GED_UNLOCKED);
}

void ged_generic_nopRun(void *v_exo) {
}

void ged_generic_nopCheckTrans(ged_state_machine_t *state_machine, void *v_exo) {
}

/*----------------------------------------------------------------------------*/
/*                             UNLOCKED                                       */
/*----------------------------------------------------------------------------*/

void ged_unlockedRun(void *v_exo) { //run the friction compensator
  exo_t *exo = (exo_t *) v_exo;
}

/*----------------------------------------------------------------------------*/
/*                                LOCKED                                      */
/*----------------------------------------------------------------------------*/

void ged_lockedCheckTrans(ged_state_machine_t *state_machine, void *v_exo) {
  exo_t *exo = (exo_t *) v_exo;
  if (bd_getFSW(&exo->sensor_inputs, BD_GREEN_FSW))
    ged_transition(state_machine, ged_leftSwing, exo);
}

/*----------------------------------------------------------------------------*/
/*                                L SWING                                      */
/*----------------------------------------------------------------------------*/

void ged_leftSwingOnEntry(void *v_exo) {
  exo_t *exo = (exo_t *) v_exo;
  stim_pattern_t *const pattern_brd1  = &exo->pattern_brd1;
  stim_pattern_t *const pattern_brd2  = &exo->pattern_brd2;

  stimpat_initPattern(pattern_brd1,
  &gait_walk_L_B1_PP,
  &gait_walk_L_B1_PW,
  gait_walk_L_duration,
  1000);
  stimpat_activatePattern(pattern_brd1);

  stimpat_initPattern(pattern_brd2,
  &gait_walk_L_B2_PP,
  &gait_walk_L_B2_PW,
  gait_walk_L_duration,
  1000);
  stimpat_activatePattern(pattern_brd2);



}

void ged_leftSwingRun(void *v_exo) {
  exo_t *exo = (exo_t *) v_exo;
  bd_inputs_t *const sensor_inputs = &exo->sensor_inputs;

}

void ged_leftSwingCheckTrans(ged_state_machine_t *state_machine, void *v_exo) {
  exo_t *exo = (exo_t *) v_exo;

  const float l_hip_angle = bd_analogToLHipAngle(bd_getAin(&exo->sensor_inputs,
        BD_AN_4_L_HIP_ENC), exo->l_hip_zero_offset);

  if (bd_getFSW(&exo->sensor_inputs, BD_GREEN_FSW))
    ged_transition(state_machine, ged_leftDblStance, exo);

  if ((exo->l_knee_joint.position < exo->l_knee_extension_thresh) &&
      (l_hip_angle > exo->l_hip_flexion_thresh)) {
    if (bd_getAin(&exo->sensor_inputs, BD_AN_0_L_FSR_HEEL) >
        exo->l_weight_acc_fsr_thresh) {
      ged_transition(state_machine, ged_leftDblStance, exo);
    }
  }
}

/*----------------------------------------------------------------------------*/
/*                                R SWING                                      */
/*----------------------------------------------------------------------------*/

void ged_rightSwingOnEntry(void *v_exo) {
  exo_t *exo = (exo_t *) v_exo;

  stim_pattern_t *const pattern_brd1  = &exo->pattern_brd1;
  stim_pattern_t *const pattern_brd2  = &exo->pattern_brd2;

  stimpat_initPattern(pattern_brd1,
  &gait_walk_R_B1_PP,
  &gait_walk_R_B1_PW,
  gait_walk_R_duration,
  1000);
  stimpat_activatePattern(pattern_brd1);

  stimpat_initPattern(pattern_brd2,
  &gait_walk_R_B2_PP,
  &gait_walk_R_B2_PW,
  gait_walk_R_duration,
  1000);
  stimpat_activatePattern(pattern_brd2);
}

void ged_rightSwingRun(void *v_exo) {
  exo_t *exo = (exo_t *) v_exo;
  bd_inputs_t *const sensor_inputs = &exo->sensor_inputs;
}

void ged_rightSwingCheckTrans(ged_state_machine_t *state_machine, void *v_exo) {
  exo_t *exo = (exo_t *) v_exo;

  const float r_hip_angle = bd_analogToRHipAngle(bd_getAin(&exo->sensor_inputs,
        BD_AN_5_R_HIP_ENC), exo->r_hip_zero_offset);

  if (bd_getFSW(&exo->sensor_inputs, BD_GREEN_FSW))
    ged_transition(state_machine, ged_rightDblStance, exo);

  if ((exo->r_knee_joint.position < exo->r_knee_extension_thresh) &&
      (r_hip_angle > exo->r_hip_flexion_thresh)) {
    if (bd_getAin(&exo->sensor_inputs, BD_AN_2_R_FSR_HEEL) >
        exo->r_weight_acc_fsr_thresh) {
      ged_transition(state_machine, ged_rightDblStance, exo);
    }
  }
}

/*----------------------------------------------------------------------------*/
/*                                L DBL STANCE                                */
/*----------------------------------------------------------------------------*/

void ged_leftDblStanceCheckTrans(ged_state_machine_t *state_machine, void *v_exo) {
  exo_t *exo = (exo_t *) v_exo;
  if (bd_getFSW(&exo->sensor_inputs, BD_GREEN_FSW))
    ged_transition(state_machine, ged_rightSwing, exo);

  if (exo->auto_walk &&
      (bd_getAin(&exo->sensor_inputs, BD_AN_0_L_FSR_HEEL) > exo->l_high_fsr_thresh) &&
      (bd_getAin(&exo->sensor_inputs, BD_AN_2_R_FSR_HEEL) < exo->r_low_fsr_thresh))
    ged_transition(state_machine, ged_rightSwing, exo);
}

/*----------------------------------------------------------------------------*/
/*                                R DBL STANCE                                */
/*----------------------------------------------------------------------------*/


void ged_rightDblStanceCheckTrans(ged_state_machine_t *state_machine, void *v_exo) {
  exo_t *exo = (exo_t *) v_exo;
  if (bd_getFSW(&exo->sensor_inputs, BD_GREEN_FSW))
    ged_transition(state_machine, ged_leftSwing, exo);

  if (exo->auto_walk &&
      (bd_getAin(&exo->sensor_inputs, BD_AN_2_R_FSR_HEEL) > exo->r_high_fsr_thresh) &&
      (bd_getAin(&exo->sensor_inputs, BD_AN_0_L_FSR_HEEL) < exo->l_low_fsr_thresh))
    ged_transition(state_machine, ged_leftSwing, exo);
}

