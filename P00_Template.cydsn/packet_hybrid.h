#ifndef PACKET_HYBRID_H_
#define PACKET_HYBRID_H_

#ifdef ARDUINO
#include "packet.h"
#else
#include <packet.h>
#endif

typedef enum pkt_type_e {
  PKT_ACK         = 0x0A,
  PKT_SET_PID,
  PKT_GET_PID,
  PKT_DBG_PID,
  PKT_ZERO_PID,
  PKT_STR_MSG,
  PKT_LOG_DATA,
  PKT_LOG_PARAMS,
  PKT_SET_PARAMS,
  PKT_GET_PARAMS,
  PKT_SET_STATE,
  PKT_CALIBRATE_KNEE,
  PKT_CALIBRATE_LHIP,
  PKT_CALIBRATE_RHIP,
  PKT_QUERY_MODULE,
  PKT_MODULE_DATA,
  PKT_LOG_CAN_DATA,
  PKT_MODULE_SET_PARAMS,
  PKT_MODULE_GET_PARAMS,
  PKT_MODULE_GET_PARAMS_REPLY,
  PKT_MODULE_SET_SHARED_PARAMS,
  PKT_MODULE_SET_SIT_TO_STAND_PARAMS,
  PKT_MODULE_SET_BURST_PARAMS,
  PKT_SHUTDOWN_STIM,
  PKT_REENABLE_STIM,
  PKT_STIM_CONFIGURE,
  PKT_LEFT_BIOTILC,
  PKT_RIGHT_BIOTILC,
  PKT_LOG_LEFT_BIOTILC,
  PKT_LOG_RIGHT_BIOTILC,
  PKT_DATA_START  = 's',
  PKT_DATA_STOP   = 'n',
  PKT_EN_MOTOR    = 'e',
  PKT_DIS_MOTOR   = 'd',
  PKT_SYN         = 0xFF
} pkt_type_t;


/** Macro to easily add parameters and ensure that they are synced with gui
 * labels */
#define FOREACH_PARAM(PARAM)                      \
  PARAM(PKT_HEAD_MODULE_FC_ALPHA)                 \
  PARAM(PKT_HEAD_MODULE_BC_TORQUE)                \
  PARAM(PKT_HEAD_MODULE_BC_THRESH)                \
  PARAM(PKT_HEAD_MODULE_R_WEIGHT_ACC_FSR_THRESH)  \
  PARAM(PKT_HEAD_MODULE_R_LOW_FSR_THRESH)         \
  PARAM(PKT_HEAD_MODULE_R_HIGH_FSR_THRESH)        \
  PARAM(PKT_HEAD_MODULE_L_WEIGHT_ACC_FSR_THRESH)  \
  PARAM(PKT_HEAD_MODULE_L_LOW_FSR_THRESH)         \
  PARAM(PKT_HEAD_MODULE_L_HIGH_FSR_THRESH)        \
  PARAM(PKT_HEAD_MODULE_L_KNEE_EXTENSION_THRESH)  \
  PARAM(PKT_HEAD_MODULE_L_HIP_FLEXION_THRESH)     \
  PARAM(PKT_HEAD_MODULE_R_KNEE_EXTENSION_THRESH)  \
  PARAM(PKT_HEAD_MODULE_R_HIP_FLEXION_THRESH)     \
  PARAM(PKT_HEAD_MODULE_BURST_FLEXION_TORQUE)     \
  PARAM(PKT_HEAD_MODULE_BURST_EXTENSION_TORQUE)   \
  PARAM(PKT_HEAD_MODULE_BURST_TIME)               \
  PARAM(PKT_HEAD_MODULE_STS_TORQUE)               \
  PARAM(PKT_HEAD_MODULE_MODE)                     \
  PARAM(PKT_HEAD_MODULE_AUTO_WALK)                \
  PARAM(PKT_HEAD_MODULE_MOTOR_FORCE_OFF)          \
  PARAM(PKT_HEAD_MODULE_PARAM_MAX)                \


#define FOREACH_MODULE_PARAM(PARAM)     \
  PARAM(PKT_MODULE_FC_ALPHA)            \
  PARAM(PKT_MODULE_BC_TORQUE)           \
  PARAM(PKT_MODULE_BC_THRESH)           \
  PARAM(PKT_MODULE_BURST_FLEXION_TORQUE)     \
  PARAM(PKT_MODULE_BURST_EXTENSION_TORQUE)   \
  PARAM(PKT_MODULE_BURST_TIME)               \
  PARAM(PKT_MODULE_STS_TORQUE)               \
  PARAM(PKT_MODULE_MOTOR_FORCE_OFF)     \
  PARAM(PKT_MODULE_PARAM_MAX)           \

