#include <stim_configurable.h>
//#include <GAITVCK5.h>
/*
float configurable_pattern1_duration = 2.0;

uint8_t configurable_pattern1_AMP[STIM_NUM_CHANNELS] =
  { 38,		38,		38,		38,		38,		38,		38,		38,		0,		0,		0,		0 };

uint16_t configurable_pattern1_PP[STIM_NUM_CHANNELS][STIM_NUM_POINTS] = {
		{ 0,		5000,	5500,	6000,	7500,	8800,	9400,	10000 },
		{ 0,		1000,	5000,	7500,	8800,	9400,	9500,	10000 },
		{ 0,		5000,	5500,	6000,	7500,	8800,	9400,	10000 },
		{ 0,		1000,	5000,	8000,	9000,	9500,	9800,	10000 },
		{ 0,		5000,	5500,	6000,	7500,	8800,	9400,	10000 },
		{ 0,		1000,	5000,	7500,	8800,	9400,	9500,	10000 },
		{ 0,		1000,	5000,	7500,	8800,	9400,	9500,	10000 },
		{ 0,		1000,	5000,	7500,	8800,	9400,	9500,	10000 },
		{ 0,		789,	2000,	5526,	6000,	6500,	7105,	10000 },
		{ 0,		789,	2000,	5526,	6000,	6500,	7105,	10000 },
		{ 0,		789,	2000,	5526,	6000,	6500,	7105,	10000 },
		{ 0,		789,	2000,	5526,	6000,	6500,	7105,	10000 }
};


// Pulse Width
uint8_t configurable_pattern1_PW[STIM_NUM_CHANNELS][STIM_NUM_POINTS] = {
		{ 0,		0,		0,		0,		0,		0,		0,		0 },
		{ 0,		0,		0,		0,		0,		0,		0,		0 },
		{ 0,		0,		0,		0,		0,		0,		0,		0 },
		{ 0,		0,		0,		0,		0,		0,		0,		0 },
		{ 0,		0,		0,		0,		0,		0,		0,		0 },
		{ 0,		0,		0,		0,		0,		0,		0,		0 },
		{ 0,		0,		0,		0,		0,		0,		0,		0 },
		{ 0,		0,		0,		0,		0,		0,		0,		0 },
		{ 0,		0,		0,		0,		0,		0,		0,		0 },
		{ 0,		0,		0,		0,		0,		0,		0,		0 },
		{ 0,		0,		0,		0,		0,		0,		0,		0 },
		{ 0,		0,		0,		0,		0,		0,		0,		0 }
};
*/
/*
void cf_setPattern(struct cf_interface_s *interface, size_t board_number, size_t
    pattern_number, size_t channel_number, float *new_pw, float *new_pp) {


      memcpy(&interface->boards[board_number].patterns[pattern_number].pulse_width[channel_number][0],
          new_pw, sizeof(uint8_t)*STIM_NUM_POINTS);

      memcpy(&interface->boards[board_number].patterns[pattern_number].pulse_percent[channel_number][0],
          new_pp, sizeof(uint16_t)*STIM_NUM_POINTS);
         
}
 */
/*
void cf_interface_init(struct cf_interface_s *interface) {
 
  //set all the pattern stuff here.

  //Left step board 1
  interface->boards[STIM_BOARD_1].patterns[STIM_WALK_L].pulse_percent =
    gait_walk_L_B1_PP;
  interface->boards[STIM_BOARD_1].patterns[STIM_WALK_L].pulse_width =
    gait_walk_L_B1_PW;

  //Right step board 1
  interface->boards[STIM_BOARD_1].patterns[STIM_WALK_R].pulse_percent =
    gait_walk_R_B1_PP;
  interface->boards[STIM_BOARD_1].patterns[STIM_WALK_R].pulse_width =
    gait_walk_R_B1_PW;

  //Left step board 2
  interface->boards[STIM_BOARD_2].patterns[STIM_WALK_L].pulse_percent =
    gait_walk_L_B2_PP;
  interface->boards[STIM_BOARD_2].patterns[STIM_WALK_L].pulse_width =
    gait_walk_L_B2_PW;

  //Right step board 2
  interface->boards[STIM_BOARD_2].patterns[STIM_WALK_R].pulse_percent =
    gait_walk_R_B2_PP;
  interface->boards[STIM_BOARD_2].patterns[STIM_WALK_R].pulse_width =
    gait_walk_R_B2_PW;


  //stand B1
  interface->boards[STIM_BOARD_1].patterns[STIM_STAND].pulse_percent =
    gait_stand_B1_PP;
  interface->boards[STIM_BOARD_1].patterns[STIM_STAND].pulse_width =
    gait_stand_B1_PW;

  //stand B2
  interface->boards[STIM_BOARD_2].patterns[STIM_STAND].pulse_percent =
    gait_stand_B2_PP;
  interface->boards[STIM_BOARD_2].patterns[STIM_STAND].pulse_width =
    gait_stand_B2_PW;


  //sit B1
  interface->boards[STIM_BOARD_1].patterns[STIM_SIT].pulse_percent =
    gait_sit_B1_PP;
  interface->boards[STIM_BOARD_1].patterns[STIM_SIT].pulse_width =
    gait_sit_B1_PW;

  //sit B2
  interface->boards[STIM_BOARD_2].patterns[STIM_SIT].pulse_percent =
    gait_sit_B2_PP;
  interface->boards[STIM_BOARD_2].patterns[STIM_SIT].pulse_width =
    gait_sit_B2_PW;


  //misc1 B1
  interface->boards[STIM_BOARD_1].patterns[STIM_MISC1].pulse_percent =
    gait_misc1_B1_PP;
  interface->boards[STIM_BOARD_1].patterns[STIM_MISC1].pulse_width =
    gait_misc1_B1_PW;

  //misc1 B2
  interface->boards[STIM_BOARD_2].patterns[STIM_MISC1].pulse_percent =
    gait_misc1_B2_PP;
  interface->boards[STIM_BOARD_2].patterns[STIM_MISC1].pulse_width =
    gait_misc1_B2_PW;

  //misc2 B1
  interface->boards[STIM_BOARD_1].patterns[STIM_MISC2].pulse_percent =
    gait_misc2_B1_PP;
  interface->boards[STIM_BOARD_1].patterns[STIM_MISC2].pulse_width =
    gait_misc2_B1_PW;

  //misc2 B2
  interface->boards[STIM_BOARD_2].patterns[STIM_MISC2].pulse_percent =
    gait_misc2_B2_PP;
  interface->boards[STIM_BOARD_2].patterns[STIM_MISC2].pulse_width =
    gait_misc2_B2_PW;

  //misc3 B1
  interface->boards[STIM_BOARD_1].patterns[STIM_MISC3].pulse_percent =
    gait_misc3_B1_PP;
  interface->boards[STIM_BOARD_1].patterns[STIM_MISC3].pulse_width =
    gait_misc3_B1_PW;

  //misc3 B2
  interface->boards[STIM_BOARD_2].patterns[STIM_MISC3].pulse_percent =
    gait_misc3_B2_PP;
  interface->boards[STIM_BOARD_2].patterns[STIM_MISC3].pulse_width =
    gait_misc3_B2_PW;
  
}
*/
