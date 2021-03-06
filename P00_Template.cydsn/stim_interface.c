#include <stim_interface.h>
#include <board_io.h>

/* example usage:
 * to set up the stim board:
 *
 * create a pattern:
 *  stim_pattern_t active_stim_pattern;
 *  stimpat_initPattern(&active_stim_pattern,
 *      &gait_stand_B1_PP,
 *      &gait_stand_B1_PW,
 *      2.0,
 *      1000);
 *
 * Initialize the board:
 *   stimint_initPercBoardUART(&cwru_stim_brd1, STIM_UART_PORT_1);
 *   stimint_initPercBoard(&cwru_stim_brd1, 30);
 *
 * Apply the pattern, increment the counter
 *  stimpat_applyPatternPercLoop(&cwru_stim_brd1, &active_stim_pattern);
 *  stimpat_incrementCounter(&active_stim_pattern, 20);
 *
 * Reset time and percent for next cycle
 *  stimpat_resetTimeAndPercent(&active_stim_pattern);
 *
 * Set the pattern as active
 *  stimpat_activatePattern(&active_stim_pattern);
 */

#define BD_DELAY 100 //set this?

//defines for IST 16 board
#define MSG_DES_ADDR_ICM 0x0A
#define CHANNEL_MAX_IST 16
#define CHANNEL_MAX_PERC 12
#define REFRESH_MSG_LEN 3
#define EVENT_COMMAND_MSG 0x1C
#define REFRESH_EVENT 21

// Magic number setup for implant
// 0A 80 20 05 04 00 00 00 00 4C (IRS 8, Port 0)
static const uint8_t ICM_IRS_SET_0_MSG[] = {MSG_DES_ADDR_ICM, MSG_SRC_ADDR,
                                        0x20, 0x05, 0x04, 0x00,
                                        0x00, 0x00, 0x00, 0x4C};

// 0A 80 20 05 05 00 00 00 00 4B (IRS 8, Port 1)
static const uint8_t ICM_IRS_SET_1_MSG[] = {MSG_DES_ADDR_ICM, MSG_SRC_ADDR,
                                        0x20, 0x05, 0x05, 0x00,
                                        0x00, 0x00, 0x00, 0x4B};

// 0A 80 20 05 10 00 00 00 00 40 (IST16, Port 0)
static const uint8_t ICM_IST_SET_0_MSG[] = {MSG_DES_ADDR_ICM, MSG_SRC_ADDR,
                                        0x20, 0x05, 0x10, 0x00,
                                        0x00, 0x00, 0x00, 0x40};

// 0A 80 20 05 11 00 00 00 00 3F (IST16, Port 1)
static const uint8_t ICM_IST_SET_1_MSG[] = {MSG_DES_ADDR_ICM, MSG_SRC_ADDR,
                                        0x20, 0x05, 0x11, 0x00,
                                        0x00, 0x00, 0x00, 0x40};

// send those two msg at the end of setup
static const uint8_t ICM_RFPWR_EVNT_0[] = {MSG_DES_ADDR_ICM, MSG_SRC_ADDR,
                                  0x1C, 0x04, 0x06, 0x80, 0x00, 0xFF, 0xCE};                //Port 0
static const uint8_t ICM_RFPWR_EVNT_1[] = {MSG_DES_ADDR_ICM, MSG_SRC_ADDR,
                                  0x1C, 0x04, 0x06, 0x80, 0x40, 0xFF, 0x8E};                //Port 1

static void checkdata();

static char getPCdata();
void stim_crtISTSchedEvents(cwru_stim_struct_t *stim_board, uint8_t ipi);

size_t stim_sendRefreshEvent(cwru_stim_struct_t *stim_board);

size_t stim_sendRefreshEvent(cwru_stim_struct_t *stim_board) {
  // calculate message size
  size_t size = REFRESH_MSG_LEN + UECU_MSG_EXTRAL_LEN;
  // build message content
  uint8_t msg[REFRESH_MSG_LEN + UECU_MSG_EXTRAL_LEN] =
  { stim_board->MSG_DES_ADDR,
    MSG_SRC_ADDR,
    EVENT_COMMAND_MSG,
    REFRESH_MSG_LEN,
    REFRESH_EVENT,
    0x00, //priority
    0x00, //port/channel
    0x00
  };

  // Insert checksum byte
  msg[size-1] = stim_checksum(msg,size);

  // Send message
  return stim_uart_print_array (stim_board, msg, sizeof(msg)/sizeof(uint8_t));
}

/*
 * to start associated serial: hal_uart_start(cwru_stim_brd1.port_id);
 * to init: stim_initpercboard
 * to create schedule: stim_cmd_crt_sched(&cwru_stim_brd1, UECU_SYNC_MSG, ipi);
 * to change ipi: stim_cmd_set_sched(&cwru_stim_brd1, sched_id, UECU_SYNC_MSG, ipi);
 * to change amp/pw: stimpat_setevent_wrap
 * to start stim:
 */