#define FOREACH_SHARED_MODULE_PARAM(PARAM)  \
  PARAM(PKT_SHARED_BC_TORQUE)               \
  PARAM(PKT_SHARED_BC_THRESH)               \
  PARAM(PKT_SHARED_ASSIST_TORQUE)           \
  PARAM(PKT_SHARED_ASSIST_SWITCH_ANGLE)     \
  PARAM(PKT_SHARED_MODE)                    \
  PARAM(PKT_SHARED_MAX)                     \

//sit to stand enum generator
#define FOREACH_MODULE_STS_PARAM(PARAM) \
  PARAM(PKT_MODULE_SHARED_STS_TORQUE)   \
  PARAM(PKT_MODULE_STS_MAX)             \

#define FOREACH_MODULE_SHARED_BURST_PARAM(PARAM)   \
  PARAM(PKT_MODULE_SHARED_BURST_FLEXION_TORQUE)    \
  PARAM(PKT_MODULE_SHARED_BURST_EXTENSION_TORQUE)  \
  PARAM(PKT_MODULE_SHARED_BURST_TIME)              \
  PARAM(PKT_MODULE_SHARED_BURST_MAX)               \

#define FOREACH_STIM_CONFIG(PARAM)      \
  PARAM(PKT_STIM_SCALING_FACTOR)        \
  PARAM(PKT_L_KNEE_EXTEND_ASSIST_ANGLE) \
  PARAM(PKT_R_KNEE_EXTEND_ASSIST_ANGLE) \
  PARAM(PKT_STIM_CONFIG_MAX)            \

#define FOREACH_BIOTILC(PARAM)        \
  PARAM(PKT_BT_HIP_FLEXION_TORQUE)    \
  PARAM(PKT_BT_KNEE_FLEXION_TORQUE)   \
  PARAM(PKT_BT_KNEE_EXTENSION_TORQUE) \
  PARAM(PKT_BT_HIP_FLEXION_SF)        \
  PARAM(PKT_BT_KNEE_FLEXION_SF)       \
  PARAM(PKT_BT_KNEE_EXTENSION_SF)     \
  PARAM(PKT_BT_MAX)                   \

#define FOREACH_LOG_BIOTILC(PARAM)        \
  PARAM(PKT_LOG_BT_HIP_FLEXION_TORQUE)    \
  PARAM(PKT_LOG_BT_KNEE_FLEXION_TORQUE)   \
  PARAM(PKT_LOG_BT_KNEE_EXTENSION_TORQUE) \
  PARAM(PKT_LOG_BT_HIP_FLEXION_SF)        \
  PARAM(PKT_LOG_BT_KNEE_FLEXION_SF)       \
  PARAM(PKT_LOG_BT_KNEE_EXTENSION_SF)     \
  PARAM(PKT_LOG_HIP_FLEXION_ERROR)        \
  PARAM(PKT_LOG_KNEE_FLEXION_ERROR)       \
  PARAM(PKT_LOG_KNEE_EXTENSION_ERROR)     \
  PARAM(PKT_LOG_PSI_11)                   \
  PARAM(PKT_LOG_PSI_12)                   \
  PARAM(PKT_LOG_PSI_13)                   \
  PARAM(PKT_LOG_PSI_14)                   \
  PARAM(PKT_LOG_PSI_15)                   \
  PARAM(PKT_LOG_PSI_16)                   \
  PARAM(PKT_LOG_PSI_21)                   \
  PARAM(PKT_LOG_PSI_22)                   \
  PARAM(PKT_LOG_PSI_23)                   \
  PARAM(PKT_LOG_PSI_24)                   \
  PARAM(PKT_LOG_PSI_25)                   \
  PARAM(PKT_LOG_PSI_26)                   \
  PARAM(PKT_LOG_PSI_31)                   \
  PARAM(PKT_LOG_PSI_32)                   \
  PARAM(PKT_LOG_PSI_33)                   \
  PARAM(PKT_LOG_PSI_34)                   \
  PARAM(PKT_LOG_PSI_35)                   \
  PARAM(PKT_LOG_PSI_36)                   \
  PARAM(PKT_LOG_BT_MAX)                   \

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

typedef enum pkt_param_e {
  FOREACH_PARAM(GENERATE_ENUM)
} pkt_param_t;

typedef enum pkt_module_param_e {
  FOREACH_MODULE_PARAM(GENERATE_ENUM)
} pkt_module_param_t;

typedef enum pkt_shared_module_param_e {
  FOREACH_SHARED_MODULE_PARAM(GENERATE_ENUM)
} pkt_shared_module_param_t;

typedef enum pkt_set_module_sts_param_e {
  FOREACH_MODULE_STS_PARAM(GENERATE_ENUM)
} pkt_set_module_sts_param_t;

typedef enum pkt_set_module_burst_param_e {
  FOREACH_MODULE_SHARED_BURST_PARAM(GENERATE_ENUM)
} pkt_set_module_burst_param_t;

enum pkt_stim_config_e {
  FOREACH_STIM_CONFIG(GENERATE_ENUM)
};

enum pkt_biotilc_e {
  FOREACH_BIOTILC(GENERATE_ENUM)
};

