#ifndef MODULE_ID_H_
#define MODULE_ID_H_

typedef enum module_id_e {
  MODULE_LEFT_HIP = 0,
  //MODULE_RIGHT_HIP,
  //MODULE_LEFT_KNEE,
  //MODULE_RIGHT_KNEE,
  //MODULE_LEFT_ANKLE
  //MODULE_RIGHT_ANKLE,
  MODULE_MAX,
} module_id_t;

#ifdef ARDUINO //if it's a module
#define MODULE_ID MODULE_LEFT_HIP
#endif

#endif
