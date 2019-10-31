/**
 * \file
 * \author Ryan-David Reyes
 *
 * \brief Contains functions to set up the board
 */
#ifndef BOARD_SETUP_H_
#define BOARD_SETUP_H_
#include <stim_module.h>

/** Hardware and Software Initialization  */

/**
 * \brief The setup function.
 *
 * This sets up the cooperative threads as well as the hardware on the board.
 *
 * \param task_arr The array of cooperative threads.
 * \param coop_args The arguments to the cooperative threads.
 * \param stimulator The stim_module structure
 */
void setupStimulator(
    struct coop_task_s *task_arr,
    coop_args_t *coop_args,
    volatile stim_module_t *stimulator);

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
 * \param stimulator The stim_module structure
 */
void setupStimulatorVars(volatile stim_module_t *stimulator);

/**
 * \brief Sets up board specific hardware things - usb serial, timers, etc.
 */
void setupBoard();

/**
 * \brief Sets up the stimulation boards
 */
void setupStim(volatile stim_module_t *stimulator);

#endif