void stimint_initPercBoard(cwru_stim_struct_t *stim_board, uint8_t ipi) {
  stim_board->brd_type = STIM_BRD_PERC;
  stim_board->setting = STIM_SETTING_DEFAULT | STIM_SETTING_SINGLE_SCHEDULER;

  //need to make sure init-pattern and crt_sched ipi match
  //this IPI sets _current_ipi and doesn't set things on board
  stim_init_pattern(stim_board, UECU_SYNC_MSG, ipi);
  CyDelay(BD_DELAY);

  stim_init_brd_perc(stim_board);
  CyDelay(BD_DELAY);

  stim_crtPercSchedEvents(stim_board, ipi);
  stim_cmd_sync_msg(stim_board, UECU_SYNC_MSG);
}

void stimint_initSurfBoard(cwru_stim_struct_t *stim_board, uint8_t ipi) {
  stim_board->brd_type = STIM_BRD_SURF;
  stim_board->setting = STIM_SETTING_DEFAULT | STIM_SETTING_SINGLE_SCHEDULER;

  //need to make sure init-pattern and crt_sched ipi match
  //this IPI sets _current_ipi and doesn't set things on board
  stim_init_pattern(stim_board, UECU_SYNC_MSG, ipi);
  CyDelay(BD_DELAY);

  stim_init_brd_surf(stim_board);
  CyDelay(BD_DELAY);

  stim_init_chan_surf(stim_board);
  stim_init_evnt_surf(stim_board);

  stim_cmd_sync_msg(stim_board, UECU_SYNC_MSG);
}

/*
 */
void stimint_initIST16Board(cwru_stim_struct_t *stim_board, uint8_t ipi) {
  stim_board->brd_type = STIM_BRD_ICM_IST16;
  stim_board->setting = STIM_SETTING_DEFAULT | STIM_SETTING_SINGLE_SCHEDULER;

  //need to make sure init-pattern and crt_sched ipi match
  //this IPI sets _current_ipi and doesn't set things on board
  stim_init_pattern(stim_board, UECU_SYNC_MSG, ipi);
  CyDelay(BD_DELAY);

  //{ this is equivalent of stim_init_brd_perc
  stim_board->MSG_DES_ADDR = MSG_DES_ADDR_ICM;
  stim_board->STIM_CHANNEL_USED = CHANNEL_MAX_IST;

  stim_cmd_halt_rset(stim_board, UECU_RESET);
  CyDelay(BD_DELAY);
  //}

  stim_crtISTSchedEvents(stim_board, ipi);

  stim_cmd_sync_msg(stim_board, UECU_SYNC_MSG);
  CyDelay(BD_DELAY);

  stim_uart_print_array(stim_board, ICM_RFPWR_EVNT_0 ,
      sizeof(ICM_RFPWR_EVNT_0)/sizeof(uint8_t));
  CyDelay(BD_DELAY);
}

void stimpat_crtStimEvent_wrap(cwru_stim_struct_t *stim_board,
    uint8_t schedule_id,
    uint16_t delay,
    uint8_t pulse_width,
    uint8_t amplitude) {

  stim_cmd_crt_evnt(stim_board,
      schedule_id,  // sched_id = 1
      delay,        // delay = 0msec
      0,            // priority = 0
      3,            // event_type = 3, for for Stimulus Event
      0,            // port_chn_id = 0;
      pulse_width,  // pulse_width set to 0,
      amplitude,    // amplitude set to 0,
      0);           // zone not implemented;
}

/*
void stimpat_setevent_wrap(cwru_stim_struct_t *stim_board) {
  stim_cmd_set_evnt(stim_board,
      1, //uint8_t event_id,
      pulse_width,
      amplitude, 0);
}
*/

void stimpat_setActivePattern(stim_pattern_t *stim_pattern,
  uint16_t (*Active_LUT_PP)[NUM_CHANNELS][NUM_POINTS],
  uint8_t (*Active_LUT_PW)[NUM_CHANNELS][NUM_POINTS]) {
  stim_pattern->Active_LUT_PP = Active_LUT_PP;
  stim_pattern->Active_LUT_PW = Active_LUT_PW;
}

void stimpat_deactivatePattern(stim_pattern_t *stim_pattern) {
  stim_pattern->active = false;
}

void stimpat_deactivatePatternWhenComplete(stim_pattern_t *stim_pattern) {
  if (stim_pattern->cycle_percent >= 10000)
    stimpat_deactivatePattern(stim_pattern);
}

void stimpat_activatePattern(stim_pattern_t *stim_pattern) {
  stim_pattern->active = true;
}

bool stimpat_isActive(stim_pattern_t *stim_pattern){
  return stim_pattern->active;
}

void stimpat_setCounter(stim_pattern_t *stim_pattern, size_t counter) {
  stim_pattern->counter = counter;
}

void stimpat_incrementCounter(stim_pattern_t *stim_pattern, size_t increment) {
  stim_pattern->counter += increment;
}

void stimpat_setCountsPerSecond(stim_pattern_t *stim_pattern,
    uint16_t counts_per_second) {
  stim_pattern->counts_per_second = counts_per_second;
}

void stimpat_setPatternTime(stim_pattern_t *stim_pattern,
    float pattern_time) {
  stim_pattern->pattern_time = pattern_time;
}

