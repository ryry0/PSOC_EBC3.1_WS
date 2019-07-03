/**
 * \file
 *
 * \author Ryan-David Reyes
 * \brief This is the api for the state  machine.
 *
 * Uses void pointers to get around problems with circular includes.
 */
#ifndef GED_FSM_H_
#define GED_FSM_H_

typedef enum ged_states_e {
  GED_UNLOCKED,
  GED_LOCKED,
  GED_LEFT_SWING,
  GED_RIGHT_SWING,
  GED_LEFT_DBL_STANCE,
  GED_RIGHT_DBL_STANCE
  GED_PASSIVE,
  GED_SIT_TO_STAND,
  GED_STAND_TO_SIT,
  GED_STATE_MAX
} ged_state_id_t;

typedef struct ged_state_machine_s ged_state_machine_t;

typedef void (*ged_checktrans_func_t)(ged_state_machine_t*, void*);
typedef void (*ged_onentry_func_t)(void*);
typedef void (*ged_run_func_t)(void*);

typedef struct ged_state_s {
  ged_state_id_t id;

  ged_onentry_func_t    onentry_func;
  ged_checktrans_func_t checktrans_func;
  ged_run_func_t        run_func;

} ged_state_t;


struct ged_state_machine_s {
  ged_state_t state;
};

/**
 * \brief Run the necessary control algorithms, etc associated with a particular
 * state */
void ged_runState(const ged_state_machine_t *state_machine, void *v_exo);

/**
 * \brief Checks the transitions into and out of states, responsible for
 * switching states.
 */
void ged_checkTransitions(ged_state_machine_t *state_machine, void *v_exo);

/**
 * \brief Transitions to a new state
 * switching states.
 */
void ged_transition(ged_state_machine_t *state_machine, ged_state_t state, void
    *v_exo);

/**
 * \brief Initialize the state machine into a certain state
 */
void ged_init(ged_state_machine_t *state_machine, void *v_exo);

/**
 * \brief Initialize the state machine into a certain state
 */
ged_state_machine_t *ged_create();

/**
 * \brief Default transitions that will apply for every state (can be
 * overridden)
 */
void ged_defaultTrans(ged_state_machine_t *state_machine, void *v_exo);

/**
 * \brief Force a state transition depending on the ged_state_id_t enum
 */
void ged_forceTrans(ged_state_machine_t *state_machine, void *v_exo,
    ged_state_id_t state_id);

ged_state_id_t ged_getStateID(ged_state_machine_t *state_machine);
#endif
