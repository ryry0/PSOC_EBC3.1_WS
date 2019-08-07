/**
 * \file
 *
 * \author Ryan-David Reyes
 *
 * \brief This file contains an interface built on top of Lu's stimboard library
 */
#ifndef STIM_INTERFACE_H_
#define STIM_INTERFACE_H_

#include <CwruStimLib.h>
#include <stdbool.h>
#define NUM_CHANNELS 12
#define NUM_POINTS 8

/** this struct is used to store information corresponding to a pattern. */
typedef struct {
  uint16_t (*Active_LUT_PP)[NUM_CHANNELS][NUM_POINTS];
  uint8_t  (*Active_LUT_PW)[NUM_CHANNELS][NUM_POINTS];
  float pattern_time;
  uint16_t counts_per_second;
  size_t counter;
  uint16_t cycle_percent;
  bool active;
} stim_pattern_t;


/**
 * \brief Initialize a percutaneous board
 *
 * \param stim_board The stim board.
 * \param ipi Interpulse interval. - TODO should remove.
 */
void stimint_initPercBoard(cwru_stim_struct_t *stim_board, uint8_t ipi);


/**
 * \brief Initialize an implant board
 *
 * \param stim_board The stim board.
 * \param ipi Interpulse interval. - TODO should remove.
 */
void stimint_initIST16Board(cwru_stim_struct_t *stim_board, uint8_t ipi);


/**
 * \brief Initialize surface board
 *
 * \param stim_board The stim board.
 * \param ipi Interpulse interval. - TODO should remove.
 */
void stimint_initSurfBoard(cwru_stim_struct_t *stim_board, uint8_t ipi);
//stim_cmd_crt_sched(&cwru_stim_brd1, UECU_SYNC_MSG, ipi); // Sync signal = 0xAA, duration 29msec.


//void stimpat_init(stim_pattern_t *interface);

//void stimpat_crtevent_wrap(cwru_stim_struct_t *stim_board);

//void stimpat_setevent_wrap(cwru_stim_struct_t *stim_board);

/**
 * \brief Initialize a board's uart
 *
 * \param stim_board The stim board.
 * \param port_id The ID of the UART used to communicate with perc board
 */
void stimint_initBoardUART(cwru_stim_struct_t *stim_board,
    uint8_t port_id);


/**
 * \brief Initialize a stim_pattern
 *
 * \param stim_pattern The stim pattern struct which stores pattern metadata.
 * \param Active_LUT_PP The lookup table of pulse percent.
 * \param Active_LUT_PW The lookup table of pulse width.
 * \param pattern_time Time in seconds it takes for pattern to complete.
 * \param counts_per_second How many increments does it take to get to 1 second
 */
void stimpat_initPattern(stim_pattern_t *stim_pattern,
  uint16_t (*Active_LUT_PP)[12][8],
  uint8_t  (*Active_LUT_PW)[12][8],
  float pattern_time,
  uint16_t counts_per_second);

/**
 * \brief Creates Perc Board schedule and adds events to it
 *
 * \param stim_board The stim board.
 * \param ipi Interpulse interval.
 */
void stim_crtPercSchedEvents(cwru_stim_struct_t *stim_board, uint8_t ipi);

/**
 * \brief Abstraction of applying a stim pattern to a board. Meant to run inside
 * a loop.
 *
 * \param stim_board The stim board.
 * \param stim_pattern The stim pattern struct which stores pattern metadata.
 * \param amplitude The amplitude of all channels
 */
void stimpat_applyPatternLoop(cwru_stim_struct_t
    *stim_board, stim_pattern_t *stim_pattern, uint8_t amplitude);


/**
 * \brief Increments counter associated with a pattern. N counts per second is
 * set in the initPattern function.
 *
 * \param stim_pattern The stim pattern struct which stores pattern metadata.
 * \param increment How many to increment the counter by.
 */
void stimpat_incrementCounter(stim_pattern_t *stim_pattern, size_t increment);


/**
 * \brief Resets the amount of time passed, as well as the percent cycle.
 *
 * \param stim_pattern The stim pattern struct which stores pattern metadata.
 */
void stimpat_resetTimeAndPercent(stim_pattern_t *stim_pattern);


/**
 * \brief Sets the pattern as active, and the board will output stim pulses upon
 * next applyPattern.
 *
 * \param stim_pattern The stim pattern struct which stores pattern metadata.
 */
void stimpat_activatePattern(stim_pattern_t *stim_pattern);


/**
 * \brief Sets the pattern as inactive, and the board will hold last values
 * commanded. 
 *
 * \param stim_pattern The stim pattern struct which stores pattern metadata.
 */
void stimpat_deactivatePattern(stim_pattern_t *stim_pattern);

/*
  what do I need to design?

  increment counter to keep track of time in swing state.
  getpercent :: time -> percent
  stim_update :: stim_board -> pattern -> percent -> STIM ()
  pattern has PA and PW
  each data point e.g. PW has a percent pattern number associated with it.
  255 @ 35%
  200 @ 50%
  100 @ 78%
  */

/*
 * stimthread
 *  runs every 20 ms
 *  if (swing_state)
 *  increment time variable
 *  calc new percent from time
 *  calc new pw
 *
 *  if different
 *  send update
 *
 * in state functions
 *  onentry into swing
 *  set swing_state = true
 *  set timer var to 0
 *  set lookup tables for particular state
 */


void stimpat_test();
void stimpat_test_lib();
void stimpat_test_new();
void stimpat_testImplant();



#endif
