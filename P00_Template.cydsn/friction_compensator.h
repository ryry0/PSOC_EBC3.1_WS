#ifndef FRICTION_COMPENSATOR_H_
#define FRICTION_COMPENSATOR_H_

#include <math.h>

inline float fc_sigmoid(float omega) {
  return 2.0/(1.0+expf(-omega)) - 1.0;
}

inline float fc_velToTorque(float velocity) {
  const float M = 0.0334;
  const float B = 5.34;
  return M*fabs(velocity)+B;
}

inline float fc_torqueToCurrent(float torque) {
  const float M = 2.56;
  return torque/M;
}

inline float fc_compensateExp(float velocity, float alpha) {
  return alpha * fc_sigmoid(velocity) *
    fc_torqueToCurrent(
        fc_velToTorque(velocity));
}


inline float fc_compensateBounded(float velocity, float alpha) {
  const float BOUND = 10;
  if (fabs(velocity) < BOUND)
    return 0;

  else
    return alpha *
      fc_torqueToCurrent(
          fc_velToTorque(velocity));
}
#endif

