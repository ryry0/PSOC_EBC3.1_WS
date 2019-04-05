#include <trajectory.h>
#include <math.h>

/* All matrices are addressed in row major order A[row][column] */

/**
 * \brief Computes a hip coefficient based on min and max hip angles, and a row
 * of the inverse matrix.
 *
 * \param hip_angle_min Minimum hip angle
 * \param hip_angle_max Maximum hip angle
 * \param a A row of the inverse matrix.
 *
 * \return Hip coefficient.
 */
static float tr_computeHipCoeff(float hip_angle_min, float hip_angle_max, const float a[3]);

/**
 * \brief Compute the inverse of a 3x3 matrix.
 *
 * Assumes that arrays are preallocated.
 *
 * \param a The matrix to invert
 * \param dest_inv_a The destination memory for the inverted matrix
 */
static void tr_compute3x3Inverse(const float a[3][3], float dest_inv_a[3][3]);

/**
 * \brief Compute the determinant of a 3 by 3 matrix
 *
 * \param a Matrix to compute the determinant of.
 * \return the determinant.
 */

static float tr_computeDeterminant(const float a[3][3]);

static float tr_computeDeterminant(const float a[3][3]) {
  return
    a[0][0]*a[1][1]*a[2][2] +
    a[1][0]*a[2][1]*a[0][2] +
    a[2][0]*a[0][1]*a[1][2] +
    -a[0][0]*a[2][1]*a[1][2] +
    -a[2][0]*a[1][1]*a[0][2] +
    -a[1][0]*a[0][1]*a[2][2];
}

static void tr_compute3x3Inverse(const float a[3][3], float dest_inv_a[3][3]) {
  const float det = tr_computeDeterminant(a);

  dest_inv_a[0][0] = 1.0/det*(a[1][1]*a[2][2] - a[1][2]*a[2][1]);
  dest_inv_a[1][0] = 1.0/det*(a[1][2]*a[2][0] - a[1][0]*a[2][2]);
  dest_inv_a[2][0] = 1.0/det*(a[1][0]*a[2][1] - a[1][1]*a[2][0]);

  dest_inv_a[0][1] = 1.0/det*(a[0][2]*a[2][1] - a[0][1]*a[2][2]);
  dest_inv_a[1][1] = 1.0/det*(a[0][0]*a[2][2] - a[0][2]*a[2][0]);
  dest_inv_a[2][1] = 1.0/det*(a[0][1]*a[2][0] - a[0][0]*a[2][1]);

  dest_inv_a[0][2] = 1.0/det*(a[0][1]*a[1][2] - a[0][2]*a[1][1]);
  dest_inv_a[1][2] = 1.0/det*(a[0][2]*a[1][0] - a[0][0]*a[1][2]);
  dest_inv_a[2][2] = 1.0/det*(a[0][0]*a[1][1] - a[0][1]*a[1][0]);
}

static float tr_computeHipCoeff(float hip_angle_min, float hip_angle_max, const float a[3]) {
  const float gamma = hip_angle_max - hip_angle_min;
  const float delta = 0;
  const float epsilon = -hip_angle_min;

  return gamma*a[0] + delta*a[1] + epsilon*a[2];
}

void tr_computeHipTrajCoeff(tr_coeff_t *coefficients,
    float hip_angle_min, float hip_angle_max, float tf, float tps) {

  const float A[3][3] = {
    { pow(tf, 2),   pow(tf, 3),   pow(tf,4)   },
    { 2*tf,         3*pow(tf,2),  4*pow(tf,3) },
    { pow(tps, 2),  pow(tps, 3),  pow(tps,4)   }
  };

  float inv_A[3][3] = {0};

  tr_compute3x3Inverse(A, inv_A);

  coefficients->a = tr_computeHipCoeff(hip_angle_min, hip_angle_max, inv_A[2]);
  coefficients->b = tr_computeHipCoeff(hip_angle_min, hip_angle_max, inv_A[1]);
  coefficients->c = tr_computeHipCoeff(hip_angle_min, hip_angle_max, inv_A[0]);
  coefficients->d = 0.0;
  coefficients->e = hip_angle_min;
}

