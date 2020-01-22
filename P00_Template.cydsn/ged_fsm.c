#include <ged_fsm.h>
#include <board_io.h>
#include <stim_module.h>
#include <board_can.h>
#include <BAL06_gait.h>
#include <gait_zero_pattern.h>

#define KNEE_ANGLE_EXTENSION_LIMIT 15
/*----------------------------------------------------------------------------*/
/*                    GENERAL API IMPLEMENTATION                              */
/*----------------------------------------------------------------------------*/
void ged_runState(const ged_state_machine_t *state_machine, void *v_stimulator) {
  (*state_machine->state.run_func)(v_stimulator);
}

void ged_checkTransitions(ged_state_machine_t *state_machine, void *v_stimulator) {
  (*state_machine->state.checktrans_func)(state_machine, v_stimulator);
  ged_defaultTrans(state_machine, v_stimulator);
}

void ged_transition(ged_state_machine_t *state_machine, ged_state_t state, void
    *v_stimulator) {
  state_machine->state = state;
  (*state_machine->state.onentry_func)(v_stimulator);
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

static void ged_generic_LockJointsOnEntryCANSend(void *v_stimulator);
static void ged_generic_LockJointsOnEntry(void *v_stimulator);
static void ged_generic_unlockJointsOnEntry(void *v_stimulator);
static void ged_generic_nopRun(void *v_stimulator);
static void ged_generic_nopCheckTrans(ged_state_machine_t *state_machine, void *v_stimulator);

/* see documentation in hybrid-module on how to add new states */
/* RIGHT WEIGHT ACCEPTANCE */
/*----------------------------------------------------------------------------*/

static void ged_rightWeightAcceptOnEntry(void *v_stimulator);
static void ged_rightWeightAcceptRun(void *v_stimulator);
static void ged_rightWeightAcceptCheckTrans(ged_state_machine_t *state_machine, void *v_stimulator);

static ged_state_t ged_right_weight_accept = {
  .id = GED_RIGHT_WEIGHT_ACCEPTANCE,
  .onentry_func = ged_rightWeightAcceptOnEntry,
  .checktrans_func = ged_rightWeightAcceptCheckTrans,
  .run_func = ged_rightWeightAcceptRun
};

static void ged_rightWeightAcceptRun(void *v_stimulator) {

}

void ged_rightWeightAcceptCheckTrans(ged_state_machine_t *state_machine, void *v_stimulator) {
}

static void ged_rightWeightAcceptOnEntry(void *v_stimulator) {
  //stim_module_t *stimulator = (stim_module_t *) v_stimulator;
  //stim_pattern_t *const pattern_brd1  = &stimulator->pattern_brd1;
  //stim_pattern_t *const pattern_brd2  = &stimulator->pattern_brd2;

  /*
  stimpat_initPattern(pattern_brd1,
  &gait_misc1_B1_PP,
  &gait_misc1_B1_PW,
  gait_sit_duration,
  1000);
  stimpat_activatePattern(pattern_brd1);
  */

  /*
  stimpat_initPattern(pattern_brd2,
  &gait_misc1_B2_PP,
  &gait_misc1_B2_PW,
  gait_sit_duration,
  1000);
  stimpat_activatePattern(pattern_brd2);
  */
}

/* LEFT WEIGHT ACCEPTANCE */
/*----------------------------------------------------------------------------*/

static void ged_leftWeightAcceptOnEntry(void *v_stimulator);
static void ged_leftWeightAcceptRun(void *v_stimulator);
static void ged_leftWeightAcceptCheckTrans(ged_state_machine_t *state_machine, void *v_stimulator);

static ged_state_t ged_left_weight_accept = {
  .id = GED_LEFT_WEIGHT_ACCEPTANCE,
  .onentry_func = ged_leftWeightAcceptOnEntry,
  .checktrans_func = ged_leftWeightAcceptCheckTrans,
  .run_func = ged_leftWeightAcceptRun
};

static void ged_leftWeightAcceptRun(void *v_stimulator) {

}

void ged_leftWeightAcceptCheckTrans(ged_state_machine_t *state_machine, void *v_stimulator) {
}

static void ged_leftWeightAcceptOnEntry(void *v_stimulator) {
  //stim_module_t *stimulator = (stim_module_t *) v_stimulator;
  //stim_pattern_t *const pattern_brd1  = &stimulator->pattern_brd1;
  //stim_pattern_t *const pattern_brd2  = &stimulator->pattern_brd2;

  /*
  stimpat_initPattern(pattern_brd1,
  &gait_misc2_B1_PP,
  &gait_misc2_B1_PW,
  gait_sit_duration,
  1000);
  stimpat_activatePattern(pattern_brd1);
  */

  /*
  stimpat_initPattern(pattern_brd2,
  &gait_misc2_B2_PP,
  &gait_misc2_B2_PW,
  gait_sit_duration,
  1000);
  stimpat_activatePattern(pattern_brd2);
  */
}

/* HALF LEFT STEP*/
/*----------------------------------------------------------------------------*/

static void ged_halfLeftStepOnEntry(void *v_stimulator);
static void ged_halfLeftStepRun(void *v_stimulator);
static void ged_halfLeftStepCheckTrans(ged_state_machine_t *state_machine, void *v_stimulator);
static ged_state_t ged_half_left_late_swing;

static ged_state_t ged_half_left_step = {
  .id = GED_HALF_LEFT_STEP,
  .onentry_func = ged_halfLeftStepOnEntry,
  .checktrans_func = ged_halfLeftStepCheckTrans,
  .run_func = ged_halfLeftStepRun
};

static void ged_halfLeftStepOnEntry(void *v_stimulator) {
  stim_module_t *stimulator = (stim_module_t *) v_stimulator;
  stim_pattern_t *const pattern_brd1  = &stimulator->pattern_brd1;
  //stim_pattern_t *const pattern_brd2  = &stimulator->pattern_brd2;

  stimpat_initPattern(pattern_brd1,
  &gait_walk_B1_L_PP,
  &gait_walk_B1_L_PW,
  gait_walk_L_duration,
  1000);
  stimpat_activatePattern(pattern_brd1);

  /*
  stimpat_initPattern(pattern_brd2,
  &gait_walk_L_B2_PP,
  &gait_walk_L_B2_PW,
  gait_walk_L_duration,
  1000);
  stimpat_activatePattern(pattern_brd2);
  */
}

static void ged_halfLeftStepRun(void *v_stimulator) {

}

void ged_halfLeftStepCheckTrans(ged_state_machine_t *state_machine, void *v_stimulator) {
}

/* HALF LEFT LATE SWING */
/*----------------------------------------------------------------------------*/

static void ged_halfLeftLateSwingOnEntry(void *v_stimulator);
static void ged_halfLeftLateSwingRun(void *v_stimulator);
static void ged_halfLeftLateSwingCheckTrans(ged_state_machine_t *state_machine, void *v_stimulator);

static ged_state_t ged_half_left_late_swing = {
  .id = GED_HALF_LEFT_LATE_SWING,
  .onentry_func = ged_halfLeftLateSwingOnEntry,
  .checktrans_func = ged_halfLeftLateSwingCheckTrans,
  .run_func = ged_halfLeftLateSwingRun
};

static void ged_halfLeftLateSwingOnEntry(void *v_stimulator) {
}

static void ged_halfLeftLateSwingRun(void *v_stimulator) {

}

void ged_halfLeftLateSwingCheckTrans(ged_state_machine_t *state_machine, void *v_stimulator) {
}

/* SIT TO STAND */
/*----------------------------------------------------------------------------*/

static void ged_sitToStandRun(void *v_stimulator);
static void ged_sitToStandOnEntry(void *v_stimulator);

static ged_state_t ged_sit_to_stand = {
  .id = GED_SIT_TO_STAND,
  .onentry_func = ged_sitToStandOnEntry,
  .checktrans_func = ged_generic_nopCheckTrans,
  .run_func = ged_sitToStandRun
};

static void ged_sitToStandRun(void *v_stimulator) {

}

static void ged_sitToStandOnEntry(void *v_stimulator) {
  stim_module_t *stimulator = (stim_module_t *) v_stimulator;
  stim_pattern_t *const pattern_brd1  = &stimulator->pattern_brd1;
  //stim_pattern_t *const pattern_brd2  = &stimulator->pattern_brd2;

  stimpat_initPattern(pattern_brd1,
  &gait_stand_B1_PP,
  &gait_stand_B1_PW,
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

static void ged_standToSitRun(void *v_stimulator);
static void ged_standToSitOnEntry(void *v_stimulator);

static ged_state_t ged_stand_to_sit = {
  .id = GED_STAND_TO_SIT,
  .onentry_func = ged_standToSitOnEntry,
  .checktrans_func = ged_generic_nopCheckTrans,
  .run_func = ged_standToSitRun
};

static void ged_standToSitRun(void *v_stimulator) {

}

static void ged_standToSitOnEntry(void *v_stimulator) {
  stim_module_t *stimulator = (stim_module_t *) v_stimulator;
  stim_pattern_t *const pattern_brd1  = &stimulator->pattern_brd1;
  stim_pattern_t *const pattern_brd2  = &stimulator->pattern_brd2;

  stimpat_initPattern(pattern_brd1,
  &gait_zero_PP,
  &gait_zero_PW,
  gait_sit_duration,
  1000);
  stimpat_activatePattern(pattern_brd1);

  stimpat_initPattern(pattern_brd2,
  &gait_zero_PP,
  &gait_zero_PW,
  gait_sit_duration,
  1000);
  stimpat_activatePattern(pattern_brd2);
}

/* UNLOCKED */
/*----------------------------------------------------------------------------*/
static ged_state_t ged_unlocked = {
  .id = GED_UNLOCKED,
  .onentry_func = ged_generic_unlockJointsOnEntry,
  .checktrans_func = ged_generic_nopCheckTrans,
  .run_func = ged_generic_nopRun,
};

/* LOCKED */
/*----------------------------------------------------------------------------*/
static void ged_lockedCheckTrans(ged_state_machine_t *state_machine, void
    *stimulator);

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
static void ged_leftSwingOnEntry(void *v_stimulator);
static void ged_leftSwingCheckTrans(ged_state_machine_t *state_machine, void
    *stimulator);

static ged_state_t ged_leftSwing = {
  .id = GED_LEFT_SWING,
  .onentry_func = ged_leftSwingOnEntry,
  .checktrans_func = ged_leftSwingCheckTrans,
  .run_func = ged_generic_nopRun,
};

/* RIGHT SWING */
/*----------------------------------------------------------------------------*/
static void ged_rightSwingOnEntry(void *v_stimulator);
static void ged_rightSwingCheckTrans(ged_state_machine_t *state_machine, void
    *stimulator);

static ged_state_t ged_rightSwing = {
  .id = GED_RIGHT_SWING,
  .onentry_func = ged_rightSwingOnEntry,
  .checktrans_func = ged_rightSwingCheckTrans,
  .run_func = ged_generic_nopRun,
};


/* LEFT DBL STANCE */
/*----------------------------------------------------------------------------*/
void ged_leftDblStanceCheckTrans(ged_state_machine_t *state_machine, void *v_stimulator);

static ged_state_t ged_leftDblStance = {
  .id = GED_LEFT_DBL_STANCE,
  .onentry_func = ged_generic_LockJointsOnEntry,
  .checktrans_func = ged_leftDblStanceCheckTrans,
  .run_func = ged_generic_nopRun,
};

/* RIGHT DBL STANCE */
/*----------------------------------------------------------------------------*/
void ged_rightDblStanceCheckTrans(ged_state_machine_t *state_machine, void *v_stimulator);

static ged_state_t ged_rightDblStance = {
  .id = GED_RIGHT_DBL_STANCE,
  .onentry_func = ged_generic_LockJointsOnEntry,
  .checktrans_func = ged_rightDblStanceCheckTrans,
  .run_func = ged_generic_nopRun,
};

/*----------------------------------------------------------------------------*/
/*                             STATE IMPLEMENTATION                           */
/*----------------------------------------------------------------------------*/

void ged_init(ged_state_machine_t *state_machine, void *v_stimulator) {
  ged_transition(state_machine, ged_locked_no_send, v_stimulator);
}

void ged_defaultTrans(ged_state_machine_t *state_machine, void *v_stimulator) {
  stim_module_t *stimulator = (stim_module_t *) v_stimulator;
  if (bd_getFSW(&stimulator->sensor_inputs, BD_BLUE_FSW)) {
    ged_transition(state_machine, ged_unlocked, stimulator);
    ged_sendStateMsg(GED_UNLOCKED);
  }
  else if (bd_getFSW(&stimulator->sensor_inputs, BD_RED_FSW)) {
    ged_transition(state_machine, ged_locked, stimulator);
    ged_sendStateMsg(GED_LOCKED);
  }
}

void ged_forceTrans(ged_state_machine_t *state_machine, void *v_stimulator,
    ged_state_id_t state_id) {

  stim_module_t *stimulator = (stim_module_t *) v_stimulator;
  ged_state_id_t current_state = ged_getStateID((ged_state_machine_t *) &stimulator->state_machine);

  if (state_id == current_state)
    return; //do nothing

  if (state_id >= GED_STATE_MAX) //if it's an invalid state
    return; //do nothing

  switch(state_id) {
    case GED_UNLOCKED:
      ged_transition(state_machine, ged_unlocked, stimulator);
      break;

    case GED_LOCKED:
      ged_transition(state_machine, ged_locked, stimulator);
      break;

    case GED_LEFT_SWING:
      ged_transition(state_machine, ged_leftSwing, stimulator);
      break;

    case GED_RIGHT_SWING:
      ged_transition(state_machine, ged_rightSwing, stimulator);
      break;

    case GED_LEFT_DBL_STANCE:
      ged_transition(state_machine, ged_leftDblStance, stimulator);
      break;

    case GED_RIGHT_DBL_STANCE:
      ged_transition(state_machine, ged_rightDblStance, stimulator);
      break;

    case GED_PASSIVE:
      ged_transition(state_machine, ged_passive, stimulator);
      break;

    case GED_SIT_TO_STAND:
      ged_transition(state_machine, ged_sit_to_stand, stimulator);
      break;

    case GED_STAND_TO_SIT:
      ged_transition(state_machine, ged_stand_to_sit, stimulator);
      break;

    case GED_HALF_LEFT_STEP:
      ged_transition(state_machine, ged_half_left_step, stimulator);
      break;

    case GED_HALF_LEFT_LATE_SWING:
      ged_transition(state_machine, ged_half_left_late_swing, stimulator);
      break;

    case GED_LEFT_WEIGHT_ACCEPTANCE:
      ged_transition(state_machine, ged_left_weight_accept, stimulator);
      break;

    case GED_RIGHT_WEIGHT_ACCEPTANCE:
      ged_transition(state_machine, ged_right_weight_accept, stimulator);
      break;

    default:
      break;
  }
}
/*----------------------------------------------------------------------------*/
/*                     GENERIC STATE IMPLEMENTATION                           */
/*----------------------------------------------------------------------------*/

void ged_generic_LockJointsOnEntry(void *v_stimulator) {
  stim_module_t *stimulator = (stim_module_t *) v_stimulator;
  stim_pattern_t *const pattern_brd1  = &stimulator->pattern_brd1;
  stim_pattern_t *const pattern_brd2  = &stimulator->pattern_brd2;

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

void ged_generic_LockJointsOnEntryCANSend(void *v_stimulator) {
  ged_generic_LockJointsOnEntry(v_stimulator);
  ged_sendStateMsg(GED_LOCKED);
}

void ged_generic_unlockJointsOnEntry(void *v_stimulator) {

  ged_sendStateMsg(GED_UNLOCKED);
}

void ged_generic_nopRun(void *v_stimulator) {
}

void ged_generic_nopCheckTrans(ged_state_machine_t *state_machine, void *v_stimulator) {
}


/*----------------------------------------------------------------------------*/
/*                                LOCKED                                      */
/*----------------------------------------------------------------------------*/

void ged_lockedCheckTrans(ged_state_machine_t *state_machine, void *v_stimulator) {
  stim_module_t *stimulator = (stim_module_t *) v_stimulator;
  if (bd_getFSW(&stimulator->sensor_inputs, BD_GREEN_FSW))
    ged_transition(state_machine, ged_leftSwing, stimulator);
}

/*----------------------------------------------------------------------------*/
/*                                L SWING                                      */
/*----------------------------------------------------------------------------*/

void ged_leftSwingOnEntry(void *v_stimulator) {
  stim_module_t *stimulator = (stim_module_t *) v_stimulator;
  stim_pattern_t *const pattern_brd1  = &stimulator->pattern_brd1;
  //stim_pattern_t *const pattern_brd2  = &stimulator->pattern_brd2;

  stimpat_initPattern(pattern_brd1,
  &gait_walk_B1_L_PP,
  &gait_walk_B1_L_PW,
  gait_walk_L_duration,
  1000);
  stimpat_activatePattern(pattern_brd1);

  /*
  stimpat_initPattern(pattern_brd2,
  &gait_walk_L_B2_PP,
  &gait_walk_L_B2_PW,
  gait_walk_L_duration,
  1000);
  stimpat_activatePattern(pattern_brd2);
  */

}


void ged_leftSwingCheckTrans(ged_state_machine_t *state_machine, void *v_stimulator) {
}

/*----------------------------------------------------------------------------*/
/*                                R SWING                                      */
/*----------------------------------------------------------------------------*/

void ged_rightSwingOnEntry(void *v_stimulator) {
  stim_module_t *stimulator = (stim_module_t *) v_stimulator;

  stim_pattern_t *const pattern_brd1  = &stimulator->pattern_brd1;
  //stim_pattern_t *const pattern_brd2  = &stimulator->pattern_brd2;

  stimpat_initPattern(pattern_brd1,
  &gait_walk_B1_R_PP,
  &gait_walk_B1_R_PW,
  gait_walk_R_duration,
  1000);
  stimpat_activatePattern(pattern_brd1);

  /*
  stimpat_initPattern(pattern_brd2,
  &gait_walk_R_B2_PP,
  &gait_walk_R_B2_PW,
  gait_walk_R_duration,
  1000);
  stimpat_activatePattern(pattern_brd2);
  */
}

void ged_rightSwingCheckTrans(ged_state_machine_t *state_machine, void *v_stimulator) {
}

/*----------------------------------------------------------------------------*/
/*                                L DBL STANCE                                */
/*----------------------------------------------------------------------------*/

void ged_leftDblStanceCheckTrans(ged_state_machine_t *state_machine, void *v_stimulator) {
}

/*----------------------------------------------------------------------------*/
/*                                R DBL STANCE                                */
/*----------------------------------------------------------------------------*/


void ged_rightDblStanceCheckTrans(ged_state_machine_t *state_machine, void *v_stimulator) {
}

