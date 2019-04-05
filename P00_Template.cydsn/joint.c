#include <joint.h>
#include <friction_compensator.h>

void joint_initMotor(joint_t *joint,
    size_t enable_pin,
    size_t direction_pin,
    bool reversed,
    pwm_pin_write_func_t pwm_func,
    pwm_pin_setup_func_t pwm_setup) {

  mtr_init(&joint->motor,
      enable_pin,
      direction_pin,
      reversed,
      pwm_func,
      pwm_setup);
}

void joint_initSpeedPID(joint_t *joint,
    float proportional_gain,
    float integral_gain,
    float derivative_gain,
    float max_integral_guard,
    float min_integral_guard) {

  pid_init(&joint->speed_pid,
      proportional_gain,
      integral_gain,
      derivative_gain,
      max_integral_guard,
      min_integral_guard);
}

void joint_initPositionPID(joint_t *joint,
    float proportional_gain,
    float integral_gain,
    float derivative_gain,
    float max_integral_guard,
    float min_integral_guard) {

  pid_init(&joint->position_pid,
      proportional_gain,
      integral_gain,
      derivative_gain,
      max_integral_guard,
      min_integral_guard);
}

void joint_initIO(joint_t *joint,
    bd_analog_to_knee_angle_func_t analog_to_knee_angle_func,
    bd_analog_pins_t current_pin,
    bd_analog_pins_t speed_pin,
    bd_analog_pins_t position_pin,
    size_t lock_pin) {

  joint->analog_to_knee_angle_func = analog_to_knee_angle_func;
  joint->current_pin = current_pin;
  joint->speed_pin = speed_pin;
  joint->position_pin = position_pin;
  joint->lock_pin = lock_pin;
}

void joint_initValues(joint_t *joint) {
  joint->setpoint = 0;
  joint->current = 0;
  joint->speed = 0;
  joint->position = 0;
  joint->speed_command = 0;
  joint->current_command = 0;
  joint->fc_alpha = 0;
  joint->bc_threshold = 0.0;
  joint->bc_current = 0.0;
  joint->control_func = joint_runPID;
  joint->force_motor_off = false;
}

void joint_measure(joint_t *joint, bd_inputs_t *sensor_inputs) {
  joint->current =
    mtr_analogToCurrent(bd_getAin(sensor_inputs, joint->current_pin));

  joint->speed =
    mtr_analogToOutDPS(bd_getAin(sensor_inputs, joint->speed_pin));

  joint->position =
    (*joint->analog_to_knee_angle_func)(bd_getAin(sensor_inputs,
          joint->position_pin));

  if (mtr_isReversed(&joint->motor)) {
    joint->current = -joint->current;
    joint->speed = -joint->speed;
  }
}

void joint_runPID(joint_t *joint, float setpoint, float dt) {
  if (!mtr_isEnabled(&joint->motor))
    return;

  joint->setpoint = setpoint;

  joint->speed_command = pid_FeedbackCtrl(&joint->position_pid,
      joint->setpoint, joint->position, dt, pid_velocUpdate);

  joint_commandCurrent(joint,
      pid_FeedbackCtrl(&joint->speed_pid,
      joint->speed_command, joint->speed, dt,
      pid_minPIUpdate));
}

void joint_runFC(joint_t *joint, float setpoint, float dt) {
  if (!mtr_isEnabled(&joint->motor))
    return;

  joint_commandCurrent(joint, fc_compensateExp(joint->speed, joint->fc_alpha));
}

void joint_runBC(joint_t *joint, float setpoint, float dt) {
  if (!mtr_isEnabled(&joint->motor))
    return;

  joint_commandCurrent(joint, joint->bc_current);
  if(fabs(joint->speed) > joint->bc_threshold) {
    joint->control_func = joint_runFC;
  }
}

void joint_lock(joint_t *joint) {
  bd_clearPin(joint->lock_pin);
}

void joint_unlock(joint_t *joint) {
  bd_setPin(joint->lock_pin);
}
