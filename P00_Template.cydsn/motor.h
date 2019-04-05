/** \file
 * \author Ryan-David Reyes
 *
 * \brief This file contains an easy to use interface to the motors.
*/

#ifndef MOTOR_H_
#define MOTOR_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

//@{
/** \brief Maxon motor specific constants */
#define MTR_MAX_CURRENT 13.0
#define MTR_MIN_CURRENT -13.0
#define MTR_MAX_SPEED 324
#define MTR_MIN_SPEED -324
//@}

/* types */
/** \brief The directions the motor can move in */
typedef enum mtr_direction_e {
  MTR_DIR_POS,
  MTR_DIR_NEG
} mtr_direction_t;

//@{
/** \brief Function pointer typedefs for dealing with abstracted PWMS. */
typedef void (*pwm_pin_write_func_t)(uint16_t);
typedef void (*pwm_pin_setup_func_t)(void);
typedef uint16_t (*current_to_pwm_func_t)(float);
//@}

/** \brief Structure representing motors. */
typedef struct motor_s {
  size_t enable_pin;
  size_t direction_pin;

  pwm_pin_write_func_t pwm_func;
  pwm_pin_setup_func_t pwm_setup;

  uint16_t throttle;
  mtr_direction_t direction;
  bool enabled;
  bool reversed;

} motor_t;

/* function declarations */
/**
 * \brief Initialize the motor structure.
 *
 * \param motor The motor structure.
 * \param enable_pin Memory address of the enable pin.
 * \param direction_pin Memory address of the direction pin.
 * \param pwm_func Function that sets the PWM value on the board.
 * \param pwm_setup Function that initializes the PWM for that pin.
 */
void mtr_init(motor_t *motor,
    size_t enable_pin,
    size_t direction_pin,
    bool reversed,
    pwm_pin_write_func_t pwm_func,
    pwm_pin_setup_func_t pwm_setup);

/**
 * \brief Enable the motor.
 *
 * \param motor The motor structure.
 */
void mtr_enable(motor_t *motor);


/**
 * \brief Disable the motor.
 *
 * \param motor The motor structure.
 */
void mtr_disable(motor_t *motor);


/**
 * \brief Move the motor.
 *
 * \param motor The motor structure.
 * \param direction The direction which the motor will spin.
 * \param throttle PWM value sent to the motor.
 */
void mtr_move(motor_t *motor, mtr_direction_t direction,
    uint16_t throttle);


/**
 * \brief Move the motor via a commanded current
 *
 * The current command will be translated into a PWM value.
 * The current can be (-13, 13).
 *
 * \param motor The motor structure.
 * \param current The commanded current.
 */
void mtr_commandCurrent(motor_t *motor, float current);

/**
 * \brief Translate the current into PWM value.
 *
 * \param current The current.
 * \return PWM value.
 */
uint16_t mtr_currentToPWM(float current);


/**
 * \brief Translate the current into PWM value with direction.
 * Used when ESCON does not use the direction pin.
 *
 * -13 @ 10% 13 @ 90% 50% = 0
 *
 * \param current The current.
 * \return PWM value.
 */
uint16_t mtr_currentToPWMDir(float current);

/**
 * \brief Translate analog values into RPM values.
 *
 * \param analog_count The 12bit analog reading.
 * \return RPM value.
 */
float mtr_analogToRPM(int16_t analog_count);

/**
 * \brief Translate analog values into degrees per second values.
 *
 * \param analog_count The 12bit analog reading.
 * \return DPS value.
 */
float mtr_analogToDPS(int16_t analog_count);

/**
 * \brief Translate analog values into degrees per second value, on the output
 * shaft of the transmission.
 *
 * \param analog_count The 12bit analog reading.
 * \return DPS value.
 */
float mtr_analogToOutDPS(int16_t analog_count);

/**
 * \brief Translate analog values into current values.
 *
 * \param analog_count The 12bit analog reading.
 * \return current value.
 */
float mtr_analogToCurrent(uint16_t analog_count);

/**
 * \brief Translate analog values into voltage values.
 *
 * \param analog_count The 12bit analog reading.
 * \return voltage value.
 */
float mtr_analogToVolt(int16_t analog_count);

/**
 * \brief Translate RPM to degrees per second
 *
 * \param rpm The RPM
 * \return DPS value.
 */
float mtr_RPMToDPS(float rpm);

/**
 * \brief Translate input shaft degrees per second to output shaft.
 *
 * \param input_deg_sec Input shaft degrees per second.
 * \return Output shaft DPS.
 */
float mtr_inputToOutputDPS(float input_deg_sec);

/**
 * \brief Returns whether the motor is enabled or not.
 *
 * \param motor The motor structure.
 * \return Motor enabled state.
 */
inline bool mtr_isEnabled(motor_t *motor) {
  return motor->enabled;
}

/**
 * \brief Returns whether the motor is reversed or not.
 *
 * \param motor The motor structure.
 * \return Motor reversed or not.
 */
inline bool mtr_isReversed(motor_t *motor) {
  return motor->reversed;
}
#endif
