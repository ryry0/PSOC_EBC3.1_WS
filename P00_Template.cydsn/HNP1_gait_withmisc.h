// StimPattern.h - Head file for storage Stim Patterns.
// Created by Lu Li (lxl361@case), SEP, 2015.
// Version 1.0
// Repo: https://github.com/lilulab/CwruStim_beta

#ifndef HNP1_GAIT_WITHMISC_H_
#define HNP1_GAIT_WITHMISC_H_

#include <stdint.h>

//#include <avr/pgmspace.h>


// ------ Walk ------

// Channel Amplitude
extern uint8_t gait_B1_AMP[12];
extern uint8_t gait_B2_AMP[12];

// Step Duration
extern float gait_walk_L_duration;
extern float gait_walk_R_duration;


// Board 1------------------

// Left Step ---------

// Percent Pattern
extern uint16_t gait_walk_L_B1_PP[12][8];


// Pulse Width
extern uint8_t gait_walk_L_B1_PW[12][8];

// Inter Phase Interval
extern uint8_t gait_walk_L_B1_IPI[12];

// Right Step ---------

// Percent Pattern
extern uint16_t gait_walk_R_B1_PP[12][8];

// Pulse Width
extern uint8_t gait_walk_R_B1_PW[12][8];

// Inter Phase Interval
extern uint8_t gait_walk_R_B1_IPI[12];



// Board 2------------------

// Left Step ---------

// Percent Pattern
extern uint16_t gait_walk_L_B2_PP[12][8];

// Pulse Width
extern uint8_t gait_walk_L_B2_PW[12][8];

// Inter Phase Interval
extern uint8_t gait_walk_L_B2_IPI[12];


// Right Step ---------

// Percent Pattern
extern uint16_t gait_walk_R_B2_PP[12][8];

// Pulse Width
extern uint8_t gait_walk_R_B2_PW[12][8];

// Inter Phase Interval
extern uint8_t gait_walk_R_B2_IPI[12];


// ------ Stand ------

// Step Duration
extern float gait_stand_duration;


// Board 1------------------

// Percent Pattern
extern uint16_t gait_stand_B1_PP[12][8];

// Pulse Width
extern uint8_t gait_stand_B1_PW[12][8];


// Inter Phase Interval
extern uint8_t gait_stand_B1_IPI[12];



// Board 2------------------

// Percent Pattern
extern uint16_t gait_stand_B2_PP[12][8];

// Pulse Width
extern uint8_t gait_stand_B2_PW[12][8];

// Inter Phase Interval
extern uint8_t gait_stand_B2_IPI[12];


// ------ Sit ------

// Step Duration
extern float gait_sit_duration;


// Board 1------------------

// Percent Pattern
extern uint16_t gait_sit_B1_PP[12][8];

// Pulse Width
extern uint8_t gait_sit_B1_PW[12][8];

// Inter Phase Interval
extern uint8_t gait_sit_B1_IPI[12];



// Board 2------------------

// Percent Pattern
extern uint16_t gait_sit_B2_PP[12][8];

// Pulse Width
extern uint8_t gait_sit_B2_PW[12][8];

// Inter Phase Interval
extern uint8_t gait_sit_B2_IPI[12];


// ------ Miscellaneous ------

// Step Duration
extern float gait_misc1_duration;


// Board 1------------------

// Percent Pattern
extern uint16_t gait_misc1_B1_PP[12][8];

// Pulse Width
extern uint8_t gait_misc1_B1_PW[12][8];

// Inter Phase Interval
extern uint8_t gait_misc1_B1_IPI[12];


// Board 2------------------

// Percent Pattern
extern uint16_t gait_misc1_B2_PP[12][8];

// Pulse Width
extern uint8_t gait_misc1_B2_PW[12][8];

// Inter Phase Interval
extern uint8_t gait_misc1_B2_IPI[12];

// ------ Miscellaneous ------

// Step Duration
extern float gait_misc2_duration;


// Board 1------------------

// Percent Pattern
extern uint16_t gait_misc2_B1_PP[12][8];

// Pulse Width
extern uint8_t gait_misc2_B1_PW[12][8];

// Inter Phase Interval
extern uint8_t gait_misc2_B1_IPI[12];


// Board 2------------------

// Percent Pattern
extern uint16_t gait_misc2_B2_PP[12][8];

// Pulse Width
extern uint8_t gait_misc2_B2_PW[12][8];

// Inter Phase Interval
extern uint8_t gait_misc2_B2_IPI[12];

// ------ Miscellaneous ------

// Step Duration
extern float gait_misc3_duration;


// Board 1------------------

// Percent Pattern
extern uint16_t gait_misc3_B1_PP[12][8];

// Pulse Width
extern uint8_t gait_misc3_B1_PW[12][8];

// Inter Phase Interval
extern uint8_t gait_misc3_B1_IPI[12];


// Board 2------------------

// Percent Pattern
extern uint16_t gait_misc3_B2_PP[12][8];

// Pulse Width
extern uint8_t gait_misc3_B2_PW[12][8];

// Inter Phase Interval
extern uint8_t gait_misc3_B2_IPI[12];
// File End

#endif
