/**
 * \file
 * \author Ryan-David Reyes
 *
 * \brief This is an implementation of a joint object.
 */
#ifndef JOINT_H_
#define JOINT_H_

#include <motor.h>
#include <PID.h>
#include <board_io.h>
#include <stdbool.h>

struct joint_s;

typedef void (*joint_control_func_t)(struct joint_s *, float, float);

/** \brief The joint object */
typedef struct joint_s {
  motor_t motor;
  pid_data_t speed_pid;
  pid_data_t position_pid;

  bd_analog_pins_t current_pin;
  bd_analog_pins_t speed_pin;
  bd_analog_pins_t position_pin;
  size_t lock_pin;

  bd_analog_to_knee_angle_func_t analog_to_knee_angle_func;

  joint_control_func_t control_func;

  bool locked;
  bool force_motor_off;

  float setpoint;
  float current;
  float speed;
  float position;
  float speed_command;
  float current_command;
  float fc_alpha;     //friction compensator fudge factor
  float bc_threshold; //breakaway speed threshold
  float bc_current;   //breakaway commanded current
} joint_t;


/**
 * \brief Initializes the motor associated with the joint.
 *
 * \param joint The joint structure.
 * \param enable_pin The address of the enable pin
 * \param direction_pin The address of the direction pin
 * \param reversed Whether the motor needs to have its commanded directions
 * reversed.
 * \param pwm_fun Function used to write a pwm to the associated pin.
 * \param pwm_setup Function used to set up the pwm.
 */
void joint_initMotor(joint_t *joint,
    size_t enable_pin,
    size_t direction_pin,
    bool reversed,
    pwm_pin_write_func_t pwm_func,
    pwm_pin_setup_func_t pwm_setup);

/**
 * \brief Enable the motor at the joint.
 *
 * \param joint The joint structure.
 */
inline void joint_enableMotor(joint_t *joint) {
  if (joint->force_motor_off)
    return;

  mtr_enable(&joint->motor);
}

/**
 * \brief set force motor off override state. Setting this makes the motors off
 * and stay off in any circumstance.
 *
 * \param joint The joint structure.
 * \param motor_off Whether the joint motor is forced on or off
 */
inline void joint_setMotorOffOverride(joint_t *joint, bool motor_off) {
  joint->force_motor_off = motor_off;
}

/**
 * \brief Set the joint's friction compensation factor.
 *
 * \param joint The joint structure.
 */

inline void joint_initFC(joint_t *joint, float alpha) {
  joint->fc_alpha = alpha;
}


/**
 * \brief Disable the motor at the joint.
 *
 * \param joint The joint structure.
 */
inline void joint_disableMotor(joint_t *joint) {
  mtr_disable(&joint->motor);
}


/**
 * \brief Initialize the speed PID for motor control
 *
 * \param joint The joint structure.
 * \param proportional_gain Kp for the control algorithm.
 * \param integral_gain Ki for the control algorithm.
 * \param derivative_gain Kd for the control algorithm.
 * \param max_integral_guard Maximum constraint for antiwindup.
 * \param min_integral_guard Minimum constraint for antiwindup.
 */
void joint_initSpeedPID(joint_t *joint,
    float proportional_gain,
    float integral_gain,
    float derivative_gain,
    float max_integral_guard,
    float min_integral_guard);


/**
 * \brief Initialize the postion PID for motor control
 *
 * \param joint The joint structure.
 * \param proportional_gain Kp for the control algorithm.
 * \param integral_gain Ki for the control algorithm.
 * \param derivative_gain Kd for the control algorithm.
 * \param max_integral_guard Maximum constraint for antiwindup.
 * \param min_integral_guard Minimum constraint for antiwindup.
 */
void joint_initPositionPID(joint_t *joint,
    float proportional_gain,
    float integral_gain,
    float derivative_gain,
    float max_integral_guard,
    float min_integral_guard);

/**
 * \brief Initialize the analog pins associated with the joint.
 *
 * \param joint The joint structure.
 * \param analog_to_knee_angle_func Function to turn analog reading to correct
 * angle for the joint.
 * \param current_pin Analog pin connected to current signal.
 * \param speed_pin Analog pin connected to speed signal.
 * \param position_pin Analog pin connected to encoder signal.
 * \parma lock_pin Pin connected to the hydraulic lock.
 */
void joint_initIO(joint_t *joint,
    bd_analog_to_knee_angle_func_t analog_to_knee_angle_func,
    bd_analog_pins_t current_pin,
    bd_analog_pins_t speed_pin,
    bd_analog_pins_t position_pin,
    size_t lock_pin);

/**
 * \brief Initialize the internal values for the joint.
 *
 * \param joint The joint structure.
 */
void joint_initValues(joint_t *joint);


/**
 * \brief Run the control algorithm for the joint.
 *
 * \param joint The joint structure.
 * \param setpoint The angle setpoint.
 * \param dt The time since last control iteration.
 */
inline void joint_runSwingCtrl(joint_t *joint, float setpoint, float dt) {
  (*joint->control_func)(joint, setpoint, dt);
}

/**
 * \brief Run the PID algorithm for the joint.
 *
 * \param joint The joint structure.
 * \param setpoint The angle setpoint.
 * \param dt The time since last control iteration.
 */
void joint_runPID(joint_t *joint, float setpoint, float dt);

/**
 * \brief Update the joint speed, current, and angle.
 *
 * \param joint The joint structure.
 * \param sensor_inputs The sensor input structure.
 */
void joint_measure(joint_t *joint, struct bd_inputs_s *sensor_inputs);

/**
 * \brief Run the friction compensation algorithm for the joint.
 *
 * \param joint The joint structure.
 * \param setpoint The angle setpoint.
 * \param dt The time since last control iteration.
 */
void joint_runFC(joint_t *joint, float setpoint, float dt);

/**
 * \brief Locks the joint.
 */
void joint_lock(joint_t *joint);

/**
 * \brif Unlocks the joint.
 */
void joint_unlock(joint_t *joint);


/**
 * \brief Directly command a joint current
 *
 * \param joint The joint object.
 * \param current The commanded current.
 */
inline void joint_commandCurrent(joint_t *joint, float current) {
  joint->current_command = current;
  mtr_commandCurrent(&joint->motor, joint->current_command);
}


/**
 * \brief Run breakaway torque compensation
 *
 * \param joint The joint object.
 * \param current The commanded current.
 */
void joint_runBC(joint_t *joint, float setpoint, float dt);

#endif
