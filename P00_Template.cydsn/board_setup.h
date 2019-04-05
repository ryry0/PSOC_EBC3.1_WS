/**
 * \file
 * \author Ryan-David Reyes
 *
 * \brief Contains functions to set up the board
 */
#ifndef BOARD_SETUP_H_
#define BOARD_SETUP_H_
#include <exo.h>

/** Hardware and Software Initialization  */

/**
 * \brief The setup function.
 *
 * This sets up the cooperative threads as well as the hardware on the board.
 *
 * \param task_arr The array of cooperative threads.
 * \param coop_args The arguments to the cooperative threads.
 * \param exo The exoskeleton structure.
 */
void setupExo(
    struct coop_task_s *task_arr,
    coop_args_t *coop_args,
    volatile exo_t *exo);

/**
 * \brief Sets up the cooperative threads
 *
 * \param task_list The structure that represents the tasks. This is used with
 * the cooperative scheduler.
 *
 * \param task_arr Array of structures that represent how long a thread will
 * repeat.
 */
void setupThreads(struct coop_task_list_s *task_list, struct coop_task_s
    *task_arr);

/**
 * \brief Sets up the variables associated with the exoskeleton
 *
 * \param exo The exoskeleton structure.
 */
void setupExoVars(volatile exo_t *exo);

/**
 * \brief Sets up board specific hardware things - usb serial, timers, etc.
 */
void setupBoard();

/**
 * \brief Initializes the joint.
 */
void setupRightKneeJoint(joint_t *r_knee_joint);

/**
 * \brief Initializes the joint.
 */
void setupLeftKneeJoint(joint_t *l_knee_joint);

/**
 * \brief Calculates the trajectories used in the lookup tables.
 */
void setupTrajectories(volatile exo_t *exo);

/**
 * \brief Sets up the stimulation boards
 */
void setupStim(volatile exo_t *exo);

#endif