void stimpat_calcPercent(stim_pattern_t *stim_pattern, float pattern_time,
    uint16_t counts_per_second) {
  stim_pattern->cycle_percent =
    stim_pattern->counter/(counts_per_second*pattern_time) * 10000;
}

inline void stimpat_calcPatternPercent(stim_pattern_t *stim_pattern) {
  stimpat_calcPercent(stim_pattern, stim_pattern->pattern_time,
      stim_pattern->counts_per_second);
}


void stimpat_resetTimeAndPercent(stim_pattern_t *stim_pattern) {
  stim_pattern->counter = 0;
  stim_pattern->cycle_percent = 0;
}

void stimpat_setCyclePercent(stim_pattern_t *stim_pattern, uint16_t
    cycle_percent) {
  stim_pattern->cycle_percent = cycle_percent;
}

void stimpat_applyPatternPWOneChannel(cwru_stim_struct_t *stim_board,
                        stim_pattern_t *stim_pattern,
                        uint8_t channel,
                        uint8_t event,
                        uint8_t amplitude) {
  uint8_t pulse_width_value =
    stim_get_PW_ramping(channel,
        stim_pattern->Active_LUT_PP,
        stim_pattern->Active_LUT_PW,
        stim_pattern->cycle_percent);

  /*
   * if different
   * apply update
   */
  if (pulse_width_value != stim_board->_current_pulse_width[channel]) {
    //is event ID always 1?
    stim_cmd_set_evnt(stim_board, event, pulse_width_value, amplitude, 0);

    stim_board->_current_pulse_width[channel] = pulse_width_value;
  }
}

void stimpat_applyPatternPWOneChannelScaling(cwru_stim_struct_t *stim_board,
                        stim_pattern_t *stim_pattern,
                        uint8_t channel,
                        uint8_t event,
                        uint8_t amplitude,
                        float channel_scaling) {
  uint8_t pulse_width_value = 0;
  uint16_t scaled_pw_value =
    channel_scaling*stim_get_PW_ramping(channel,
        stim_pattern->Active_LUT_PP,
        stim_pattern->Active_LUT_PW,
        stim_pattern->cycle_percent);
  if (scaled_pw_value > 255)
    pulse_width_value = 255;
  else
    pulse_width_value = scaled_pw_value;

  /*
   * if different
   * apply update
   */
  if (pulse_width_value != stim_board->_current_pulse_width[channel]) {
    //is event ID always 1?
    stim_cmd_set_evnt(stim_board, event, pulse_width_value, amplitude, 0);

    stim_board->_current_pulse_width[channel] = pulse_width_value;
  }
}

/* assumes event ID's are consecutive numbered you can't use this if you
 * delete then add events
 */
void stimpat_applyPatternAllChannels(cwru_stim_struct_t *stim_board,
                        stim_pattern_t *stim_pattern, uint8_t amplitude) { //0-10000

  for (size_t i = 0; i < stim_board->STIM_CHANNEL_USED; ++i) {
    stimpat_applyPatternPWOneChannel(stim_board, stim_pattern, i, i+1,
        stim_pattern->channel_amplitudes[i]);
  }
}

void stimpat_applyPatternAllChannelsScaling(cwru_stim_struct_t *stim_board,
                        stim_pattern_t *stim_pattern, uint8_t amplitude, float *channel_scaling) { //0-10000

  for (size_t i = 0; i < stim_board->STIM_CHANNEL_USED; ++i) {
    stimpat_applyPatternPWOneChannelScaling(stim_board, stim_pattern, i, i+1,
        stim_pattern->channel_amplitudes[i], channel_scaling[i]);
  }
}

void stimpat_applyPatternLoop(cwru_stim_struct_t
    *stim_board, stim_pattern_t *stim_pattern, uint8_t amplitude) {

  if (stimpat_isActive(stim_pattern)) {
    stimpat_calcPatternPercent(stim_pattern);

    stimpat_applyPatternAllChannels(stim_board,
        stim_pattern, amplitude);

    /*
    sprintf(new_val, "%d\n", active_stim_pattern.cycle_percent);
    bd_putStringReady(new_val);
    */

    stimpat_deactivatePatternWhenComplete(stim_pattern);
  }
}

void stimpat_applyPatternLoopScaling(cwru_stim_struct_t
    *stim_board, stim_pattern_t *stim_pattern, uint8_t amplitude, float
    *channel_scaling) {

  if (stimpat_isActive(stim_pattern)) {
    stimpat_calcPatternPercent(stim_pattern);

    stimpat_applyPatternAllChannelsScaling(stim_board,
        stim_pattern, amplitude, channel_scaling);

    stimpat_deactivatePatternWhenComplete(stim_pattern);
  }
}