enum pkt_log_biotilc_e {
  FOREACH_LOG_BIOTILC(GENERATE_ENUM)
};

typedef enum pkt_pid_id_e {
  PKT_PID_L_KNEE_POSITION,
  PKT_PID_L_KNEE_SPEED,

  PKT_PID_R_KNEE_POSITION,
  PKT_PID_R_KNEE_SPEED,
} pkt_pid_id_t;
/*----------------------------------------------------------------------------*/
typedef struct {
  float setpoint;
  float Kp;
  float Ki;
  float Kd;
  float current;
  float speed;
  float position;
  float current_command;
  float speed_command;
  float pwm_command;
  float direction;
  uint8_t pid_id;
} pkt_dbg_pid_t;

/*----------------------------------------------------------------------------*/
typedef struct {
  float r_hip_angle;
  float l_hip_angle;

  float r_knee_current;
  float r_knee_velocity;
  float r_knee_angle;

  float r_knee_current_commanded;
  float r_knee_velocity_commanded;
  float r_knee_angle_commanded;

  float l_knee_current;
  float l_knee_velocity;
  float l_knee_angle;

  float l_knee_current_commanded;
  float l_knee_velocity_commanded;
  float l_knee_angle_commanded;

  uint32_t timestamp;

  uint16_t an_0_l_fsr_heel;
  uint16_t an_2_r_fsr_heel;
  uint16_t an_1_l_fsr_ff;
  uint16_t an_3_r_fsr_ff;
  uint16_t an_11_bat_volt;

  uint8_t state;
  uint8_t time_marker;
} pkt_log_data_t;

/*----------------------------------------------------------------------------*/
typedef struct {
  float float_params[PKT_MODULE_PARAM_MAX];
  uint8_t mode;
  uint8_t module_id;
} pkt_module_params_t;

typedef pkt_module_params_t pkt_module_get_params_reply_t;
typedef pkt_module_params_t pkt_module_set_params_t;


/*----------------------------------------------------------------------------*/
typedef struct {
  //just send a bunch of floats they can get typecasted in the firmware.
  //You'll only get a couple bytes of overhead anyway.
  //It is too painful to need to update struct names manually every time.
  float float_params[PKT_HEAD_MODULE_PARAM_MAX];
} pkt_log_params_t;

typedef pkt_log_params_t pkt_set_params_t;
typedef pkt_log_params_t pkt_get_params_t;

/*----------------------------------------------------------------------------*/
typedef struct {
  uint8_t state;
} pkt_set_state_t;

/*----------------------------------------------------------------------------*/
typedef struct {
  float setpoint;
  float Kp;
  float Ki;
  float Kd;
  float guard_max;
  float guard_min;
  uint8_t pid_id;
} pkt_set_pid_t;

typedef pkt_set_pid_t pkt_get_pid_t;

/*----------------------------------------------------------------------------*/
typedef struct {
  char msg[MAX_PAYLOAD_LENGTH];
} pkt_str_msg_t;

/*----------------------------------------------------------------------------*/
typedef struct {
  float joint_current;
  float joint_velocity;
  float joint_angle;

  float joint_current_commanded;
  float joint_velocity_commanded;
  float joint_angle_commanded;

  float fsr_val_heel;
  float fsr_val_ff;

  float bus_voltage;
  float bus_current;
  float load_cell;

  uint8_t module_id;
} pkt_module_data_t;

/*----------------------------------------------------------------------------*/

typedef struct {
  pkt_module_data_t module_data[4];

  uint32_t timestamp;
  uint16_t an_11_bat_volt;

  uint8_t state;
  uint8_t time_marker;
} pkt_log_can_data_t;

/*----------------------------------------------------------------------------*/

typedef struct {
  uint8_t command_state;
  uint8_t module_id;
} pkt_query_module_t;

typedef pkt_query_module_t pkt_module_get_params_t;

/*----------------------------------------------------------------------------*/
typedef struct {
  float float_params[PKT_SHARED_MAX];
} pkt_shared_module_params_t;

typedef pkt_shared_module_params_t pkt_set_shared_module_params_t;

/*----------------------------------------------------------------------------*/
typedef struct {
  float float_params[PKT_MODULE_STS_MAX];
} pkt_module_sts_params_t;

/*----------------------------------------------------------------------------*/
typedef struct {
  float float_params[PKT_MODULE_SHARED_BURST_MAX];
} pkt_set_module_burst_params_t;

/*----------------------------------------------------------------------------*/
typedef struct {
  float float_params[PKT_STIM_CONFIG_MAX];
} pkt_stim_config_t;

/*----------------------------------------------------------------------------*/
typedef struct {
  float float_params[PKT_BT_MAX];
} pkt_biotilc_t;

/*----------------------------------------------------------------------------*/
typedef struct {
  float float_params[PKT_LOG_BT_MAX];
} pkt_log_biotilc_t;

#endif
