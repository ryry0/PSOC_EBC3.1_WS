#ifndef STIM_CONFIGURABLE_H_
#define STIM_CONFIGURABLE_H_

//#include <exo.h>
#include <stdint.h>
#include <stdlib.h>

    /*
#define STIM_NUM_CHANNELS 12
#define STIM_NUM_POINTS 8

#define STIM_BOARD_1 0
#define STIM_BOARD_2 1
#define STIM_NUM_BOARDS 2

enum stim_pattern_id_e {
  STIM_WALK_L,
  STIM_WALK_R,
  STIM_STAND,
  STIM_SIT,
  STIM_MISC1,
  STIM_MISC2,
  STIM_MISC3,
  STIM_ID_MAX
};

extern uint8_t configurable_pattern1_AMP[STIM_NUM_CHANNELS];
extern uint16_t configurable_pattern1_PP[STIM_NUM_CHANNELS][STIM_NUM_POINTS];
extern uint8_t configurable_pattern1_PW[STIM_NUM_CHANNELS][STIM_NUM_POINTS];
extern float configurable_pattern1_duration;

struct cf_pattern_s {
  uint16_t (*pulse_percent)[STIM_NUM_CHANNELS][STIM_NUM_POINTS];
  uint8_t  (*pulse_width)[STIM_NUM_CHANNELS][STIM_NUM_POINTS];
};

struct cf_board_s {
  struct cf_pattern_s patterns[STIM_ID_MAX];
};

struct cf_interface_s {
  struct cf_board_s boards[STIM_NUM_BOARDS];
};

void cf_setPattern(struct cf_interface_s *interface, size_t board_number, size_t
    pattern_number, size_t channel_number, float *new_pw, float *new_pp);

void cf_interface_init(struct cf_interface_s *interface);

    */
#endif