void stim_crtISTSchedEvents(cwru_stim_struct_t *stim_board, uint8_t ipi) {
  const int NUM_EVENTS = stim_board->STIM_CHANNEL_USED;

  stim_cmd_crt_sched(stim_board, UECU_SYNC_MSG, ipi); // Sync signal = 0xAA, duration 29msec.
  CyDelay(BD_DELAY); //this delay needs to be here

  stim_uart_print_array(stim_board, ICM_IST_SET_0_MSG,
      sizeof(ICM_IST_SET_0_MSG)/sizeof(uint8_t));
  CyDelay(BD_DELAY);

  for (size_t i = 0; i < NUM_EVENTS; ++i) {
    stim_cmd_crt_evnt(stim_board,
        1,    // sched_id = 1
        4+i,    // delay = 0msec
        0,    // priority = 0
        3,    // event_type = 3, for for Stimulus Event
        i,    // port_chn_id = 0;
        0,    // pulse_width set to 0,
        0x00, // amplitude set to 0,
        0);   // zone not implemented;

  }
}

void stim_crtPercSchedEvents(cwru_stim_struct_t *stim_board, uint8_t ipi) {
  const int NUM_EVENTS = stim_board->STIM_CHANNEL_USED;

  stim_cmd_crt_sched(stim_board, UECU_SYNC_MSG, ipi); // Sync signal = 0xAA, duration 29msec.
  CyDelay(BD_DELAY); //this delay needs to be here

  for (size_t i = 0; i < NUM_EVENTS; ++i) {
    stim_cmd_crt_evnt(stim_board,
        1,    // sched_id = 1
        4+i*2,    // delay = 0msec
        0,    // priority = 0
        3,    // event_type = 3, for for Stimulus Event
        i,    // port_chn_id = 0;
        0,    // pulse_width set to 0,
        0x10, // amplitude set to 0,
        0);   // zone not implemented;
  }
}

void stimpat_initPattern(stim_pattern_t *stim_pattern,
  uint16_t (*Active_LUT_PP)[NUM_CHANNELS][NUM_POINTS],
  uint8_t  (*Active_LUT_PW)[NUM_CHANNELS][NUM_POINTS],
  float pattern_time,
  uint16_t counts_per_second) {

  stimpat_resetTimeAndPercent(stim_pattern);
  stimpat_setActivePattern(stim_pattern,
      Active_LUT_PP,
      Active_LUT_PW);

  stimpat_setPatternTime(stim_pattern, pattern_time);
  stimpat_setCountsPerSecond(stim_pattern, counts_per_second);
  stimpat_deactivatePattern(stim_pattern);
}

void stimint_initBoardUART(cwru_stim_struct_t *stim_board,
    uint8_t port_id) {
  stim_board->port_id = port_id;

  hal_uart_start(stim_board->port_id);
  //bd_putStringReady("Uart started\n");
  CyDelay(BD_DELAY);
}

/* TODO:
 * set all zero
 * x check if all chan works
 * x? setup stim thread
 * x mock time
 * add state for sit to stand
 * x does zero after step?
 * x increment counter
 * x calc perc pattern due to properties
 * test ipi changing
 */


/* TEST FUNCTIONS FOUND BELOW: -----------------------------------------------
 */

void stimpat_test_new() {
  cwru_stim_struct_t cwru_stim_brd1;
  /* ... wtf there's something about it needing to be
   * 100ms
   */
  uint16_t cycle_percent = 0;
  char pc_input = 0;
  stim_pattern_t active_stim_pattern;
  /*
  stimpat_initPattern(&active_stim_pattern,
      &gait_stand_B1_PP,
      &gait_stand_B1_PW,
      2.0,
      1000);
      */

  //setupBoard();
  const uint8_t STIM_BOARD_IPI = 30;

  stimint_initBoardUART(&cwru_stim_brd1, STIM_UART_PORT_1);
  stimint_initPercBoard(&cwru_stim_brd1, STIM_BOARD_IPI);

  CyDelay(5000);
  bd_putStringReady("end delay\n");

  //checkdata();

  for (;;) {
    const uint8_t THREAD_DELAY = 20;

    pc_input = getPCdata();

    if (pc_input != 0) {
      switch(pc_input) {
        case '+':
          cycle_percent += 200;
          break;
        case '-':
          cycle_percent -= 200;
          break;

        case ' ':
          stimpat_resetTimeAndPercent(&active_stim_pattern);
          stimpat_activatePattern(&active_stim_pattern);

          break;
      }

      /*
      stimpat_applyPatternPWPercOneChannel(&cwru_stim_brd1,
          &active_stim_pattern, 0, 1, 38);
          */
    } //end if

    stimpat_applyPatternLoop(&cwru_stim_brd1, &active_stim_pattern, 0x26);
    stimpat_incrementCounter(&active_stim_pattern, 20);
    CyDelay(THREAD_DELAY);

  } //end for(;;);
}

