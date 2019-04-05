/**
 * \file
 *
 * \author Ryan-David Reyes
 *
 * \brief This file contains the implementation of the abstraction layer
 * tailored for the psoc.
 */
#include <board_io.h>
#include <project.h>
#include <stdlib.h>
#include <numerical.h>

/** \brief This function reads all the analog inputs */
static void bd_readAin(uint16_t *a_in) {
  AMuxSeq_1_Start(); //make sure it starts at 0
  for (size_t i = 0; i < BD_AN_PIN_MAX; i++) {
    AMuxSeq_1_Next();
    ADC_SAR_1_StartConvert();
    ADC_SAR_1_IsEndConversion(ADC_SAR_1_WAIT_FOR_RESULT);

    a_in[i] = ADC_SAR_1_GetResult16(); //get the result
    if (AMuxSeq_1_GetChannel() == -1)
      break;
  }
}

/** \brief This function reads all the fingerswitches */
static void bd_readFSWEdge(struct bd_fsw_s *fsw) {
  fsw->current_val = CyPins_ReadPin(fsw->pin);

  if ((fsw->current_val == false) &&
      (fsw->prev_val == true))
    fsw->falling_edge = true;
  else
    fsw->falling_edge = false;

  fsw->prev_val = fsw->current_val;
}

void bd_readSensors(bd_inputs_t *inputs) {
  bd_readAin(inputs->a_in);
  for (size_t i = 0; i < BD_FSW_MAX; ++i) {
    bd_readFSWEdge(&inputs->fsws[i]);
  }
}

void bd_setFSWPins(bd_inputs_t *inputs, size_t green_fsw_pin, size_t
    blue_fsw_pin, size_t red_fsw_pin, size_t white_fsw_pin) {
 inputs->fsws[BD_GREEN_FSW].pin = green_fsw_pin;
 inputs->fsws[BD_BLUE_FSW].pin = blue_fsw_pin;
 inputs->fsws[BD_RED_FSW].pin = red_fsw_pin;
 inputs->fsws[BD_WHITE_FSW].pin = white_fsw_pin;
}

float bd_analogToRKneeAngle(uint16_t analog_count) {
  //you need both of these measurements because the encoder is mounted mirrored
  const float AN_ZERO = 2088;
  const float AN_RANGE = (3.3/5.0)*4096;
  const float POT_RANGE = 340;

  return -(analog_count - AN_ZERO)/AN_RANGE*POT_RANGE;
}

float bd_analogToLKneeAngle(uint16_t analog_count) {
  const float AN_ZERO = 704;
  const float AN_RANGE = (3.3/5.0)*4096;
  const float POT_RANGE = 340;

  //const float AN_MAX_FLEX = 148;
  //const float M = BD_MAX_KNEE_FLEXION_DEG/(AN_MAX_FLEX - AN_ZERO);

  return (analog_count - AN_ZERO)/AN_RANGE*POT_RANGE;
}

void bd_usbTimeout() {
  int8_t led = 0;
  LED_R_Write(led);
  while(!USBUART_1_CDCIsReady()) {
    led=!led;
    LED_R_Write(led);
  }
  led = 1;
  LED_R_Write(led);
}

void bd_putStringReady(char *str) {
  USBUART_1_PutString(str);
  bd_usbTimeout();
}

void bd_putDataReady(uint8_t *data, size_t len) {
  USBUART_1_PutData(data, len);
  bd_usbTimeout();
}

void bd_delay(int32_t num) {
  for (volatile int32_t i = 0; i < num; ++i)
    for (volatile int32_t b = 0; b < 10000; b++);

}

void bd_putLargeDataReady(uint8_t *data, size_t len) {
  const size_t MAX_BUFFER_LEN = 0x39;
  //technically 0x40, but then need to send zero length packet
  size_t buffer_size = MAX_BUFFER_LEN;
  for (size_t index = 0; index < len; index+=buffer_size) {
    buffer_size = nm_constrain(len - index, 0, MAX_BUFFER_LEN);
    bd_putDataReady(data + index, buffer_size);
  }
}

void bd_setPin(size_t addr) {
  CyPins_SetPin(addr);
}

void bd_clearPin(size_t addr) {
  CyPins_ClearPin(addr);
}
