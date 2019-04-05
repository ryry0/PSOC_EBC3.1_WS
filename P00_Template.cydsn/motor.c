#include "motor.h"
#include <math.h>
#include <stdlib.h>
#include <numerical.h>
#include <board_io.h>
//TODO: clean up interface for direction


uint16_t mtr_currentToPWM(float current) {
  const float CURR_SLOPE = 0.0615;
  const float CURR_OFF = 0.1;
  const float PERC_MAX = 0.9;
  const float PERC_MIN = 0.1;

  const uint16_t PWM_MAX = 3000;
  const uint16_t PWM_MIN = 0;

  float percent = CURR_SLOPE*fabs(current) + CURR_OFF;
  percent = nm_constrain(percent, PERC_MIN, PERC_MAX);

  return nm_constrain(PWM_MAX * percent, PWM_MIN, PWM_MAX);
}


uint16_t mtr_currentToPWMDir(float current) {
  const float CURR_SLOPE = 0.8/26;
  const float CURR_OFF = 0.5;
  const float PERC_MAX = 0.9;
  const float PERC_MIN = 0.1;

  const uint16_t PWM_MAX = 3000;
  const uint16_t PWM_MIN = 0;

  float percent = CURR_SLOPE*current + CURR_OFF;
  percent = nm_constrain(percent, PERC_MIN, PERC_MAX);

  return nm_constrain(PWM_MAX * percent, PWM_MIN, PWM_MAX);
}

float mtr_analogToVolt(int16_t analog_count) {
  const float M = 1.0/4096.0;
  const float B = 5.0;

  return M*analog_count*B;
}

void mtr_init(motor_t *motor,
    size_t enable_pin,
    size_t direction_pin,
    bool reversed,
    pwm_pin_write_func_t pwm_func,
    pwm_pin_setup_func_t pwm_setup) {

  motor->enable_pin = enable_pin;
  motor->direction_pin = direction_pin;
  motor->pwm_func = pwm_func;
  motor->pwm_setup = pwm_setup;
  motor->reversed = reversed;

  (*motor->pwm_setup)(); //run the pwm setup function.
  mtr_disable(motor);
}

void mtr_enable(motor_t *motor) {
  bd_setPin(motor->enable_pin);
  motor->enabled = true;
}

void mtr_disable(motor_t *motor) {
  bd_clearPin(motor->enable_pin);
  motor->enabled = false;
}

void mtr_move(motor_t *motor, mtr_direction_t direction, uint16_t throttle) {
  motor->throttle = throttle;

  switch(direction) {
    case MTR_DIR_POS:
      bd_setPin(motor->direction_pin);
      motor->direction = MTR_DIR_POS;
      break;

    case MTR_DIR_NEG:
      bd_clearPin(motor->direction_pin);
      motor->direction = MTR_DIR_NEG;
      break;
  }

  (*motor->pwm_func)(throttle); //write throttle to pwm
}

static inline mtr_direction_t mtr_setDir(float current, bool reversed) {
  mtr_direction_t direction;

  direction = MTR_DIR_POS;
  if (current < 0.0)
    direction = MTR_DIR_NEG;

  if (reversed)
    direction = !direction;

  return direction;
}

void mtr_commandCurrent(motor_t *motor, float current) {
  if (motor->reversed)
    current = -current;

  mtr_move(motor, mtr_setDir(current, motor->reversed),
      mtr_currentToPWMDir(current));
}

float mtr_analogToCurrent(uint16_t analog_count) {
  const float M = 6.5;
  const float B = -13.0;

  const float volts = mtr_analogToVolt(analog_count);

  const float motor_current = M*volts + B;

  return motor_current;
}

float mtr_analogToRPM(int16_t analog_count) {
  const float M = 3000.0;
  const float B = -6000.0;
  const float volts = mtr_analogToVolt(analog_count);

  return M*volts+B;
}

float mtr_analogToDPS(int16_t analog_count) {
  return mtr_RPMToDPS(mtr_analogToRPM(analog_count));
}

float mtr_analogToOutDPS(int16_t analog_count) {
  return
    mtr_inputToOutputDPS(
        mtr_RPMToDPS(
          mtr_analogToRPM(analog_count)));
}


float mtr_RPMToDPS(float rpm) {
  return (360.0*rpm)/60.0;
}

float mtr_inputToOutputDPS(float input_deg_sec) {
  const float GEAR_RATIO = 1.0/100.0;
  return GEAR_RATIO*input_deg_sec;
}
