#ifndef STIM_MODULE_CONSTANTS_H_
#define STIM_MODULE_CONSTANTS_H_

//#define STIM_ENABLE_USB

#define STIM_CONFIG_2_PERC //configure 2 perc boards

//#define STIM_CONFIG_2_IMPLANT //configure 2 implant boards.

//#define STIM_CONFIG_3RD_SURFACE

#if defined(STIM_CONFIG_2_PERC)

#define STIM_SETUP_FUNC stimint_initPercBoard
#define STIM_PATTERN_B1_PP
#define STIM_PATTERN_B1_PW

#define STIM_PATTERN_B2_PP
#define STIM_PATTERN_B2_PW

#define STIM_AMPLITUDE 38

#elif defined(STIM_CONFIG_2_IMPLANT)

#define STIM_SETUP_FUNC stimint_initIST16Board
#define STIM_PATTERN_B1_PP
#define STIM_PATTERN_B1_PW

#define STIM_PATTERN_B2_PP
#define STIM_PATTERN_B2_PW

#define STIM_AMPLITUDE 0x08

#endif

#define STIM_MODULE_SCALING_FACTOR 1.0

typedef enum {
  MODULE_RIGHT_HIP_ID = 0,
  MODULE_RIGHT_KNEE_ID,
  MODULE_LEFT_HIP_ID,
  MODULE_LEFT_KNEE_ID,
  MODULE_ID_MAX
} module_id_t;

#endif
