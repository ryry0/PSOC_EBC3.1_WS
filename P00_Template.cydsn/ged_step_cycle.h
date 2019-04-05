#ifndef GED_STEP_CYCLE_H_
#define GED_STEP_CYCLE_H_

static void ged_leftStepOnEntry(exo_t *exo);
static void ged_leftStepRun(exo_t *exo);
static void ged_leftStepCheckTrans(ged_state_machine_t *state_machine, exo_t
    *exo);

static ged_state_t ged_leftStep = {
  .id = GED_LEFT_STEP,
  .onentry_func = ged_leftStepOnEntry,
  .checktrans_func = ged_leftStepCheckTrans,
  .run_func = ged_leftStepRun,
};


static void ged_rightStepOnEntry(exo_t *exo);
static void ged_rightStepRun(exo_t *exo);
static void ged_rightStepCheckTrans(ged_state_machine_t *state_machine, exo_t
    *exo);

static ged_state_t ged_rightStep = {
  .id = GED_RIGHT_STEP,
  .onentry_func = ged_rightStepOnEntry,
  .checktrans_func = ged_rightStepCheckTrans,
  .run_func = ged_rightStepRun,
};

#endif
