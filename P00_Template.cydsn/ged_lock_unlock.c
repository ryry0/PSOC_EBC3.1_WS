#include <ged_locked_unlock.h>


/*----------------------------------------------------------------------------*/
/*                    STATE FUNCTION AND TYPE PROTOTYPES                      */
/*----------------------------------------------------------------------------*/

static void ged_lockedOnEntry(exo_t *exo);
static void ged_lockedRun(exo_t *exo);
static void ged_lockedCheckTrans(ged_state_machine_t *state_machine, exo_t
    *exo);

static void ged_unlockedOnEntry(exo_t *exo);
static void ged_unlockedRun(exo_t *exo);
static void ged_unlockedCheckTrans(ged_state_machine_t *state_machine, exo_t
    *exo);

static const ged_state_t ged_unlocked = {
  .id = GED_UNLOCKED,
  .onentry_func = ged_unlockedOnEntry,
  .checktrans_func = ged_unlockedCheckTrans,
  .run_func = ged_unlockedRun,
};

static const ged_state_t ged_locked = {
  .id = GED_LOCKED,
  .onentry_func = ged_lockedOnEntry,
  .checktrans_func = ged_lockedCheckTrans,
  .run_func = ged_lockedRun,
};

/*----------------------------------------------------------------------------*/
/*                             UNLOCKED                                       */
/*----------------------------------------------------------------------------*/
void ged_unlockedOnEntry(exo_t *exo) {
  joint_t *const r_knee_joint = &exo->r_knee_joint;
  joint_t *const l_knee_joint = &exo->l_knee_joint;

  joint_enableMotor(r_knee_joint);
  joint_enableMotor(l_knee_joint);
  joint_unlock(r_knee_joint);
  joint_unlock(l_knee_joint);
}

void ged_unlockedCheckTrans(ged_state_machine_t *state_machine, exo_t *exo) {
}

void ged_unlockedRun(exo_t *exo) {
  joint_t *const r_knee_joint = &exo->r_knee_joint;
  joint_t *const l_knee_joint = &exo->l_knee_joint;

  joint_runFC(r_knee_joint, r_knee_joint->setpoint, EXO_DELTA_TIME);
  joint_runFC(l_knee_joint, l_knee_joint->setpoint, EXO_DELTA_TIME);
}

/*----------------------------------------------------------------------------*/
/*                                LOCKED                                      */
/*----------------------------------------------------------------------------*/
void ged_lockedOnEntry(exo_t *exo) {
  joint_t *const r_knee_joint = &exo->r_knee_joint;
  joint_t *const l_knee_joint = &exo->l_knee_joint;

  joint_disableMotor(r_knee_joint);
  joint_disableMotor(l_knee_joint);
  joint_lock(r_knee_joint);
  joint_lock(l_knee_joint);
}

void ged_lockedRun(exo_t *exo) {
}

void ged_lockedCheckTrans(ged_state_machine_t *state_machine, exo_t *exo) {
  if (bd_getFSW(&exo->sensor_inputs, BD_GREEN_FSW))
    ged_transition(state_machine, &ged_leftStep, exo);
}