void stimpat_test_lib() {
  int8_t sucessStatus = 1u;
  cwru_stim_struct_t cwru_stim_brd1;
  //cwru_stim_struct_t cwru_stim_brd2;

  uint8_t cmd = 0;
  uint8_t mode = 0;
  uint8_t setting = 0;
  float param = 5000.0;
  char pc_input = 0;

  CyDelay(5000);

  bd_putStringReady("end delay\n");

  for (size_t i = 0; i < STIM_CHANNEL_MAX; ++i) {
    cwru_stim_brd1._current_pw_gains[i] = 1;
    cwru_stim_brd1._current_amp_gains[i] = 1;

    //cwru_stim_brd2._current_pw_gains[i] = 1;
    //cwru_stim_brd2._current_amp_gains[i] = 1;
  }

  // CwruStimLib Related
  sucessStatus *= cwru_stim_init(&cwru_stim_brd1,
      STIM_MODE_PERC, //USE MODE
      STIM_UART_PORT_0,
      STIM_SETTING_SINGLE_SCHEDULER | STIM_SETTING_DEFAULT);
  CyDelay(BD_DELAY);
  checkdata();

  /*
  sucessStatus *= cwru_stim_init(&cwru_stim_brd2,
      STIM_BRD_PERC,
      STIM_UART_PORT_1,
      STIM_SETTING_DEFAULT);
  CyDelay(BD_DELAY);
  checkdata();
  */

  // Call start func to send sync message to brds
  //sucessStatus *= cwru_stim_start(cwru_stim_struct_t *cwru_stim, const uint8_t sync_signal)
  //sucessStatus *= cwru_stim_start(&cwru_stim_brd1, UECU_SYNC_MSG);
  stim_cmd_sync_msg(&cwru_stim_brd1, UECU_SYNC_MSG);
  CyDelay(BD_DELAY);
  checkdata();

  cmd = STIM_UPDATE_GAIT_CYCLE;
  mode = VCK5_BRD1|AMP; // (Board type with id) with (IPI or PW or AMP)
  setting = PATTERN_STAND;
  sucessStatus *= cwru_stim_update(&cwru_stim_brd1, cmd, mode, setting, param);

  /*
  sucessStatus *= cwru_stim_start(&cwru_stim_brd2, UECU_SYNC_MSG);
  CyDelay(BD_DELAY);
  checkdata();
  */

  for(;;)
  {
    // CwruStimLib Related
    char new_val[30] = {0};
    uint8_t data = 0;

    pc_input = getPCdata();

    if (pc_input != 0) {

      switch(pc_input) {
        case '+':
          bd_putStringReady("plus\n");
          param += 500.0;
          break;

        case '-':
          bd_putStringReady("minus\n");
          param -= 500.0;
          break;

        case 'c':
          checkdata();
          for (size_t i = 0; i < 8; ++i) {
            sprintf(new_val, "%d %d\n", cwru_stim_brd1._current_amplitude[i],
                cwru_stim_brd1._current_pulse_width[i]);
            bd_putStringReady(new_val);
          }
          break;
      }
      sprintf(new_val, "%d\n", (int) param);
      bd_putStringReady(new_val);
    }

    data = UART_STIM_0_GetChar();
    if (data != 0) {
      sprintf(new_val, "%x\n", (int) data);
      bd_putStringReady(new_val);
    }

    // Example 1: update IPI or PW or AMP based on preset pattern
    //          and gait percentage value from GED
    cmd = STIM_UPDATE_GAIT_CYCLE;
    mode = VCK5_BRD1|PW; // (Board type with id) with (IPI or PW or AMP)
    setting = PATTERN_STAND;
    //param = 5000; // 0-10000 for 0%-100% gait cycle
    // update board 1, based on VCK5 Board 1 Stand pattern, change PW value
    sucessStatus *= cwru_stim_update(&cwru_stim_brd1, cmd, mode, setting, param);

    /*
    cmd = STIM_UPDATE_GAIT_CYCLE;
    mode = VCK5_BRD2|PW; // (Board type with id) with (IPI or PW or AMP)
    setting = PATTERN_STAND;
    //param = 5000; // 0-10000 for 0%-100% gait cycle
    // update board 2, based on VCK5 Board 2 Stand pattern, change PW value
    sucessStatus *= cwru_stim_update(&cwru_stim_brd2, cmd, mode, setting, param);
    */

    /*
    // Example 1: update IPI or PW or AMP based on preset pattern
    //          and gait percentage value from GED
    cmd = STIM_UPDATE_GAIN;
    mode = PW; // PW or AMP
    setting = 1; // Channel ID 0-STIM_CHANNEL_USED
    //param = 0.5; // 0-1 for float for 0%-100% of gain factor
    // update board 1, change channel 1 PW gain factor to 50%.
    sucessStatus *= cwru_stim_update(&cwru_stim_brd1, cmd, mode, setting, param);
    */

  } // end for(;;)
}