float tr_computeHipTraj(tr_coeff_t *coefficients, float time) {
  return
    coefficients->a*pow(time, 4) +
    coefficients->b*pow(time, 3) +
    coefficients->c*pow(time, 2) +
    coefficients->d*pow(time, 1) +
    coefficients->e;
}

float tr_computeKneeTraj(tr_coeff_t *coefficients, float hip_angle) {
  return coefficients->a*powf(hip_angle, 3) + coefficients->d;
}

void tr_computePreSwingKneeTrajCoeff(tr_coeff_t *coefficients,
    float knee_angle_max, float hip_angle_min) {
  coefficients->a = -knee_angle_max/pow(hip_angle_min,3);
  coefficients->b = 0.0;
  coefficients->c = 0.0;
  coefficients->d = knee_angle_max;
  coefficients->e = 0.0;
}

void tr_computeLateSwingKneeTrajCoeff(tr_coeff_t *coefficients,
    float knee_angle_max, float hip_angle_max) {
  coefficients->a = -knee_angle_max/pow(hip_angle_max,3);
  coefficients->b = 0.0;
  coefficients->c = 0.0;
  coefficients->d = knee_angle_max;
}

void tr_computeHipTrajCoeffScaled(tr_coeff_t *coefficients,
    float hip_angle_min, float hip_angle_max, float tf, float tps_perc) {

  tr_computeHipTrajCoeff(coefficients, hip_angle_min, hip_angle_max, tf,
      tps_perc*tf);
}

float tr_computeFullKneeTraj(
    tr_coeff_t *preswing_knee_coeff,
    tr_coeff_t *lateswing_knee_coeff,
    float hip_angle) {
  if(hip_angle < 0)
    return tr_computeKneeTraj(preswing_knee_coeff, hip_angle);
  else
    return tr_computeKneeTraj(lateswing_knee_coeff, hip_angle);
}


size_t tr_getLIndex(size_t hip_index_min, size_t hip_index_max,
    uint16_t hip_angle_count) {
  int32_t index = hip_angle_count - hip_index_min;
  index = nm_constrain(index, 0, hip_index_max - hip_index_min);
  return index;
}

size_t tr_getRIndex(size_t hip_index_min, size_t hip_index_max,
    uint16_t hip_angle_count) {
  int32_t index = -(hip_angle_count - hip_index_min);
  index = nm_constrain(index, 0, hip_index_min - hip_index_max);
  return index;
}

size_t tr_genKneeLookupTable(
    tr_coeff_t *preswing_knee_coeff,
    tr_coeff_t *lateswing_knee_coeff,
    float *dest_array,
    size_t array_size,
    float increment,
    float hip_min_angle,
    float hip_max_angle) {

  size_t i = 0;

  for (i = 0; i < array_size; ++i) {
    float angle = hip_min_angle + i*increment;
    if (angle > hip_max_angle)
      break;

    dest_array[i] = tr_computeFullKneeTraj(preswing_knee_coeff,
        lateswing_knee_coeff, angle);
  }
  return i;
}

void tr_genKneeTimeLookupTable(tr_coeff_t *hip_coeff,
    tr_coeff_t *preswing_knee_coeff,
    tr_coeff_t *lateswing_knee_coeff,
    float *dest_array,
    size_t array_size,
    float increment,
    float pre_swing_time,
    float final_time) {

  for (size_t i = 0; i < array_size; ++i) {
    float curr_time = i*increment;
    if (curr_time > final_time)
      curr_time = final_time;

    if (curr_time < pre_swing_time) {
      dest_array[i] = tr_computeKneeTraj(preswing_knee_coeff,
          tr_computeHipTraj(hip_coeff, curr_time));
    }
    else {
      dest_array[i] = tr_computeKneeTraj(lateswing_knee_coeff,
          tr_computeHipTraj(hip_coeff, curr_time));
    }
  }
}

