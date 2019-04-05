#include <stdio.h>
#include <math.h>
#include "trajectory.h"
#include <numerical.h>

int main(int argc, char ** argv) {
  const float tf = 1.50;
  const float tps_perc = 0.5;
  const float tps = tf*tps_perc;
  const float hip_angle_min = -15;
  const float hip_angle_max = 15;
  const float knee_angle_max = 40;
  float *time;
  size_t len_time = nm_genRange(0, 0.01, tf, &time);

  tr_coeff_t hip_coeff;
  tr_coeff_t knee_pre_coeff;
  tr_coeff_t knee_late_coeff;
  tr_computeHipTrajCoeff(&hip_coeff, hip_angle_min, hip_angle_max, tf, tps);
  tr_computePreSwingKneeTrajCoeff(&knee_pre_coeff, knee_angle_max,
      hip_angle_min);
  tr_computeLateSwingKneeTrajCoeff(&knee_late_coeff, knee_angle_max,
      hip_angle_max);

  printf("time,hip_traj,knee_pre_traj, knee_late_traj\n");
  for (size_t i = 0; i < len_time; ++i) {
    printf("%f,%f,%f,%f\n", time[i],
        tr_computeHipTraj(&hip_coeff, time[i]),
        tr_computeKneeTraj(&knee_pre_coeff, tr_computeHipTraj(&hip_coeff, time[i])),
        tr_computeKneeTraj(&knee_late_coeff, tr_computeHipTraj(&hip_coeff, time[i]))
        );
  }

  return 0;
}