void stimpat_test() {
  cwru_stim_struct_t cwru_stim_brd1;
  //cwru_stim_struct_t cwru_stim_brd2;

  uint8_t sched_id = 1;
  uint8_t pulse_width = 0xF0;
  uint8_t amplitude = 0x26;
  uint8_t ipi = FIXED_SCHED_ID1_IPI;


  //setupBoard();

  CyDelay(5000);

  bd_putStringReady("end delay\n");

  cwru_stim_brd1.brd_type = STIM_BRD_PERC;
  cwru_stim_brd1.port_id = STIM_UART_PORT_0;
  cwru_stim_brd1.setting = STIM_SETTING_DEFAULT;

  hal_uart_start(cwru_stim_brd1.port_id);
  bd_putStringReady("Uart started\n");
  CyDelay(BD_DELAY);
  checkdata();

  //need to make sure init-pattern and crt_sched ipi match
  stim_init_pattern(&cwru_stim_brd1, UECU_SYNC_MSG, ipi);
  bd_putStringReady("Init Pattern\n");
  CyDelay(BD_DELAY);
  checkdata();

  stim_init_brd_perc(&cwru_stim_brd1);
  bd_putStringReady("init brd\n");
  CyDelay(BD_DELAY);
  checkdata();

  stim_cmd_crt_sched(&cwru_stim_brd1, UECU_SYNC_MSG, ipi); // Sync signal = 0xAA, duration 29msec.
  bd_putStringReady("crt_sched\n");
  CyDelay(BD_DELAY);
  checkdata();

  stim_cmd_crt_evnt(&cwru_stim_brd1,
      sched_id,  // sched_id = 1
      10,  // delay = 0msec
      0,  // priority = 0
      3,  // event_type = 3, for for Stimulus Event
      0,  // port_chn_id = 0;
      pulse_width, // pulse_width set to 0,
      amplitude, // amplitude set to 0,
      0); // zone not implemented;

  bd_putStringReady("crt_event\n");
  CyDelay(BD_DELAY);
  checkdata();

  bd_putStringReady("Finish Bd setup\n");

  stim_cmd_sync_msg(&cwru_stim_brd1, UECU_SYNC_MSG);
  bd_putStringReady("sync\n");
  CyDelay(BD_DELAY);
  checkdata();

  bd_putStringReady("Start\n");

  for(uint8_t i = 0;; i++) {
    uint8_t data = 0;
    char array[30] = {0};
    char pc_input = 0;

    // Check for input data from PC
    data = UART_STIM_1_GetChar();
    sprintf(array, "%x", data);
    //bd_putLargeDataReady(&data, 1);

    //bd_putStringReady(array);

    pc_input = getPCdata();

    if (pc_input != 0) {
      char new_val[30] = {0};

      switch(pc_input) {
        case '+':
          pulse_width += 5;
          stim_cmd_set_evnt(&cwru_stim_brd1,
              1, //uint8_t event_id,
              pulse_width,
              amplitude, 0);
          break;

        case '-':
          pulse_width -= 5;
          stim_cmd_set_evnt(&cwru_stim_brd1,
              1, //uint8_t event_id,
              pulse_width,
              amplitude, 0);
          break;

        case ']':
          amplitude += 1;
          stim_cmd_set_evnt(&cwru_stim_brd1,
              1, //uint8_t event_id,
              pulse_width,
              amplitude, 0);
          break;

        case '[':
          amplitude -= 1;
          stim_cmd_set_evnt(&cwru_stim_brd1,
              1, //uint8_t event_id,
              pulse_width,
              amplitude, 0);
          break;

        case ',':
          ipi -= 5;
          stim_cmd_set_sched(&cwru_stim_brd1, sched_id, UECU_SYNC_MSG, ipi);
          break;

        case '.':
          ipi += 5;
          stim_cmd_set_sched(&cwru_stim_brd1, sched_id, UECU_SYNC_MSG, ipi);
          //this command changes
          //stim_cmd_chg_evnt_sched(&cwru_stim_brd1,
          //1, //uint8_t event_id,
          //1, //uint8_t sched_id,
          //, //uint16_t delay,
          //0)
          //
          break;
      }

      bd_putStringReady("Change\n");

      sprintf(new_val, "%x", pulse_width);
      bd_putStringReady(new_val);
    }


    //psoc_putStringReady("D: ");
    //bd_putLargeDataReady(&i,
    //1);
    //CyDelay(100);
  }
}

static void checkdata() {
    uint8_t data = 0;
    char array[30] = {0};

  do {
    data = UART_STIM_0_GetChar();
    sprintf(array, "%x", data);
    //bd_putLargeDataReady(&data, 1);

    bd_putStringReady(array);
    bd_putStringReady(" Check\n");
  } while (data != 0);
}

static char getPCdata() {
  char uart_buffer[128] = {0};
  uint8_t uart_count = USBUART_1_GetCount();

  /* Check for input data from PC */
  if(uart_count != 0) {
    USBUART_1_GetAll(uart_buffer);
    bd_usbTimeout();

    return uart_buffer[0];
  } //end if(uart_count
  return 0;
}

