/**
 * \file
 *
 * \author Ryan-David Reyes
 *
 * \brief This file contains an abstraction layer to interface with the
 * psoc.
 */
#ifndef BOARD_IO_H_
#define BOARD_IO_H_
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

//@{
/** Board specific sensor conversion constants */
#define BD_NUM_ANALOG 13
//maximum flexion in degrees
#define BD_MAX_KNEE_FLEXION_DEG 125
#define BD_HIP_ANGLE_SLOPE (360.0/2703.0)
#define BD_R_HIP_ZERO_OFF (-255.98)
#define BD_L_HIP_ZERO_OFF (-255.98)
//@}

typedef enum bd_analog_pins_e {
  BD_AN_0_L_FSR_HEEL = 0,
  BD_AN_2_R_FSR_HEEL,
  BD_AN_4_L_HIP_ENC,
  BD_AN_6_L_KNEE_ENC,
  BD_AN_12_MOT1_ANOUT1,
  BD_AN_14_MOT2_ANOUT1,
  BD_AN_1_L_FSR_FF,
  BD_AN_3_R_FSR_FF,
  BD_AN_5_R_HIP_ENC,
  BD_AN_7_R_KNEE_ENC,
  BD_AN_11_BAT_VOLT,
  BD_AN_13_MOT1_ANOUT2,
  BD_AN_15_MOT2_ANOUT2,
  BD_AN_PIN_MAX
} bd_analog_pins_t;

typedef enum bd_fsw_pins_e {
  BD_GREEN_FSW,
  BD_BLUE_FSW,
  BD_RED_FSW,
  BD_WHITE_FSW,
  BD_FSW_MAX
} bd_fsw_pins_t;

typedef float (*bd_analog_to_knee_angle_func_t)(uint16_t);

/** this struct is used to detect fingerswitch rising edges. */
struct bd_fsw_s {
  bool falling_edge;
  bool current_val;
  bool prev_val;
  size_t pin;
};

/* Function prototypes */

/** This structure represents the board inputs */
typedef struct bd_inputs_s {
  uint16_t a_in[BD_AN_PIN_MAX];
  struct bd_fsw_s fsws[BD_FSW_MAX];
} bd_inputs_t;

/**
 * \brief Read all the sensors on the hybrid
 *
 * \param inputs The input structure storing all the updated values.
 */
void bd_readSensors(bd_inputs_t *inputs);

/**
 * \brief Set the pins for the fsw structs.
 *
 * \param inputs the input structure
 * \param green_fsw_pin address of green pin
 * \param blue_fsw_pin address of blue pin
 * \param red_fsw_pin address of red pin
 * \param white_fsw_pin address of white pin
 */
void bd_setFSWPins(bd_inputs_t *inputs, size_t green_fsw_pin, size_t
    blue_fsw_pin, size_t red_fsw_pin, size_t white_fsw_pin);

/**
 * \brief Return the value of a particular analog pin.
 *
 * \param inputs the input structure
 * \param an_pin_num Pin address from the enum
 */
inline uint16_t bd_getAin(const bd_inputs_t *inputs, bd_analog_pins_t an_pin_num) {
  return inputs->a_in[an_pin_num];
}


/**
 * \brief Return the value of a fingerswitch state
 *
 * \param inputs the input structure
 * \param an_pin_num Pin address from the enum
 */
inline bool bd_getFSW(const bd_inputs_t *inputs, bd_fsw_pins_t fsw_pin_num) {
  return inputs->fsws[fsw_pin_num].falling_edge;
}

/**
 * \brief Calculate the knee angle from analog readings.
 *
 * \param analog_count The analog reading.
 */
float bd_analogToRKneeAngle(uint16_t analog_count);

/**
 * \brief Calculate the knee angle from analog readings.
 *
 * \param analog_count The analog reading.
 */
float bd_analogToLKneeAngle(uint16_t analog_count);

/**
 * \brief Calculate the hip angle from analog readings.
 *
 * \param analog_count The analog reading.
 */
inline float bd_analogToLHipAngle(uint16_t analog_count, float zero_offset) {
  return BD_HIP_ANGLE_SLOPE*analog_count + zero_offset;
}

/**
 * \brief Calculate the hip angle from analog readings.
 *
 * \param analog_count The analog reading.
 */
inline float bd_analogToRHipAngle(uint16_t analog_count, float zero_offset) {
  return -(BD_HIP_ANGLE_SLOPE*analog_count + zero_offset);
}

/**
 * \brief Back Calculate the analog reading from hip angle.
 *
 * \param angle The hip angle.
 */
inline uint16_t bd_lHipAngleToAnalog(float angle, float zero_offset) {
  return (angle - zero_offset)/(float)BD_HIP_ANGLE_SLOPE;
}

inline uint16_t bd_rHipAngleToAnalog(float angle, float zero_offset) {
  return (angle + zero_offset)/(float)-BD_HIP_ANGLE_SLOPE;
}

/**
 * \brief Looping delay, not tested for actual timing correctness.
 */
void bd_delay(int32_t num);

/**
 * \brief Sends string and waits until usb is ready for another transfer.
 */
void bd_putStringReady(char *str);

/**
 * \brief Sends data and waits until usb is ready for another transfer.
 *
 * Works for lengths < 64
 */
void bd_putDataReady(uint8_t *data, size_t len);

/**
 * \brief Waits until the usb is ready for another transfer.
 */
void bd_usbTimeout();

/**
 * \brief Sends data and waits until usb is ready for another transfer.
 *
 * Works for any length
 */
void bd_putLargeDataReady(uint8_t *data, size_t len);


/**
 * \brief Sets a GPIO Pin High
 */
void bd_setPin(size_t addr);

/**
 * \brief Sets a GPIO Pin Low
 */
void bd_clearPin(size_t addr);
#endif
