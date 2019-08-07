/** \file
 * \author Ryan-David Reyes
 * \author Mark Nandor
 */

/* ========================================
 *
 * Copyright Advanced Platform Technology Center
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF The Advanced Platform Technology Center.
 *
 * ========================================
 */

#include <exo.h>
#include <board_setup.h>
#include <coop.h> //cooperative thread structure
#include <CwruStimLib.h>
#include <stim_interface.h>

/**
 * \mainpage Power Hybrid Exoskeleton
 *
 * \section arch_sec Architecture
 *
 * The firmware is architected into two distinct pieces: A hard real-time 1kHz
 * interrupt ISR, and Cooperative threads.
 *
 * The real-time interrupt provides timing guarantees that the control loop
 * will run once very millisecond. The cooperative threads give a timing
 * guarantee that is upper bounded by the worst case scenario: The sum of the
 * time it takes all threads to run sequentially.
 *
 * The main function is responsible for executing the scheduled tasks.
 * A great place to start perusing the code is looking at the dependency graph
 * of main.c.
 *
 * The timer interrupt and the scheduler for the cooperative threads are found
 * in exo.h and exo.c.
 *
 * The exo.* files contain how the exoskeleton is defined. The board_setup files
 * are responsible for initializing the exoskeleton object and associated
 * hardware. The board_io_files contain an abstraction layer for the hardware so
 * that it would be very easy to port the program to another microcontroller if
 * need be.
 *
 */

/**
 * Looks like stim needs to go like this:
 *
 * PP vs PW?
 *
 * stim channel max perc = 8
 *
 * enable uart
 * delay 50ms for stim to power up
 * create schedule
 * delay 25 ms
 * create events: zero pw 0x26 amplitude, initial delay 2ms
 * delay 25 ms
 * send sync message
 * chg event schedule to change ipi
 * set event to change pw/amp
 *
 * the stim_cmd_* just need MSG_DES_ADDR and STIM_SERIAL to be set.
 */

/*---------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/**
 * \brief The main function.
 *
 * Sets up the program and hardware, then executes all cooperative threads.
 */
int main() {
  coop_task_t tasks[EXO_TASK_MAX];
  coop_args_t coop_args;

  setupExo(tasks, &coop_args, &hybrid_exoskeleton);
  //stimpat_testImplant();

  for(;;) {
    coop_executeTasks(
        (struct coop_task_list_s *) &hybrid_exoskeleton.coop_task_list, &coop_args);
  } // end for(;;) loop
} // end main

/*---------------------------------------------------------------------------*/
/* [] END OF FILE */