void stimpat_testImplant() {
  cwru_stim_struct_t cwru_stim_brd1;
  //cwru_stim_struct_t cwru_stim_brd2;

  uint8_t sched_id = 1;
  uint8_t pulse_width = 0x64;
  uint8_t amplitude = 0x08;
  uint8_t ipi = FIXED_SCHED_ID1_IPI;


  //setupBoard();

  CyDelay(5000);

  bd_putStringReady("end delay\n");

  cwru_stim_brd1.brd_type = STIM_BRD_ICM_IST16;
  cwru_stim_brd1.port_id = STIM_UART_PORT_1;
  cwru_stim_brd1.setting =
    STIM_SETTING_DEFAULT | STIM_SETTING_SINGLE_SCHEDULER;

  hal_uart_start(cwru_stim_brd1.port_id);
  bd_putStringReady("Uart started\n");
  CyDelay(BD_DELAY);
  checkdata();

  //need to make sure init-pattern and crt_sched ipi match
  stim_init_pattern(&cwru_stim_brd1, UECU_SYNC_MSG, ipi);
  bd_putStringReady("Init Pattern\n");
  CyDelay(BD_DELAY);
  checkdata();

  //initialize the board.
  cwru_stim_brd1.MSG_DES_ADDR = MSG_DES_ADDR_ICM;
  cwru_stim_brd1.STIM_CHANNEL_USED = CHANNEL_MAX_IST;

  stim_cmd_halt_rset(&cwru_stim_brd1, UECU_RESET);

  bd_putStringReady("init brd\n");
  CyDelay(BD_DELAY);
  checkdata();

  //crt_sched
  stim_cmd_crt_sched(&cwru_stim_brd1, UECU_SYNC_MSG, ipi); // Sync signal = 0xAA, duration 29msec.
  bd_putStringReady("crt_sched\n");
  CyDelay(BD_DELAY);
  checkdata();

  //send the setup signal
  stim_uart_print_array(&cwru_stim_brd1, ICM_IST_SET_0_MSG,
      sizeof(ICM_IST_SET_0_MSG)/sizeof(uint8_t));
  bd_putStringReady("icm ist set 0\n");
  CyDelay(BD_DELAY);
  checkdata();

  /*
  //create an event
  stim_cmd_crt_evnt(&cwru_stim_brd1,
      sched_id,  // sched_id = 1
      10,  // delay = 0msec
      0,  // priority = 0
      3,  // event_type = 3, for for Stimulus Event
      8,  // port_chn_id = 0;
      pulse_width, // pulse_width set to 0,
      amplitude, // amplitude set to 0,
      0); // zone not implemented;

  //create another event
  stim_cmd_crt_evnt(&cwru_stim_brd1,
      sched_id,  // sched_id = 1
      10,  // delay = 0msec
      0,  // priority = 0
      3,  // event_type = 3, for for Stimulus Event
      0,  // port_chn_id = 0;
      pulse_width, // pulse_width set to 0,
      amplitude, // amplitude set to 0,
      0); // zone not implemented;
      */

  for (size_t i = 0; i < 12; ++i) {
    stim_cmd_crt_evnt(&cwru_stim_brd1,
        sched_id,    // sched_id = 1
        i*2,    // delay = 0msec
        0,    // priority = 0
        3,    // event_type = 3, for for Stimulus Event
        i,    // port_chn_id = 0;
        pulse_width,    // pulse_width set to 0,
        amplitude, // amplitude set to 0,
        0);   // zone not implemented;
  }


  bd_putStringReady("crt_event\n");
  CyDelay(BD_DELAY);
  checkdata();

  bd_putStringReady("Finish Bd setup\n");

  //send a sync message
  stim_cmd_sync_msg(&cwru_stim_brd1, UECU_SYNC_MSG);
  bd_putStringReady("sync\n");
  CyDelay(BD_DELAY);
  checkdata();

  //send rfpower event
  stim_uart_print_array(&cwru_stim_brd1, ICM_RFPWR_EVNT_0 ,
      sizeof(ICM_RFPWR_EVNT_0)/sizeof(uint8_t));
  CyDelay(BD_DELAY);
  checkdata();

  bd_putStringReady("Start\n");

  for(uint8_t i = 0;; i++) {
    uint8_t data = 0;
    char array[30] = {0};
    char pc_input = 0;

    // Check for input data from PC
    data = UART_STIM_1_GetChar();
    sprintf(array, "%x", data);
    //bd_putLargeDataReady(&data, 1);

    //bd_putStringReady(array);

    pc_input = getPCdata();

    if (pc_input != 0) {
      char new_val[30] = {0};

      switch(pc_input) {
        case '+':
          pulse_width += 5;
          break;

        case '-':
          pulse_width -= 5;
          break;

        case ']':
          amplitude += 1;
          break;

        case '[':
          amplitude -= 1;
          break;

        case ',':
          ipi -= 5;
          stim_cmd_set_sched(&cwru_stim_brd1, sched_id, UECU_SYNC_MSG, ipi);
          break;

        case '.':
          ipi += 5;
          stim_cmd_set_sched(&cwru_stim_brd1, sched_id, UECU_SYNC_MSG, ipi);
          //this command changes
          //stim_cmd_chg_evnt_sched(&cwru_stim_brd1,
          //1, //uint8_t event_id,
          //1, //uint8_t sched_id,
          //, //uint16_t delay,
          //0)
          //
          break;
      }

      stim_cmd_set_evnt(&cwru_stim_brd1,
          1, //uint8_t event_id,
          pulse_width,
          amplitude, 0);

      stim_cmd_set_evnt(&cwru_stim_brd1,
          2, //uint8_t event_id,
          0xff - pulse_width,
          8 - amplitude, 0);

      bd_putStringReady("Change\n");

      sprintf(new_val, "%x %x\n", pulse_width, amplitude);
      bd_putStringReady(new_val);
    }


    //psoc_putStringReady("D: ");
    //bd_putLargeDataReady(&i,
    //1);
    //CyDelay(100);
  }
}

