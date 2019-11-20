// StimPattern.h - Head file for storage Stim Patterns.
// Created by Lu Li (lxl361@case), SEP, 2015.
// Version 1.0
// Repo: https://github.com/lilulab/CwruStim_beta

#ifndef BAL_06_H_
#define BAL_06_H_
#include <stdint.h>


// ------ Walk ------

// Step Duration 
extern float gait_walk_R_duration; 
extern float gait_walk_L_duration; 

// Channel Amplitude 
extern uint8_t gait_B1_AMP[16];


// Board_IST16------------------

// Left Step ---------

// Percent Pattern
extern uint16_t gait_walk_B1_L_PP[16][8];


// Pulse Width
extern uint8_t gait_walk_B1_L_PW[16][8];


// Inter Phase Interval
extern uint8_t gait_walk_B1_L_IPI[16];


// Right Step ---------

// Percent Pattern
extern uint16_t gait_walk_B1_R_PP[16][8] ;


// Pulse Width
extern uint8_t gait_walk_B1_R_PW[16][8] ;


// Inter Phase Interval
extern uint8_t gait_walk_B1_R_IPI[16];


// ------ Stand ------

// Step Duration 
extern float gait_stand_duration; 


// Board_IST16------------------

// Step ---------

// Percent Pattern
extern uint16_t gait_stand_B1_PP[16][8] ;


// Pulse Width
extern uint8_t gait_stand_B1_PW[16][8] ;


// Inter Phase Interval
extern uint8_t gait_stand_B1_IPI[16] ;


// ------ Sit ------

// Step Duration 
extern float gait_sit_duration; 


// Board_IST16------------------

// Step ---------

// Percent Pattern
extern uint16_t gait_sit_B1_PP[16][8] ;


// Pulse Width
extern uint8_t gait_sit_B1_PW[16][8] ;


// Inter Phase Interval
extern uint8_t gait_sit_B1_IPI[16] ;

// File End
#endif