void stimpat_testImplantIntermediate() {
  cwru_stim_struct_t cwru_stim_brd1;
  //cwru_stim_struct_t cwru_stim_brd2;

  uint8_t sched_id = 1;
  uint8_t pulse_width = 0x64;
  uint8_t amplitude = 0x08;
  uint8_t ipi = FIXED_SCHED_ID1_IPI;

  //setupBoard();

  CyDelay(5000);

  bd_putStringReady("end delay\n");

  stimint_initBoardUART(&cwru_stim_brd1, STIM_UART_PORT_1);
  bd_putStringReady("Uart started\n");
  CyDelay(BD_DELAY);
  checkdata();

  stimint_initIST16Board(&cwru_stim_brd1, ipi);


  /*
  for (size_t i = 0; i < 12; ++i) {
    stim_cmd_crt_evnt(&cwru_stim_brd1,
        sched_id,    // sched_id = 1
        i*2,    // delay = 0msec
        0,    // priority = 0
        3,    // event_type = 3, for for Stimulus Event
        i,    // port_chn_id = 0;
        pulse_width,    // pulse_width set to 0,
        amplitude, // amplitude set to 0,
        0);   // zone not implemented;
  }
  */

  bd_putStringReady("Start\n");

  for(uint8_t i = 0;; i++) {
    uint8_t data = 0;
    char array[30] = {0};
    char pc_input = 0;

    // Check for input data from PC
    data = UART_STIM_1_GetChar();
    sprintf(array, "%x", data);
    //bd_putLargeDataReady(&data, 1);

    //bd_putStringReady(array);

    pc_input = getPCdata();

    if (pc_input != 0) {
      char new_val[30] = {0};

      switch(pc_input) {
        case '+':
          pulse_width += 5;
          break;

        case '-':
          pulse_width -= 5;
          break;

        case ']':
          amplitude += 1;
          break;

        case '[':
          amplitude -= 1;
          break;

        case ',':
          ipi -= 5;
          stim_cmd_set_sched(&cwru_stim_brd1, sched_id, UECU_SYNC_MSG, ipi);
          break;

        case '.':
          ipi += 5;
          stim_cmd_set_sched(&cwru_stim_brd1, sched_id, UECU_SYNC_MSG, ipi);
          //this command changes
          //stim_cmd_chg_evnt_sched(&cwru_stim_brd1,
          //1, //uint8_t event_id,
          //1, //uint8_t sched_id,
          //, //uint16_t delay,
          //0)
          //
          break;
      }

      for (size_t i = 0; i < 6; ++i) {
        stim_cmd_set_evnt(&cwru_stim_brd1,
            1 + 2*i, //uint8_t event_id,
            pulse_width,
            amplitude, 0);

        stim_cmd_set_evnt(&cwru_stim_brd1,
            2 + 2*i, //uint8_t event_id,
            0xff - pulse_width,
            8 - amplitude, 0);
      }

      bd_putStringReady("Change\n");

      sprintf(new_val, "%x %x\n", pulse_width, amplitude);
      bd_putStringReady(new_val);
    }
  }
}

void stimpat_testImplantPattern() {
  cwru_stim_struct_t cwru_stim_brd1;
  //cwru_stim_struct_t cwru_stim_brd2;
  stim_pattern_t active_stim_pattern;

  uint8_t sched_id = 1;
  uint8_t pulse_width = 0x64;
  uint8_t amplitude = 0x08;
  uint8_t ipi = FIXED_SCHED_ID1_IPI;

  CyDelay(5000);

  bd_putStringReady("end delay\n");

  /*
  stimpat_initPattern(&active_stim_pattern,
      &gait_walk_L_B1_PP,
      &gait_walk_L_B1_PW,
      gait_walk_L_duration,
      1000);
      */

  const uint8_t STIM_BOARD_IPI = 30;

  stimint_initBoardUART(&cwru_stim_brd1, STIM_UART_PORT_1);
  stimint_initIST16Board(&cwru_stim_brd1, STIM_BOARD_IPI);


  for(uint8_t i = 0;; i++) {
    uint8_t data = 0;
    char array[30] = {0};
    char pc_input = 0;

    // Check for input data from PC
    data = UART_STIM_1_GetChar();
    sprintf(array, "%x", data);
    //bd_putLargeDataReady(&data, 1);

    //bd_putStringReady(array);

    pc_input = getPCdata();

    if (pc_input != 0) {
      switch(pc_input) {
        case '+':
          stimpat_incrementCounter(&active_stim_pattern, 20);
          break;
        case '-':
          break;

        case ' ':
          stimpat_resetTimeAndPercent(&active_stim_pattern);
          stimpat_activatePattern(&active_stim_pattern);

          break;
      }

      bd_putStringReady("Change\n");
    }

    stimpat_applyPatternLoop(&cwru_stim_brd1, &active_stim_pattern, amplitude);
  }
}
